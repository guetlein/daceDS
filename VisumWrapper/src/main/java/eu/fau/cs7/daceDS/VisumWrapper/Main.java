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


public class Main {

	
	public static void main(String[] args) {
		if(args.length < 2) {
			System.out.println("Usage exe scenarioID simulatorID");
			return;
		}
		String scenarioID = args[0];
		String instanceID = args[1];
		
		//todo: infer automatically from domain
		String provisionPrePattern = "link\\.";
		String provisionPostPattern = "";
		String demoScenario = "";
		
		if(args.length == 3) {
			demoScenario = args[2]; //"/daceDS/Scenario/Demo/Visum.sce";
		}
		try {
			System.out.println("Starting VisumWrapper for sce="+scenarioID+" with instID="+instanceID);
			VisumWrapper v = new VisumWrapper(scenarioID, instanceID, provisionPrePattern,provisionPostPattern, demoScenario);
			v.run();
		}
		catch(Exception e) {
			System.out.println(e);
		}
	}
}
		

	