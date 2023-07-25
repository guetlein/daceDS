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
import org.knowm.xchart.CSVExporter;
import org.knowm.xchart.SwingWrapper;
import org.knowm.xchart.XYChart;
import org.knowm.xchart.XYChartBuilder;
import org.knowm.xchart.XYSeries.XYSeriesRenderStyle;
import org.knowm.xchart.style.Styler.LegendPosition;

import eu.fau.cs7.daceDS.Component.Simulator;
import eu.fau.cs7.daceDS.datamodel.DemoDouble;

public class DoubleSimulator extends Simulator<DemoDouble> {

	private static String provisionPrePattern;
	private static String provisionPostPattern;
	private static String provisionPreTopic;
	private static String provisionPostTopic;


	public DoubleSimulator(String scenarioID, String instanceID, String provisionPrePattern,
			String provisionPostPattern, String demoScenario) {
		super(scenarioID, instanceID, provisionPrePattern, provisionPostPattern, demoScenario);
	}

	private XYChart chart;

	private Map<String,List<Integer>> xDataMap = new HashMap<String,List<Integer>>();
	private Map<String,List<Double>> yDataMap = new HashMap<String,List<Double>>();
	private JFrame sw;



	protected int NUM_ELEMENTS = 0;
	public final String LAYER = "doubles";
	protected ArrayList<DemoDouble> numbers = new ArrayList<DemoDouble>();
	private int seed;
	private String functionString;

	
	public static void main(String[] args) {

		if(args.length < 2) {
			System.out.println("Usage exe scenarioID simulatorID [demo]");
			return;
		}
		String scenarioID = "gs_translation";//args[0];
		String instanceID = args[1];
		String demoScenario = "";

		if(args.length == 3) {
			demoScenario = "daceDS/Scenario/Demo/Translation_gS.sce";
		}

		provisionPrePattern = "container\\.";
		provisionPostPattern = "\\.variables";
		provisionPreTopic = "container";
		provisionPostTopic = "variables";
		
		
		
		 DoubleSimulator instance = new DoubleSimulator(scenarioID, instanceID, provisionPrePattern,provisionPostPattern, demoScenario);
		 
			if(demoScenario.length()>0) {
				instance.scenario = Utils.getDemoScenario(demoScenario);
				instance.scenario.setID(scenarioID);
				instance.instanceNumber = instanceID.equals("a")? 0 : 1;
				instance.simulator = Utils.getBBFromScenario(instanceID, instance.scenario);
			}	
			

		 instance.run();
	}
	
	
	public void preInit() {		
		NumbersReference nr = new NumbersReference("negative", -100000, 0);
		NumbersReference pr = new NumbersReference("positive", 0, 1000000);
		ArrayList<NumbersReference> references = new ArrayList<NumbersReference>();
		references.add(nr);
		references.add(pr);
		domainHelper = new DomainHelperNumbers(references,simulator.getResponsibilities());	
		
	}


	@Override
	public void preLoopEvent() {
	
		

		 chart = new XYChartBuilder().width(600).height(500).title("DoubleSimulator").xAxisTitle("X dbl").yAxisTitle("Y").build();		 // Customize Chart
		 chart.getStyler().setDefaultSeriesRenderStyle(XYSeriesRenderStyle.Line);
		 chart.getStyler().setChartTitleVisible(false);
		 chart.getStyler().setLegendPosition(LegendPosition.InsideSW);
		 chart.getStyler().setMarkerSize(1);		 
		 sw = new SwingWrapper(chart).displayChart();
		 
		 
		
		
		

		

		for(DemoDouble n : numbers) {
			String name = n.getId().toString();
			double v = n.getValue();
			xDataMap.put(name, new LinkedList<Integer>());
			xDataMap.get(name).add(0);
			yDataMap.put(name, new LinkedList<Double>());
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
			// am i responsible to simulate?
			log(t+" simulating step " + t );
			for(int i=0;i<numbers.size();i++) {
				DemoDouble number = numbers.get(i);
				double value = number.getValue();
				if( ((DomainHelperNumbers)domainHelper).isResponsibleInternal(number) ) {

					//dist
//					value += (int)Utils.fakePseudoRandSine(t,15,50)*Utils.fakePseudoRandSine(t+(int)(value),10,50);
//					number.setValue(value);
//					
					if(functionString.equals("sine")) {
						int s = 20;
						int w = 2;
						number.setValue(Utils.sineFunction(t,number.getId().toString(),seed,s,w));
					}
					//grow dist
					else if(functionString.equals("growingSine")) {
						int s = 20;
						int w = 2;
						String id = number.getId().toString();
						int d = (id.hashCode() + id.hashCode()*seed)%360;
						number.setValue(Utils.growingSine((int) value, t, d,simulator.getStepLength()));
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
		log(t+" processing step " + t );
		ArrayList<DemoDouble> toDelete = new ArrayList<DemoDouble>();
		for(DemoDouble number : numbers) {
			if(!((DomainHelperNumbers)domainHelper).isResponsibleInternal(number) ) {
				String topic = "provision.simulation."+scenarioID+"."+Utils.DOMAIN+"."+LAYER+"."+provisionPreTopic+"."+((DomainHelperNumbers)domainHelper).getLayerReference(number)+"."+provisionPostTopic;
				provisionHandler.sendNDM(topic, number, t); //todo: task of ovservers
						log(t+" send out number");
				toDelete.add(number);
			}
		}

		numbers.removeAll(toDelete);
		toDelete.clear();
	}
	
	/*
	 * Here we have a valid state.
	 */
	@Override
	public void postStepEvent(int t) {


		for(DemoDouble number : numbers) {
			double value = number.getValue();
			String name = number.getId().toString();


			if(!xDataMap.containsKey(name)) {
				xDataMap.put(name, new LinkedList<Integer>());
				yDataMap.put(name, new LinkedList<Double>());
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

	public void processMessage(ConsumerRecord r) {
		ConsumerRecord<String, DemoDouble> msg = r;
		DemoDouble d =  msg.value();
		setNumber(d);
		
	}
	
	public void setNumber(DemoDouble d) {
		log(" received number "+d.getId()+" "+d.getValue());
		numbers.add(d);		
//		log("  provision channel set number to "+i);
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
