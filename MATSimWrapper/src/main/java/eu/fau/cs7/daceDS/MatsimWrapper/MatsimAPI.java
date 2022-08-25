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


import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.text.DateFormat;
import java.text.SimpleDateFormat;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;
import org.matsim.api.core.v01.Id;
import org.matsim.api.core.v01.TransportMode;
import org.matsim.api.core.v01.events.LinkEnterEvent;
import org.matsim.api.core.v01.events.LinkLeaveEvent;
import org.matsim.api.core.v01.events.handler.LinkEnterEventHandler;
import org.matsim.api.core.v01.events.handler.LinkLeaveEventHandler;
import org.matsim.api.core.v01.network.Link;
import org.matsim.api.core.v01.network.Network;
import org.matsim.api.core.v01.network.Node;
import org.matsim.api.core.v01.population.Activity;
import org.matsim.api.core.v01.population.Leg;
import org.matsim.api.core.v01.population.Person;
import org.matsim.api.core.v01.population.Plan;
import org.matsim.api.core.v01.population.PlanElement;
import org.matsim.api.core.v01.population.Route;
import org.matsim.core.mobsim.framework.MobsimAgent;
import org.matsim.core.mobsim.framework.MobsimDriverAgent;
import org.matsim.core.mobsim.framework.PassengerAgent;
import org.matsim.core.mobsim.qsim.QSim;
import org.matsim.core.mobsim.qsim.agents.PersonDriverAgentImpl;
import org.matsim.core.mobsim.qsim.interfaces.MobsimVehicle;
import org.matsim.core.mobsim.qsim.interfaces.NetsimNetwork;
import org.matsim.core.mobsim.qsim.qnetsimengine.QLaneI;
import org.matsim.core.mobsim.qsim.qnetsimengine.QLinkImpl;
import org.matsim.core.mobsim.qsim.qnetsimengine.QVehicle;
import org.matsim.core.mobsim.qsim.qnetsimengine.QVehicleImpl;
import org.matsim.core.population.PopulationUtils;
import org.matsim.core.population.routes.RouteUtils;
import org.matsim.lanes.Lane;
import org.matsim.vehicles.Vehicle;
import org.matsim.vehicles.VehicleType;
import org.matsim.vehicles.VehicleUtils;
import org.matsim.vis.snapshotwriters.VisLink;
import org.matsim.vis.snapshotwriters.VisVehicle;

import java.util.TimeZone;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.OrchestrationHandler;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.ResourceFile;
import eu.fau.cs7.daceDS.datamodel.Scenario;

public class MatsimAPI implements LinkEnterEventHandler, LinkLeaveEventHandler{

	private static final double AVG_VEH_LENGTH = 7.5; //with gap
	private static final double LOWER_BOUND =  0.0001;
	private static final int FLOW_TIMEFRAME = 3600;
	private static final int SECONDS_PER_HOUR = 3600;
	private static final int METERS_PER_KM = 1000;
	static Logger logger = Logger.getLogger("MatsimAPI");
	static QSim qsim;
	private static Network network;

	public static void init(QSim q) {
		qsim=q;
	}
	public static void init(Network n) {
		network=n;
	}

