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
package eu.fau.cs7.daceDS.Component;

import org.apache.log4j.Logger;


public abstract class Projector extends Instance{

	Logger logger = Logger.getLogger("Projector");

	protected eu.fau.cs7.daceDS.datamodel.Projector projectorDescription;
	
	
	protected int epoch;
	protected int t;
	
	public Projector(String scenarioID, String instanceID, String demoScenario){
		this.scenarioID = scenarioID;
		this.instanceID = instanceID;
		this.demoScenario = demoScenario;
	}
	
	public void init() {		

	}
	
	public abstract void preLoopEvent();
	public abstract void postStepEvent(int t);
	public abstract void stepEvent(int t);
	public abstract void preSimStepEvent();

	public void close(){		
		super.close();
	}

}