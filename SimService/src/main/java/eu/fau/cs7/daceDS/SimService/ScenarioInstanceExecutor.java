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
package eu.fau.cs7.daceDS.SimService;
import java.util.HashMap;
import java.util.Map.Entry;

import org.apache.kafka.common.errors.TimeoutException;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Projector;
import eu.fau.cs7.daceDS.datamodel.Scenario;
import eu.fau.cs7.daceDS.datamodel.Translator;


/**
 * A ScenarioRunner thread spawns several other related threads that are used
 * for the execution of further processes and waits for them to finish.
 * 
 * @author guetlein
 */
public class ScenarioInstanceExecutor extends Thread{


	private String basePath;
	private Scenario scenario;
	private boolean debug;
	private String scenarioID;
	private String kafkaID;
	private boolean done = false;
	private long startTime = -1;
	
	//Each thread executes one instance and hold its status
	HashMap<String, ExecutorThread> threads;

	static Logger logger  = Logger.getLogger(ScenarioInstanceExecutor.class.getName());

	public ScenarioInstanceExecutor(Scenario scenario, boolean debug) {
		this.scenario = scenario;
		this.scenarioID = scenario.getScenarioID().toString();
		this.debug = debug;
		this.kafkaID = scenario.getScenarioID()+ScenarioInstanceExecutor.class.getName();
		this.basePath = Config.get(Config.EXECUTABLES_ROOT);
	}

	public void run() {
		
		logger.info("ScenarioRunnerThread is started for " + scenarioID);
		
		
		//todo: and now wait or terminate this thread?
		for(Thread t : threads.values()) {
			try {
				t.join();
			} catch (InterruptedException e) {
				logger.error(e.getLocalizedMessage());
			}
		}
		threads.clear();

		logger.info("ScenarioRunnerThread is exiting for " + scenarioID);
	}
	
	public boolean hasCorruptedParts() {
		for(Thread t : threads.values()) {
			if (!t.isAlive()) return false;
		}
		return true;
	}
	
	//run tools, provide resources and so on
	public boolean instantiateScenario() {
		
		startTime = System.currentTimeMillis();

		/* Start requested simulators and translators */
		threads = executeInstances(scenario,debug);
		try {
			Thread.sleep(Config.DEFAULT_WAIT_MS);
		} catch (InterruptedException e) {
			logger.info(e.getLocalizedMessage());
		}
		

		/* Repost ScenarioFile --> acts as ACK and can be used by the tools to read parameters */
		logger.info("Acking SCE to "+Config.getProvisionTopic(scenarioID, Config.get(Config.TOPIC_SCENARIO)));
		boolean succPub = true;
		try {
			ProducerImplKafka<Scenario> kafkaWriter = new ProducerImplKafka<Scenario>(kafkaID);	
			kafkaWriter.init();
			succPub = kafkaWriter.publish(Config.getProvisionTopic(scenarioID, Config.get(Config.TOPIC_SCENARIO)), scenario, 0);
			kafkaWriter.close();
		} catch (TimeoutException e) {
			logger.error(e.getLocalizedMessage());
			return false;
		}
		if(!succPub) return false;

		return true;
	}

	public HashMap<String, ExecutorThread> executeInstances(Scenario scenario, boolean debug) {
		logger.info("Running scenario " + scenario.getScenarioID());
		logger.info("No of trafficsims: " + scenario.getBuildingBlocks().size());
		
		HashMap<String, ExecutorThread> threads= new HashMap<String,ExecutorThread>();

		for(BB sim : scenario.getBuildingBlocks()) {
			ExecutorThread t = new ExecutorThread(basePath,sim.getType().toString(), sim.getInstanceID().toString(), scenario);
			threads.put(sim.getInstanceID().toString(), t);
			logger.info("Added thread for "+sim.getInstanceID().toString());
			t.start();
		}
		
		for(Projector proj : scenario.getProjectors()) {
			ExecutorThread t = new ExecutorThread(basePath, "Projectors/"+proj.getType().toString(), proj.getProjectorID().toString(), scenario);
			threads.put(proj.getProjectorID().toString(), t);
			logger.info("Added thread for "+proj.getProjectorID().toString());
			t.start();
		}		

		for(Translator trans : scenario.getTranslators()) {
			ExecutorThread t = new ExecutorThread(basePath, "Translators/"+trans.getType().toString(), trans.getTranslatorID().toString(), scenario); //todo: take from SCE
			threads.put(trans.getTranslatorID().toString(), t);
			logger.info("Added thread for "+trans.getTranslatorID().toString());
			t.start();
		}
		
		return threads;
	}
	

	public void printStatus() {
		logger.info("\nScenarioInstanceExector "+scenarioID+", done="+done+", there are "+threads.size()+" threads:");
		for(Entry<String, ExecutorThread> entry : threads.entrySet()) {
			logger.info(entry.getKey()+": running="+entry.getValue().isRunning()+", exitCode="+entry.getValue().getExitVal()+", pid="+entry.getValue().getPID());
		}
	}
	
	public long getAge() {
		return System.currentTimeMillis() - startTime;
	}
	
	public void kill() {
		for(Entry<String, ExecutorThread> entry : threads.entrySet()) {
			ExecutorThread et = entry.getValue();
			if (et == null)
				continue;	
			et.terminate();
		}

		done = true;
	}
}
