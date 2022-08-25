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

import java.util.Date;
import java.util.Random;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.datamodel.Scenario;

/*
 * 
 * This: Executes methods in correct order.
 * 
 */

public abstract class Instance {

	public String logDir = "/tmp/";
	public String scenarioID = "";
	protected String instanceID = "";
	protected Scenario scenario;
	protected String demoScenario = "";
	protected ProducerImplKafka<String> stringWriter = null;
	private String topicStatus = "status";

	Logger logger = Logger.getLogger("Instance");
	protected TimeSync timeSync;

	protected DomainHelper domainHelper;
	protected int stepLengthInMs = 1000;
	protected boolean synchronizing;
	
	protected Random rng;
	
	public void runPreperations() {
		loadConfig();
		
		preInit();
		
		init();
		
		prepare();
	}
	
	public void runCleanup() {
		System.out.println("Sim is over!");
		close();

		System.out.println("bye.");
//		System.exit(0);
	}

	public void run() {
		logInternal("Instance is in run stage");
		try {
		
			runPreperations();
			logInternal("Instance finished runPreperations");
	
			scenarioLoop();
			logInternal("Instance finished scenarioLoop");
			
			runCleanup();
			logInternal("Instance finished runCleanup");
		} catch (Exception e) {
			System.out.println(e);
		}
	}
	
	public void scenarioLoop() {
		
		preLoopEvent();
		
		for(int t = (int) scenario.getSimulationStart()*1000; t <= scenario.getSimulationEnd()*1000; t+=stepLengthInMs ) {
		
//			log(t+ " pre step");
			preStepEvent(t);
//			log(t+ " step");
			stepEvent(t);			//calculate next sim step
//			log(t+ " process step");
			processStepEvent(t);	//process step, i.e. send out ndms
//			log(t+ " sync event");
			synchronizeEvent(t);	//request next step
//			log(t+ " post step");
			postStepEvent(t);		//here we have incorporated all incoming ndms and can publish observer stuff

		}

		postLoopEvent();
	}

	
	

	protected abstract void preInit();

	protected abstract void init();
	
	protected void loadConfig() {
		String l = eu.fau.cs7.daceDS.Component.Config.LOG_DIR;
		String lf = eu.fau.cs7.daceDS.Component.Config.LOG_FILE;
		System.setProperty(l,"/tmp/daceDS/");
		System.setProperty(lf,"/tmp/daceDS/"+scenarioID+"_"+instanceID+".log");
		eu.fau.cs7.daceDS.Component.Config.readConfig();
		logDir = eu.fau.cs7.daceDS.Component.Config.getInstanceLogDir(scenarioID, instanceID);
		System.setProperty(l, logDir);
		System.setProperty(lf, logDir+"/log.txt");
		System.setProperty("jogl.disable.openglcore", "true");

		eu.fau.cs7.daceDS.Component.Config.getOutputDir(scenarioID,instanceID);

	}
	
	protected void prepare() {

		
		//status writer
		stringWriter = new ProducerImplKafka<String>(scenarioID+"_"+instanceID+"_stringWriter", true);
		logger.info("INIT stringWriter");
		stringWriter.initPlainString();
		topicStatus = Config.getStatusTopic(scenarioID);
		log("started");
		

		int noParticipants = (int)scenario.getExecution().getSyncedParticipants();		
		timeSync = new TimeSync();
		logger.info("Init KafkaTimeClient");
		timeSync.init(scenarioID, instanceID, noParticipants);
		timeSync.join();
		logger.info("Joining timing --> done");
		

		long seed = (int)scenario.getExecution().getRandomSeed();
		rng = new Random(seed);		

	}
	
	public void synchronizeEvent(int t) {
		if(synchronizing) {
//			log("requesting to go to "+(t+stepLengthInMs));
			timeSync.timeAdvance(t+stepLengthInMs);

//			log("got LBTS and MESSAGES for proceeding to "+(t+stepLengthInMs));

//			//for testing to have at least a common start
//		} else {
//			if (t==1) {
//				timeSync.timeAdvance(2);
//			}
//			try {
//				Thread.sleep(212);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
		}
		else {
			log("sync not enabled");
		}
		

		
	}

	protected abstract void preLoopEvent();
	protected abstract void preStepEvent(int t);
	protected abstract void stepEvent(int t);
	protected abstract void processStepEvent(int t);
	protected abstract void postStepEvent(int t);
	protected abstract void postLoopEvent();
	
	protected void log(String msg) {
		if(stringWriter!=null)
			stringWriter.publish(topicStatus, instanceID+": "+msg, -1);
		logger.info(instanceID+": "+msg);
	}
	
	protected void logInternal(String msg) {
		logger.info(instanceID+": "+msg);
	}

	protected void close(){

		log("closing");
		
		if(stringWriter!=null)
			stringWriter.close();
		
		try{
			timeSync.leave();
			timeSync.close();
		}catch(Exception e){ 
			logger.error("failed to close timing");
		}
	}

	protected abstract void processMessage(ConsumerRecord r);
	protected abstract void processInteraction(ConsumerRecord r);

}