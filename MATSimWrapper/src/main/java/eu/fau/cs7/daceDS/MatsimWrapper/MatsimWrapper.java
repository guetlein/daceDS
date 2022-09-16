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

 package eu.fau.cs7.daceDS.MatsimWrapper;

import java.nio.file.Files;
import java.nio.file.Path;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.matsim.api.core.v01.Id;
import org.matsim.api.core.v01.events.LinkEnterEvent;
import org.matsim.api.core.v01.events.handler.LinkEnterEventHandler;
import org.matsim.contrib.otfvis.OTFVisLiveModule;
import org.matsim.core.config.ConfigUtils;
import org.matsim.core.controler.AbstractModule;
import org.matsim.core.controler.Controler;
import org.matsim.core.controler.OutputDirectoryHierarchy.OverwriteFileSetting;
import org.matsim.core.controler.events.ShutdownEvent;
import org.matsim.core.controler.listener.ControlerListener;
import org.matsim.core.controler.listener.ShutdownListener;
import org.matsim.core.events.handler.EventHandler;
import org.matsim.core.mobsim.framework.events.MobsimAfterSimStepEvent;
import org.matsim.core.mobsim.framework.events.MobsimBeforeSimStepEvent;
import org.matsim.core.mobsim.framework.events.MobsimInitializedEvent;
import org.matsim.core.mobsim.framework.listeners.MobsimAfterSimStepListener;
import org.matsim.core.mobsim.framework.listeners.MobsimBeforeSimStepListener;
import org.matsim.core.mobsim.framework.listeners.MobsimInitializedListener;
import org.matsim.core.mobsim.framework.listeners.MobsimListener;
import org.matsim.core.mobsim.qsim.ActivityEndRescheduler;
import org.matsim.core.mobsim.qsim.QSim;
import org.matsim.core.mobsim.qsim.interfaces.MobsimVehicle;
import org.matsim.vehicles.Vehicle;
import org.matsim.vis.otfvis.OTFVisConfigGroup;
import org.matsim.withinday.controller.ExampleWithinDayController;
import org.matsim.withinday.controller.WithinDayModule;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.OrchestrationHandler;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;
import eu.fau.cs7.daceDS.Component.Simulator;
import eu.fau.cs7.daceDS.Component.TimeSync;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.Observer;
import eu.fau.cs7.daceDS.datamodel.Scenario;


import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;

/*
 * 
 * Base class for wrapping MATSim. As MATSim is self orchestrated we will not use 
 * the Simulator.run() method, but build our sim loop with MATSim events.
 * 
 */

public class MatsimWrapper extends Simulator<Meso> implements MobsimBeforeSimStepListener, ShutdownListener, MobsimAfterSimStepListener, MobsimInitializedListener, LinkEnterEventHandler{



	private Controler matsimController;
	private QSim qsim = null;
	private HashMap<String,ArrayList<Meso>> transferBuffer = new HashMap<String, ArrayList<Meso>>();
	private MatsimObserver observerImpl;
	private MobsimListener m;
	private EventHandler e;
	private ControlerListener c;
	private MatsimAPI api;
	private EventHandler apihandler;


	public MatsimWrapper(String scenarioID, String instanceID, String provisionPrePattern, String provisionPostPattern,
			String demoScenario) {
		super(scenarioID, instanceID, provisionPrePattern, provisionPostPattern, demoScenario);
		// TODO Auto-generated constructor stub
	}

	private static final String RESULTS_FILE = "results.zip";
	

	public static void main(String[] args) {

		if(args.length < 2) {
			System.out.println("Usage exe scenarioID simulatorID");
			return;
		}
		String scenarioID = args[0];
		String instanceID = args[1];		
		String demoScenario = "";
		String provisionPrePattern = "link\\."; 
		String provisionPostPattern = "\\.vehicles";
		
		if(args.length == 3) {
			demoScenario = args[2];
		}
		
		
		MatsimWrapper matsim = new MatsimWrapper(scenarioID, instanceID, provisionPrePattern, provisionPostPattern, demoScenario);
		matsim.log("running preperations! \n\n\n\n\n");
		matsim.loadConfig();
		matsim.preInit();

		matsim.log("running matsim! \n\n\n\n\n");
		matsim.runMatsim();		

		matsim.log("Sim is over, returning results!");

		
		matsim.close();
		
	}

