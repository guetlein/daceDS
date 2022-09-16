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

import java.util.ArrayList;
import java.util.List;

import org.apache.kafka.clients.consumer.ConsumerRecord;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.OrchestrationHandler;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;
import eu.fau.cs7.daceDS.Component.Translator;
import eu.fau.cs7.daceDS.datamodel.Micro;
import eu.fau.cs7.daceDS.datamodel.Sensor;
import eu.fau.cs7.daceDS.datamodel.SubMicro;
import eu.fau.cs7.daceDS.datamodel.Vec3;
import eu.fau.cs7.daceDS.datamodel.Waypoint;


public class SubmicroMicroTranslator<A,B> extends Translator<A,B> {


	private OrchestrationHandler orchestrationHandler;

	public SubmicroMicroTranslator(String scenarioID, String instanceID, String layerA, String layerAProvisionPrePattern,
			String layerAProvisionPostPattern, String layerB, String layerBProvisionPrePattern,
			String layerBProvisionPostPattern, String demoScenario) {
		super(scenarioID, instanceID, layerA, layerAProvisionPrePattern, layerAProvisionPostPattern, layerB,
				layerBProvisionPrePattern, layerBProvisionPostPattern, demoScenario);
	}

	public static void main(String[] args) {

		if(args.length < 2) {
			System.out.println("Usage exe scenarioID instanceID [demo]");
			return;
		}
		String scenarioID = args[0];
		String instanceID = args[1];
		String demoScenario = "";

		if(args.length == 3) {
			demoScenario = "/daceDS/Scenario/Demo/Translation_micromeso.sce";
		}

		String layerA = "submicro";
		String layerB = "micro";
		String layerAProvisionPrePattern = "lane\\.";
		String layerAProvisionPostPattern = "\\.vehicles";
		String layerBProvisionPrePattern = "edge\\.";
		String layerBProvisionPostPattern = "\\.vehicles";
		
		SubmicroMicroTranslator instance = new SubmicroMicroTranslator<SubMicro,Micro>(scenarioID, instanceID, layerA, layerAProvisionPrePattern, layerAProvisionPostPattern,
			 layerB, layerBProvisionPrePattern, layerBProvisionPostPattern, demoScenario);
		
		instance.run();
		 
		 
	}

	@Override
	public void preInit() {
		
		System.out.println("SubmicroMicroTranslator is started!");
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


		String microMapPath = orchestrationHandler.getAbsolutePathByType("MicroNetwork");
		if(microMapPath.length()>0) {
			((DomainHelperTraffic)domainHelperLayerA).initMicroMap(microMapPath);
			((DomainHelperTraffic)domainHelperLayerB).initMicroMap(microMapPath);
		} else if(demoScenario.length()>0 ) {
			microMapPath = "/daceDS/Scenario/Demo/Town01_extended.net.xml";
			((DomainHelperTraffic)domainHelperLayerA).initMicroMap(microMapPath);
			((DomainHelperTraffic)domainHelperLayerB).initMicroMap(microMapPath);
		}
		
		((DomainHelperTraffic)domainHelperLayerA).setInternalResponsibilitiesForSubmicro();
		System.out.println("domainHelperLayerA.getInternalResponsibilities() = "+domainHelperLayerA.getInternalResponsibilities());

		((DomainHelperTraffic)domainHelperLayerB).setInternalResponsibilitiesForMicro();		
		System.out.println("domainHelperLayerB.getInternalResponsibilities() = "+domainHelperLayerB.getInternalResponsibilities());
		
	}
		
	

