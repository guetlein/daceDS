/*******************************************************************************
 * Copyright 2022 Moritz Gütlein
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

package eu.fau.cs7.daceDS.Demo;

import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Component.InteractionHandler;
import eu.fau.cs7.daceDS.Component.Projector;
import eu.fau.cs7.daceDS.Component.ProvisionHandler;
import eu.fau.cs7.daceDS.datamodel.Scenario;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;


public class DemoProjector extends Projector{

	Logger logger = Logger.getLogger("Translator");
	private ProvisionHandler<DemoInteger> numbersIntegerProvisionHandler;
	private InteractionHandler colorsRGB3InteractionHandler;
	private List<DemoInteger> collectedIntegers = new ArrayList<DemoInteger>();
	private ArrayList<String> idMap = new ArrayList<String>();
	private Map<Integer,Integer> map = new HashMap<Integer,Integer>();


	public static void main(String[] args) {

		if(args.length < 2) {
			System.out.println("Usage exe scenarioID simulatorID [demo]");
			return;
		}
		String scenarioID = "gs_proj_3bit3";
		String instanceID = args[1];
		String demoScenario = "";

		if(args.length == 3) {
			demoScenario = "daceDS/Scenario/Demo/Projection_sine.sce";
		}


		DemoProjector instance = new DemoProjector(scenarioID, instanceID, demoScenario);
		instance.run();
		 
	}
	
	public DemoProjector(String scenarioID, String instanceID, String demoScenario){
		super(scenarioID, instanceID, demoScenario);
	}
	
	public void init() {

		super.init();
		
		if(demoScenario.length()>0) {
			scenario = Utils.getDemoScenario(demoScenario);
			scenario.setID(scenarioID);
			projectorDescription = Utils.getProjectorFromScenario(instanceID, scenario);
		}
		
		logger.info("creating the provision handler");

		numbersIntegerProvisionHandler = new ProvisionHandler<DemoInteger>(scenario, projectorDescription, "numbers", "integers", "", "", "DemoInteger");
		numbersIntegerProvisionHandler.init();
		
		colorsRGB3InteractionHandler = new InteractionHandler(scenario, projectorDescription, "colors", "RGB3", true);
		colorsRGB3InteractionHandler.init();
		
		logger.info("init finished");	
	}
	

	public void scenarioLoop(Scenario scenario) {
		
		preLoopEvent();
	
		for(t = 0; t < scenario.getSimulationEnd(); t++) {
	

			boolean gotSomethingNew = false;
			epoch = 1;	
			
			do {
				timeSync.timeAdvance(t,epoch);
				gotSomethingNew = numbersIntegerProvisionHandler.getBufferCount(t, epoch) > 0;
				numbersIntegerProvisionHandler.processBuffer(t, epoch, this);
				epoch++;				
			} while(gotSomethingNew);
			

			
			boolean first=true;
			for(DemoInteger i : collectedIntegers) {
				
				if(!map.containsKey(t)) {
					map.put(t,0);
				}
				String id = i.getId().toString();
				if(i.getValue()>0 ) {//&& id.charAt(id.length()-1)%2==1) {
//					String cmd = "+,"+idMap.indexOf(i.getId().toString());
//					colorsRGB3InteractionHandler.send(cmd, t);
					map.put(t,map.get(t)+1);
				}
				if(i.getValue()<0 ) {//&& id.charAt(id.length()-1)%2==0) {
//					String cmd = "-,"+idMap.indexOf(i.getId().toString());
//					colorsRGB3InteractionHandler.send(cmd, t);
					map.put(t,map.get(t)-1);
				}

				if(t>4) {
					if (map.get(t) > 1 && map.get(t-1) > 1 && map.get(t-2) > 1 && map.get(t-3) > 1 && map.get(t-4) > 1) {
						colorsRGB3InteractionHandler.send("+", t);
						map.put(t, 0);
					}
					if (map.get(t) < -1 && map.get(t-1) < -1 && map.get(t-2) < -1 && map.get(t-3) < -1 && map.get(t-4) < -1) {
						colorsRGB3InteractionHandler.send("-", t);
						map.put(t, 0);
					}
				}
			}
			collectedIntegers.clear();
			
			
			////////////////////////////////////////////////////
			//make one big step to t+1
			
			timeSync.timeAdvance(t+1, 0);		
			numbersIntegerProvisionHandler.processBuffer(t, this);
			log("processed all messages with t <"+(t+1));
//			}

		}


	}
	

	public void close(){
		numbersIntegerProvisionHandler.close();
		super.close();
	}


	public void processMessage(ConsumerRecord r) {
			ConsumerRecord<String,DemoInteger> i = r;
			collectedIntegers.add(i.value());
			//assign unique id
			if(idMap.indexOf(i.value().getId().toString())==-1) {
				idMap.add(i.value().getId().toString());
			}
			
	
	}


	@Override
	protected void preStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}

	@Override
	protected void processStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void preLoopEvent() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void postStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void stepEvent(int t) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void preSimStepEvent() {
		// TODO Auto-generated method stub
		
	}

	@Override
	protected void postLoopEvent() {
		// TODO Auto-generated method stub
		
	}

	@Override
	protected void preInit() {
		// TODO Auto-generated method stub
		
	}

	@Override
	protected void processInteraction(ConsumerRecord r) {
		// TODO Auto-generated method stub
		
	}

}
