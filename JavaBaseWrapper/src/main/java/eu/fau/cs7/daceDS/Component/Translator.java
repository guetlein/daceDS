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

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.log4j.Logger;



public abstract class Translator<A,B> extends Instance{

	Logger logger = Logger.getLogger("Translator");

	protected eu.fau.cs7.daceDS.datamodel.Translator translatorDescription;
	
	protected ProvisionHandler<A> layerAProvisionHandler = null;
	protected ProvisionHandler<B> layerBProvisionHandler= null;

	protected String layerAProvisionPostPattern;
	protected String layerAProvisionPrePattern;
	protected String layerBProvisionPrePattern;
	protected String layerBProvisionPostPattern;
	protected String layerA;
	protected String layerB;
	Class<A> classA;
	Class<B> classB;
	protected int epoch;
	protected String topicProvideB;
	protected String topicProvideA;

	protected DomainHelper domainHelperLayerA;
	protected DomainHelper domainHelperLayerB;

	private int sentA = 0;

	private int sentB = 0;


	
	public Translator(String scenarioID, String instanceID, String layerA, String layerAProvisionPrePattern, String layerAProvisionPostPattern, String layerB, String layerBProvisionPrePattern, String layerBProvisionPostPattern, String demoScenario){
		this.layerAProvisionPrePattern=layerAProvisionPrePattern;
		this.layerAProvisionPostPattern=layerAProvisionPostPattern;
		this.layerBProvisionPrePattern=layerBProvisionPrePattern;
		this.layerBProvisionPostPattern=layerBProvisionPostPattern;
		this.layerA=layerA;
		this.layerB=layerB;
		this.scenarioID = scenarioID;
		this.instanceID = instanceID;
		this.demoScenario = demoScenario;
	}
	
	
	public void init() {
		
		logger.info("creating the two provision handlers");

		System.out.println("domainHelperLayerA.getInternalResponsibilities() = "+domainHelperLayerA.getInternalResponsibilities());
		layerAProvisionHandler = new ProvisionHandler<A>(scenario, translatorDescription, layerA, layerAProvisionPrePattern, layerAProvisionPostPattern, domainHelperLayerA);
		System.out.println("domainHelperLayerB.getInternalResponsibilities() = "+domainHelperLayerB.getInternalResponsibilities());
		layerBProvisionHandler = new ProvisionHandler<B>(scenario, translatorDescription, layerB, layerBProvisionPrePattern, layerBProvisionPostPattern, domainHelperLayerB);
		layerAProvisionHandler.init();
		layerBProvisionHandler.init();
		

		synchronizing = true;
		
		logger.info("init finished");
		

	}

	public String getProvisionPreTopicLayerA() {
		return "provision.simulation."+scenarioID+"."+translatorDescription.getDomain()+"."+layerA+".";
	}
	public String getProvisionPreTopicLayerB() {
		return "provision.simulation."+scenarioID+"."+translatorDescription.getDomain()+"."+layerB+".";
	}
	

	public abstract String getProvisionTopicLayerA(A a);
	public abstract String getProvisionTopicLayerB(B b);
	
	@Override
	public void preLoopEvent() {

	}

	public void synchronizeEvent(int t){
		try {
	//		log(t + "/"+scenario.getSimulationEnd()*1000);
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
				gotSomethingNew = layerAProvisionHandler.getBufferCount(t, epoch) > 0 || layerBProvisionHandler.getBufferCount(t, epoch) > 0;
				layerAProvisionHandler.processBuffer(t, epoch, this);
				layerBProvisionHandler.processBuffer(t, epoch, this);
				logInternal("processed all messages with t <"+(t)+"."+epoch);
				//now we are at [t].[epoch]
				epoch++;
				
				
			} while(gotSomethingNew);
			
			
			////////////////////////////////////////////////////
			//make one big step to t+1		
			timeSync.timeAdvance(t+stepLengthInMs, 0);		
			
			layerAProvisionHandler.processBuffer(t, this);
			layerBProvisionHandler.processBuffer(t, this);
			if(t%100000 == 0) {
				logInternal("processed all messages with t <"+(t+1));
				logInternal("sent a and b:" + sentA + " " + sentB);
			}
			
		}
		catch ( Exception e) {
			System.out.println("synchronizeEvent failed!");
			System.out.println(e.getMessage());
		}
	}



	@Override
	public void stepEvent(int t) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void preStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void processStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void postStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void postLoopEvent() {
		// TODO Auto-generated method stub
		
	}
	


	public void close(){
		layerAProvisionHandler.close();
		layerBProvisionHandler.close();
		
		super.close();
	}


	public void processMessage(ConsumerRecord r) {
//		log("processMessage");
		try {
			if(r.topic().contains(layerA)) {
				ConsumerRecord<String, A> msg = r;
				B b = translateA2B(msg);
				sendB(r,b);			
			} else if(r.topic().contains(layerB)) {
				ConsumerRecord<String, B> msg = r;
				A a = translateB2A(msg);
				sendA(r,a);
			} else {
				log("got message from unkown class to process");
			}
		}
		catch ( Exception e) {
			System.out.println("Translation failed!");
			System.out.println(e.getMessage());
		}
	}

	protected abstract A translateB2A(ConsumerRecord record);

	protected abstract B translateA2B(ConsumerRecord record);

	
	//todo: replace is not very good, e.g. does break for macro where postpattern is empty
	protected void sendA(ConsumerRecord record, A a) {
		try {
			if(a==null) return;
			String newTopic = getProvisionPreTopicLayerA()+getProvisionTopicLayerA(a);
	//		log("converted incoming topic ("+record.topic()+") to "+newTopic);
			layerAProvisionHandler.sendNDM(newTopic, a, (int) timeSync.getLocaltime(), epoch); //todo retrieve epoch from timesync
			sentA++;
		}
		catch ( Exception e) {
			System.out.println("sendA failed!");
			System.out.println(e.getMessage());
		}
	};
	


	//todo: replace is not very good, e.g. does break for macro where postpattern is empty
	protected void sendB(ConsumerRecord record, B b) {
		try {
			if(b==null) return;
			String newTopic = getProvisionPreTopicLayerB()+getProvisionTopicLayerB(b);
	//		log("converted incoming topic ("+record.topic()+") to "+newTopic);
			layerBProvisionHandler.sendNDM(newTopic, b, (int) timeSync.getLocaltime(), epoch); //todo retrieve epoch from timesync
			sentB++;
		}
		catch ( Exception e) {
			System.out.println("sendB failed!");
			System.out.println(e.getMessage());
		}
	};	
	

	@Override
	protected void processInteraction(ConsumerRecord r) {
		//a translator will not subscribe to and process interaction		
	}

}