    public B translateA2B(ConsumerRecord record) {
    
//    	log("translateA2B");
    	SubMicro submicro = (SubMicro) record.value();
    	Micro micro = new Micro();
	    micro.setVehicleID(submicro.getVehicleID()); 
	
	  	String edge = ((DomainHelperTraffic)domainHelperLayerA).subMicroRoad2MicroEdge(submicro.getLane().toString());
	  	if(edge == null) {
	  		System.out.println("did not found micro edge for submicro edge "+submicro.getRoad());
	  		return null;
	  	} else {
	  		micro.setEdge(edge);
	  	}
		micro.setLane(Math.abs(submicro.getLane())); //invert?
		  
		ArrayList<CharSequence> route = new ArrayList<CharSequence>();
		for(Waypoint w : submicro.getRoute()) {
			String s = ((DomainHelperTraffic)domainHelperLayerA).subMicroRoad2MicroEdge(submicro.getLane().toString());
		  	if(s == null) {
		  		System.out.println("did not found micro edge fpr submicro edge "+w.getRoadId());
		  	} else {
		  		route.add(s);
		  	}
		}
		micro.setRoute(route); 
		micro.setType(submicro.getType());
		
		double a = Math.sqrt(submicro.getAcceleration().getX()*submicro.getAcceleration().getX()+submicro.getAcceleration().getY()*submicro.getAcceleration().getY()+submicro.getAcceleration().getZ()+submicro.getAcceleration().getZ());
		micro.setAcceleration(a);
		micro.setAngle(0.0); //todo
		
		micro.setPosition(submicro.getPosition());
		micro.setPositionEdge(0.0); //todo
		micro.setSlope(0.0) ;  //todo
		  
		micro.setSpeed(submicro.getSpeed());
		return (B)micro;
    }

    protected A translateB2A(ConsumerRecord record) {

//    	log("translateB2A");
    	Micro micro = (Micro) record.value();

    	SubMicro submicro = new SubMicro();
    	submicro.setVehicleID(micro.getVehicleID());

    	String roadstr =  ((DomainHelperTraffic)domainHelperLayerA).microEdge2SubMicroRoad(micro.getEdge().toString());
    	long ri = Long.valueOf(roadstr.split("_")[0]);
    	int li = Integer.valueOf(roadstr.split("_")[1]);
//    	submicro.setLane(ri+"-"+li); //pre
    	submicro.setRoad(ri); 
    	submicro.setLane(li); 
    	
        List<Waypoint> route = new ArrayList<Waypoint>();
        for(CharSequence edge : micro.getRoute()) {
        	String tmp =  ((DomainHelperTraffic)domainHelperLayerA).microEdge2SubMicroRoad(edge.toString());
        	if(tmp.length()>0) {
        		try {
		        	long r = Long.valueOf(tmp.split("_")[0]);
		        	int l = Integer.valueOf(tmp.split("_")[1]);
		        	Waypoint w = new Waypoint();
		        	w.setLaneId(l); 
		        	w.setRoadId(r);
		        	w.setS(0); //todo
        	       	route.add(w);
        		} catch (Exception e) {
        			
        		}
        	}
        	else {
        		System.out.println("no corresponding road found, trying to skip element for the route");
        	}
        }
    	submicro.setRoute(route);
    	submicro.setType(micro.getType());
    	submicro.setSpeed(micro.getSpeed());   
    	submicro.setPosition(micro.getPosition());
    	submicro.setRotation(new Vec3(micro.getSlope() ,micro.getAngle(), 0. ));
    	
    	submicro.setVelocity(new Vec3(0.,0.,0.)); //if these are all zero, we will use speed
    	submicro.setAcceleration(new Vec3(0.,0.,0.)); //if these are all zero, we will use physics engine
    	
    	submicro.setSensor(new ArrayList<Sensor>());
    	submicro.setBrake(0.0);
    	submicro.setThrottle(0.0);
    	submicro.setGear(0.0);
    	submicro.setSteer(0.0);
    	    	
        return (A) submicro;
    }

	@Override
	protected void processInteraction(ConsumerRecord r) {
		
	}
	
	@Override
	public String getProvisionTopicLayerA(A a) {
		SubMicro submicro = (SubMicro) a;
		return "lane."+Config.escapeSpecialChars(submicro.getRoad()+"_"+submicro.getLane())+".vehicles";
	}
	
	@Override
	public String getProvisionTopicLayerB(B b) {
		Micro micro = (Micro) b;
		return "edge."+Config.escapeSpecialChars(micro.getEdge().toString())+".vehicles";
	}


}