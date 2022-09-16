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

import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.time.Duration;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.Properties;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import org.apache.kafka.clients.consumer.Consumer;
import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;
import org.apache.kafka.common.KafkaException;
import org.apache.kafka.common.errors.SerializationException;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Kafka.ConsumerImplKafka;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.datamodel.Micro;
import eu.fau.cs7.daceDS.datamodel.ResourceFile;
import eu.fau.cs7.daceDS.datamodel.Scenario;


/**
 * The simulation service main loop. Listens for new requests and triggers the
 * execution of a simulation scenario by calling the ScenarioManager.
 * 
 * @author guetlein
 *
 */
public class SimulationService
{
	private static Consumer<String,Scenario> scenarioConsumer;
	private static ProducerImplKafka<String> kAProducer;
	private static ConsumerImplKafka<String> kAConsumer;
	private static AtomicBoolean MAINLOOP_STOPPED = new AtomicBoolean(false);
	private static int keepAliveCounter = 0;
	private static int noRunning = 0;
	private static int noTerm = 0;
	private static KeepAlive keepAlive;
	private static boolean exitOnFailure =true;
	private static String statusTopic = "orchestration.simulation.status";

	
	static Logger logger = Logger.getLogger(SimulationService.class.getName());
	private static ProducerImplKafka<String> stringWriter;

	public static void main( String[] args)
	{
		logger.info("");
		logger.info("");
		logger.info("");		
		logger.info("SimulationService started");	
		logger.info("");
		logger.info("");
		logger.info("Connecting to "+ Config.get(Config.KAFKA_BROKER));
		logger.info("Connecting... kafka="+Config.get(Config.KAFKA_BROKER)+", registry="+Config.get(Config.SCHEMA_REGISTRY));

		String timestamp = Long.toString((new Date()).getTime());
		Properties sceProperties =  Config.getProperties("sce"+timestamp, true);
		sceProperties.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "false");
		scenarioConsumer = new KafkaConsumer<String, Scenario>(sceProperties);  

		keepAlive = new KeepAlive(timestamp); 
		keepAlive.init();
		
		stringWriter = new ProducerImplKafka<String>("SimService");  
		stringWriter.initPlainString();


		// Properties ctrlProperties = Config.getProperties("ctrl"+timestamp, true);
		// ctrlConsumer = new KafkaConsumer<String, CtrlMsg>(ctrlProperties);  
		
		// Properties statusProperties =  Config.getProperties("status"+timestamp, true);
		// statusConsumer = new KafkaConsumer<String, StatusMsg>(statusProperties);    

		//check for connectivity, todo: current one does not show all failures
		try {
			int size = scenarioConsumer.listTopics().keySet().size();
			logger.info("Online! " + size + " topics are listed by broker");
		} catch(Exception e) {
			logger.info("Failed to connect to broker, exiting...");
			System.exit(-1);
		}
		publishStatus("SimulationService (re)started");
		
		publishDefinitions();
		publishResources();
		
//		publishDemoData();

		Thread l = listenForRequests();
		//listenCtrl();
		//listenStatus();
		try{
			l.join();
		} catch (InterruptedException e) {
		}
		
		stringWriter.close();
		
		logger.info("SimulationService stopped");	
		publishStatus("SimulationService stopped");
		System.exit(0); //todo: close all threads, otherwise no return from main()
	}

