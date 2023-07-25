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


import java.io.File;
import java.util.Map;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map.Entry;
import javax.swing.JFrame;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.knowm.xchart.*;
import org.knowm.xchart.XYSeries.XYSeriesRenderStyle;
import org.knowm.xchart.style.Styler.LegendPosition;
import eu.fau.cs7.daceDS.Component.Simulator;
import eu.fau.cs7.daceDS.Component.TimeSync;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;
import eu.fau.cs7.daceDS.datamodel.Observer;


public class IntegerSimulator extends Simulator<DemoInteger> {

	public IntegerSimulator(String scenarioID, String instanceID, String provisionPrePattern,
			String provisionPostPattern, String demoScenario) {
		super(scenarioID, instanceID, provisionPrePattern, provisionPostPattern, demoScenario);
	}

	private XYChart chart;
	private JFrame sw;
	
	public final String LAYER = "integers";
	public final String DOMAIN = "numbers";
	protected ArrayList<DemoInteger> numbers = new ArrayList<DemoInteger>();
	private Map<String,List<Integer>> xDataMap = new HashMap<String,List<Integer>>();
	private Map<String,List<Integer>> yDataMap = new HashMap<String,List<Integer>>();
	private String functionString;
	private int seed;


	private static String provisionPreTopic;
	private static String provisionPostTopic;
	

	
	public static void main(String[] args) {

		if(args.length < 2) {
			System.out.println("Usage exe scenarioID simulatorID [demo]");
			return;
		}
		String scenarioID = "taa";//args[0];
		String instanceID = args[1];
		String demoScenario = "";

		if(args.length == 3) {
			demoScenario = "daceDS/Scenario/Demo/Example.sce";
		}

		String provisionPrePattern = "container\\.";
		String provisionPostPattern = "\\.variables";
		provisionPreTopic = "container";
		provisionPostTopic = "variables";

		IntegerSimulator instance = new IntegerSimulator(scenarioID, instanceID, provisionPrePattern,provisionPostPattern, demoScenario);
		
		if(demoScenario.length()>0) {
			instance.scenario = Utils.getDemoScenario(demoScenario);
			instance.scenario.setID(scenarioID);
			instance.instanceNumber = instanceID.equals("a")? 0 : 1;
			instance.simulator = Utils.getBBFromScenario(instanceID, instance.scenario);
		}	
		
		instance.run();

		
//		instance.scenario = Utils.getDemoScenario(demoScenario);
//		instance.simulator = Utils.getBBFromScenario( instanceID, instance.scenario );
//		instance.runSimple();
		
		
//		
	}
	
	@Override
	public void preInit() {
		NumbersReference nr = new NumbersReference("negative", -1000000, 0);
		NumbersReference pr = new NumbersReference("positive", 0, 1000000);
		ArrayList<NumbersReference> references = new ArrayList<NumbersReference>();
		references.add(nr);
		references.add(pr);
		domainHelper = new DomainHelperNumbers(references, simulator.getResponsibilities());
		
		System.out.println("pre init");
	}

	@Override
	public void preLoopEvent() {

		 chart = new XYChartBuilder().width(600).height(500).title("IntegerSimulator").xAxisTitle("X int").yAxisTitle("Y").build();		 // Customize Chart
		 chart.getStyler().setDefaultSeriesRenderStyle(XYSeriesRenderStyle.Line);
		 chart.getStyler().setChartTitleVisible(false);
		 chart.getStyler().setLegendPosition(LegendPosition.InsideSW);
		 chart.getStyler().setMarkerSize(1);		 
		 sw = new SwingWrapper(chart).displayChart();
		 
		NumbersReference nr = new NumbersReference("negative", -1000000, 0);
		NumbersReference pr = new NumbersReference("positive", 0, 1000000);
		ArrayList<NumbersReference> references = new ArrayList<NumbersReference>();
		references.add(nr);
		references.add(pr);
		domainHelper = new DomainHelperNumbers(references,simulator.getResponsibilities());
		
		
		
		if(instanceID.equals("intSim")){
//			numbers.add(new DemoInteger("0", -400));
//			numbers.add(new DemoInteger("1", -300));
//			numbers.add(new DemoInteger("2", -200));
//			numbers.add(new DemoInteger("3", -100));
//			numbers.add(new DemoInteger("4", 0));
//			numbers.add(new DemoInteger("5", 100));
//			numbers.add(new DemoInteger("6", 200));
//			numbers.add(new DemoInteger("7", 300));
			numbers.add(new DemoInteger("0", -350));
			numbers.add(new DemoInteger("1", -916));
		}
		
		if(instanceID.equals("negSim")){
//			numbers.add(new DemoInteger("0", -400));
//			numbers.add(new DemoInteger("1", -300));
//			numbers.add(new DemoInteger("2", -200));
//			numbers.add(new DemoInteger("3", -100));
			
			//grow
//			numbers.add(new DemoInteger("0", -1369));
//			numbers.add(new DemoInteger("1", -3941));

			numbers.add(new DemoInteger("0", -350));
			numbers.add(new DemoInteger("1", -916));

		}
		
		if(instanceID.equals("posSim")){
//			numbers.add(new DemoInteger("4", 0));
//			numbers.add(new DemoInteger("5", 100));
//			numbers.add(new DemoInteger("6", 200));
//			numbers.add(new DemoInteger("7", 300));
			
			//sine example
//			numbers.add(new DemoInteger("0", 19));
//			numbers.add(new DemoInteger("1", 8));
		}
		
//		if(instanceID.equals("mlInt")){
////			numbers.add(new DemoInteger("4", 0));
////			numbers.add(new DemoInteger("5", 10));
//			numbers.add(new DemoInteger("0", -6));
//			numbers.add(new DemoInteger("1", -10));
//		}
		

		for(DemoInteger n : numbers) {
			String name = n.getId().toString();
			int v = n.getValue();
			xDataMap.put(name, new LinkedList<Integer>());
			xDataMap.get(name).add(0);
			yDataMap.put(name, new LinkedList<Integer>());
			yDataMap.get(name).add(v);
			chart.addSeries(name, xDataMap.get(name), yDataMap.get(name));
		}		
		String f = "function";
		for(Entry<CharSequence, CharSequence> c : simulator.getLayerParams().entrySet()) {
			if(c.getKey().toString().equals(f)) {
				functionString = c.getValue().toString();
			};
		}
		System.out.println("functionString="+functionString);
		seed = (int) scenario.getExecution().getRandomSeed();
		
	}
	
