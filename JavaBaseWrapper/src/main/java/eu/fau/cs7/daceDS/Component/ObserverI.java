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

import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Observer;

/*
 * Has logic to extract information from simulators and provides kafka writers to publish these infos. 
 */
public abstract class ObserverI {
	protected String scenarioID = "";
	protected String instanceID = "";
	protected BB instance;
	
	protected ObserverI(String scenarioID, BB instance){
		this.scenarioID = scenarioID;
		this.instance = instance;
		this.instanceID = instance.getInstanceID().toString();
	}
	protected abstract void processObserver(Observer o);
}
