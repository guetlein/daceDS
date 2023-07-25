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

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

import eu.fau.cs7.daceDS.Component.DomainHelper;
import eu.fau.cs7.daceDS.datamodel.DemoDouble;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;

public class DomainHelperNumbers extends DomainHelper {

	Map<String,NumbersReference> allReferenceElements = new HashMap<String,NumbersReference>();

	//references are all in the domain
	//resp my subset of that
	DomainHelperNumbers(List<NumbersReference> references, List<CharSequence> responsibilities){
		this.setExternalResponsibilities(responsibilities.stream()
                .map(String::valueOf)
                .collect(Collectors.toList())); 
		for(NumbersReference reference : references) {
			this.allReferenceElements.put(reference.getID(), reference);
		}
		
		//in this case we use the same resps internally
		for(CharSequence responsibility : responsibilities) {
			internalResponsibilities.add(responsibility.toString());
			System.out.println("added responsibility called "+responsibility.toString());
		}
	}
	
	boolean isResponsibleInternal(DemoInteger number) {
		String layerReference = getLayerReference(number);
		return internalResponsibilities.contains(layerReference);
	}
	boolean isResponsibleInternal(DemoDouble number) {

		String layerReference = getLayerReference(number);
		return internalResponsibilities.contains(layerReference);
	}

	/*
	 * LayerReference ist der layer interne ort andem ndms gesendet und empfangen werden
	 */
	String getLayerReference(DemoInteger number) {
		for(NumbersReference reference : allReferenceElements.values()) {
			int lowerBound = reference.getLowerBound();
			int upperBound = reference.getUpperBound();
			if(number.getValue() >= lowerBound && number.getValue() < upperBound){
				return reference.getID();
			}
		}
		return null;
	}	
	
	String getLayerReference(DemoDouble number) {
		for(NumbersReference reference : allReferenceElements.values()) {
			int lowerBound = reference.getLowerBound();
			int upperBound = reference.getUpperBound();
			if(number.getValue() >= lowerBound && number.getValue() < upperBound){
				return reference.getID();
			}
		}
		return null;
	}
	
}


//return number.getValue() % 2 != instanceNumber;
