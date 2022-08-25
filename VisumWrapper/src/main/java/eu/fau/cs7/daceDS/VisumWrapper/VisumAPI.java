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

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;



import com.jacob.activeX.ActiveXComponent;
import com.jacob.com.Dispatch;
import com.jacob.com.Variant;

import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.MacroPath;
import eu.fau.cs7.daceDS.datamodel.Path;

import com.jacob.com.*;


import org.apache.log4j.Logger;

public class VisumAPI {

	private static final String TRANSPORT_SYSTEM = "P"; //for PKW, needs to match the transport system key specified in Visum
	static ActiveXComponent oNetCalendarPeriod;
	static ActiveXComponent oNetCalendarPeriodAnalysisTimeIntervalSet;
	static ActiveXComponent oNetLinks;
	static ActiveXComponent oNetNodes;
	static ActiveXComponent oNetTurns;
	static Dispatch oProcedures;
	static ActiveXComponent ax;


	static Logger logger = Logger.getLogger("VisumAPI");
	private static Dispatch oDemandSegment;
	private static ActiveXComponent oDemandSegments;
	private static ActiveXComponent oZones;
	private static ActiveXComponent oTSystems;
	private static ActiveXComponent oODPairs;
	
	private static HashMap<Integer, ArrayList<Integer>> connectedZones = new HashMap<Integer,ArrayList<Integer>>();;
	
	
	
	
	public static void startVisum() {
		ax = new ActiveXComponent("Visum.Visum.22");
	}

	
	public static void openProcedure(String path) {
		Dispatch.call(oProcedures, "Open", path, "True", "True", "True");
	}	

	public static void execProcedure(String path) {
		Dispatch.call(oProcedures, "Execute");
	}

	public static void addTimeIntervall(long start, long end) {
		//Visum.Net.CalendarPeriod.AnalysisTimeIntervalSet.AddTimeInterval('0026','00:25:00','00:26:00',1)
		System.out.println("start="+start+", end="+end);
		System.out.println("Dispatch.call(oNetCalendarPeriodAnalysisTimeIntervalSet, AddTimeInterval, "+getTimeString(start)+", "+getConfigTimeString(start)+", "+getConfigTimeString(end)+",1);");
		Dispatch.call(oNetCalendarPeriodAnalysisTimeIntervalSet, "AddTimeInterval", getTimeString(start), getConfigTimeString(start), getConfigTimeString(end),1);
	}

	public static void addIntervalls(long start, long end, long step) {
		System.out.println("adding intervalls from "+start+" to "+end+" with steplength of "+step);
		long cur = start;
		while(cur+step <= end) {
			addTimeIntervall(cur,cur+step);
			cur+=step;
		}
		//part missing?
		if(cur<end) {
			addTimeIntervall(cur,end);
		}
	}

	public static void loadVer(String path) {
		Dispatch.call(ax, "LoadVersion", path);
		oProcedures = ax.getProperty("Procedures").toDispatch();
		oNetLinks = ax.getPropertyAsComponent("Net").getPropertyAsComponent("Links");
		oNetNodes = ax.getPropertyAsComponent("Net").getPropertyAsComponent("Nodes");
		oNetTurns = ax.getPropertyAsComponent("Net").getPropertyAsComponent("Turns");
		oTSystems = ax.getPropertyAsComponent("Net").getPropertyAsComponent("TSystems");
		oODPairs = ax.getPropertyAsComponent("Net").getPropertyAsComponent("ODPairs");
		oNetCalendarPeriod = ax.getPropertyAsComponent("Net").getPropertyAsComponent("CalendarPeriod");
		oNetCalendarPeriodAnalysisTimeIntervalSet = ax.getPropertyAsComponent("Net").getPropertyAsComponent("CalendarPeriod").getPropertyAsComponent("AnalysisTimeIntervalSet");

		oDemandSegments = ax.getPropertyAsComponent("Net").getPropertyAsComponent("DemandSegments");
		oDemandSegment = Dispatch.call(oDemandSegments, "ItemByKey", TRANSPORT_SYSTEM).toDispatch();		

		oZones = ax.getPropertyAsComponent("Net").getPropertyAsComponent("Zones");
				
	}
	
