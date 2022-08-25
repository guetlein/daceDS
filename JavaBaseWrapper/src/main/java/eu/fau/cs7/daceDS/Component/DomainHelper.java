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
import java.util.ArrayList;
import java.util.List;


public class DomainHelper {

	protected List<String> externalResponsibilities = new ArrayList<String>(); //e.g., nodes
	protected List<String> internalResponsibilities = new ArrayList<String>(); //e.g., edges
	
	public void setInternalResponsibilities(List<String> internalResponsibilities) {
		this.internalResponsibilities = internalResponsibilities;
	}

	public List<String> getInternalResponsibilities(){
		return internalResponsibilities;
	}
	
	public boolean isResponsibleExternal(String toNode) {
		for(CharSequence node : getExternalResponsibilities()) {
			if(node.toString().equals(toNode)) {
				return true;
			}
		}
		return false;
	}	
	
	public boolean isResponsibleInternal(String toNode) {
		for(CharSequence node : internalResponsibilities) {
			if(node.toString().equals(toNode)) {
				return true;
			}
		}
		return false;
	}

	public List<String> getExternalResponsibilities() {
		return externalResponsibilities;
	}

	public void setExternalResponsibilities(List<String> externalResponsibilities) {
		this.externalResponsibilities = externalResponsibilities;
	}

}