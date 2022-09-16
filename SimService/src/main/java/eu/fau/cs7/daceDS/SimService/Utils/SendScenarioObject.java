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
package eu.fau.cs7.daceDS.SimService.Utils;


import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.Micro;
import eu.fau.cs7.daceDS.datamodel.ResourceFile;
import eu.fau.cs7.daceDS.datamodel.Scenario;
import eu.fau.cs7.daceDS.datamodel.SubMicro;
import eu.fau.cs7.daceDS.datamodel.Translator;

import java.io.File;
import java.io.IOException;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.time.Duration;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.regex.Pattern;


import org.apache.log4j.Logger;

import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;
import org.apache.kafka.common.KafkaException;

public class SendScenarioObject
{
	static Scenario scenario;
	static Path resourcePath;
	static Path scepath;
	private static String scenarioID;
	static final int TIMEOUT = 1000;
	static boolean running = true;
	static boolean avroListener = false;
	static boolean resultsListener = false;
	static boolean jsonListener = false;
	static boolean fileReference = false;
	static int receiveCounter = 0;

	public static final String ANSI_RESET = "\u001B[0m";
	public static final String ANSI_BLACK = "\u001B[30m";
	public static final String ANSI_RED = "\u001B[31m";
	public static final String ANSI_GREEN = "\u001B[32m";
	public static final String ANSI_YELLOW = "\u001B[33m";
	public static final String ANSI_BLUE = "\u001B[34m";

	static Logger logger;
	private static long simStartInMS;
	private static long simDurationInMS;

	public static String getTopic(String topic) {
		return String.valueOf(scenario.getScenarioID())+"."+topic;
	}