	public void runMatsim() {
		matsimController.run();		
	}
	
	public void preInit() {
		Config.readConfig();
		System.setProperty("jogl.disable.openglcore", "true");

		System.out.println("MatsimWrapper is started!");
		
		String configPath = "";
		if(demoScenario.length() == 0) {
			orchestrationHandler.gatherScenarioData(); //returns wenn scenario ack is received and .ver file is received
			scenario = orchestrationHandler.getScenario();
			simulator = orchestrationHandler.getSimulator();
			configPath = MatsimAPI.createDirAndConfig(scenario, simulator, orchestrationHandler);
		} else {
			configPath = "/home/guetlein/Code/git/daceDS/Scenario/Demo/Line_Matsim/matsim.cfg";
//			configPath = "/tmp/SimulationService/1649847374863/matsim0/resource/matsim.matsimcfg";
			System.out.println("DEMO MODE!!! USING " + configPath + " and " + demoScenario);
			scenario = ScenarioUtils.getDemoScenario(demoScenario);
			scenario.setScenarioID(scenarioID);
			simulator = ScenarioUtils.getBBFromScenario(instanceID, scenario);
		}

		log("scenario="+scenario);
		log("simulator="+simulator);
		log("configPath="+configPath);

		org.matsim.core.config.Config config = ConfigUtils.loadConfig( configPath, new OTFVisConfigGroup() );

		config.controler().setOverwriteFileSetting(OverwriteFileSetting.deleteDirectoryIfExists);
		config.controler().setOutputDirectory(Config.getOutputDir(scenarioID,instanceID));
		
		Logger.getRootLogger().setLevel(Level.WARN);

//		final Logger logger = LogManager.getLogger();
		

		//customized paramters?
		String[] cpams = simulator.getParameters().toString().split(" ");
		for (String cpam : cpams) {
			String[] v = cpam.split("=");
			if(v.length==2 && v[0].equals("iterations")) {
				try {
					int iters = Integer.valueOf(v[1]);
					config.controler().setLastIteration(iters);
				}catch(Exception e) {
					System.out.println(v[1] + " is no number");
				}
			}
		}
		
		matsimController = new Controler(config);	
		m = ((MobsimListener) this);
		c = ((ControlerListener) this);
		e = ((EventHandler) this);
		
		api = new MatsimAPI();
		apihandler = ((EventHandler) api);
		matsimController.addOverridingModule(new AbstractModule() {
		public void install() {
				addMobsimListenerBinding().toInstance(m) ;
				addControlerListenerBinding().toInstance(c);
				addEventHandlerBinding().toInstance(e);
				addEventHandlerBinding().toInstance(apihandler);

//				install(new WithinDayModule());
//				addControlerListenerBinding().to(MatsimWrapper.class);
			}
		});		


	}


	@Override
	public void notifyMobsimInitialized(MobsimInitializedEvent e) {
		
//		log("\n\n\n notifyMobsimInitialized \n\n\n");

		//gets called twice, probably due to addMobsimListenerBinding &	addEventHandlerBinding
		if(qsim!=null)
			return;
		qsim  = (QSim)e.getQueueSimulation();	
		

//		Logger.getRootLogger().setLevel(Level.WARN);
		MatsimAPI.init(qsim);
		
		//the following needs to happen after having a qsim instance and before the first iteration is started
		//therefore, the only possibility is here
		
		domainHelper = new DomainHelperTraffic(null,simulator.getResponsibilities());
		String mesoMapPath = orchestrationHandler.getAbsolutePathByType("RoadMap");
		if(mesoMapPath.length()>0) {
			((DomainHelperTraffic)domainHelper).initMesoMap(mesoMapPath);
		} else {
//			System.out.println("can not init domain helper, missing map path");
//			System.exit(1);
			//ASSUMING DEMO MODE
			mesoMapPath = "/daceDS/Scenario/Demo/Line_Matsim/matsim.net";

			((DomainHelperTraffic)domainHelper).initMesoMap(mesoMapPath);
		}
		((DomainHelperTraffic)domainHelper).setInternalResponsibilitiesForMeso();
		System.out.println("domainHelper.getInternalResponsibilities() = "+domainHelper.getInternalResponsibilities());
		
		//now, we can call the super impl to set up provision handler
		super.init();
		super.prepare();
//		
		//register observers
		observerImpl = new MatsimObserver(scenarioID, simulator, qsim);
		observerImpl.init();
		for(Observer o : simulator.getObservers()) {
			provisionHandler.addObserver(o, observerImpl);
		}
		

		preLoopEvent();

		Logger.getRootLogger().setLevel(Level.INFO);
		Logger.getRootLogger().setLevel(Level.INFO);
	}