	public static void close() {
		oProcedures.safeRelease();
		oNetLinks.safeRelease();
		oNetNodes.safeRelease();
		oNetTurns.safeRelease();
		oNetCalendarPeriod.safeRelease();
		oNetCalendarPeriodAnalysisTimeIntervalSet.safeRelease();
		
		ax.safeRelease();

		ComThread.InitSTA();
		ComThread.Release();
		ComThread.quitMainSTA();
	}

	
	



	public static ArrayList<VisumLink> getOutLinks(String nodeId) {
		ArrayList<VisumLink> links = new ArrayList<VisumLink>();
		Dispatch nodeDispatch = getNode(nodeId);
		Dispatch linksDispatch = Dispatch.call(nodeDispatch, "OutLinks").toDispatch();
		EnumVariant en = new EnumVariant(linksDispatch);
		while (en.hasMoreElements()) {
			Dispatch link = en.nextElement().getDispatch();
			//System.out.println(Dispatch.call(link, "AttValue", "Name").getString()); //name is optional
			String id = String.valueOf((int) Math.round(Dispatch.call(link, "AttValue", "No").getDouble())); //id comes as double!!!
			Long fromNode = (Long) Math.round(Dispatch.call(link, "AttValue", "FromNodeNo").getDouble()); //id comes as double!!!
			Long toNode = (Long) Math.round(Dispatch.call(link, "AttValue", "ToNodeNo").getDouble()); //id comes as double!!!
			links.add(new VisumLink(id,fromNode,toNode));
		}
		return links;
	}

	public static ArrayList<VisumLink> getInLinks(String nodeId) {
		ArrayList<VisumLink> links = new ArrayList<VisumLink>();
		Dispatch nodeDispatch = getNode(nodeId);
		if(nodeDispatch == null) {
			System.out.println("DID NOT FIND NODE:'"+nodeId+"'");
			
			return links;
		}
		Dispatch linksDispatch = Dispatch.call(nodeDispatch, "InLinks").toDispatch();
		EnumVariant en = new EnumVariant(linksDispatch);
		while (en.hasMoreElements()) {
			Dispatch link = en.nextElement().getDispatch();
			//System.out.println(Dispatch.call(link, "AttValue", "Name").getString()); //name is optional
			String id = String.valueOf((int) Math.round(Dispatch.call(link, "AttValue", "No").getDouble())); //id comes as double!!!
			Long fromNode = (Long) Math.round(Dispatch.call(link, "AttValue", "FromNodeNo").getDouble()); //id comes as double!!!
			Long toNode = (Long) Math.round(Dispatch.call(link, "AttValue", "ToNodeNo").getDouble()); //id comes as double!!!
			links.add(new VisumLink(id,fromNode,toNode));
		}
		return links;
	}	

	public static Dispatch getLink(Long from, Long to) {
		return Dispatch.call(oNetLinks, "ItemByKey",String.valueOf(from), String.valueOf(to)).toDispatch();
	}
	
	public static ArrayList<VisumLink> getLinks() {
		ArrayList<VisumLink> links = new ArrayList<VisumLink>();

		EnumVariant en = new EnumVariant(oNetLinks);
		while (en.hasMoreElements()) {
			Dispatch link = en.nextElement().getDispatch();
			String id = String.valueOf((int) Math.round(Dispatch.call(link, "AttValue", "No").getDouble())); //id comes as double!!!
			Long fromNode = (Long) Math.round(Dispatch.call(link, "AttValue", "FromNodeNo").getDouble()); //id comes as double!!!
			Long toNode = (Long) Math.round(Dispatch.call(link, "AttValue", "ToNodeNo").getDouble()); //id comes as double!!!
			links.add(new VisumLink(id,fromNode,toNode));
		}
		return links;
	}		
	
	public static void updateConnectedZones() {
		connectedZones.clear();

		System.out.println("updateConnectedZones");


		EnumVariant en = new EnumVariant(oODPairs);
		while (en.hasMoreElements()) {
			Dispatch odpair = en.nextElement().getDispatch();
			double dist = Dispatch.call(odpair, "AttValue", "DirectDist").getDouble();
			int fromZone = (int) Math.round(Dispatch.call(odpair, "AttValue", "FromZoneNo").getDouble());
			int toZone = (int) Math.round(Dispatch.call(odpair, "AttValue", "ToZoneNo").getDouble()); 
			int isDiagonal = (int) Math.round(Dispatch.call(odpair, "AttValue", "IsDiagonal").getDouble()); 
			if(isDiagonal == 1) {
				continue;
			}
			if(!connectedZones.containsKey(fromZone)) {
				connectedZones.put(fromZone, new ArrayList<Integer>());
			}
			connectedZones.get(fromZone).add(toZone);
			System.out.println("connection between zone "+fromZone+" and zone "+toZone +"(isdiagnol="+isDiagonal);
			
		}
		return;
	}	

