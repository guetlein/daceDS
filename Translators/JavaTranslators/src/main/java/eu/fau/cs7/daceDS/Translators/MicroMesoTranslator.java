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

import org.apache.kafka.clients.consumer.ConsumerRecord;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.OrchestrationHandler;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;
import eu.fau.cs7.daceDS.Component.Translator;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.Micro;
import eu.fau.cs7.daceDS.datamodel.Vec3;


public class MicroMesoTranslator<A,B> extends Translator<A,B> {


	private OrchestrationHandler orchestrationHandler;

	public MicroMesoTranslator(String scenarioID, String instanceID, String layerA, String layerAProvisionPrePattern,
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

		String layerA = "micro";
		String layerB = "meso";
		String layerAProvisionPrePattern = "edge\\.";
		String layerAProvisionPostPattern = "\\.vehicles";
		String layerBProvisionPrePattern = "link\\.";
		String layerBProvisionPostPattern = "\\.vehicles";
		
		MicroMesoTranslator instance = new MicroMesoTranslator<Micro, Meso>(scenarioID, instanceID, layerA, layerAProvisionPrePattern, layerAProvisionPostPattern,
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


		String mesoMapPath1 = orchestrationHandler.getAbsolutePathByType("MesoNetwork");
		String mesoMapPath2 = orchestrationHandler.getAbsolutePathByType("Meso.RoadMap");
		String mesoMapPath3 = orchestrationHandler.getAbsolutePathByType("DomainReference");
		String mesoMapPath4 = orchestrationHandler.getAbsolutePathByType("Micro.RoadMap");
		
		if(mesoMapPath1.length()>0) {
			((DomainHelperTraffic)domainHelperLayerA).initMesoMap(mesoMapPath1);
			((DomainHelperTraffic)domainHelperLayerB).initMesoMap(mesoMapPath1);
		}
		else if(mesoMapPath2.length()>0) {
			((DomainHelperTraffic)domainHelperLayerA).initMesoMap(mesoMapPath2);
			((DomainHelperTraffic)domainHelperLayerB).initMesoMap(mesoMapPath2);
		}
		
		((DomainHelperTraffic)domainHelperLayerA).setInternalResponsibilitiesForMicro();
		System.out.println("domainHelperLayerA.getInternalResponsibilities() = "+domainHelperLayerA.getInternalResponsibilities());

		((DomainHelperTraffic)domainHelperLayerB).setInternalResponsibilitiesForMeso();		
		System.out.println("domainHelperLayerB.getInternalResponsibilities() = "+domainHelperLayerB.getInternalResponsibilities());
		
	}
		
	

    public B translateA2B(ConsumerRecord record) {
//    	log("translateA2B");
    	Micro micro = (Micro) record.value();
        Meso meso = new Meso();
        meso.setVehicleID(micro.getVehicleID());
        meso.setLink(micro.getEdge());
        meso.setRoute(micro.getRoute());
        meso.setType(micro.getType());
        return (B) meso;
    }

    //todo: get num of lanes
    int c=0;
    
    protected A translateB2A(ConsumerRecord record) {
//    	log("translateB2A");
    	Meso meso = (Meso) record.value();

        Micro micro = new Micro();
        micro.setVehicleID(meso.getVehicleID()); 
        micro.setEdge(meso.getLink()); 
        
        //strip old edges
        boolean routeStarted = false;
        ArrayList<CharSequence> route = new ArrayList<CharSequence>();
        for(CharSequence edge : meso.getRoute()) {
        	if(edge.toString().equals(micro.getEdge().toString())){
        		routeStarted = true;
        	}
        	
        	if(routeStarted) {
        		route.add(edge);
        	}
        }
        micro.setRoute(route); 
        micro.setType(meso.getType());
        
        //todo: sample values
        micro.setAcceleration(-1);
        micro.setAngle(-1);
        
        //todo: query number of lanes and use some reasonable prob. distribution
        int numLanes = 3;
        micro.setLane((c++)%numLanes);
        
        
        Vec3 v = new Vec3();
        v.setX(0);
        v.setY(0);
        v.setZ(0);
        micro.setPosition(v);
        //based on the speed (which we currently don't know) the vehicle might be somewhere between [0, v*1s]
        double p = rng.nextDouble()*7.5;
        micro.setPositionEdge(p);
        micro.setSlope(0) ;
        micro.setSpeed(-1);
        return (A) micro;
    }

	@Override
	protected void processInteraction(ConsumerRecord r) {
		
	}
	
	@Override
	public String getProvisionTopicLayerA(A a) {
		Micro micro = (Micro) a;
		return "edge."+Config.escapeSpecialChars(micro.getEdge().toString())+".vehicles";
	}
	
	@Override
	public String getProvisionTopicLayerB(B b) {
		Meso meso = (Meso) b;
		return "link."+Config.escapeSpecialChars(meso.getLink().toString())+".vehicles";
	}



}