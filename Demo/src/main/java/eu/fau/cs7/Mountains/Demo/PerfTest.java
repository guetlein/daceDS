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
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.time.Duration;
import java.time.Instant;
import java.util.ArrayList;
import java.util.LinkedList;

import javax.swing.JFrame;
import javax.swing.JPanel;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import eu.fau.cs7.daceDS.Component.InteractionHandler;
import eu.fau.cs7.daceDS.Component.Simulator;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.DemoInteger;
import eu.fau.cs7.daceDS.datamodel.DemoRGB3;
import eu.fau.cs7.daceDS.datamodel.Scenario;
 

public class PerfTest extends Simulator<DemoInteger> {
 
   
	private Instant pre;
	private Instant preLoop;

	public PerfTest(String scenarioID, String instanceID, String provisionPrePattern, String provisionPostPattern, String demoScenario) {        
		super(scenarioID, instanceID, provisionPrePattern, provisionPostPattern, demoScenario);
    }

	static ArrayList<Thread> threads = new ArrayList<Thread>(); 
	static LinkedList<Integer> times = new LinkedList<Integer>(); 
 
    public static void main(String[] args) {
		if(args.length < 4) {
			System.out.println("Usage exe sce noParticipants noIterations ");
			return;
		}
		String scenarioID = args[0];//"perftest_"+(Math.random()*1000);
		String instanceID = args[1];//"perftest_"+(Math.random()*1000);
		int noParticipants = Integer.valueOf(args[2]);
		int noIterations = Integer.valueOf(args[3]);

		String provisionPrePattern = "container\\.";
		String provisionPostPattern = "\\.variables";

//		for(int i=0;i<noParticipants;i++) {
//			String instanceID = "instance"+i;
//			Runnable r =  new Runnable() {
//	            @Override
//	            public void run() {
	            	PerfTest instance = new PerfTest(scenarioID, instanceID, provisionPrePattern,provisionPostPattern, "");
	    			instance.scenario = Utils.getDummyScenario(scenarioID, noIterations, noParticipants);
	    			instance.simulator = Utils.getDummyBB(instanceID);
	    			instance.run();
//	            }
//	        };
//	        Thread t = new Thread(r);
//	        t.start();
//	        threads.add(t);
//		}
		
		
    }

    
    public void init() {
		super.init();
    }
    
	@Override
	protected void processInteraction(ConsumerRecord r) {
	}


	@Override
	public void preLoopEvent() {
		System.out.println("will run until " + scenario.getSimulationEnd() + " with steps of " + simulator.getStepLength());
		preLoop = Instant.now() ;
	}



	@Override
	protected void preStepEvent(int t) {
	}



	@Override
	protected void stepEvent(int t) {

	}



	@Override
	protected void processStepEvent(int t) {
		pre = Instant.now() ;
	}



	@Override
	protected void postStepEvent(int t) {
		Duration d = Duration.between( pre , Instant.now() );
//		System.out.println(d.getNano() + " nano seconds");
		times.add(d.getNano());
	}



	@Override
	protected void postLoopEvent() {
		Duration d = Duration.between( preLoop , Instant.now() );
		
		try {
			FileWriter writer = new FileWriter("measures_"+scenarioID+"_"+instanceID+".csv");
	
			for(int i : times) {
					writer.write(String.valueOf(i)+"\n");
			}
			
		    writer.close();			

			writer = new FileWriter("total_"+scenarioID+"_"+instanceID+".csv");
			writer.write("seconds: " + String.valueOf(d.getSeconds())+"\n");
			writer.write("nano se: " + String.valueOf(d.getNano())+"\n");
					
		    writer.close();			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}



	@Override
	protected void preInit() {
		NumbersReference nr = new NumbersReference("negative", -1000000, 0);
		NumbersReference pr = new NumbersReference("positive", 0, 1000000);
		ArrayList<NumbersReference> references = new ArrayList<NumbersReference>();
		references.add(nr);
		references.add(pr);
		ArrayList<CharSequence> l = new ArrayList<CharSequence>();
		domainHelper = new DomainHelperNumbers(references, l);
	}



	@Override
	protected void processMessage(ConsumerRecord r) {
		// TODO Auto-generated method stub
		
	}

}
