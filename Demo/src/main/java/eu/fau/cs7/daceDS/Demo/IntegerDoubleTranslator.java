/*******************************************************************************
 * Copyright 2022 Moritz Gütlein
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

package eu.fau.cs7.daceDS.Demo;


import java.util.ArrayList;

import org.apache.kafka.clients.consumer.ConsumerRecord;

import eu.fau.cs7.daceDS.Component.Translator;
import eu.fau.cs7.daceDS.datamodel.DemoDouble;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;

/*
 *
 */

public class IntegerDoubleTranslator<A,B> extends Translator<A,B>{


	public IntegerDoubleTranslator(String scenarioID, String instanceID, String layerA,
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
		String scenarioID = "gs_translationa111";
		String instanceID = args[1];
		String demoScenario = "";

		if(args.length == 3) {
			demoScenario = "daceDS/Scenario/Demo/TranslationExample.sce";
		}

		String layerA = "integers";
		String layerB = "doubles";
		String layerAProvisionPrePattern = "container\\.";
		String layerAProvisionPostPattern = "\\.variables";
		String layerBProvisionPrePattern = "container\\.";
		String layerBProvisionPostPattern = "\\.variables";
		
		IntegerDoubleTranslator instance = new IntegerDoubleTranslator<DemoInteger, DemoDouble>(scenarioID, instanceID, layerA, layerAProvisionPrePattern, layerAProvisionPostPattern,
			 layerB, layerBProvisionPrePattern, layerBProvisionPostPattern, demoScenario);
		
		instance.run();
		 
		 
	}
	
	@Override
	public void preInit() {
		NumbersReference nr = new NumbersReference("negative", -1000000, 0);
		NumbersReference pr = new NumbersReference("positive", 0, 1000000);
		ArrayList<NumbersReference> references = new ArrayList<NumbersReference>();
		references.add(nr);
		references.add(pr);
		domainHelperLayerA = new DomainHelperNumbers(references, translatorDescription.getResponsibilitiesA());
		domainHelperLayerB = new DomainHelperNumbers(references, translatorDescription.getResponsibilitiesB());
		
	}
	
	public void init() {
		super.init();
		if(demoScenario.length()>0) {
			scenario = Utils.getDemoScenario(demoScenario);
			scenario.setID(scenarioID);
			translatorDescription = Utils.getTranslatorFromScenario(instanceID, scenario);
		}
	}
	

	@Override
	protected B translateA2B(ConsumerRecord record) {
		DemoInteger aa = (DemoInteger) record.value();
		DemoDouble b = new DemoDouble();
		b.setId(aa.getId());
		
		double r = Math.random()-0.5; //todo pseudo
		b.setValue(aa.getValue()+r);
		if (aa.getValue()>=0 && b.getValue() < 0)
			b.setValue(0.123);
		

//		b.setValue(aa.getValue());
		
		log("translateA2B translated "+aa.getValue()+" to "+b.getValue() + " of " +aa.getId());
		return (B) b;
	}

	@Override
	protected A translateB2A(ConsumerRecord record) {
		DemoDouble bb = (DemoDouble) record.value();
		DemoInteger a = new DemoInteger();
		a.setId(bb.getId());
		a.setValue((int) Math.round(bb.getValue()));
		if (bb.getValue()<0 && a.getValue() ==0)
			a.setValue(-1);
		log("translateB2A translated "+bb.getValue()+" to "+a.getValue() + " of " +bb.getId());
		
		return (A) a;
	}


	@Override
	public String getProvisionTopicLayerA(A a) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String getProvisionTopicLayerB(B b) {
		// TODO Auto-generated method stub
		return null;
	}











		


}
