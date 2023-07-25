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


public class NumbersReference {
	
	private String id;
	private int lowerBound;
	private int upperBound;
	
	public NumbersReference(String id, int l, int u) {
		this.id=id;
		this.lowerBound=l;
		this.upperBound=u;
	}
	public String getID() {
		return id;
	}	
	public int getLowerBound() {
		return lowerBound;
	}	
	public int getUpperBound() {
		return upperBound;
	}


}