	@Override
	public void preStepEvent(int t) {
		// TODO Auto-generated method stub
		
	}
	
	public void stepEvent(int t) {
		for(int i=0;i<numbers.size();i++) {
			DemoInteger number = numbers.get(i);
			int value = number.getValue();
			if( ((DomainHelperNumbers)domainHelper).isResponsibleInternal(number) ) {

				//distribution test
				if(functionString.equals("pseudoRandSine")) {
				number.setValue(Utils.pseudoRandSine(t,value,seed));
				}
				//ml test
				else if(functionString.equals("sine")) {
					int s = 20;
					int w = 2;
					number.setValue((int) Math.round(Utils.sineFunction(t,number.getId().toString(),seed,s,w)));
				}
				//grow dist
				else if(functionString.equals("grow")) {
					int s = 20;
					int w = 2;
					String id = number.getId().toString();
					int d = id.hashCode();
					number.setValue(Utils.grow(value, d, seed));
				}
				//grow dist
				else if(functionString.equals("growingSine")) {
					int s = 20;
					int w = 2;
					String id = number.getId().toString();
					int d = (id.hashCode() + id.hashCode()*seed)%360;
					number.setValue((int) Utils.growingSine(value, t, d, simulator.getStepLength()));
				}
				else {
					System.out.println("no mod function callled");
					System.exit(1);
				}

				log(t+" is responsible to modify " + number.getId()+", set it from "+value+" to " + number.getValue());
			}
			

		}			 
	}
	
	@Override
	protected void processStepEvent(int t) {
		//am i still responsible?
		ArrayList<DemoInteger> toDelete = new ArrayList<DemoInteger>();
		for(DemoInteger number : numbers) {
			if(!((DomainHelperNumbers)domainHelper).isResponsibleInternal(number) ) {
				String topic = "provision.simulation."+scenarioID+"."+DOMAIN+"."+LAYER+"."+provisionPreTopic+"."+((DomainHelperNumbers)domainHelper).getLayerReference(number)+"."+provisionPostTopic;
				provisionHandler.sendNDM(topic, number, t); //todo: task of ovservers
				log(t+" send out number");
				toDelete.add(number);
			}
		}
		log("removing "+toDelete.size()+" elemets, numbers.size went from "+numbers.size());
		numbers.removeAll(toDelete);
		toDelete.clear();
		log("to"+numbers.size());
		
		
		
		//general observers
		log(observers.size()+" observers");
		for(Observer o : observers) {
			if(t%o.getPeriod() == 0) {
				if(o.getElement().toString().equals("DemoInteger") && o.getTask().toString().equals("publish") && o.getFilter().length()==0 && o.getTrigger().length()==0) {
						
					for(DemoInteger number : numbers) {
						String topic = "provision.simulation."+scenarioID+"."+DOMAIN+"."+LAYER+"."+o.getElement();
						provisionHandler.sendNDM(topic, number, t);
					}
					continue;						
					
				}
				log(o.getElement().toString() +','+ o.getTask()  +','+ o.getFilter().length()  +','+ o.getTrigger().length());
				log("requested observer is not implemented");
			}
		}
		
	}
	
	
	public void processMessage(ConsumerRecord r) {
		ConsumerRecord<String, DemoInteger> msg = r;
		DemoInteger i =  msg.value();
		setNumber(i);
		
		System.out.println("got var"+i.getId()+ " with "+ i.getValue() + " at " + TimeSync.getLocaltime());
		
	}
	
	public void setNumber(DemoInteger i) {
		log(" received number "+i.getId()+" "+i.getValue());
		numbers.add(i);		
	}


	@Override
	public void postStepEvent(int t) {

		

			for(DemoInteger number : numbers) {
				int value = number.getValue();
				String name = number.getId().toString();


				if(!xDataMap.containsKey(name)) {
					xDataMap.put(name, new LinkedList<Integer>());
					yDataMap.put(name, new LinkedList<Integer>());
					xDataMap.get(name).add(t);
					yDataMap.get(name).add(value);
					chart.addSeries(name, xDataMap.get(name), yDataMap.get(name));
				} else {
					xDataMap.get(name).add(t);
					yDataMap.get(name).add(value);
					chart.updateXYSeries(name, xDataMap.get(name), yDataMap.get(name), null);
				}

			}

		 sw.revalidate();
		 sw.repaint();
		 		
	}


	
	public void postLoopEvent() {
	String dir = scenario.getScenarioID()+"/"+instanceID+"/";
		new File(dir).mkdirs();
		CSVExporter.writeCSVColumns(chart, dir);
	}

	@Override
	protected void processInteraction(ConsumerRecord r) {
		// TODO Auto-generated method stub
		
	}


}