	public static Dispatch getNode(String id) {
		Dispatch r = null;
		try{
			r = Dispatch.call(oNetNodes, "ItemByKey", id).toDispatch();
		} catch(Exception e) {
			System.out.println(e);
		}
		return r;
	}
	
	public static ArrayList<String> getNodes() {
		ArrayList<String> nodes = new ArrayList<String>();
		EnumVariant en = new EnumVariant(oNetNodes);
		while (en.hasMoreElements()) {
			Dispatch node = en.nextElement().getDispatch();
			String id = String.valueOf((int) Math.round(Dispatch.call(node, "AttValue", "No").getDouble())); //id comes as double!!!
			nodes.add(id);
		}
		return nodes;
	}

	public static ArrayList<VisumTurn> getTurnList(String timeStr) {
		EnumVariant turns = new EnumVariant(oNetTurns);		
		ArrayList<VisumTurn> turnList = new ArrayList<VisumTurn>();
		while (turns.hasMoreElements()) {
			Dispatch turn = turns.nextElement().getDispatch();
			String viaNode = String.valueOf((int) Math.round(Dispatch.call(turn, "AttValue", "ViaNodeNo").getDouble())); //id comes as double!!!
			String fromNode = String.valueOf((int) Math.round(Dispatch.call(turn, "AttValue", "FromNodeNo").getDouble())); //id comes as double!!!
			String toNode = String.valueOf((int) Math.round(Dispatch.call(turn, "AttValue", "ToNodeNo").getDouble())); //id comes as double!!!

			double cap = Dispatch.call(turn, "AttValue", "Capprt").getDouble(); 

			double vol = Dispatch.call(turn, "AttValue", "VOLVEHPRT("+timeStr+")").getDouble();	

			System.out.println(fromNode+"->"+viaNode+"->"+toNode+": "+cap+ " "+vol);

			turnList.add(new VisumTurn(fromNode, viaNode, toNode, cap, vol));
		}
		return turnList;
	}


	
	
	public static int getNumberOfZones() {
		return Dispatch.call(oZones, "Count").getInt();
	}

	//Visum.Net.DemandSegments.ItemByKey("P").GetNumPaths(1,2)
	public static int getNumberOfPaths(int from, int to) {
		return Dispatch.call(oDemandSegment, "GetNumPaths", from, to).getInt();
	}
	

	
	//Visum.Net.DemandSegments.ItemByKey("P").GetPathFlow(1,2,1)
	public static List<Long> getNodesOfPath(int fromZone, int toZone, int pathId) {
//		System.out.println("getNodesOfPath from="+fromZone+", toZone="+toZone+", pathId="+pathId);
		Dispatch d = Dispatch.call(oDemandSegment, "GetPathNodes", fromZone, toZone, pathId).toDispatch();
		EnumVariant en = new EnumVariant(d);
		List<Long> nodes = new ArrayList<Long>();
		while (en.hasMoreElements()) {
			Dispatch node = en.nextElement().getDispatch();
			nodes.add((Long) Math.round(Dispatch.call(node, "AttValue", "No").getDouble())); //id comes as double!!!
		}
		return nodes;
	}	
	