//	private static void publishDemoData() {
//		ProducerImplKafka p = new ProducerImplKafka<m>("SimService");  
//		p.init();
//		Micro m = new Micro();
//		p.publish("demo.micro", m);
//	}

	public static void publishStatus(String msg) {
		String timestr = (new Date()).toString();
		stringWriter.publish(statusTopic,timestr+" | "+msg, 0);
	}
	
	public static Thread listenForRequests() {
		Runnable r = new Runnable() {


			public void run(){
				logger.info("Starting to listen on "+Config.get(Config.CHANNEL_ORCHESTRATION));
				scenarioConsumer.subscribe(Arrays.asList(Config.get(Config.CHANNEL_ORCHESTRATION)));
				
					while (!MAINLOOP_STOPPED.get()) {
						try {
							ConsumerRecords<String, Scenario> rawmsg = scenarioConsumer.poll(Duration.ofMillis(5000));

							synchronized(Config.get(Config.CHANNEL_ORCHESTRATION)) {

								for (ConsumerRecord<String, Scenario> record : rawmsg) {	
									if(record.value() == null) {
										logger.debug("Received null message");
										continue;
									}
									
									Scenario scenario = record.value();
									
									if(scenario == null) {
										logger.debug("Received null scenario");
										continue;
									}
									logger.info("Received new request: " + scenario.toString());
									int errorAdding = ScenarioManager.addScenarioRun(scenario, record.timestamp());

									if(errorAdding == 0) {
										publishStatus("Successfully added "+scenario.getScenarioID()+" to pool!");										
									}

									if (errorAdding == 1) {
										publishStatus("Request of scenario "+scenario.getScenarioID()+" is too old, ignoring!");										
									}
									if (errorAdding == 2) {
										publishStatus("Requested scenario id "+scenario.getScenarioID()+" is already known, ignoring!");										
									}
									
									if(exitOnFailure && errorAdding > 0) {
										publishStatus("Failed to add "+scenario.getScenarioID()+" to pool!");
										MAINLOOP_STOPPED.set(true);
									}
								}
							}
						} catch (SerializationException e){
							logger.error("Error when deserializing: " +e.getMessage());
							publishStatus("Error when deserializing: " +e.getMessage());						
							if(exitOnFailure) {
								MAINLOOP_STOPPED.set(true);
							}
						} catch(KafkaException e){
							logger.error("KafkaException: " +e.getMessage());
							publishStatus("KafkaException " +e.getMessage());							
							if(exitOnFailure) {
								MAINLOOP_STOPPED.set(true);
							}
						}catch (Exception e) {
							logger.error("Error when polling. " +e.getMessage());
							if(exitOnFailure) {
								MAINLOOP_STOPPED.set(true);
							}
						}finally {
							try{
								scenarioConsumer.commitSync();
							} catch (Exception e) {
								logger.error("Error when commiting. " + e.getMessage());
								publishStatus("Error when commiting. " +e.getMessage());	
								if(exitOnFailure) {
									MAINLOOP_STOPPED.set(true);
								}
							}
						}
					int killed = ScenarioManager.monitorRunningScenarios();
					if(killed > 0){
						publishStatus(killed + " scenario was killed for running longer than maxSimTime.");
						logger.info(killed + " scenario was killed for running longer than maxSimTime.");
					}

					monitorChanges();
					boolean succKA = keepAlive.keepAlive();
					if(exitOnFailure && !succKA) {
						MAINLOOP_STOPPED.set(true);
					}
				}

				logger.info("Closing listener on "+Config.get(Config.CHANNEL_ORCHESTRATION));
				scenarioConsumer.close();
				logger.info("Returning");
				return;
			}

		};   

		Thread thread = new Thread(r);
		thread.start();  
		return thread;
	}

	public static void monitorChanges() {
		
		ArrayList<String> ended = ScenarioManager.cleanTerminatedScenarios();
		for(String scen : ended) {
			logger.info(scen + " was ended ");
			publishStatus(scen + " was terminated.");
			//getResults(scen);
		}


		if(ScenarioManager.getNoRunningScenarios() != noRunning || ScenarioManager.getNoTerminatedScenarios() != noTerm){
			noRunning = ScenarioManager.getNoRunningScenarios();
			noTerm = ScenarioManager.getNoTerminatedScenarios();

			logger.info(noRunning + " running scenarios, " + noTerm + " terminated scenarios");
		}
		//todo: corrupted threads? notifiy about misery
	}
	
	
	public static String concatAllFilesInDir(String dirs) {
		
		String ddir = Config.get(Config.DIR_DEFINITION);
		File dir = new File(ddir+dirs);
		
		String config = "";
		try {
			boolean first = true;
			for (final File fileEntry : dir.listFiles()) {
		        if (fileEntry.isFile()) {
		           String content = Files.readString(Path.of(fileEntry.getPath()));
		           if(!first) {config+=";\n\n";} else {first=false;}
		           config+=content;
			   }
		    }
		 } catch (IOException e) {
				System.out.println("Failed to read definition files for "+dirs);
				e.printStackTrace();
			}
		return config;
	}
	

	
	public static void publishDefinitions() { 
		//all in one message for the definitions
		stringWriter.publish(Config.get(Config.CHANNEL_ORCHESTRATION)+".definitions.domains", concatAllFilesInDir("/domains"), 0);
		stringWriter.publish(Config.get(Config.CHANNEL_ORCHESTRATION)+".definitions.layers", concatAllFilesInDir("/layers"), 0);
		stringWriter.publish(Config.get(Config.CHANNEL_ORCHESTRATION)+".definitions.components", concatAllFilesInDir("/components"), 0);
		stringWriter.publish(Config.get(Config.CHANNEL_ORCHESTRATION)+".definitions.connectors", concatAllFilesInDir("/connectors"), 0);
	}

	
	public static void publishResources() { 
		
		ProducerImplKafka<ResourceFile> kafkaResourceFileWriter = new ProducerImplKafka<ResourceFile>("kafkaResourceFileWriter");
		kafkaResourceFileWriter.init();
		
		//single msg for  each resource
		String ddir = Config.get(Config.DIR_DEFINITION);
		File dir = new File(ddir+"/resources");
		
		String config = "";
		try {
			boolean first = true;
			for (final File fileEntry : dir.listFiles()) {
		        if (fileEntry.isFile()) {
		           ResourceFile rf = new ResourceFile();
		           rf.setID(fileEntry.getName());
				   ByteBuffer buffer = ByteBuffer.wrap(Files.readAllBytes(fileEntry.toPath()));
		           rf.setFile(buffer);
		           rf.setType("");
		           kafkaResourceFileWriter.publish(Config.get(Config.CHANNEL_ORCHESTRATION)+".definitions.resources", rf, 0);
			   }
		    }
		 } catch (IOException e) {
				System.out.println("Failed to read resource files");
				e.printStackTrace();
			}		
	}
}
