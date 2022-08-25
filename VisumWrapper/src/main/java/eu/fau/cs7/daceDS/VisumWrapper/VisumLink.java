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

class VisumLink {
	public VisumLink(String id2, Long fromNode2, Long toNode2) {
		this.setId(id2);
		this.setFromNode(fromNode2);
		this.setToNode(toNode2);
	}	
	
	public VisumLink(String id) {
		this.setId(id);
		String[] e = id.split("_");
		if(e.length==3) {
			id= e[0];
			this.setFromNode(Long.valueOf(e[1]));
			this.setToNode(Long.valueOf(e[2]));
		}
		else if(e.length==2) {
			this.setFromNode(Long.valueOf(e[0]));
			this.setToNode(Long.valueOf(e[1]));
		} else {
			System.out.println("MALFORMED? cannot create regular link from '"+id+"'");
		}
	}
	private String id;
	private Long fromNode = -1L;
	private Long toNode = -1L;
	
	//todo: is it possible to have multiple links between a and b? if yes, include id
	public String getEdgeID() {
		return getFromNode()+"_"+getToNode();
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public Long getFromNode() {
		return fromNode;
	}

	public void setFromNode(Long fromNode) {
		this.fromNode = fromNode;
	}

	public Long getToNode() {
		return toNode;
	}

	public void setToNode(Long toNode) {
		this.toNode = toNode;
	}

}