	//todo:check if type exists
	public static String createDirAndConfig(Scenario scenario, BB bb, OrchestrationHandler orchestrationHandler) {
		String scenarioID = scenario.getScenarioID().toString();
		String instanceID = bb.getInstanceID().toString();
		//dir was created with first receveived file
		System.out.println("resource dir is "+Config.getResourceDir(scenarioID, instanceID));
		//copy default config and replace placeholders
		String config = Config.getResourceDir(scenarioID, instanceID) +"/matsim.matsimcfg";

		String defaultConf = Config.get("defaultConf");

		Path modifiedConfigPath = Paths.get(config);
		Path originalPath = Paths.get(defaultConf);

		List<String> input = new ArrayList<String>();
		try {
			input = Files.readAllLines(originalPath);
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
		List<String> output = new ArrayList<String>();

		String routeFiles ="";

		//check for local traffic resources
		for(Entry<ResourceFile, Boolean> s : orchestrationHandler.getBufferedResources().entrySet()) {
			ResourceFile rf = s.getKey();
			boolean received = s.getValue();
			
			if(rf.getType().toString().equals(Config.RESOURCE_TRAFFIC)){
				if (received){
					routeFiles = rf.getID().toString();
				}	
			}	
		}

//		//Input is an optional field, local resources take precedence
//		if(routeFiles.length()  == 0){
//			for(Input i : scenario.getInputs()){
//				if(!i.getLayer().toString().equals(bb.getLayer().toString())|| !i.getProcessing().toString().equals(Config.get("nativeProcessingString")) ){
//					System.out.println("ignoring " + i.getPath()+ ", " + i.getLayer() +" vs "+bb.getLayer()+" and " + i.getProcessing() + " vs "+Config.get("nativeProcessingString"));
//					continue;
//				}
//
//				if(routeFiles.length()>0) {
//					System.out.println("only one native input allowed for matsim");
//					continue;
//				}
//				routeFiles = i.getPath().toString();
//			}
//		}

		//no traffic provided at all
		if(routeFiles.length()  == 0){
			routeFiles = Config.get("defaultTraffic");
			System.out.println("no traffic file given, adding " +Config.get("defaultTraffic"));
		}

		//sce format start in s, sl is in ms, matsim format is "00:30:00", 
		DateFormat formatter = new SimpleDateFormat("HH:mm:ss");
		formatter.setTimeZone(TimeZone.getTimeZone("GMT"));
		Date start = new Date(scenario.getSimulationStart()*1000);
		Date end = new Date(scenario.getSimulationEnd()*1000);
		Date sl = new Date(bb.getStepLength());

		
		String net = orchestrationHandler.getFilenameByType("RoadMap");

		String modifiedConfig = "";
		for(String line : input) {
			modifiedConfig += line + "\r\n";
		}

		modifiedConfig = modifiedConfig.replaceAll("\\{ROU\\}", routeFiles);
		modifiedConfig = modifiedConfig.replaceAll("\\{NET\\}", net);
		modifiedConfig = modifiedConfig.replaceAll("\\{START\\}",formatter.format(start)); 
		modifiedConfig = modifiedConfig.replaceAll("\\{END\\}",formatter.format(end));
		modifiedConfig = modifiedConfig.replaceAll("\\{SL\\}",formatter.format(sl));

		try {
			Files.createDirectories(modifiedConfigPath.getParent());

			Files.write(modifiedConfigPath, modifiedConfig.getBytes());
	
			//copy fallback no.rou.xml
			// Path src = Paths.get(Config.getExecutablesRootDir() + "/" +trafficSimulatorDescription.getType() + "/" +Config.get("defaultTraffic"));
			Path src = Paths.get(Config.get("defaultTraffic"));
			Path copy = Paths.get(Config.getResourceDir(scenarioID, instanceID) + "/" + src.getFileName());
			Files.copy(src, copy, StandardCopyOption.REPLACE_EXISTING);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return config;
	}
	
	public static MobsimVehicle getVehicle(String id) {
		Id<Vehicle> vid = Id.create( id , Vehicle.class);
		return qsim.getVehicles().get(vid);
	}

	public static Link getVehicleLink(String vid) {
		MobsimVehicle veh = getVehicle(vid);
		if(veh == null) {
			return null;
		}
		return veh.getCurrentLink();
	}	
	
	public static String getVehicleType(String vid) {
		MobsimVehicle veh = getVehicle(vid);
		if(veh == null) {
			return null;
		}
		Vehicle v = veh.getVehicle();
		return v.getType().getId().toString();
	}
	
	public static Route getVehicleRoute(String vid) {
		MobsimVehicle veh = getVehicle(vid);
		if(veh == null) {
			return null;
		}
		PersonDriverAgentImpl p = (PersonDriverAgentImpl)veh.getDriver();
		if(p == null) {
			return null;
		}
		
		PlanElement planElement = p.getCurrentPlanElement();
		if(planElement==null) {
			logger.info("planElement of "+veh.getId().toString()+" is null, probably just created");
			return null;
		}
		Leg leg;
		try {
			leg = (Leg)planElement;
		}catch(Exception e) {
			logger.info("leg of "+veh.getId().toString()+" could not be casted from planElement");
			return null;
		}
		Route r = leg.getRoute();
		
		return r;
	}

	public static double getExpectedTravelDistance(String vid) {
		MobsimVehicle veh = getVehicle(vid);
		if(veh == null) {
			return -1.0;
		}
		MobsimAgent agent = veh.getDriver();
		if(agent == null && veh.getPassengers().size()>0) {
			agent = (MobsimAgent) veh.getPassengers().toArray()[0];
		}
		if(agent == null) {
			return -1.0;
		}
		return agent.getExpectedTravelDistance();
	}
	public static double getExpectedTravelTime(String vid) {
		MobsimVehicle veh = getVehicle(vid);
		if(veh == null) {
			return -1.0;
		}
		MobsimAgent agent = veh.getDriver();
		if(agent == null && veh.getPassengers().size()>0) {
			agent = (MobsimAgent) veh.getPassengers().toArray()[0];
		}
		if(agent == null) {
			return -1.0;
		}
		return agent.getExpectedTravelTime().seconds();
	}


	static Meso getMeso(String id) {
		MobsimVehicle m = getVehicle(id);
		if(m == null) {
			return null;
		}
		return getMeso(m);
	}
	
	static Meso getMeso(MobsimVehicle m) {
		
		
		logger.info("getMeso for vehicle "+m.getId().toString());
		PersonDriverAgentImpl p = (PersonDriverAgentImpl)m.getDriver();
		// MobsimDriverAgent p = m.getDriver();

		Vehicle v = m.getVehicle();
		v.getType().getCapacity();
		v.getType().getDescription();
		v.getType().getWidth();
		String type = v.getType().getId().toString();
		
		if(p==null) {
			logger.info("PersonDriverAgentImpl of "+m.getId().toString()+" is null");
			// logger.info("MobsimDriverAgent of "+m.getId().toString()+" is null");
			return null;
		}

		
		Meso meso = new Meso();
		meso.setVehicleID(m.getId().toString());

		Route r = getVehicleRoute(m.getId().toString());
		
		//do we need to strip the current edge of the route for the data model?
		String routeString = r.getRouteDescription();
		String[] routeArrayWithImaginaryStartingEdge = routeString.split(" ");
		String[] routeArray = Arrays.copyOfRange(routeArrayWithImaginaryStartingEdge, 1, routeArrayWithImaginaryStartingEdge.length);
		meso.setRoute(Arrays.asList(routeArray));
		meso.setType(type);
		meso.setLink(m.getCurrentLink().getId().toString());

		logger.info("stripped imaginary item "+routeArrayWithImaginaryStartingEdge[0]+" route is therefore " + Arrays.toString(routeArray));
		
		return meso;
	}
	

	
	
	// is addParkedVehicle better?
	// https://github.com/matsim-org/matsim-libs/blob/fc97a437f52e4c23d50935e8ea6790e2bfbe10e8/matsim/src/test/java/org/matsim/core/mobsim/hermes/FlowCapacityTest.java
	public static void addAgentToMobSim(String id, String currentLink, List<CharSequence> route) {
		logger.info("trying to add agent to mobsim. id="+id+", edge="+currentLink + "', routesize"+route.size());

		try {

			ArrayList<Id<Link>> routeBody = new ArrayList<Id<Link>>();	
			//skip all links before the current links
			boolean alreadyVisited = true;	
			for(int i=0;i<route.size();i++) {
				String edge = route.get(i).toString();
				if(edge.equals(currentLink)){
					alreadyVisited = false;
				}
				if(alreadyVisited){
					logger.info("skipping "+edge);
					continue;
				}
				Id<Link> link = Id.create( edge , Link.class);
				routeBody.add(link);
				logger.info("adding edge '"+edge + "' to effective route");
				logger.info("length of " +  edge+ " is " + qsim.getScenario().getNetwork().getLinks().get(link).getLength());
			}
	
			//todo: is there a better solution to traverse "startLink?
			//todo: fails if route is null or inlink to startlink is null
			Link startLink = qsim.getScenario().getNetwork().getLinks().get(routeBody.get(0));
			Link pre = (Link) startLink.getFromNode().getInLinks().values().toArray()[0];
			routeBody.add(0, pre.getId());
	
			Route routeObject = RouteUtils.createNetworkRoute(routeBody, qsim.getScenario().getNetwork());	
			Activity homeAct = PopulationUtils.createActivityFromLinkId("h", routeBody.get(0));
			homeAct.setEndTime(qsim.getSimTimer().getTimeOfDay()); //most important line
			Leg leg1 = PopulationUtils.createLeg(TransportMode.car );
			leg1.setRoute(routeObject);			
			Activity workAct = PopulationUtils.createActivityFromLinkId("w", routeBody.get(routeBody.size()-1));
	
			Plan plan = PopulationUtils.createPlan();
			plan.addActivity(homeAct);
			plan.addLeg(leg1);
			plan.addActivity(workAct);
			
	
			PopulationUtils.createAndAddActivityFromLinkId(plan, "w", routeBody.get(0));
			PersonDriverAgentImpl driver = null;
	
			MobsimVehicle msv = qsim.getVehicles().get(Id.create(id, Vehicle.class));
			
			if(msv != null){
	//			System.out.println(id + " is already here, just add new leg");
	//
	//			Person p = qsim.getScenario().getPopulation().getPersons().get(Id.create(id, Person.class));
	//			p.addPlan(plan);	
	//			plan.setPerson(p);
	//			
	//			QVehicle qv =((QVehicle) msv);				
	//			qv.setCurrentLink(pre);
	//
	//			driver = new PersonDriverAgentImpl(plan, qsim);
	//			qv.setDriver(driver);
	//
	//			// results in "Exception in thread "main" java.lang.RuntimeException: Could not remove parked vehicle with id y on the link x. Maybe it is currently used by someone else? (In which case ignoring this exception would lead to duplication of this vehicle.) Maybe was never placed onto a link?"
	
				// --> use a different name
				if (id.contains("_gen")) {
					int len = id.length();
					String pr = id.substring(0, len - 1);
					id = pr + String.valueOf(id.charAt(len - 1) + 1);
					
				} else {
					id += "_gen1";
				}
			} 
	//		else {
	
	
				Person p = PopulationUtils.getFactory().createPerson(Id.create(id, Person.class));
				p.addPlan(plan);
				
				
				
				Vehicle vehicle = VehicleUtils.getFactory().createVehicle( Id.create( id, Vehicle.class ), VehicleUtils.getDefaultVehicleType());
				QVehicle veh = new QVehicleImpl(vehicle);
			
				
			
			
				driver = new PersonDriverAgentImpl(p.getSelectedPlan(), qsim);
			
				try {
					qsim.insertAgentIntoMobsim(driver); 
					veh.setDriver(driver);
			
					driver.setVehicle(veh);
					qsim.addParkedVehicle(veh, Id.create(currentLink, Link.class)); 
			
			
			
					logger.info("added agent successfullly");
				}catch(Exception e) {
					logger.info("Exception while trying to add " + id);
					logger.info(e.getLocalizedMessage());
				}
	//		}
				
	
			qsim.rescheduleActivityEnd(driver);
		}catch(Exception e) {
			logger.info("Outer exception while trying to add " + id);
			logger.info(e.getLocalizedMessage());
		}

	}


	/*
	 * 
	 * 		NETWORK
	 * 
	 * 
	 */

	
	public static ArrayList<String> getOutLinksStr(String node){
		Node n = qsim.getScenario().getNetwork().getNodes().get(node);
		ArrayList<String> links = new ArrayList<String>();
		for(Entry<Id<Link>, ? extends Link> l : n.getOutLinks().entrySet()) {
			links.add(l.getKey().toString());
		}
		return links;
	}
	public static Collection<? extends Link> getOutLinks(String node){
		Node n = qsim.getScenario().getNetwork().getNodes().get(Id.create(node, Node.class));
		if(n==null) {
			logger.error("could not find node object for node string "+node);
			return (new ArrayList<Link>());
		}
		return n.getOutLinks().values();
	}

	public static Collection<? extends Link> getLinks() {
		if(qsim != null) {
			return qsim.getScenario().getNetwork().getLinks().values();
		} else if (network != null) {
			network.getLinks().values();
		} 
		return null;
	}



	public static double getLastMovementTimeOfFirstVehicle(String link){
		NetsimNetwork queueNetwork = qsim.getNetsimNetwork();
		QLinkImpl ql = (QLinkImpl) queueNetwork.getNetsimLink(Id.create(link, Link.class));
		QLaneI qlane = ql.getAcceptingQLane();
		return qlane.getLastMovementTimeOfFirstVehicle();
	
	}	
	public static double getLoadIndicator(String link){
		NetsimNetwork queueNetwork = qsim.getNetsimNetwork();
		QLinkImpl ql = (QLinkImpl) queueNetwork.getNetsimLink(Id.create(link, Link.class));
		QLaneI qlane = ql.getAcceptingQLane();
		return qlane.getLoadIndicator();
	}	
	public static double getStorageCapacity(String link){
		NetsimNetwork queueNetwork = qsim.getNetsimNetwork();
		QLinkImpl ql = (QLinkImpl) queueNetwork.getNetsimLink(Id.create(link, Link.class));
		QLaneI qlane = ql.getAcceptingQLane();
		return qlane.getStorageCapacity();
	}	

	public static double getLength(String link){
		NetsimNetwork queueNetwork = qsim.getNetsimNetwork();
		QLinkImpl ql = (QLinkImpl) queueNetwork.getNetsimLink(Id.create(link, Link.class));
		return ql.getLink().getLength();
	}	

	public static double getOccupancy(String link){
		double li = getLoadIndicator(link);
		double occ = (li*AVG_VEH_LENGTH) / getLength(link);
		return occ;
	}	

	
	public static ArrayList<String> getVehicleIds(String link){
		ArrayList<String> ids = new ArrayList<String>();
		
		NetsimNetwork queueNetwork = qsim.getNetsimNetwork();
		QLinkImpl ql = (QLinkImpl) queueNetwork.getNetsimLink(Id.create(link, Link.class));
		for(MobsimVehicle veh : ql.getAllNonParkedVehicles()){
			ids.add(veh.getId().toString());
		}
		return ids;
	}	
	
	public static ArrayList<MobsimVehicle> getVehicles(String link){
		ArrayList<MobsimVehicle> vehs = new ArrayList<MobsimVehicle>();
		
		NetsimNetwork queueNetwork = qsim.getNetsimNetwork();
		QLinkImpl ql = (QLinkImpl) queueNetwork.getNetsimLink(Id.create(link, Link.class));
		for(MobsimVehicle veh : ql.getAllNonParkedVehicles()){
			vehs.add(veh);
		}
		return vehs;
	}
	public static ArrayList<Meso> getVehiclesMeso(String link){
		ArrayList<Meso> vehs = new ArrayList<Meso>();
		
		NetsimNetwork queueNetwork = qsim.getNetsimNetwork();
		QLinkImpl ql = (QLinkImpl) queueNetwork.getNetsimLink(Id.create(link, Link.class));
		for(MobsimVehicle veh : ql.getAllNonParkedVehicles()){
			vehs.add(getMeso(veh));
		}
		return vehs;
	}
	

	private static Map<String, LinkedHashMap<Integer, Integer>> volumes = new HashMap<String,LinkedHashMap<Integer,Integer>>();
	private static Map<String, Map<String, Integer>> enterTimes = new HashMap<String,Map<String,Integer>>();
	private static Map<String, Double> speeds = new HashMap<String,Double>();
	
	public static double getFlow(String link){
		if(!volumes.containsKey(link)) {
			return 0.0;
		}
		
		int curTime = (int) qsim.getSimTimer().getTimeOfDay();
		int timeframe = Math.min(FLOW_TIMEFRAME, curTime);
		
		LinkedHashMap<Integer, Integer> linkVolumes = volumes.get(link);
		
		List<Integer> rk = new ArrayList<Integer>(linkVolumes.keySet());   
		Collections.reverse(rk);
		double volSum = 0;
		for (Integer time : rk) {
			if (curTime - time > timeframe)
				break;
			volSum+= linkVolumes.get(time);
		}
		
		return volSum/((double)timeframe);
	}	
	
	public static int getCurrentVehicleNumber(String link){
		if(!volumes.containsKey(link)) {
			return 0;
		}
		int curTime = (int) qsim.getSimTimer().getTimeOfDay();		
		LinkedHashMap<Integer, Integer> linkVolume = volumes.get(link);
		

		if(!linkVolume.containsKey(curTime)) {
			return 0;
		}
		
		return linkVolume.get(curTime);
	}	
	
	
	public static double getSpeed(String link){
		//no vehicle -> no speed
//		if (getOccupancy(link) < LOWER_BOUND) {
//			return 0;
//		}
		
		// we could also return the free flow speed for small occupancy rates.
		// anyway, that could be wrong (e.g., only accepting link is congested, 
		// one vehicle on current link -> occ is very small, but speed = 0) 
		// therefore, we will only use historic speeds
		return speeds.getOrDefault(link, 0.);
	}


	
	@Override
	public void handleEvent(LinkEnterEvent event) {
		String l = event.getLinkId().toString();
		String v = event.getVehicleId().toString();
		int t = (int) event.getTime();
		
		if(!volumes.containsKey(l)) {
			volumes.put(l, new LinkedHashMap<Integer,Integer>());
		}
		int count = volumes.get(l).getOrDefault(t, 0);
		volumes.get(l).put(t, count + 1);
		
		
		if(!enterTimes.containsKey(l)) {
			enterTimes.put(l, new HashMap<String,Integer>());
		}
		enterTimes.get(l).put(v, t);
		
		
	}

	
	// used to calc speeds
	@Override
	public void handleEvent(LinkLeaveEvent event) {
		String l = "linkfoo";
		String v = "vehfoo";
		int leavetime = -1;
		int entertime = -1;
	
		try {
			l = event.getLinkId().toString();
			v = event.getVehicleId().toString();
			leavetime = (int)event.getTime();
			entertime = enterTimes.get(l).get(v);
			double duration = (leavetime - entertime);//  minus the last sim step
			double length = getLength(l);// /METERS_PER_KM;
			double speed = length / duration;
//			System.out.println("adding "+speed+"m/s for link" +l+ "   (length is "+length+" and duration was "+duration+")");
			speeds.put(l, speed);
		} 
		catch (Exception e){
//			System.out.println("couldnt calc speed for "+v+ " on "+ l + "(enter:"+entertime+",leave:"+leavetime+")");
		}		
	}
}		