	public static void main( String[] args)
	{		

		if(args.length < 1) {
			System.out.println("Usage: .jar ScenarioFile [-a|--AvroListener] [-j|--JSONListener] [-r|--resultsListener] [-f|--fileReference] ");
			return;
		}

		for (String arg : args){
			if (arg.equals("-a") || arg.equals("--AvroListener") ){
				avroListener = true;
			}
			if (arg.equals("-j") || arg.equals("--JSONListener") ){
				jsonListener = true;
			}
			if (arg.equals("-r") || arg.equals("--resultsListener") ){
				resultsListener = true;
			}
			if (arg.equals("-f") || arg.equals("--fileReference") ){
				fileReference = true;
			}
		}

		String r = String.valueOf((new Date()).getTime());
		System.setProperty(Config.LOG_FILE, "/tmp/SendScenarioObject_"+r+".txt");
		Config.readConfig();
		System.setProperty(Config.LOG_DIR, Config.get(Config.LOG_DIR));
		System.setProperty(Config.LOG_FILE, Config.get(Config.LOG_DIR)+"/SendScenarioObject_"+r+".txt");

		logger = Logger.getLogger(SendScenarioObject.class.getName());

		scepath = Paths.get(args[0]);
		resourcePath = scepath.getParent();			
		System.out.println("resourcepath is "+resourcePath);
//		scenario = RunSimFromFile.parseFile(scepath);
		scenario = ScenarioUtils.getDemoScenario(args[0]);
		if(scenario.getScenarioID().length()==0) {	
		    Date date = Calendar.getInstance().getTime();  
		    DateFormat dateFormat = new SimpleDateFormat("MMddhhmmss");  
		    String strDate = dateFormat.format(date);  
		    int lastPeriodPos = scepath.getFileName().toString().lastIndexOf('.');
            String fp = "";
		    if (lastPeriodPos > 0) {
            	fp = scepath.getFileName().toString().substring(0, lastPeriodPos);
            }
			scenario.setScenarioID( fp + strDate );
		}

		logger.info("ScenarioDescription parsed!");

		logger.info("json="+scenario.toString());

		
		
		/************************************************************/
		/* init kafka, send scenario object and wait for ack */
		/************************************************************/

		ProducerImplKafka<Scenario> kafkaWriter = new ProducerImplKafka<Scenario>(r+"sendScenarioObject");
		kafkaWriter.init();

		//		SendRadioMsg.init();
		logger.info("KafkaPublisher initialized.");
		kafkaWriter.publish(Config.get("channelOrchestration"),scenario,0);
		logger.info("ScenarioDescription sent to "+Config.get("channelOrchestration")+". Waiting for ack...");
		waitForAck(r+"_sendScenarioObject_ackconsumer");

		//		/* workaorund */
		//		ByteBuffer b = ByteBuffer.allocate(3);
		//		ResourceFile rf = new ResourceFile("asd","asd", b);
		//		kafkaWriter.publish(Config.getProvisionTopic(scenario.getScenarioID().toString(), "result"), "foo", rf);

		/************************************************************/
		/* Publish referenced resources */ //todo: should further communication be done, or is every resource already there? 
		/************************************************************/

		ProducerImplKafka<ResourceFile> kafkaResourceFileWriter = new ProducerImplKafka<ResourceFile>(r+"_kafkaResourceFileWriter");
//		kafkaResourceFileWriter.setExactlyOnce(true);
		kafkaResourceFileWriter.init();
		logger.info("Publishing resources...");
		String resourceTopic = Config.getProvisionTopic(scenarioID, Config.get("topicResource"));
//		/* road map */
//		try {
//			if(scenario.getRoadMap().toString().length()>0) {
//				logger.debug("scenario.getRoadMap().toString()='"+scenario.getRoadMap().toString()+"'");
//				ResourceFile resourceFile;
//				String ID = scenario.getRoadMap().toString(); //f.getName();
//				String type = "RoadMap";
//
//				if(!fileReference){
//					File f = new File(String.valueOf(parentpath+"/"+scenario.getRoadMap()));
//					byte[] mapresource;
//					mapresource = Files.readAllBytes(f.toPath());
//					ByteBuffer mapresourcebuffer = ByteBuffer.wrap(mapresource);
//					resourceFile = new ResourceFile(ID, type, mapresourcebuffer, null);
//				} 
//				logger.info("Sending roadmap to "+resourceTopic);
//				kafkaResourceFileWriter.publish(resourceTopic, resourceFile);
//			}
//		} catch (IOException e) {
//			e.printStackTrace();
//		}

//		/* input */
//		try {
//			for(Input i : scenario.getInputs()) {
//				String p = parentpath+"/"+i.getPath();
//				logger.debug("trying to send input: " + p);
//				File f = new File(String.valueOf(p));
//				byte[] inputresource = Files.readAllBytes(f.toPath());
//				ByteBuffer inputbuffer = ByteBuffer.wrap(inputresource);
//				ResourceFile resourceFileInput = new ResourceFile(f.getName(), "Input", inputbuffer, null);
//				kafkaResourceFileWriter.publish(resourceTopic, resourceFileInput);
//				logger.info("Published input: "+f.getName());
//			}
//		} catch (IOException e) {
//			e.printStackTrace();
//		}	

		/* tool specific resources resources */
		try {

			//Currently, the first resource gets dropped sometimes....
			ResourceFile dummy = new ResourceFile("dummy", "dummy", null, null);
			kafkaResourceFileWriter.publish(resourceTopic, dummy,0);
			logger.info("Published dummy resource ");

			for(BB t : scenario.getBuildingBlocks()) {
				// for(Map.Entry<String,String> kv : t.getResources().entrySet()) {
				for(Map.Entry<CharSequence,CharSequence> kv : t.getResources().entrySet()) {
					String type = kv.getValue().toString();
					String ID = kv.getKey().toString();
					ResourceFile resourceFileInput = null;

					if(ID.startsWith("s3://")){
//						String awsPath = "";
//							... customize ...
//						resourceFileInput = new ResourceFile(ID, type, null, awsPath);
					}
					else if(ID.startsWith("file://")) {
						String refPath = ID; //ID is already absolute path
						ID = Paths.get(ID).getFileName().toString(); //strip path
						resourceFileInput = new ResourceFile(ID, type, null, refPath);
					}
					else {
						File f = new File(String.valueOf(resourcePath+"/"+ID));
						byte[] inputresource = Files.readAllBytes(f.toPath());
						ByteBuffer inputbuffer = ByteBuffer.wrap(inputresource);
						resourceFileInput = new ResourceFile(f.getName(), type, inputbuffer, null);
					} 


					kafkaResourceFileWriter.publish(resourceTopic, resourceFileInput,0);
					logger.info("Published resource: "+ID +" as "+ type);
				}
			}
			//same for translator resources
			for(Translator t : scenario.getTranslators()) {
				// for(Map.Entry<String,String> kv : t.getResources().entrySet()) {
				for(Map.Entry<CharSequence,CharSequence> kv : t.getResources().entrySet()) {
					String type = kv.getValue().toString();
					String ID = kv.getKey().toString();
					ResourceFile resourceFileInput = null;

					if(!fileReference){
						File f = new File(String.valueOf(resourcePath+"/"+ID));
						byte[] inputresource = Files.readAllBytes(f.toPath());
						ByteBuffer inputbuffer = ByteBuffer.wrap(inputresource);
						resourceFileInput = new ResourceFile(f.getName(), type, inputbuffer, null);
					} 
					kafkaResourceFileWriter.publish(resourceTopic, resourceFileInput,0);
					logger.info("Published resource: "+ID +" as "+ type);
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		}		

		logger.info("Publishing resources... Done!");

		/************************************************************/
		/* Listen for feedback? */
		/************************************************************/
		Runnable statusListener = new Runnable() {	  
			public void run(){
				logger.info("String-Listener is triggered for status");
				// String topic = "orchestration\\.simulation\\."+scenarioID+"\\.(\\S)+";
				String topic = "orchestration\\.simulation\\."+scenarioID+"\\.status";
				listenForStringObservers(topic);

			}};	
			/* Listen for observer results */
			Runnable observerListener = new Runnable() {	  
				public void run(){
					if(avroListener){
						logger.info("Avro-Listener is triggered");
						String topic = "provision\\.simulation\\."+scenarioID+"\\.(\\S)+"; //.micro is legacy
						listenForObservers(topic);		
					}
				}};		

				Runnable stringObserverListener = new Runnable() {	  
					public void run(){
						if(jsonListener){
							logger.info("JSON-Listener is triggered");
							String topic = "provision\\.simulation\\."+scenarioID+"\\.(\\S)+";
							listenForStringObservers(topic);		
						}
					}};

					Runnable resultListener = new Runnable() {	  
						public void run(){
							if(resultsListener){
								String topic = "provision.simulation."+scenarioID+".results)";
								listenForResults(topic);		
							}
						}};

						Thread statusListenerThread = new Thread(statusListener);
						statusListenerThread.start(); 
						Thread observerListenerThread = new Thread(observerListener);
						observerListenerThread.start();  
						Thread stringObserverListenerThread = new Thread(stringObserverListener);
						stringObserverListenerThread.start(); 
						Thread resultListenerThread = new Thread(resultListener);
						resultListenerThread.start(); 

						try {
							observerListenerThread.join();
							stringObserverListenerThread.join();
							resultListenerThread.join();
						}
						catch (InterruptedException e) {
							e.printStackTrace();
						}

						if(!jsonListener && !avroListener && !resultsListener){
							logger.info("No listener was requested! Simulation should run -> I am done!");
						}
						try {
							statusListenerThread.join();
						} 
						catch (InterruptedException e) {
							e.printStackTrace();
						}
	}


	private static void listenForObservers(String topic) {
		Properties kafkaProps = Config.getProperties(scenarioID+"_listenForObservers", false);

		KafkaConsumer<String, org.apache.avro.specific.SpecificRecord> consumer = new KafkaConsumer<String, org.apache.avro.specific.SpecificRecord>(kafkaProps);    
		// KafkaConsumer<String, Micro> consumer = new KafkaConsumer<String, Micro>(kafkaProps);    

		//		consumer.subscribe(topics);
		System.out.println("listening to pattern="+topic);
		consumer.subscribe(Pattern.compile(topic));
		int msgCount = 0;
		String key = "";
		while(running) {
			try {
				ConsumerRecords<String, org.apache.avro.specific.SpecificRecord> msg = consumer.poll(Duration.ofMillis(TIMEOUT));
				// ConsumerRecords<String, Micro> msg = consumer.poll(Duration.ofMillis(TIMEOUT));
				msgCount = msg.count() ;
				//				System.out.print("returned from poll: msg.count="+msgCount);
				String timestr = "";
				for (ConsumerRecord<String, org.apache.avro.specific.SpecificRecord> record : msg) {
					// for (ConsumerRecord<String, Micro> record : msg) {
					key = record.key() ;

					
					try {
						long logicalTime = ScenarioUtils.bytesToLong(record.headers().lastHeader("time").value());
						timestr = "logical: "+logicalTime;
					} catch (Exception e1) {
						e1.printStackTrace();
						timestr = "msg: "+ record.timestamp() ;
					}

					if(record.value() == null) {
						logger.error("listenForObservers: "+"received null msg");
						continue;
					}	


					// System.out.println("Micro Subscriber got new message on "+ record.topic());
					receiveCounter++;	
					// System.out.println("Got " + receiveCounter + " in total");
					//continue;

					if(record.value() instanceof SubMicro) {

						System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr+ " | " + record.topic() + " | " + record.key() + " | "  );
						SubMicro m = (SubMicro) record.value();
						if(m!=null) {
							System.out.println(" veh="+m.getVehicleID()+", acc="+m.getAcceleration()+", roadID="+m.getRoad()+", laneID="+m.getLane()+", pos="+m.getPosition().getX()+"|"+m.getPosition().getY()+", route="+m.getRoute()+", speed="+m.getSpeed()+", vType="+m.getType());
						}
					}	
//					else if(record.value() instanceof SubMicroList) {
//						SubMicroList ml = (SubMicroList)record.value();
//
//						for(SubMicro m : ml.getContents()){
//							if(m!=null) {
//								System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
//
//								System.out.println(" veh="+m.getVehicleID()+", acc="+m.getAcceleration()+", roadID="+m.getRoad()+", laneID="+m.getLane()+", pos="+m.getPosition().getX()+"|"+m.getPosition().getY()+", route="+m.getRoute()+", speed="+m.getSpeed()+", vType="+m.getType());
//							}
//						}
//						if(ml.getContents().size() == 0){
//							//System.out.println(" (List is empty)"); 
//						}
//					}	
					else if(record.value() instanceof Micro) {

						System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
						Micro m = (Micro) record.value();
						if(m!=null) {
							System.out.println(" veh="+m.getVehicleID()+", acc="+m.getAcceleration()+", angle="+m.getAngle()+", edge="+m.getEdge()+", lane="+m.getLane()+", distToEdge="+m.getPositionEdge()+", pos="+m.getPosition().getX()+"|"+m.getPosition().getY()+", route="+m.getRoute()+", speed="+m.getSpeed()+", vType="+m.getType());
						}
					}	
//					else if(record.value() instanceof MicroList) {
//						MicroList ml = (MicroList)record.value();
//
//						for(Micro m : ml.getContents()){
//							if(m!=null) {
//								System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
//
//								System.out.println(" veh="+m.getVehicleID()+", acc="+m.getAcceleration()+", angle="+m.getAngle()+", edge="+m.getEdge()+", lane="+m.getLane()+", distToEdge="+m.getPositionEdge()+", pos="+m.getPosition().getX()+"|"+m.getPosition().getY()+", route="+m.getRoute()+", speed="+m.getSpeed()+", vType="+m.getType());
//							}
//						}
//						if(ml.getContents().size() == 0){
//							//System.out.println(" (List is empty)"); 
//						}
//					}	
//					else if(record.value() instanceof MesoList) {
//
//						MesoList ml = (MesoList)record.value();
//						for(Meso m : ml.getContents()){
//							if(m!=null) {
//
//								System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
//								System.out.println(" veh="+m.getVehicleID()+", edge="+m.getLink()+", route="+m.getRoute()+", Type="+m.getType());
//							}
//						}
//						if(ml.getContents().size() == 0){
//							//System.out.println(" (List is empty)"); 
//						}
//					}	
					else if(record.value() instanceof Meso) {
						Meso m = (Meso) record.value();
						if(m!=null) {
							System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
							System.out.println(" veh="+m.getVehicleID()+", edge="+m.getLink()+ ", route=" +m.getRoute());
						}
					} 
					else if(record.value() instanceof Macro) {
						Macro m = (Macro) record.value();
						if(m!=null) {
							System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
							System.out.println(" roadID="+m.getLinkID()+", flow="+m.getFlow()+ ", density=" +m.getDensity()+ ", speed=" +m.getSpeed() +", noTuProbs="+m.getTurningProbabilities().size()+", paths="+m.getPaths());
						}
					} 
//					else if(record.value() instanceof MacroList) {
//						MacroList ml = (MacroList) record.value();
//	
//						for(Macro m : ml.getContents()){
//							if(m!=null) {
//								System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
//								System.out.println(" roadID="+m.getLinkID()+", flow="+m.getFlow()+ ", density=" +m.getDensity()+ ", speed=" +m.getSpeed() +", noTuProbs="+m.getTurningProbabilities().size());
//			
//							}
//						}
//					} 
					else if(record.value() instanceof ResourceFile) {
						ResourceFile m = (ResourceFile) record.value();
						if(m!=null) {
							System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
							System.out.println(" ID="+m.getID()+", type="+m.getType()+ ", file="+(m.getFile()==null? "null" : +m.getFile().remaining() + " bytes")+ ", fileRef=" +m.getFileReference());
						}
					} 
					else if(record.value() instanceof Scenario) {
						Scenario m = (Scenario) record.value();
						if(m!=null) {
							System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
							System.out.println(" ScenarioID="+m.getScenarioID());
						}
					} 
					else {
						System.out.print(ANSI_GREEN+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic() + " | " + record.key() + " | "  );
						System.out.println(" (Don't know schema for deserialization) ");
					}
				}
				consumer.commitSync();
				Thread.sleep(1000);
			} catch (InterruptedException e1) {
				e1.printStackTrace();
			}catch (KafkaException e2) {
				logger.error(e2.getLocalizedMessage());
				logger.error(e2.getMessage());
				logger.error(e2.getStackTrace().toString());
				logger.error(e2.toString());
				logger.error("msg.count()");
				logger.error(msgCount);
				logger.error("last key" + key);
				//System.exit(1);

			}catch (Exception e1) {
				logger.error(e1.getLocalizedMessage());
			} finally {
				consumer.commitSync();
			}
		}

		consumer.close();
	}

	private static void listenForStringObservers(String topic) {
		Properties kafkaProps = Config.getProperties(scenarioID+"listenForStringObservers", false, true);
		KafkaConsumer<String, String> consumer = new KafkaConsumer<String, String>(kafkaProps);    

		System.out.println("listening to pattern="+topic);
		Pattern p= Pattern.compile(topic);
		consumer.subscribe(p);
		//		consumer.subscribe(topics);

		int sceFinishedCounter = 5;
		boolean sceAboutToBeFinished = false;
		String timestr = "";
		while(running) {
			try {
				ConsumerRecords<String, String> msg = consumer.poll(Duration.ofMillis(TIMEOUT));
				for (ConsumerRecord<String, String> record : msg) {	
					
					
					if(record.value() == null) {
						logger.error("received null msg");
						continue;
					}
					
					try {
						long logicalTime = ScenarioUtils.bytesToLong(record.headers().lastHeader("time").value());
						timestr = "logical: "+logicalTime;
					} catch (Exception e1) {
//						e1.printStackTrace();
						timestr = "msg: "+ record.timestamp() ;
					}
					
					// System.out.println("String subscriber got new message on "+ record.topic());
					receiveCounter++;	
					// System.out.println("Got " + receiveCounter + " in total");
					//continue;

					System.out.print(ANSI_BLUE+"["+String.format("%07d" , receiveCounter)+"] "+ANSI_RESET+timestr + " | " + record.topic());
					System.out.println(" | " + record.value().toString());

					if(record.value().toString().contains("finished")) {
						sceAboutToBeFinished = true;
						simDurationInMS = record.timestamp()-simStartInMS;
					}
					if(record.value().toString().contains("simulating")) {
						simStartInMS = record.timestamp();
					}

				}
				consumer.commitSync();
				
				if(sceAboutToBeFinished) {
					sceFinishedCounter--;
					if(sceFinishedCounter<1) {
						running=false;
						System.out.println("Found 'finished' status: assuming sce to be done and exiting.");
						System.out.println("The scenario took roughly "+ simDurationInMS/1000 + " seconds. Bye bye :-) ");
						System.out.println(scenarioID + " "+ simDurationInMS + "ms\n\n\n");
					}
				}
				Thread.sleep(1000);
			} catch (InterruptedException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}			catch (Exception e1) {
				logger.error(e1.getLocalizedMessage());
			}
		}
		consumer.close();		
	}

	private static void listenForResults(String topic) {
		Properties kafkaProps = Config.getProperties(scenarioID+"listenForResults", false);
		KafkaConsumer<String, ResourceFile> consumer = new KafkaConsumer<String, ResourceFile>(kafkaProps);    

		// System.out.println("listenig to topic pattern="+topic);
		// consumer.subscribe(Pattern.compile(topic));
		System.out.println("listening to topic="+topic);
		List<String> topics = new ArrayList<String>();
		topics.add(topic);
		consumer.subscribe(topics);

		while(running) {
			try {
				ConsumerRecords<String, ResourceFile> msg = consumer.poll(Duration.ofSeconds(10));
				for (ConsumerRecord<String, ResourceFile> record : msg) {	
					if(record.value() == null) {
						System.out.println("listenForResults: "+"received null msg");
						continue;
					}		
					String path = "results/result_"+scenarioID+".zip";
					ResourceFile rf = record.value();
					try{	
						FileOutputStream outputStream = new FileOutputStream(path);
						outputStream.write(rf.getFile().array());
						outputStream.close();
						System.out.println(record.timestamp() + " | " + record.topic() + " | saved to " + path);
					} catch (Exception e){
						System.out.println(record.timestamp() + " | " + record.topic() + " | failed saving to " + path);
						System.out.println(e.getLocalizedMessage());
					}
					//wait only for one result
					running = false;					 
				}
				consumer.commitSync();
				Thread.sleep(1000);
			} catch (InterruptedException e1) {
				// TODO Auto-generated catch block
				logger.error(e1.getLocalizedMessage());
			}
		}
		consumer.close();		
	}

	private static boolean waitForAck(String id) {
		Properties kafkaProps = Config.getProperties(id, false);
		KafkaConsumer<String, Scenario> consumer = new KafkaConsumer<String, Scenario>(kafkaProps);   

		String t = Config.get("topicScenario");
		String provsionTopic = Config.getProvisionTopic(scenario.getScenarioID().toString(), t);
		
		consumer.subscribe(Arrays.asList(provsionTopic));
		System.out.println("Wait for acceptance by checking ack in "+provsionTopic);
		boolean listening = true;
		while(listening) {
			try {
				ConsumerRecords<String, Scenario> msg = consumer.poll(Duration.ofMillis(TIMEOUT));

				for (ConsumerRecord<String, Scenario> record : msg) {	
					if(record.value() == null) {
						System.out.println("RunSimFromKafka: "+"received null scenario description");
						continue;
					}

					//					Scenario scenario = record.value();

					if(record.value() != null) {
						System.out.println("SendScenarioObject: Scenario accepted");
						scenarioID = record.value().getScenarioID().toString();
						consumer.close();
						return true;
					} else {
						System.out.println("SendScenarioObject: Scenario NOT accepted");
						consumer.close();
						return false;
					} 
				}

				consumer.commitSync();

				Thread.sleep(1000);
			} catch (InterruptedException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
		}

		consumer.close();
		return false;
	}
}