	//Visum.Net.DemandSegments.ItemByKey("P").GetPathFlowTi(1,2,1)
	public static ArrayList<Double> getDBSFlowsOfPathPerTime(int fromZone, int toZone, int pathId) {
//		Dispatch d = Dispatch.call(oDemandSegment, "GetPathFlowTi", from, to, pathId).toDispatch();
		Variant d = Dispatch.call(oDemandSegment, "GetPathFlowTi", fromZone, toZone, pathId);
		SafeArray s = d.toSafeArray();
//		EnumVariant flowsEV = d.toEnumVariant();
		
		ArrayList<Double> fL = new ArrayList<Double>();
		for (int i=0;i< s.getElemSize(); i++) {
			fL.add(s.getDouble(i));
//			fL.add(flowsEV.nextElement().getDouble());
		}
		return fL;
	}
	//Visum.Net.DemandSegments.ItemByKey("P").GetPathFlow(1,2,1)
	public static double getSBAFlowOfPath(int fromZone, int toZone, int pathId, int intervalID) {
		ArrayList<Double> l =getDBSFlowsOfPathPerTime(fromZone,toZone,pathId);
		if(intervalID < 0 || intervalID >= l.size()) {
			System.out.println("Cannot find interval"+intervalID);
			return -1.;
		}
		return l.get(intervalID);
	}	
	//Visum.Net.DemandSegments.ItemByKey("P").GetPathFlow(1,2,1)
	public static double getTotalFlowOfPath(int fromZone, int toZone, int pathId) {
		return Dispatch.call(oDemandSegment, "GetPathFlow", fromZone, toZone, pathId).getDouble();
	}	

	
	
	
	public static  ArrayList<MacroPath> getPathsViaNode(Long nodeID) {

		System.out.println("getPathsViaNode for "+nodeID);
		updateConnectedZones();
		ArrayList<MacroPath> paths = new ArrayList<MacroPath>();
		
		//get all connected zones
		double totalFlow=0;
		for(Entry<Integer, ArrayList<Integer>> connection : connectedZones.entrySet()) {
			int fromZone = connection.getKey();
			// for each connected target zone
			for(int toZone : connection.getValue()) {
				int noPaths = getNumberOfPaths(fromZone, toZone);
				//get all paths between two connected nodes
				for(int pathId = 1; pathId <= noPaths; pathId++) {
					//get all nodes on each path and compare to nodeID
					List<Long> nodes = getNodesOfPath(fromZone, toZone, pathId);
					for(Long nid : nodes) {
						if(nodeID.equals(nid)) {
							System.out.println(nodeID + " is contained in "+fromZone+"|"+toZone+"|"+pathId);
							double f = getTotalFlowOfPath(fromZone,toZone,pathId);
							totalFlow += f;
							paths.add(new MacroPath(fromZone, toZone, pathId, f, 0., nodes));
						}
					}
				}
			}
		}
		for(MacroPath p : paths) {
			p.setTotalFlow(totalFlow);
		}

		return paths;
		
	}
	
	public static ArrayList<MacroPath> getPathsViaLink(VisumLink link) {
		
		System.out.println("getPathsViaLink for "+link.getEdgeID());
		updateConnectedZones();
		
		ArrayList<MacroPath> paths = new ArrayList<MacroPath>();
		
		//get all connected zones
		double totalFlow=0;
		for(Entry<Integer, ArrayList<Integer>> connection : connectedZones.entrySet()) {
			int fromZone = connection.getKey();
			// for each connected target zone
			for(int toZone : connection.getValue()) {
				int noPaths = getNumberOfPaths(fromZone, toZone);
				//get all paths between two connected nodes
				for(int pathId = 1; pathId <= noPaths; pathId++) {
					//get all nodes on each path and compare to nodeID
					Long previousNode = -1L;
					List<Long> nodes = getNodesOfPath(fromZone, toZone, pathId);
					for(Long nid : nodes) {
						//if we have a single match, we can add the path and continue with the next
						if(link.getFromNode().equals(previousNode) && link.getToNode().equals(nid)) {
							double f = getTotalFlowOfPath(fromZone,toZone,pathId);
							totalFlow += f;
							System.out.println(link.getEdgeID() + " is contained in "+fromZone+"|"+toZone+"|"+pathId+ " and has a total volume of "+f);
							paths.add(new MacroPath(fromZone, toZone, pathId, f, 0., nodes));
							continue;
						}else {
							System.out.println(link.getEdgeID() + " is not matching "+previousNode+"_"+nid);
							System.out.println( "      "+link.getFromNode()+"?=?"+previousNode+" =  "+ (link.getFromNode().equals(previousNode)));
							System.out.println( "      "+link.getToNode()+"?=?"+nid+" =  "+ (link.getToNode().equals(nid)));
						}
						previousNode = nid;
					}
				}
			}
		}
		
		for(MacroPath p : paths) {
			p.setTotalFlow(totalFlow);
		}
		
		return paths;
	}
	
	
	

