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

import java.util.ArrayList;

class VisumPath {
	public VisumPath(int fromZone, int toZone, int pathID, double flow, ArrayList<Integer> nodes) {
		this.setFromZone(fromZone);
		this.setToZone(toZone);
		this.setPathID(pathID);
		this.setFlow(flow);
		this.setNodes((ArrayList<Integer>) nodes.clone());
	}	
	
	public VisumPath(String id) {
		String[] e = id.split("_");
		if(e.length==3) {
			this.setFromZone(Integer.valueOf(e[0]));
			this.setToZone(Integer.valueOf(e[1]));
			this.setPathID(Integer.valueOf(e[2]));
		} else {
			System.out.println("MALFORMED? cannot create regular path from '"+id+"'");
		}
	}
	private int pathID = -1;
	private int fromZone = -1;
	private int toZone = -1;
	private double flow = -1.;
	private double totalFlow = 1;
	private ArrayList<Integer> nodes;
	
	//todo: is it possible to have multiple links between a and b? if yes, include id
	public String getEdgeID() {
		return getFromZone()+"_"+getToZone()+"_"+getPathID();
	}

	public int getPathID() {
		return pathID;
	}

	public void setPathID(int pid) {
		this.pathID = pid;
	}

	public int getFromZone() {
		return fromZone;
	}

	public void setFromZone(int fromZone) {
		this.fromZone = fromZone;
	}

	public int getToZone() {
		return toZone;
	}

	public void setToZone(int toZone) {
		this.toZone = toZone;
	}
	public double getFlow() {
		return flow;
	}

	public void setFlow(double f) {
		this.flow = f;
	}

	public void setTotalFlow(double totalFlow) {
		this.totalFlow  = totalFlow;
	}
	
	public double getProbability() {
		return flow/totalFlow;
	}

	public ArrayList<Integer> getNodes() {
		return nodes;
	}

	public void setNodes(ArrayList<Integer> nodes) {
		this.nodes = nodes;
	}

}