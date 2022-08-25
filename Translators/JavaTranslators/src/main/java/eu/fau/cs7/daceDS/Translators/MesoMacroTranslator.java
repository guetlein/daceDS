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

package eu.fau.cs7.daceDS.Translators;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;
import java.util.Random;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.Consumer;
import eu.fau.cs7.daceDS.Component.ConsumerCallback;
import eu.fau.cs7.daceDS.Component.OrchestrationHandler;
import eu.fau.cs7.daceDS.Component.Producer;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;
import eu.fau.cs7.daceDS.Component.TimeSync;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.DemoDouble;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.MacroPath;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.Scenario;
import eu.fau.cs7.daceDS.Component.Translator;


public class MesoMacroTranslator<A,B> extends Translator<A, B> {


	private Map<String, Long > arrivalEvents = new HashMap<String, Long >();
	private Map<String, Macro > edgeValues = new HashMap<String, Macro >();
	private Map<String, Map<Long,Meso>> arrivedVehicles = new HashMap<String, Map<Long,Meso>>();
	private long macroStepLength = 900000; //todo: get from sce
	private OrchestrationHandler orchestrationHandler;


	public MesoMacroTranslator(String scenarioID, String instanceID, String layerA,
			String layerAProvisionPrePattern, String layerAProvisionPostPattern, String layerB,
			String layerBProvisionPrePattern, String layerBProvisionPostPattern, String demoScenario) {
		super(scenarioID, instanceID, layerA, layerAProvisionPrePattern, layerAProvisionPostPattern,
			 layerB, layerBProvisionPrePattern, layerBProvisionPostPattern, demoScenario);
	}
	
	public static void main(String[] args) {

		if(args.length < 2) {
			System.out.println("Usage exe scenarioID instanceID [demo]");
			return;
		}
		String scenarioID =  args[0];
		String instanceID = args[1];
		String demoScenario = "";

		if(args.length == 3) {
			demoScenario = "/home/guetlein/Code/git/daceDS/Scenario/Demo/Translation_mesomacro.sce";
		}

		String layerA = "meso";
		String layerB = "macro";
		String layerAProvisionPrePattern = "link\\.";
		String layerAProvisionPostPattern = "\\.vehicles";
		String layerBProvisionPrePattern = "link\\.";
		String layerBProvisionPostPattern = "";
		
		MesoMacroTranslator instance = new MesoMacroTranslator<Meso, Macro>(scenarioID, instanceID, layerA, layerAProvisionPrePattern, layerAProvisionPostPattern,
			 layerB, layerBProvisionPrePattern, layerBProvisionPostPattern, demoScenario);
		
		instance.run();
		 
		 
	}
	
	@Override
	public void preInit() {
		System.out.println("MicroMesoTranslator is started!");
		orchestrationHandler = new OrchestrationHandler(scenarioID, instanceID);
		
		String configPath = "";
		if(demoScenario.length() == 0) {
			orchestrationHandler.gatherScenarioData(); //returns wenn scenario ack is received and .ver file is received
			scenario = orchestrationHandler.getScenario();
			translatorDescription = orchestrationHandler.getTranslator();
		} else {
			scenario = ScenarioUtils.getDemoScenario(demoScenario);
			scenario.setScenarioID(scenarioID);
			translatorDescription = ScenarioUtils.getTranslatorFromScenario(instanceID, scenario);
		}
		



		System.out.println("Creating domain Helpers!");
		domainHelperLayerA = new DomainHelperTraffic(null, translatorDescription.getResponsibilitiesA());
		System.out.println("translatorDescription.getResponsibilitiesA() = "+translatorDescription.getResponsibilitiesA());
		
		domainHelperLayerB = new DomainHelperTraffic(null, translatorDescription.getResponsibilitiesB());
		System.out.println("translatorDescription.getResponsibilitiesB() = "+translatorDescription.getResponsibilitiesB());
		
		if(demoScenario.length() == 0) {
			String mesoMapPath = orchestrationHandler.getAbsolutePathByType("MesoNetwork");
			if(mesoMapPath.length()>0) {
				((DomainHelperTraffic)domainHelperLayerA).initMesoMap(mesoMapPath);
				((DomainHelperTraffic)domainHelperLayerB).initMesoMap(mesoMapPath);
			}
		} else {
			((DomainHelperTraffic)domainHelperLayerA).initMesoMap("/home/guetlein/Code/git/daceDS/Szenario/matsim/Town01_extended.network.xml");
			((DomainHelperTraffic)domainHelperLayerB).initMesoMap("/home/guetlein/Code/git/daceDS/Szenario/matsim/Town01_extended.network.xml");
		}
		
		((DomainHelperTraffic)domainHelperLayerA).setInternalResponsibilitiesForMeso();
		System.out.println("domainHelperLayerA.getInternalResponsibilities() = "+domainHelperLayerA.getInternalResponsibilities());
		((DomainHelperTraffic)domainHelperLayerB).setInternalResponsibilitiesForMacro();
		System.out.println("domainHelperLayerB.getInternalResponsibilities() = "+domainHelperLayerB.getInternalResponsibilities());
	}
	
