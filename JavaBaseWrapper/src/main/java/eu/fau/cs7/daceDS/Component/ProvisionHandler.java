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
package eu.fau.cs7.daceDS.Component;


import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Kafka.ConsumerImplKafka;
import eu.fau.cs7.daceDS.Kafka.ConsumerCallbackImplKafka;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Observer;
import eu.fau.cs7.daceDS.datamodel.Scenario;
import eu.fau.cs7.daceDS.datamodel.Translator;
import eu.fau.cs7.daceDS.datamodel.Projector;


/**
 * The ProvisionHandler is taking care of all messages that are related to the provision channel.
 * Most importantly, this is about realizing the simulation coupling by subscribing to responsibilities
 * and providing NDM tuples of entities that are leaving the own scope. 
 * 
 * In addition, all observers are registered and operated here.
 * 
 * @author guetlein
 *
 * @param <T> native data model of layer.
 */
public class ProvisionHandler<T> implements ConsumerCallbackImplKafka {
	
	private final boolean AUTO_POLL = true;
	private final boolean FETCH_LATEST = false;
	Logger logger = Logger.getLogger(this.getClass());


	private List<Observer> observers =  new ArrayList<Observer>();
	private List<String> responsibilities = new ArrayList<String>();
	private String responsibilityString = "";
	List<String> subscribedTopics = new ArrayList<String>();

	private ProducerImplKafka<T> writerCounting;
	private HashMap<Long, ArrayList<ConsumerRecord>> buffer = new HashMap<Long, ArrayList<ConsumerRecord>>();

	private final String scenarioID;
	private final String instanceID;
	private final String layer;
	private final String domain;
	private final String provisionPrePattern;
	private final String provisionPostPattern;

	private String topicProvisionPattern;
	private ConsumerImplKafka<T> patternReader;
	private ObserverI observerImpl;

	public ProvisionHandler(Scenario scenarioDescription, BB sim, DomainHelper domainHelper) {
		this(scenarioDescription, sim, "", "",domainHelper);
	}
	
	public ProvisionHandler(Scenario scenarioDescription, BB sim, String provisionPrePattern, String provisionPostPattern, DomainHelper domainHelper) {
		this.scenarioID = scenarioDescription.getScenarioID().toString();
		this.instanceID = sim.getInstanceID().toString();
		this.layer = sim.getLayer().toString();
		this.domain = sim.getDomain().toString();
		this.provisionPrePattern = provisionPrePattern;
		this.provisionPostPattern = provisionPostPattern;
		this.responsibilities = domainHelper.getInternalResponsibilities();
		
	}
	public ProvisionHandler(Scenario scenarioDescription, Translator translator, String layer, String provisionPrePattern, String provisionPostPattern, DomainHelper domainHelper) {
		this.scenarioID = scenarioDescription.getScenarioID().toString();
		this.instanceID = translator.getTranslatorID().toString();
		this.layer = layer;
		this.domain = translator.getDomain().toString();
		this.provisionPrePattern = provisionPrePattern;
		this.provisionPostPattern = provisionPostPattern;
		this.responsibilities = domainHelper.getInternalResponsibilities();
		
	}
	public ProvisionHandler(Scenario scenarioDescription, Projector projector, String domain, String layer, String provisionPrePattern, String provisionPostPattern, String responsibilityString) {
		this.scenarioID = scenarioDescription.getScenarioID().toString();
		this.instanceID = projector.getProjectorID().toString();
		this.layer = layer;
		this.domain = domain;
		this.provisionPrePattern = provisionPrePattern;
		this.provisionPostPattern = provisionPostPattern;
		this.responsibilityString = responsibilityString;
	}

	public String getPre() {
		return "provision\\.simulation\\."+scenarioID+"\\."+domain+"\\."+layer+"\\.";
	}
	