	static String getTimeString(long milliseconds) {
		int seconds = (int) (milliseconds / 1000) % 60 ;
		int minutes = (int) ((milliseconds / (1000*60)) % 60);
		int hours   = (int) ((milliseconds / (1000*60*60)) % 24);
		//if seconds are not zero, visum requires them
		String timeStr = "";
		if(seconds > 0) {
			timeStr = String.format("%02d%02d%02d", hours,minutes,seconds);
		}
		//else the second part is dropped
		else {
			timeStr = String.format("%02d%02d", hours,minutes);
		}
		System.out.println("converted "+milliseconds+" to " +timeStr);
		return timeStr;
	}
	
	static int getIntervalID(long milliseconds,  int stepLengthInMinutes) {
		int minutes = (int) (milliseconds / (1000*60));
		return minutes / stepLengthInMinutes;
	}

	static String getConfigTimeString(long milliseconds) {
		int seconds = (int) (milliseconds / 1000) % 60 ;
		int minutes = (int) ((milliseconds / (1000*60)) % 60);
		int hours   = (int) ((milliseconds / (1000*60*60)) % 24);

		return String.format("%02d:%02d:%02d", hours,minutes,seconds);

	}
	
	


	public static void setValue(Dispatch target, String attribute, Variant value) {

		Variant[] indecies = new Variant[1];
		indecies[0] = new Variant(attribute);

		Variant[] variants = new Variant[indecies.length + 1];

		for (int i = 0; i < indecies.length; i++) {
			variants[i] = indecies[i];
		}
		variants[variants.length - 1] = value;

		Dispatch.invoke(target, "AttValue", Dispatch.Put, variants,new int[variants.length]);
	}


	public static void setCap(VisumLink link, double flow) {
		Dispatch dl = getLink(link.getFromNode(), link.getToNode());
		Variant variant = new Variant();
		variant.putDouble(flow);
		setValue(dl,"Capprt",variant);

	}





	/*
	 * 
	 * 
	 * DUE
	 * 
	 */
	//belegung pro zeiteinheit
	public static double getFlow(VisumLink link, String time, long stepLengthInMinutes) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "VOLVEHPRT("+time+")");		
		//normalize to veh/hour
		double flow = v.getDouble() * (60 / stepLengthInMinutes);
		return flow;
	}
	public static double getSpeed(VisumLink link, String time) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "VCur_PrTSysTI(P,"+time+")");
		return v.getDouble();
	}
	public static double getDensity(VisumLink link, String time, long stepLengthInMinutes) {
		double flow = getFlow(link, time,stepLengthInMinutes);
		double speed = getSpeed(link, time);
		double density = flow / speed;
		return density;

	}

	public static double getOccupation(VisumLink link, String time, long stepLengthInMinutes) {
		double density = getDensity(link, time,stepLengthInMinutes);
		double length = getLength(link);
		double occ =  density / length;
		return occ;
	}
	
	
	/*
	 * 
	 * SBA 
	 * 
	 */

	//we will use the incoming cars
	public static double getSBAFlow(VisumLink link,  String time, int stepLengthInMinutes) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "SBAVEHSENTERED("+time+")"); //absolute amount of vehicles that entered link within last simstep
		if(v.getvt() == Variant.VariantEmpty) {
			return 0.0;
		}
		//normalize to veh/hour
		double flow = v.getDouble() * (60 / stepLengthInMinutes);
		return flow;
	}

	public static double getSBASpeed(VisumLink link, String time) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "SBAvCur_PrTSys(P,"+time+")"); 
		if(v.getvt() == Variant.VariantEmpty) {
			return 0.0;
		}
		return v.getDouble();
	}

	//"Average density measured in the simulation, i.e. average number of vehicles on the link divided by (SBA length times number of lanes)."
	// veh/km
	public static double getSBADensity(VisumLink link, String time) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "SBADENSITY("+time+")");
		return v.getDouble();
	}

	//In der Simulation gemessener Auslastungsgrad der Strecke. 
	//SBAUtilization SBA utilization Degree of utilization of the link measured in the simulation.
	//bessere Density?
	public static double getSBAUtilization(VisumLink link, String time) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "SBAUTILIZATION("+time+")");
		return v.getDouble();
	}
