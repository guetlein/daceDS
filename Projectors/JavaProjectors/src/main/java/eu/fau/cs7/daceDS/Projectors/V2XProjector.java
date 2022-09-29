/*******************************************************************************
 * Copyright 2021 Moritz Gütlein
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/

package eu.fau.cs7.daceDS.Projectors;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.kafka.clients.consumer.ConsumerRecord;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.ConsumerCallback;
import eu.fau.cs7.daceDS.Component.ConsumerRecordComparator;
import eu.fau.cs7.daceDS.Component.Instance;
import eu.fau.cs7.daceDS.Component.OrchestrationHandler;
import eu.fau.cs7.daceDS.Component.Projector;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;
import eu.fau.cs7.daceDS.Component.Translator;
import eu.fau.cs7.daceDS.Kafka.ConsumerCallbackImplKafka;
import eu.fau.cs7.daceDS.Kafka.ConsumerImplKafka;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.datamodel.InteractionMsg;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.Micro;
import eu.fau.cs7.daceDS.datamodel.Vec3;


public class V2XProjector extends Projector implements ConsumerCallbackImplKafka  {


	private OrchestrationHandler orchestrationHandler;
	private ConsumerImplKafka<Micro> patternReader;
	private HashMap<Long, ArrayList<ConsumerRecord>> buffer = new HashMap<Long, ArrayList<ConsumerRecord>>();
	private int callcount = 0;
	private ProducerImplKafka<InteractionMsg> writerCounting;
	private String consumingScenarioID;

	public V2XProjector(String scenarioID, String instanceID, String demoScenario) {
		super(scenarioID, instanceID, "");
	}

	public static void main(String[] args) {

		if(args.length < 2) {
			System.out.println("Usage exe scenarioID instanceID [demo]");
			return;
		}
		String scenarioID = args[0];
		String instanceID = args[1];
		String demoScenario = "";
		
		V2XProjector instance = new V2XProjector(scenarioID, instanceID, demoScenario);
		
		instance.run();
		 
	}
	

	@Override
	public void preInit() {
		
		System.out.println("MicroMesoTranslator is started!");
		orchestrationHandler = new OrchestrationHandler(scenarioID, instanceID);
		
		if(demoScenario.length() == 0) {
			orchestrationHandler.gatherScenarioData(); //returns wenn scenario ack is received 
			scenario = orchestrationHandler.getScenario();
			projectorDescription = orchestrationHandler.getProjector();
		} 
		
		if(projectorDescription.getParameters().containsKey("sceID")) {
			consumingScenarioID = projectorDescription.getParameters().get("sceID").toString();
		} else {
			consumingScenarioID = scenarioID;
		}

		patternReader = new ConsumerImplKafka<Micro>(scenarioID+"_"+instanceID+"_patternReader", true);
		patternReader.initPattern("provision\\.simulation\\."+scenarioID+"\\.traffic\\.micro\\.vehicle", (ConsumerCallback)this, false, true);			
		
		writerCounting = new ProducerImplKafka<InteractionMsg>(scenarioID+"_"+instanceID+"_iwriter", true);
		writerCounting.init();

	}
		
	@Override
	public <T> void receive(ConsumerRecord r, long time, int epoch, String sender) {

		Long timeepoch = ((long) (1000*time))+epoch;
//		logger.info("CB Received object time is "+timeepoch);
		synchronized(buffer) {
			if(!buffer.containsKey(timeepoch)) {
				ArrayList<ConsumerRecord> l = new ArrayList<ConsumerRecord>();
				buffer.put(timeepoch, l);
			}
			buffer.get(timeepoch).add(r); //todo: sorting when adding might be faster
		}
		
	}

//	public void synchronizeEvent(int t) {
//		super.synchronizeEvent(t);
//		processBuffer(t, this);
//	}
	
	public void synchronizeEvent(int t){
		try {
			boolean gotSomethingNew = false;
			epoch = 1;	
			do {
				////////////////////////////////////////////////////
				//make one ministep			
				logInternal("requesting to go to "+(t)+"."+epoch);
				timeSync.timeAdvance(t,epoch);
				logInternal("got LBTS and MESSAGES for proceeding to "+(t)+"."+epoch);
				//now we can sort and process all incoming data (sorting gets done already done every time a msg is appended)
				//we will process all messages that are OLDER than t+1, in order to be able to modify our sate for time=t
				gotSomethingNew = getBufferCount(t, epoch) > 0;
				processBuffer(t, epoch, this);
				logInternal("processed all messages with t <"+(t)+"."+epoch);
				//now we are at [t].[epoch]
				epoch++;
				
				
			} while(gotSomethingNew);
			
			
			////////////////////////////////////////////////////
			//make one big step to t+1		
			timeSync.timeAdvance(t+stepLengthInMs, 0);		
			
			processBuffer(t, 0, this);
			if(t%100000 == 0) {
				logInternal("processed all messages with t <"+(t+1));
			}
			
		}
		catch ( Exception e) {
			System.out.println("synchronizeEvent failed!");
			System.out.println(e.getMessage());
		}
	}
	
	public int getBufferCount(long time) {
		return getBufferCount(time, 0);
	}
	
	public int getBufferCount(long time, int epoch) {
		Long timeepoch = ((long) (1000*time))+epoch; //todo: set reasonable constant as MAX_EPOCH
		synchronized(buffer) {
			if(!buffer.containsKey(timeepoch)) return 0;
			return buffer.get(timeepoch).size();
		}
	}
	
	public void processBuffer(long time, int epoch, Instance instance) {
		Long timeepoch = ((long) (1000*time))+epoch;

		synchronized(buffer) {
			ArrayList<Long> toRemove = new ArrayList<Long>();
			for(Entry<Long, ArrayList<ConsumerRecord>> e : buffer.entrySet()) {
				if(e.getKey() < timeepoch) {	
					ArrayList<ConsumerRecord> msgs = e.getValue();
			        Collections.sort(msgs, new ConsumerRecordComparator());
					for(ConsumerRecord r : msgs) {
						processMessage(r);
					}
					toRemove.add(e.getKey());
				}
			}
			
			for(Long key : toRemove) {
				buffer.remove(key);
			}
		}
	}	
	

	@Override
	protected void processMessage(ConsumerRecord r) {		
		Micro m = (Micro) r.value();
		InteractionMsg imsg = new InteractionMsg();
		imsg.setCallID(instanceID+(callcount ++));
		imsg.setMethodID("node.position.set");
		
		Map<CharSequence, CharSequence> inputMap = new HashMap<CharSequence, CharSequence>();
		inputMap.put("vehicleID", m.getVehicleID());
		inputMap.put("x", String.valueOf(m.getPosition().getX()));
		inputMap.put("y", String.valueOf(m.getPosition().getY()));
		inputMap.put("z", String.valueOf(m.getPosition().getZ()));
		imsg.setInput(inputMap );
		
		Map<CharSequence, CharSequence> outputMap = new HashMap<CharSequence, CharSequence>();
		imsg.setOutput(outputMap);
		System.out.println("Publishing interaction request");
		writerCounting.publish("interaction.simulation."+consumingScenarioID+".communication.80211p.request", imsg, timeSync.getLocaltime());
		
	}

	@Override
	protected void processInteraction(ConsumerRecord r) {			
		//not needed	
	}

}