	@Override
	public void preLoopEvent() {		
//		log("\n\n\n preSimLoopEvent \n\n\n");
		log("simulating");
	}

	@Override
	protected void preStepEvent(int t) {		
//		log("\n\n\n preSimStepEvent \n\n\n");

//		Logger.getRootLogger().setLevel(Level.WARN);
	}

	@Override
	protected void stepEvent(int t) {
		// simulation is proceeded by matsim internally
//		log("\n\n\n simStepEvent \n\n\n");
	}

	/*
	 * Do we need to send out ndms?
	 */
	@Override
	protected void processStepEvent(int t) {	
	}

	@Override
	protected void postStepEvent(int t) {
		provisionHandler.runObservers();
	}



	@Override
	protected void postLoopEvent() {
		log("finished");
		//todo: close all kafka instances
		super.runCleanup();
	}

	
	/**********************************************************
	 * 					MATSim Listeners
	 * ********************************************************/
	
	
	/*
	 * This is called for each agent that is entering a new link.
	 * We need to check if we are still responsible for the new link.
	 * Otherwise, we will transfer the agent's ndm.
	 */
	@Override
	public void handleEvent(LinkEnterEvent event) {
//		log( event.getVehicleId().toString() + " entered " + event.getLinkId()); 

		String linkID =  event.getLinkId().toString();
		if(!((DomainHelperTraffic)domainHelper).isResponsibleInternal(linkID)) {
			linkID = Config.escapeSpecialChars(linkID);
//			log("--> now on an outgoing link");
			Id<Vehicle> vid = Id.create(event.getVehicleId().toString(), Vehicle.class);
			MobsimVehicle v = qsim.getVehicles().get(vid);
			Meso meso = MatsimAPI.getMeso(v);

			logInternal("   --> sending "+meso.getVehicleID()+ " on " + Config.getProvisionTopic(scenarioID, simulator, "link."+linkID+".vehicles"));
			
			provisionHandler.sendNDM(Config.getProvisionTopic(scenarioID, simulator, "link."+linkID+".vehicles"), meso, (int) (event.getTime()*1000));
			
			//and don't forget to remove it. we dont use ghosting on the meso level right now, so we can remove vehicle directly
			v.getDriver().endLegAndComputeNextState(event.getTime());
			
// is this useful?
//			v.getDriver().notifyArrivalOnLinkByNonNetworkMode(v.getDriver().getDestinationLinkId());
			
// or this?
//			if ( qsim instanceof ActivityEndRescheduler ) {
//				((ActivityEndRescheduler) qsim).rescheduleActivityEnd(v.getDriver());
//			} else {
//				throw new RuntimeException("mobsim does not support activity end rescheduling; aborting ...") ;
//			}
			
			logInternal("   --> ended leg of "+meso.getVehicleID()+ " in this local simulation");
			
		} else {
//			log(linkID + " is not an outgoing link -> doing nothing");
		}
		
	}


	/*
	 * We do here what
	 */
	@Override
	public void notifyMobsimAfterSimStep(MobsimAfterSimStepEvent e) {
		
		int t = (int) TimeSync.getLocaltime();
		
		preStepEvent(t);
		stepEvent(t);
		processStepEvent(t);
		synchronizeEvent(t);
		postStepEvent(t);
		
		

	}

	@Override
	public void notifyShutdown(ShutdownEvent event) {
		postLoopEvent();
		
	}

	@Override
	public void notifyMobsimBeforeSimStep(MobsimBeforeSimStepEvent e) {
		// TODO Auto-generated method stub
//		log("notifyMobsimBeforeSimStep");
	}

	@Override
	protected void processMessage(ConsumerRecord r) {
		Meso meso = (Meso) r.value();
		logInternal(TimeSync.getLocaltime() + ": adding "+meso.getVehicleID().toString());
		MatsimAPI.addAgentToMobSim(meso.getVehicleID().toString(), meso.getLink().toString(), meso.getRoute());
		
	}

	@Override
	protected void processInteraction(ConsumerRecord r) {
		log("todo: implement process interaction");		
	}


}