	public void init() {
		
		logger.info("init");
		
		//this writer will be used for the ndm
		writerCounting = new ProducerImplKafka<T>(scenarioID+"_"+instanceID+"_NDMWriter", true);
		writerCounting.init();
		
		//process responsibilities and create a regex pattern string
		if(responsibilityString.length() == 0 && responsibilities.size()>0){
			//build pattern/regex
			for(int i=0;i<responsibilities.size();i++){
				responsibilityString += Config.escapeSpecialChars(responsibilities.get(i));
				if (i<responsibilities.size()-1){
					responsibilityString += "|";
				}
			}
		}
		
		//subscribe to responsibilities
		if(responsibilityString.length() > 0) {
			topicProvisionPattern = getPre() + provisionPrePattern + "("+responsibilityString+")" + provisionPostPattern;
			patternReader = new ConsumerImplKafka<T>(scenarioID+"_"+instanceID+"_patternReader", true);
			patternReader.initPattern(topicProvisionPattern, (ConsumerCallback)this, FETCH_LATEST, AUTO_POLL);			
			logger.info("subscribing to pattern="+topicProvisionPattern);
		} else {
			logger.info("have 0 responsibilities!!! no subscrptions");
		}
	}
	

	public void sendNDM(String topic, T msg, long time) {
		sendNDM(topic, msg, time, 0);
	}
	public void sendNDM(String topic, T msg, long time, int epoch) {
//		logger.info(time+", Sending '"+msg+"' to "+topic);
		writerCounting.publish(topic, "key", msg, time, epoch, instanceID); //todo: eigtl kannd er auch buch führen pber die zeit, wird ja keine aus der vergangenheit empfangen	
	}

	@Override
	public <T> void receive(ConsumerRecord r, long time, int epoch) { //todo: eigtl wollte ich hier schon kafka agnostisch sein
		
		String sender = new String(r.headers().lastHeader("sender").value());
		if(sender.equals(instanceID)){
			//skipping own message
			return;
		}

		Long timeepoch = ((long) (1000*time))+epoch;
//		logger.info("CB Received object time is "+timeepoch);
		synchronized(buffer) {
			if(!buffer.containsKey(timeepoch)) {
				ArrayList<ConsumerRecord> l = new ArrayList<ConsumerRecord>();
				buffer.put(timeepoch, l);
			}
			buffer.get(timeepoch).add(r); //todo: sort
		}
	}

	public void processBuffer(long time, Instance instance) {
		processBuffer(time, 0, instance);
	}
	
	/*process everything that is older than time.epoch*/
	public void processBuffer(long time, int epoch, Instance instance) {
		Long timeepoch = ((long) (1000*time))+epoch;

		logger.debug("processing buffer for timeepoch " + timeepoch);
		synchronized(buffer) {
			ArrayList<Long> toRemove = new ArrayList<Long>();
			for(Entry<Long, ArrayList<ConsumerRecord>> e : buffer.entrySet()) {
				if(e.getKey() < timeepoch) {	
					for(ConsumerRecord r : e.getValue()) {
						logger.debug("processing " + r.topic() + ", "+ r.key()+ ", "+ r.timestamp());
						instance.processMessage(r);
					}
					toRemove.add(e.getKey());
				}
			}
			
			for(Long key : toRemove) {
				buffer.remove(key);
			}
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
	
	
	
	public void addObserver(Observer o, ObserverI impl) {
		observerImpl=impl; //todo: should it be possible to have different for each observer?
		observers.add(o);

		System.out.println("added observer");
	}
	
	public void runObservers() {
		long simtimeInMS = TimeSync.getLocaltime();
		for(Observer o : observers) {
			if(simtimeInMS % o.getPeriod() != 0) {
				continue;
			}
			
			if(observerImpl!=null) {
				observerImpl.processObserver(o);
			} else {
				logger.error("Error! there is no observer impl!");
			}			
		}
	}
	
	public void close(){
		if(patternReader!=null){
			try{
				patternReader.close();
			}catch(Exception e){ 
				logger.error("failed to close patternReader");
			}
		}
		try{
			writerCounting.close();
		}catch(Exception e){ 
			logger.error("failed to close writers");
		}
	}

}
