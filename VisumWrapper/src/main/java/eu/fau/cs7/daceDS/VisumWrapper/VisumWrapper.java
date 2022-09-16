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

 package eu.fau.cs7.daceDS.VisumWrapper;

import java.io.BufferedWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.Observer;
import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.OrchestrationHandler;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;
import eu.fau.cs7.daceDS.Component.Simulator;
import eu.fau.cs7.daceDS.Component.TimeSync;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.log4j.Logger;

/**
 * The main wrapper class for Visum.
 * 
 * @author guetlein
 *
 */
public class VisumWrapper extends Simulator<Macro>{

	private VisumObserver observerImpl;
	private int stepLengthInMinutes;
	static Logger logger = Logger.getLogger("VisumWrapper");
	

	public VisumWrapper(String scenarioID, String instanceID, String provisionPrePattern, String provisionPostPattern,
			String demoScenario) {
		super(scenarioID, instanceID, provisionPrePattern, provisionPostPattern, demoScenario);
	}

	@Override
	protected void preInit() {
		System.out.println("preinit");

		
	}	
	
	public void init() {
		
			
		eu.fau.cs7.daceDS.Component.Config.readConfig();
		
		log("VisumWrapper is started!");
		
		String verPath ="";
		if(demoScenario.length() == 0) {
			orchestrationHandler.gatherScenarioData(); //returns wenn scenario ack is received and .ver file is received
			verPath = Config.getResourceDir(scenarioID, instanceID)+"/"+orchestrationHandler.getFilenameByType("Version");
			scenario = orchestrationHandler.getScenario();
			simulator = orchestrationHandler.getSimulator();
		} else {
			verPath ="E:\\Code\\git\\daceDS\\Scenario\\Demo\\LineLoad.ver"; //modify accordingly
			System.out.println("DEMO MODE!!! USING "+verPath+ " and " + demoScenario);
			scenario = ScenarioUtils.getDemoScenario(demoScenario);
			scenario.setScenarioID(scenarioID);
			instanceNumber = instanceID.equals("a")? 0 : 1;
			simulator = ScenarioUtils.getBBFromScenario(instanceID, scenario);
		}
		


		verPath = verPath.replace("\\/", "/");
		verPath = verPath.replace("/", "\\");
		System.out.println("verPath is "+verPath);

		VisumAPI.startVisum();
		
		System.out.println("Load Ver");
		VisumAPI.loadVer(verPath);

		//need visum running before domain helper, because accessing road map
		//todo: probably not necessary if domain reference is up-to-date

		domainHelper = new DomainHelperTraffic(null,simulator.getResponsibilities());

		super.init();
		
		

		
		//register observers
		observerImpl = new VisumObserver(scenarioID, simulator);
		observerImpl.init();
		for(Observer o : simulator.getObservers()) {
			provisionHandler.addObserver(o, observerImpl);
		}
		

		long startTimeInS = scenario.getSimulationStart();
		long startTimeInMS = scenario.getSimulationStart() * 1000;
		long endTimeInS = scenario.getSimulationEnd();
		long endTimeInMS = scenario.getSimulationEnd() * 1000;
		int stepLengthInS = simulator.getStepLength() / 1000;
		int stepLengthInMS = simulator.getStepLength();
		stepLengthInMinutes = (int) (stepLengthInS / 60);
		int noIntervals = (int) ((endTimeInS-startTimeInS)/stepLengthInS);
		
		


		
		//create procedureFile for the received SCE
		//customize windows paths
		try {
			Path defaultProcedurePath = Paths.get("E:/Code/git/daceDS/VisumWrapper/resource/default/Procedure_DUE.xml");
//			Path defaultProcedurePath = Paths.get("E:/Code/git/daceDS/VisumWrapper/resource/default/Procedure_SBA.xml");
//			Path defaultProcedurePath = Paths.get("E:/Code/git/daceDS/VisumWrapper/resource/Town01_extended_NSLoad_DUE_Proc.xml");
			Path modifiedProcedurePath = Paths.get("E:/Code/git/daceDS/VisumWrapper/resource/ModProcedure.xml");
			List<String> input =   Files.readAllLines(defaultProcedurePath);
			String modifiedConfig = "";
			Files.createDirectories(modifiedProcedurePath.getParent());
			BufferedWriter writer = Files.newBufferedWriter(modifiedProcedurePath, StandardCharsets.UTF_8);
			for(String line : input) {
				line = line.replaceAll("\\{startTime\\}", VisumAPI.getConfigTimeString(startTimeInMS));
				line = line.replaceAll("\\{endTime\\}", VisumAPI.getConfigTimeString(endTimeInMS));			
				line = line.replaceAll("\\{numberIntervals\\}", String.valueOf(noIntervals));//in case we have a DUE procedure
				line = line.replaceAll("\\{IntervalInMinutes\\}", String.valueOf(stepLengthInMinutes));//in case we have a SBA procedure
				
				writer.write( line + "\r\n" );
			}
			writer.close();

			System.out.println("modified default conf with intervalInMinutes="+String.valueOf(stepLengthInMinutes));
//			System.out.println("modified default conf with startTime="+getConfigTimeString(startTimeInMS)+", endTime="+getConfigTimeString(endTimeInMS)+", intervalInMinutes="+String.valueOf(stepLengthInMinutes));
	
			//for sba, we need to create intervalls on our own
			VisumAPI.openProcedure(modifiedProcedurePath.toString()); 
			VisumAPI.addIntervalls(startTimeInMS,endTimeInMS+stepLengthInMS,stepLengthInMS);


			log("initialized");
			VisumAPI.execProcedure(modifiedProcedurePath.toString()); 
			log("executed procedure");
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
	}
	
	

	public void stepEvent(int t) {
		System.out.println("simStepEvent: "+t);

	}


	protected void postLoopEvent() {
		// TODO Auto-generated method stub
		System.out.println("postSimLoopEvent");

		VisumAPI.close();
		
		System.exit(0);
		
	}


	@Override
	public void preLoopEvent() {
		// TODO Auto-generated method stub
		System.out.println("preSimLoopEvent");
		
		// we can not work directly on the visum map, therefore we will do these calculations in here
		// for meso, we well use the map in a translator for instance
		// infer a list of all resp links, i.e. where the toNode is in the list of responsibilities
		for(CharSequence nodeId : ((DomainHelperTraffic)domainHelper).getExternalResponsibilities()) {
			for(VisumLink link : VisumAPI.getInLinks(nodeId.toString())){
				((DomainHelperTraffic)domainHelper).getInternalResponsibilities().add(link.getEdgeID());
			}
			System.out.println();
		}

		
		((DomainHelperTraffic)domainHelper).getOutgoingLinks().clear();
		
		//iterate over all resp nodes
		for(CharSequence nodeId : ((DomainHelperTraffic)domainHelper).getExternalResponsibilities()) {
			//get all outgoing links for each node
			for(VisumLink link : VisumAPI.getOutLinks(nodeId.toString())){
				String toNode = String.valueOf(link.getToNode());
				//check if destination node is also in resp list. if not -> outgoing link
				if(!((DomainHelperTraffic)domainHelper).getExternalResponsibilities().contains(toNode)) {
					((DomainHelperTraffic)domainHelper).getOutgoingLinks().add(link.getEdgeID());
				}
			}
		}
		
		System.out.println("i have the following outgoing links: "+ ((DomainHelperTraffic)domainHelper).getOutgoingLinks());
		System.out.println("i have the following external resps: "+ ((DomainHelperTraffic)domainHelper).getExternalResponsibilities());
		System.out.println("i have the following internal resps: "+ ((DomainHelperTraffic)domainHelper).getInternalResponsibilities());
		
	}


	@Override
	protected void preStepEvent(int t) {
		System.out.println("preSimStepEvent");
		
	}


	@Override
	protected void processStepEvent(int t) {
		System.out.println("processSimStepEvent");		
		String timeStr = VisumAPI.getTimeString(TimeSync.getLocaltime()) ;
		
		//this is done here, because directly related to the simulation logic!
		for(String respLinkStr : ((DomainHelperTraffic)domainHelper).getOutgoingLinks()) {
			VisumLink respLink = new VisumLink(respLinkStr);
			Macro macro = VisumAPI.getMacro(respLinkStr,timeStr, stepLengthInMinutes);
			log("Time: "+timeStr+", road="+macro.getLinkID()+", length="+VisumAPI.getLength(respLink));
			log("      due:   flow="+macro.getFlow()+", speed="+macro.getSpeed()+", density="+macro.getDensity());		

			String linkID = Config.escapeSpecialChars(respLinkStr);
			String topic = Config.getProvisionTopic(scenarioID, simulator, "link."+linkID); //todo: retrieve automatically
			log("publishing to "+topic);
			provisionHandler.sendNDM(topic, macro, t);
		
		}	
		
		System.out.println("processSimStepEvent is done");
			
	}


	@Override
	protected void postStepEvent(int t) {		
		System.out.println("postSimStepEvent");
		provisionHandler.runObservers();
	}
	
	@Override
	protected void processMessage(ConsumerRecord r) {
		log("todo: implement processMessage");		
	}

	@Override
	protected void processInteraction(ConsumerRecord r) {
		log("todo: implement process interaction");		
	}

}