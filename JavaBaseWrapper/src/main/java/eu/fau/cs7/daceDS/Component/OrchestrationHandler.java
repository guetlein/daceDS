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

import eu.fau.cs7.daceDS.Kafka.ConsumerImplKafka;
import eu.fau.cs7.daceDS.Kafka.ConsumerCallbackImplKafka;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.CtrlMsg;
import eu.fau.cs7.daceDS.datamodel.ResourceFile;
import eu.fau.cs7.daceDS.datamodel.Scenario;
import eu.fau.cs7.daceDS.datamodel.Translator;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

/**
 * The OrchestrationHandler is taking care of all messages that are related to the orchestration channel, besides logging and synchronization.
 * That involves mainly resources, results, and controls.
 * 
 * @author guetlein
 *
 */
public class OrchestrationHandler implements ConsumerCallbackImplKafka  {

	private static final int MAX_ERROR_COUNT = 10000;
	public Scenario scenarioDescription = null;
	private String scenarioID;
	private String simulatorID;
	private Map<ResourceFile,Boolean> bufferedResources = new HashMap<ResourceFile,Boolean>();
	private Map<String,ResourceFile> resources = new HashMap<String,ResourceFile>();
	public BB bbDescription = null;
	private Translator tDescription = null;
	private ConsumerImplKafka<CtrlMsg> ctrlReader;
	
	
	public OrchestrationHandler(String scenarioID, String simulatorID) {
		this.scenarioID = scenarioID;
		this.simulatorID = simulatorID;
	}

	/*
	 * Retrieve SCE and all resources
	 */
	public void gatherScenarioData() {
		
		//First, we wait and consume the scenario definition
		String id = scenarioID+"_"+simulatorID+"_ScenarioReader";
		ConsumerImplKafka<Scenario> scenarioReader = new ConsumerImplKafka<Scenario>(id);
		String topicACK = Config.get("channelProvision")+"."+scenarioID+"."+Config.get("topicScenario");
		String topicResource = Config.get("channelProvision")+"."+scenarioID+"."+Config.get("topicResource");
		scenarioReader.init(Arrays.asList(topicACK), (ConsumerCallback)this);
		
		System.out.println("Polling for sce on "+topicACK);
		int error_count = 0;
		while(scenarioDescription == null) {
			scenarioReader.poll();
			error_count++;
			if(error_count>MAX_ERROR_COUNT) {
				scenarioReader.close();
				System.out.println("Did not receive .sce after "+MAX_ERROR_COUNT+" tries, terminating");
				System.exit(0);
			}
		}
		System.out.println("Received sce");
		scenarioReader.close();

		//Extract interesting infos from SCE
		for(BB t : scenarioDescription.getBuildingBlocks()) {
			if(t.getInstanceID().toString().equals(simulatorID)) {
				bbDescription = t;
				break;
			}else {
				System.out.println(simulatorID+"!="+t.getInstanceID());
			}
		}
		for(eu.fau.cs7.daceDS.datamodel.Translator t : scenarioDescription.getTranslators()) {
			if(t.getTranslatorID().toString().equals(simulatorID)) {
				tDescription = t;
				break;
			}else {
				System.out.println(simulatorID+"!="+t.getTranslatorID());
			}
		}
		
		if(bbDescription == null && tDescription == null) {
			System.out.println("PROBLEM, didnt find my instance("+simulatorID+") in scenario description");
			for(BB t : scenarioDescription.getBuildingBlocks()) {
				System.out.println("'"+t.getInstanceID()+"'");
			}
			for(eu.fau.cs7.daceDS.datamodel.Translator t : scenarioDescription.getTranslators()) {
				System.out.println("'"+t.getTranslatorID()+"'");
			}
			System.exit(1);
		}
				
		//remember relevant resource
		//domain references
//		for(Entry<CharSequence, CharSequence> domainReference : scenarioDescription.getDomainReferences().entrySet()) {
//			if(domainReference.getKey().toString().equals("traffic")) {
//				bufferedResources.put(new ResourceFile(domainReference.getValue().toString(), "DomainReference", null,null), false);
//			}
//		}
		//bb resources
		if(bbDescription!=null) {
			for(Entry<CharSequence,CharSequence> kv : bbDescription.getResources().entrySet()) {
				CharSequence resourceName = kv.getKey();
				//if we have a file reference abs path, we strip id to the filename
				if(resourceName.toString().contains(":")) {
					resourceName = Paths.get(resourceName.toString()).getFileName().toString(); //strip path
				}
				CharSequence resourceType = kv.getValue();
				bufferedResources.put(new ResourceFile(resourceName, resourceType, null,null), false);
			}
		}
		//translator resources
		if(tDescription!=null) {
			for(Entry<CharSequence,CharSequence> kv : tDescription.getResources().entrySet()) {
				CharSequence resourceName = kv.getKey();				
				//if we have a file reference abs path, we strip id to the filename
				if(resourceName.toString().contains(":")) {
					resourceName = Paths.get(resourceName.toString()).getFileName().toString(); //strip path
				}
				CharSequence resourceType = kv.getValue();
				bufferedResources.put(new ResourceFile(resourceName, resourceType, null,null), false);
			}
		}
			
		
		
		//get the resources
		if(bufferedResources.size()>0) {
		
			//now, poll for the actual resources
			ConsumerImplKafka<ResourceFile> resourceReader = new ConsumerImplKafka<ResourceFile>(id);
			resourceReader.init(Arrays.asList(topicResource), (ConsumerCallback)this);
			
			//wait for resources
			System.out.print("Will wait for: ");
			for(ResourceFile resource : getBufferedResources().keySet()) {
				System.out.print(resource.getID()+" ("+resource.getType()+"); ");
			}
			System.out.println();
			
			
			for(ResourceFile resource : getBufferedResources().keySet()) {
				String type =  resource.getType().toString();
				String filename =  resource.getID().toString();
				
				System.out.println("waiting for "+filename);
				while(!received(filename)) { //todo: this implies that filenames are unique
					resourceReader.poll(100);
				}
				System.out.println("received "+filename);
			}
	
			System.out.println("got all resources!");
			resourceReader.close();
		}
		
		return;
	}

