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

class VisumTurn {
	public VisumTurn(String fromNode2, String viaNode2, String toNode2, double cap, double vol) {
		this.fromNode=fromNode2;
		this.viaNode=viaNode2;
		this.toNode=toNode2;
	}
	public String viaNode;
	public String fromNode;
	public String toNode;

	public double cap;
	public double vol; 
	
	public String getID() {
		return fromNode+"_"+viaNode+"_"+toNode;
	}
	

}