	public String getPreTopic() {
		return "provision.simulation."+scenarioID+".traffic.";
	}
	
	
	int poissonRandomNumber(double lambda) {
	    double L = Math.exp(-lambda);
	    int k = 0;
	    double p = 1;
	    do {
	        k = k + 1;
	        double u = Math.random();
	        p = p * u;
	    } while (p > L);
	    return k - 1;
	}
	
	public void sampleFromMacro() {
       /*
        *  macro -> meso
        */
        List<Meso>mesoList = new ArrayList<Meso>();
    
//        for (Entry<String, Long> e : arrivalEvents.entrySet()) {
//        	String edgeID = e.getKey();
//        	Long arrivalTime = e.getValue();
//        	Macro macro = edgeValues.get(edgeID);
//
//            if (arrivalTime <= TimeSync.getLocaltime()) {
//            	//arrivalTime is an already calculated and valid arrival time
//            	if(arrivalTime > 0) {
//            		System.out.println("sending out event for macroEdge ="+edgeID+" has expired");                                      
//                    Meso meso = sampleMeso(macro);
//                    mesoList.add(meso);
//            	}
//            	
//            	// calculate next arrival time for that edge
//            	long newArrivaltime = -1;
//                if(macro.getFlow() > 0.0001) {
//                    newArrivaltime = getNextArrivalInMs(macro.getFlow());
//                    System.out.println("sampling new arrival time for road ="+edgeID+" -> "+newArrivaltime+ "ms");
//                }
//                arrivalEvents.put(edgeID, newArrivaltime);
//            }
//        }
//        

        
        for (Entry<String, Macro> e : edgeValues.entrySet()) {
        	String edgeID = e.getKey();
        	Macro macro = e.getValue();

            if(macro.getFlow() > 0.0001) {
            	double lmbda = macro.getFlow() / (60.0 * 60.0); //arrivals per s //todo: assuming 1s step size
            	int arrivals = poissonRandomNumber(lmbda);
            	for(int i=0;i<arrivals;i++) {
                	 Meso meso = sampleMeso2(macro,i);
                    mesoList.add(meso);
            	}
            }
        
        }
        
        //send out the collected arrivals for each edge, don't send anything if list is empty

        for (Meso meso : mesoList){                
                sendA(null, (A)meso);                       
        }
                    
                    
    
        /*
         *  meso -> macro
         */
        if(TimeSync.getLocaltime() % macroStepLength == 0) {
            System.out.println("averaging meso data for macro, got data for " + (arrivedVehicles.size())+" roads");
            
			for (Entry<String, Map<Long, Meso>> e : arrivedVehicles.entrySet()) {
            	String edgeID = e.getKey();
            	Map<Long, Meso> edgeList = e.getValue();
                
                //first just update flow
                int numVehicles = edgeList.size();
                double flow = numVehicles * (macroStepLength / (1000*60*60));
                System.out.println("updated flow for"+edgeID+"is"+flow);
                Macro macro = new Macro();
                
                macro.setLinkID(((DomainHelperTraffic)domainHelperLayerA).mesoLink2MacroLink(edgeID));
                macro.setFlow(flow);
                macro.setSpeed(flow);
                macro.setDensity(flow);
                
                sendB(null, (B)macro);     
                    
                arrivedVehicles.get(edgeID).clear(); //keep edge and potentially publish a flow of zero in the next round
        }
        }
	}
	

