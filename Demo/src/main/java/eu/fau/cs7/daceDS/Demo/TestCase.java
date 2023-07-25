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


import eu.fau.cs7.daceDS.Component.Instance;
import eu.fau.cs7.daceDS.datamodel.DemoDouble;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;


public class TestCase  {


	public static void main(String[] args) {

		String scenarioID = "sce"+((int)(Math.random()*1000));
		String demoScenario = "daceDS/Scenario/Demo/Translation.sce";

		String provisionPrePattern = "container\\.";
		String provisionPostPattern = "\\.variables";
		

		Instance sim1 = new IntegerSimulator(scenarioID, "a", provisionPrePattern,provisionPostPattern, demoScenario);
		Instance sim2 = new DoubleSimulator(scenarioID, "b", provisionPrePattern,provisionPostPattern, demoScenario);
		
		String layerA = "integers";
		String layerB = "doubles";
		String layerAProvisionPrePattern = "container\\.";
		String layerAProvisionPostPattern = "\\.variables";
		String layerBProvisionPrePattern = "container\\.";
		String layerBProvisionPostPattern = "\\.variables";
		Instance translator = new IntegerDoubleTranslator<DemoInteger, DemoDouble>(scenarioID, "c", layerA, layerAProvisionPrePattern, layerAProvisionPostPattern,
				 layerB, layerBProvisionPrePattern, layerBProvisionPostPattern, demoScenario);

		Runnable r1 = new Runnable() {
			public void run() {
				sim1.run();
			}
		};
		Runnable r2 = new Runnable() {
			public void run() {
				sim2.run();
			}
		};
		Runnable r3 = new Runnable() {
			public void run() {
				translator.run();
			}
		};

		Thread t1 = new Thread(r1);
		Thread t2 = new Thread(r2);
		Thread t3 = new Thread(r3);

		t1.start();
		t2.start();
		t3.start();
		
		try {
			t1.join();
			t2.join();
			t3.join();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		 
		 
	}
}
