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

import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Observer;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.Component.ObserverI;
import eu.fau.cs7.daceDS.Component.TimeSync;


public class VisumObserver extends ObserverI{


	private ProducerImplKafka<Macro> macroWriter;
	private String id;
	private ProducerImplKafka<String> stringWriter;
	private static int stepLengthInMinutes;


	VisumObserver(String scenarioID, BB instance) {
		super(scenarioID, instance);
		id = scenarioID+"_"+instanceID+"_macroWriter";
	}

	public final static Logger logger = Logger.getLogger(VisumObserver.class.getName());


	public void init() {
		macroWriter = new ProducerImplKafka<Macro>(id, true);
		macroWriter.init();
		stringWriter = new ProducerImplKafka<String>(id, true);
		stringWriter.init();
		long stepLengthInS = instance.getStepLength() / 1000;
		stepLengthInMinutes = (int) (stepLengthInS / 60);
	}
	
	public void close() {
		macroWriter.close();
		stringWriter.close();
	}

	protected void processObserver(Observer o) {


		long simtimeInMS = TimeSync.getLocaltime();
		String timeStr = VisumAPI.getTimeString(simtimeInMS);


		logger.info("observer: " + o.getTask()+ " " + o.getFilter());

		Collection<VisumLink> subjects;
		if(o.getFilter().toString().equals(Config.get("observerFilterAll"))) {
			subjects = VisumAPI.getLinks();
			logger.info("match all links");
		}else {
			subjects = new ArrayList<VisumLink>();
			List<String> sList = Arrays.asList(o.getFilter().toString().split(" "));

			logger.info("match only "+o.getFilter());
			for(VisumLink m : VisumAPI.getLinks()) {
				if(sList.contains(m.getEdgeID())){
					subjects.add(m);
				}
			}
		}

		if(subjects.size()==0) {
			logger.info("processObservers: subject list is empty");
			return;
		}

		for(VisumLink link : subjects) {
			String linkID = link.getEdgeID();
			if(o.getElement().toString().equals("link")) {
				Macro macro = VisumAPI.getMacro(linkID,timeStr,stepLengthInMinutes);
				System.out.println(macro);

				if(o.getType().toString().equals(Config.SERIALIZATION_AVRO)) {
					logger.info("publishing "+ macro.getLinkID().toString()+ " to " + Config.getProvisionTopic(scenarioID, instance, "edge"));
					macroWriter.publish(Config.getProvisionTopic(scenarioID, instance, "edge"), macro.getLinkID().toString(), macro, simtimeInMS, 0, id);
				} else if(o.getType().toString().equals(Config.SERIALIZATION_JSON)) {

					String macroJSON = "{ roadID : \""+ macro.getLinkID().toString()+"\","+
							"density : "+macro.getDensity()+","+
							"flow : "+macro.getFlow()+","+
							"speed : "+macro.getSpeed()+"}";

					logger.info("publishing "+ macro.getLinkID().toString()+ " to " + Config.getProvisionTopic(scenarioID, instance, "edge"));
					stringWriter.publish(Config.getProvisionTopic(scenarioID, instance, "edge"), macro.getLinkID().toString(), macroJSON, simtimeInMS, 0, id);
				} else {
					logger.info("processObservers: " + o.getType().toString() + " is not known as ser type");
				}
			}
			else if(o.getElement().toString().equals("link.flow")) {
				System.out.println(VisumAPI.getFlow(link,timeStr,stepLengthInMinutes));
			}
			else if(o.getElement().toString().equals("link.density")) {
				System.out.println(VisumAPI.getDensity(link,timeStr,stepLengthInMinutes));
			}
			else if(o.getElement().toString().equals("link.speed")) {
				System.out.println(VisumAPI.getSpeed(link,timeStr));
			}
			else if(o.getElement().toString().equals("link.modes")) {
				System.out.println(VisumAPI.getModes(link));
			}
			else if(o.getElement().toString().equals("link.capacity")) {
				System.out.println("Cap:" + VisumAPI.getCap(link, timeStr));
				System.out.println("CapVolRatio: " +VisumAPI.getVolCapRatio(link, timeStr));
			}
			else if(o.getElement().toString().equals("link.turningProbabilities")) {
				System.out.println(VisumAPI.getTurningProbs(link, timeStr));
			}
			else {//todo: other action than publish
				logger.info("processObservers: no rule for element=" + o.getElement());
			}
	}

	}

}