	public Meso sampleMeso(Macro macro) {
		String mesoLink = ((DomainHelperTraffic)domainHelperLayerA).macroLink2MesoLink(macro.getLinkID().toString()); //mapMapper.getMesoLinkForMacroLink(macro.getLinkID().toString());
		Meso meso = new Meso();
        meso.setVehicleID("veh_"+TimeSync.getLocaltime()+"_"+mesoLink);
        meso.setLink(mesoLink);
        
        ArrayList<CharSequence> csl = new ArrayList<CharSequence>();
        
        List<Long> routeNodes = getRandomRouteByNodes(macro);
        System.out.println("routeNodes: "+routeNodes);
        if(routeNodes==null) {
        	System.out.println("getRandomRouteByNodes returned null, probably path field of macro tuple was empty. add fallback for this case");
        	System.exit(1);
        }
        
        List<CharSequence> routeMeso = ((DomainHelperTraffic)domainHelperLayerA).getMesoRouteForMacroRoute(routeNodes, mesoLink);
        System.out.println("routeMeso: " +routeMeso);

        meso.setRoute(routeMeso);
        meso.setType("car");
        return meso;
	}
	
	public Meso sampleMeso2(Macro macro, int idx) {
		String mesoLink = ((DomainHelperTraffic)domainHelperLayerA).macroLink2MesoLink(macro.getLinkID().toString()); 
		Meso meso = new Meso();
        meso.setVehicleID("veh_"+TimeSync.getLocaltime()+"_"+mesoLink+"_"+idx);
        meso.setLink(mesoLink);
        
        ArrayList<CharSequence> csl = new ArrayList<CharSequence>();
        
        List<Long> routeNodes = getRandomRouteByNodes(macro);
        System.out.println("routeNodes: "+routeNodes);
        if(routeNodes==null) {
        	System.out.println("getRandomRouteByNodes returned null, probably path field of macro tuple was empty. add fallback for this case");
        	System.exit(1);
        }
        
        List<CharSequence> routeMeso = ((DomainHelperTraffic)domainHelperLayerA).getMesoRouteForMacroRoute(routeNodes, mesoLink);
        System.out.println("routeMeso: " +routeMeso);

        meso.setRoute(routeMeso);
        meso.setType("car");
        return meso;
	}

	private List<Long> getRandomRouteByNodes(Macro macro) {
		double aggregatedNormalizedFlow = 0;
		double dice = rng.nextDouble();
		for(MacroPath path : macro.getPaths()) {
			double normalizedFlow = path.getFlow()/path.getTotalFlow();
			aggregatedNormalizedFlow += normalizedFlow;
			if(dice < aggregatedNormalizedFlow) {
				return path.getNodes();
			}
		}
		return null;
	}

//	private long getNextArrivalInMs(double flow) {
//		double lmbda = flow / (60 * 60 * 1000); //arrivals per ms
//		double iar = -Math.log(1.0 - Math.random())/lmbda;
//		return (long) Math.ceil(TimeSync.getLocaltime()+iar);
//	}
	
	public long getNextArrivalInMs(double flow) {
		double L = flow / (60 * 60 * 1000); //arrivals per ms
	    return TimeSync.getLocaltime() + (long) Math.ceil((Math.log(1.0-Math.random())/-L));
	}


	@Override
	public void postStepEvent(int t) {
		sampleFromMacro();		
	}
	

	@Override
	protected A translateB2A(ConsumerRecord record) {
		Macro macro = (Macro) record.value();
		System.out.println("refreshing macro values for "+macro.getLinkID());        
		//we cannot directly translate and send out converted tuple
        //just update info for the sampling loop
		if(!arrivalEvents.containsKey(macro.getLinkID().toString())){ //preserve old iar
			arrivalEvents.put(macro.getLinkID().toString(),-1L);
		}
		edgeValues.put(macro.getLinkID().toString(),macro); //save updates values for next iar calc
		return null;
	}

	@Override
	protected B translateA2B(ConsumerRecord record) {
		Meso meso = (Meso) record.value();
		System.out.println("received Meso");
		System.out.println("aggregating values for "+record.key());
        if(!arrivedVehicles.containsKey(meso.getLink().toString())){
        	arrivedVehicles.put(meso.getLink().toString(), new HashMap<Long,Meso>());
        }
    	arrivedVehicles.get(meso.getLink().toString()).put(record.timestamp(), meso);
		return null;
	}
	
	@Override
	public String getProvisionTopicLayerA(A a) {
		Meso meso = (Meso) a;
		return "link."+Config.escapeSpecialChars(meso.getLink().toString())+".vehicles";
	}
	
	@Override
	public String getProvisionTopicLayerB(B b) {
		Macro macro = (Macro) b;
		return "link."+Config.escapeSpecialChars(macro.getLinkID().toString());
	}



}