//	public static double getSBADelay(String from, String to, String time) {
//		Dispatch d = getLink(from, to);
//		Variant v = Dispatch.call(d, "AttValue", "DELAY_PRTSYSTI("+TRANSPORT_SYSTEM+","+time+")");
//		return v.getDouble();
//	}
	
	
	
	public static double getVolCapRatio(VisumLink link, String time) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "VolCapRatioPrT("+time+")");
		return v.getDouble();

	}
	public static double getCap(VisumLink link, String time) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "CapPrT");
		return v.getDouble();

	}

	public static double getSBACap(VisumLink link) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "SBACap_PrTSys("+TRANSPORT_SYSTEM+")"); //no time parameter? "always up to date"
		return v.getDouble();

	}
	
	//seem to be static for whole sim
	//if necessary, one could propably use setCap(0)
	public static List<CharSequence> getModes(VisumLink link) {
		List<CharSequence> ls = new  LinkedList<CharSequence>(); 
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "TSysSet");
		for(String s : v.getString().split(",")) {
			ls.add(s);
		}
		return ls;

	}

	public static double getLength(VisumLink link) {
		Dispatch d = getLink(link.getFromNode(), link.getToNode());
		Variant v = Dispatch.call(d, "AttValue", "Length");
		return v.getDouble();
	}

	public static HashMap<String, Double> getTurningProbs(VisumLink link, String timeStr) {

		Dispatch turns = Dispatch.call(oNetTurns, "FilteredBy", "[FromNodeNo] ="+link.getFromNode()+"& [ViaNodeNo] ="+link.getToNode()).toDispatch();
		EnumVariant turnsEV = new EnumVariant(turns);		
		double totalVol = 0;
		Map<String,Double> values = new HashMap<String,Double>();
		while (turnsEV.hasMoreElements()) {
			Dispatch turn = turnsEV.nextElement().getDispatch();
			String toNode = String.valueOf((int) Math.round(Dispatch.call(turn, "AttValue", "ToNodeNo").getDouble())); //id comes as double!!!
			String toEdge = link.getToNode()+"_"+toNode;
			double vol = Dispatch.call(turn, "AttValue", "VOLVEHPRT("+timeStr+")").getDouble(); //id comes as double!!!
			totalVol += vol;
			values.put(toEdge, vol);
		}

		//normalize
		HashMap<String,Double> probs = new HashMap<String,Double>();
		for(Entry<String, Double> a : values.entrySet()) {
			//no traffic is outgoing on that junction, make a uniform distribution
			if(totalVol<=0) {
				probs.put(a.getKey(), 1.0/values.size());	
			} 
			//no traffic is outgoing on that link, set to zero
			else if(a.getValue()<=0) {
				probs.put(a.getKey(), 0.0);
			}
			//calc prop
			else {
				probs.put(a.getKey(), a.getValue()/totalVol);
			}
		}
		return probs;
	}
	public static Map<String, HashMap<String, Double>> getAllTurningProbs( String timeStr, int stepLengthInMinutes) {
		ArrayList<VisumLink> links = getLinks();

		Map<String, HashMap<String, Double>> turningProbs = new HashMap<String,HashMap<String,Double>> ();
		//find probs for each link
		for(VisumLink link : links) {
			//only if there is traffic
			if(VisumAPI.getFlow(link,timeStr,stepLengthInMinutes )< 0.001) {
				continue;
			}
			turningProbs.put(link.getEdgeID(),getTurningProbs(link,timeStr));
		}
		return turningProbs;		
	}	
	
	
	static Macro getMacro(String linkStr, String timeStr, int stepLengthInMinutes) {
		
		System.out.println("trying to get macro for "+linkStr);
		try {
			VisumLink link = new VisumLink(linkStr);
			Macro macro = new Macro();
			macro.setLinkID(linkStr);
			macro.setAllowedModes(VisumAPI.getModes(link));
			macro.setCapacity(VisumAPI.getCap(link,timeStr));
			macro.setDensity(VisumAPI.getDensity(link,timeStr,stepLengthInMinutes));
			macro.setFlow(VisumAPI.getFlow(link,timeStr,stepLengthInMinutes ));
			macro.setLength(VisumAPI.getLength(link));
			macro.setSpeed(VisumAPI.getSpeed(link,timeStr));
			Map<CharSequence,Double> m = new HashMap<CharSequence,Double>();
			for(Entry<String, Double> entry :	VisumAPI.getTurningProbs(link,timeStr).entrySet()) {
				m.put(entry.getKey(), entry.getValue());
			}
			macro.setTurningProbabilities(m);
			
			List<MacroPath> paths = VisumAPI.getPathsViaLink(link);
			macro.setPaths(paths);
			
			return macro;
		} catch (Exception e){
			System.out.println(e.getLocalizedMessage());
		}
		return null;
	}


}