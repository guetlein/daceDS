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

 package eu.fau.cs7.daceDS.MatsimWrapper;

import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Meso;
import eu.fau.cs7.daceDS.datamodel.Observer;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.log4j.Logger;
import org.matsim.api.core.v01.Id;
import org.matsim.api.core.v01.network.Link;
import org.matsim.core.mobsim.qsim.QSim;
import org.matsim.core.mobsim.qsim.interfaces.MobsimVehicle;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.Component.ObserverI;
import eu.fau.cs7.daceDS.Component.TimeSync;


public class MatsimObserver extends ObserverI{


	private ProducerImplKafka<Meso> mesoWriter;
	private String id;
	private ProducerImplKafka<String> stringWriter;
	private QSim qsim;


	MatsimObserver(String scenarioID, BB instance, QSim qsim) {
		super(scenarioID, instance);
		id = scenarioID+"_"+instanceID;
		this.qsim = qsim;
	}

	public final static Logger logger = Logger.getLogger(MatsimObserver.class.getName());


	public void init() {
		mesoWriter = new ProducerImplKafka<Meso>(id+"_mesoWriter", true);	
		mesoWriter.init();
		stringWriter = new ProducerImplKafka<String>(id+"_sWriter", true);	
		stringWriter.initPlainString();
	}

	public void close() {
		mesoWriter.close();
		stringWriter.close();
	}

	
	/* link on persistent topic, veh not*/
	protected void processObserver(Observer o) {


		long simtimeInMS = TimeSync.getLocaltime();
		List<String> subjects = new ArrayList<String>();
		if(o.getFilter().toString().strip().length()>0) {
			subjects = Arrays.asList(o.getFilter().toString().strip().split(" "));
		}
		
		//LINK SCOPE
		final String linkScope = "link";
		final String vehicleScope = "vehicle";
		if(o.getElement().toString().startsWith(linkScope)) {
			String e = "";
			if(o.getElement().length()>linkScope.length()+1) {
				e = o.getElement().toString().substring(linkScope.length()+1);
			}
			
			for(Id<Link> link : qsim.getScenario().getNetwork().getLinks().keySet()) {
				
				//are we filtering entity ids?
				if(subjects.size()>0 && !subjects.contains(link.toString())){
					continue;
				}
				
				//whole link
				if( e.equals("") ) {

					double flow = MatsimAPI.getFlow(link.toString());
					double speed = MatsimAPI.getSpeed(link.toString());
					int num = MatsimAPI.getCurrentVehicleNumber(link.toString());
					ArrayList<String> vis = MatsimAPI.getVehicleIds(link.toString());
					double occ = MatsimAPI.getOccupancy(link.toString());
					
					
					String topic = Config.getProvisionTopic(scenarioID, instance, "link."+link.toString());
					
					String linkJSON = "{ \"linkID\" : \""+link.toString()+"\","+
							"\"flow\" : "+flow+","+
							"\"occ\" : "+occ+","+
							"\"speed\" : "+speed+","+
							"\"num\" : "+num+","+
							"\"length\" : "+ MatsimAPI.getLength(link.toString()) +","+
							"\"loadIndicator\" : "+MatsimAPI.getLoadIndicator(link.toString()) +","+
							"\"storageCapacity\" : "+MatsimAPI.getStorageCapacity(link.toString()) +","+
							"\"vids\" : [";
					
					for (int i=0;i<vis.size();i++) {
						String vid = vis.get(i);
						linkJSON += "\""+vid+"\"";
						if(i<vis.size()-1) {
							linkJSON+=",";
						}
					}
					
					linkJSON += "]}";
					
					stringWriter.publish(topic, linkJSON, simtimeInMS);
//
					
				} 
				else if (e.equals("flow")) {
					System.out.print("Flow: "+MatsimAPI.getFlow(link.toString()));
				}
				else if (e.equals("speed")) {
					System.out.print("speed: "+MatsimAPI.getSpeed(link.toString()));
				}
				else if (e.equals("vehicleIDs")) {
					System.out.print("vehicleIDs: "+MatsimAPI.getVehicleIds(link.toString()));
				}
				else if (e.equals("vehicles")) {
					System.out.print("vehicles: "+MatsimAPI.getVehiclesMeso(link.toString()));
				}
			}
		}
		
		
		//VEHICLE SCOPE
		else if(o.getElement().toString().startsWith(vehicleScope)) {
//			System.out.println("vehicle scope");
			String e = "";
			if(o.getElement().length()>vehicleScope.length()+1) {
				e = o.getElement().toString().substring(vehicleScope.length()+1);
			}
			
			for(MobsimVehicle m : qsim.getVehicles().values()){
				
				String vid = m.getId().toString();
				
				//active vehicle?
				if( m == null || m.getCurrentLink() == null || MatsimAPI.getVehicleRoute(vid) == null) {
					System.out.println(vid + " is not active, skipping.");
					continue;
				}
				
				//are we filtering entity ids?
				if(subjects.size()>0 && !subjects.contains(vid)){
					System.out.println(vid + " is not matching filter, skipping.");
					continue;
				}
						
				//whole vehicle
				if( e.equals("") ) {
			
					Meso meso = MatsimAPI.getMeso(m);
					if(meso==null) {
						continue;
						//todo: remove vehicles that left
					}
//					if(o.getType().toString().equals(Config.SERIALIZATION_AVRO)) {
//						logger.info("publishing "+ meso.getVehicleID().toString()+ " to " + Config.getProvisionTopic(scenarioID, instance, "vehicle"));
//						mesoWriter.publish(Config.getProvisionTopic(scenarioID, instance, "vehicle"), meso.getVehicleID().toString(), meso, simtimeInMS, 0, id+"_mesoWriter"); //todo: infer topic
//					} else if(o.getType().toString().equals(Config.SERIALIZATION_JSON)) {
//						String routeStr = "";
//						for(CharSequence c : meso.getRoute()) {
//							routeStr += c.toString() + " ";
//						}
//						String mesoJSON = "{ vehicleID : \""+meso.getVehicleID().toString()+"\","+
//								"edge : \""+meso.getEdge().toString()+"\","+
//								"route : \""+routeStr+"\","+
//								"type : \""+meso.getType().toString()+"\"}";
//
//						logger.info("publishing "+ meso.getVehicleID().toString()+ " to " + Config.getProvisionTopic(scenarioID, instance, "vehicle"));
//						stringWriter.publish(Config.getProvisionTopic(scenarioID, instance, "vehicle"), meso.getVehicleID().toString(), mesoJSON, simtimeInMS, 0, id+"_sWriter"); //todo: infer topic
//					} else {
//						logger.info("processObservers: " + o.getType().toString() + " is not known as ser type");
//					}
					System.out.println("vehicle: " +meso);
				}
				else if (e.equals("link")) {
					System.out.print("Link: "+MatsimAPI.getVehicleLink(vid));
				}
				else if (e.equals("route")) {
					System.out.print("route: "+MatsimAPI.getVehicleRoute(vid).getRouteDescription());
				}
				else if (e.equals("type")) {
					System.out.print("type: "+MatsimAPI.getVehicleType(vid));
				}


			}
		}
		else {
			System.out.println("NO IMPL FOR SCOPE: "+o.getElement().toString());
		}

	}

}