	private boolean received(String filename) {
		for(Entry<ResourceFile, Boolean> s : getBufferedResources().entrySet()) {
			ResourceFile rf = s.getKey();
			boolean received = s.getValue();
			if(rf.getID().toString().equals(filename))
				return received;			
		}
		return false;
	}
	
	private boolean assigned(String filename) {		
		for(Entry<ResourceFile, Boolean> s : getBufferedResources().entrySet()) {
			ResourceFile rf = s.getKey();
			if(rf.getID().toString().equals(filename))
				return true;			
		}
		return false;
	}
	
	private void update(ResourceFile f, boolean b) {
		for(Entry<ResourceFile, Boolean> s : getBufferedResources().entrySet()) {
			ResourceFile rf = s.getKey();
			if(rf.getID().toString().equals(f.getID().toString())) {
				getBufferedResources().put(rf, true);	
				return;
			}
		}
		getBufferedResources().put(f, true);	//add new
	}
	
	public String getFilenameByType(String type) {
		for(Entry<ResourceFile, Boolean> s : getBufferedResources().entrySet()) {
			ResourceFile rf = s.getKey();
			boolean received = s.getValue();
			
			if(rf.getType().toString().equals(type)){
				System.out.println("getFileByType " + type + "found, received=" + received);
				if (received){
					System.out.println("       path is " +rf.getID().toString());
					return rf.getID().toString();
				}	
			}	
		}
		return "";
	}
	

	public String getAbsolutePathByType(String type) {
		String file = getFilenameByType(type);
		if(file.length()==0) {
			return "";
		}
		return Config.getResourceDir(scenarioID, simulatorID)+"/"+ file;
	}


	public Scenario getScenario() {
		return scenarioDescription;
	}

	public BB getSimulator() {
		return bbDescription;
	}

	public Translator getTranslator() {
		return tDescription;
	}

	public <T> void receive(ConsumerRecord r, long time, int epoch) {
		String topic = r.topic();
		T o = (T)r.value();

		if(o.getClass().equals(CtrlMsg.class)) {
			System.out.println("received ctrlmsg");
			CtrlMsg m = (CtrlMsg)o;
			if(m.getCommand().toString().equals("kill")) {
				System.exit(1);
			}
		} 
		else if(o.getClass().equals(Scenario.class)) {
			System.out.println("received scenario");
			scenarioDescription = (Scenario)o;
			System.out.println(scenarioDescription.getScenarioID());
			for(BB t : scenarioDescription.getBuildingBlocks()) {
				System.out.println(t.getInstanceID());
				System.out.println(t.getLayer());
			}
		} 
		else if(o.getClass().equals(ResourceFile.class)) {
				System.out.println("receive(): received ResourceFile");
				ResourceFile f = (ResourceFile)o;
				String filename = f.getID().toString();
				System.out.println("            filename:"+filename);
				
				//for me?
				if(!assigned(filename)) {
					System.out.println(filename + " is not for me");
					return;
				}

				//file or reference?
				if(f.getFile() == null && f.getFileReference() != null){		
					String reference = f.getFileReference().toString();
					Path dest = Paths.get(Config.getResourceDir(scenarioID, simulatorID)+"/"+f.getID());
					
					if(reference.startsWith("s3://")) {
						String callString = "./retrieveFileFromDB.sh " + reference + " " + dest.toString();
						try {
							Path parentDir = dest.getParent();
							if (!Files.exists(parentDir))
								Files.createDirectories(parentDir);
	
							Process proc = null;
							Runtime rt = null;
							int exitVal = 1;
							try {
								rt = Runtime.getRuntime();
								proc = rt.exec(callString);
								exitVal = proc.waitFor();
								System.out.println("retrievecall: Exit value: " + exitVal);
							} catch (IOException | InterruptedException e) {
								if(proc != null) proc.destroyForcibly();
								System.out.println(callString + " --> Process exception: " + e.getLocalizedMessage());
							}
							if(exitVal==0){
								System.out.println("retrieved a " + f.getType().toString()+ " from external storage to " + dest.toString());
								update(f,true); //helper methods necessary, implementing hashcode and equals not possible due to avro
							} else {
								System.out.println(callString + " --> exitVal: " + exitVal);
							}
						} catch (IOException e) {
							e.printStackTrace();
						}
					}		
					else if(reference.startsWith("file://")) {
						
						String callString = "";
						//todo: use your fs
						if(System.getProperty("os.name").toLowerCase().contains("windows")){
							callString = "cmd /c start \"\" retrieveFileFromDisk.bat  " + reference.replace("file:///home/guetlein", "E:").replace("/", "\\") + " " + dest.toString();
						}
						else{
							callString = "./retrieveFileFromDisk.sh " + reference + " " + dest.toString();
						}
						
						try {
							Path parentDir = dest.getParent();
							if (!Files.exists(parentDir))
								Files.createDirectories(parentDir);
	
							Process proc = null;
							Runtime rt = null;
							int exitVal = 1;
							try {
								rt = Runtime.getRuntime();
								proc = rt.exec(callString);
								exitVal = proc.waitFor();
								System.out.println("retrievecall: Exit value: " + exitVal);
							} catch (IOException | InterruptedException e) {
								if(proc != null) proc.destroyForcibly();
								System.out.println(callString + " --> Process exception: " + e.getLocalizedMessage());
							}
							if(exitVal==0){
								System.out.println("retrieved a " + f.getType().toString()+ " from internal disk to " + dest.toString());
								update(f,true); //helper methods necessary, implementing hashcode and equals not possible due to avro
							} else {
								System.out.println(callString + " --> exitVal: " + exitVal);
							}
						} catch (IOException e) {
							e.printStackTrace();
						}
					}

				} else if(f.getFile() != null ){
					ByteBuffer bytes = f.getFile();
					Path filePath = Paths.get(Config.getResourceDir(scenarioID, simulatorID)+"/"+filename);
					try {
						Path parentDir = filePath.getParent();
						if (!Files.exists(parentDir))
							Files.createDirectories(parentDir);
						Files.write(filePath,bytes.array());
						System.out.println("saved a " + f.getType().toString()+ " file to " + filePath.toString());
						update(f,true); //helper methods necessary, implementing hashcode and equals not possible due to avro
					} catch (IOException e) {
						e.printStackTrace();
					}
				} else {
					System.out.println("ResourceFile is malformed");
					return;
				}
			} else {
			System.out.println("received: " +o.getClass());
		}
	}


	public Map<ResourceFile,Boolean> getBufferedResources() {
		return bufferedResources;
	}

	public void listenForCtrlMsgs() {
		String id = scenarioID+"_"+simulatorID+"_CtrlMsgReader";
		ctrlReader = new ConsumerImplKafka<CtrlMsg>(id);
		String topic = Config.get("channelOrchestration")+"."+scenarioID+".ctrl";
		ctrlReader.init(Arrays.asList(topic), (ConsumerCallback)this, false, true, false);
		
		System.out.println("Polling for ctrls on "+topic);
	}
	
	public void close() {
		if(ctrlReader != null) {
			ctrlReader.close();
		}
	}

}		