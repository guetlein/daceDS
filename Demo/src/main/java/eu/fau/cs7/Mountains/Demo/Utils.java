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
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Random;

import org.apache.avro.Schema;
import org.apache.avro.io.Decoder;
import org.apache.avro.io.DecoderFactory;
import org.apache.avro.specific.SpecificDatumReader;

import eu.fau.cs7.daceDS.Component.TimeSync;
import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Execution;
import eu.fau.cs7.daceDS.datamodel.Scenario;

public class Utils {
	public static final String DOMAIN = "numbers";
	
	public static Scenario getDemoScenario(String scenarioPath) {
		try {
			System.out.println("trying to open " + scenarioPath);
			Path fileName = Path.of(scenarioPath);
		    String schemaStr = Scenario.getClassSchema().toString();
	
			String genericRecordStr = Files.readString(fileName);
			Schema.Parser schemaParser = new Schema.Parser();
			Schema schema = schemaParser.parse(schemaStr);
			DecoderFactory decoderFactory = new DecoderFactory();
			Decoder decoder;
		
			decoder = decoderFactory.jsonDecoder(schema, genericRecordStr);
            System.out.println(decoder);
            
            SpecificDatumReader<Scenario> reader =
			            new SpecificDatumReader<Scenario>(schema);
            Scenario sce = reader.read(null, decoder);
			return sce;			      
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return null;
	}
	
	public static BB getDummyBB(String bbID) {
		BB bb = new BB();
		bb.setID(bbID);
		bb.setLayer("dummy");
		bb.setDomain("dummy");
		ArrayList<CharSequence> a = new ArrayList<CharSequence>();
		bb.setResponsibilities(a);		
		bb.setStepLength(1000);
		bb.setSynchronized$(true);
		return bb;
	}
	
	public static Scenario getDummyScenario(String id, long end, int s) {
		Scenario sce = new Scenario();
		sce.setID(id);
		sce.setSimulationStart(0);
		sce.setSimulationEnd(end);
		sce.setExecution(new Execution());
		sce.getExecution().setSyncedParticipants(s);
		sce.getExecution().setRandomSeed(0);
		return sce;
	}

	

	public static BB getBBFromScenario(String bbID, Scenario sce) {
		BB bb = null;
		
		for(BB t : sce.getBuildingBlocks()) {
			if(t.getID().toString().equals(bbID)) {
				bb = t;
				break;
			}else {
				System.out.println(bbID+"!="+t.getID());
			}
		}
		if(bb == null) {
			System.out.println("PROBLEM, didnt find my simulator("+bbID+") in scenario description");
			for(BB t : sce.getBuildingBlocks()) {
				System.out.println("'"+t.getID()+"'");
			}
			System.exit(1);
		}
		
		return bb;
	}
	
	public static double fakePseudoRandCropped(int i, int j) {
	    int a = i%10;
	    int b = (i/10 + j)%10;
	    int c = (i/100)%10;
	    double d= ((a^(b+j))*c-a)+(j%100)*0.01;
	    double e = d;// - j/(1.4);//#-(0.01*(j%10)));
	    e = (e<0)? 0 : e;
//	    System.out.println("a=" +a + ", b="+b + ", c="+c + ", d="+ d+", e="+e);
	    return e;
	}

	public static double fakePseudoRand(int i, int j) {
    int a = i%10;
    int b = (i/10 + j)%10;
    int c = (i/100)%10;
    double d= ((a^(b+j))*c-a)+(j%100)*0.01;
    double e = d;// - j/(1.4);//#-(0.01*(j%10)));
    if(a%2==0)e*=-1;
    return e;
}
	
	
	public static double scaledSine(int i, int scale, int stretch) {
		double a = (i*stretch)%360;  
		double b = Math.toRadians(a);  
		double c = Math.sin(b)*scale;
	    return c;
	}	
	public static double scaledSine(double i, int scale, int stretch) {
		double a = (i*stretch)%360;  
		double b = Math.toRadians(a);  
		double c = Math.sin(b)*scale;
	    return c;
	}
	

//	value += (int)Utils.fakePseudoRandSine(t,15,50)*Utils.fakePseudoRandSine(t+(int)(value),10,50);
	//final:
//	public static int pseudoRandSine(int time, int value, int seed) {
//		double f = scaledSine(value,10,10+value); //scale:10, stretch: 13,16,10!,1
//		f = (f>=0 && f<1)? 1 : f;
//		f = (f<=0 && f>-1)? -1 : f;
//		double s= scaledSine(seed*10 - value,15,50); //(time-1+seed,15,50) seed=87
//		int v = value + (int) Math.round(s*f);
//		System.out.println("time="+time+",v="+value+",seeD="+seed+",f="+f+",s="+s+": "+v);
//		return v;
//	}
	
//	public static int pseudoRandSine(int time, int value, int seed) {
//		int r = pseudoRand(value, 20);
//		int r2 = pseudoRand(value, 20)-10;
//		double bound = 20;
//		double f = Math.min(Math.max(scaledSine(1+value,10,6),-bound),bound); //scale:10, stretch: 13,16,10!,1
//		double s= Math.min(Math.max(scaledSine(value ,15,r),-bound),bound); //(time-1+seed,15,50) seed=87
//		int v = value + r2 + (int) Math.round(f*s+f+s);
//		System.out.println("time="+time+",v="+value+",seeD="+seed+",f="+f+",s="+s+": "+v);
//		return v;
//	}
	
	//timeindependent
	//seed=87
	public static int pseudoRandSine(int time, int value, int seed) {
		int r = pseudoRand(value, 20);
		int r2 = pseudoRand(value, 20)-10;
		double bound = 20;
		double f = Math.min(Math.max(scaledSine(seed+value,10,10),-bound),bound); //scale:10, stretch: 13,16,10!,1
		double s= Math.min(Math.max(scaledSine(value ,15,r),-bound),bound); //(time-1+seed,15,50) seed=87
		double g= Math.min(Math.max(scaledSine(seed ,15,r),-bound),bound); //(time-1+seed,15,50) seed=87
		int v = value + r2 + (int) Math.round(f*s+f+s+g*s+g+g*f+g*s*f);
		System.out.println("time="+time+",v="+value+",seeD="+seed+",f="+f+",s="+s+": "+v);
		return v;
	}

	public static double sineFunction(double v, String t, int seed, int s, int w){
		double o = v + t.charAt(t.length()-1)*(19+seed);
		return Utils.scaledSine(o,s,w);
	}
	
	

	
//	public static double fakePseudoRandSine(double i, int j, int k) {
//		double a = (i*k)%360;  
//		double b = Math.toRadians(a);  
//		double c = Math.sin(b)*((double)j);
//	    return c;
//	}
	
	public static double fakePseudoRandFlip(double i) {
	    return i*=-1;
	}	
	
	public static int pseudoRand(int seed) {
		Random rand = new Random(seed);
	    return rand.nextInt();
	}	public static int pseudoRand(int seed, int bound) {
		Random rand = new Random(seed);
	    return rand.nextInt(bound);
	}
	
	public static double pseudoRandCropped(double seed) {
		Random rand = new Random((long) seed);
	    return rand.nextDouble();
	}
	
	public static int pseudoRandCropped(int seed, int time, int value, int bound) {
		Random rand = new Random(seed + time + value);
	    return rand.nextInt(bound);
	}
	public static double pseudoRandCropped(double seed, int time, double value) {
		Random rand = new Random((long) (seed + time + value));
		double d = rand.nextDouble();
		return d>0?d:-d;
	}


	public static eu.fau.cs7.daceDS.datamodel.Translator getTranslatorFromScenario(String instanceID, Scenario sce) {
		eu.fau.cs7.daceDS.datamodel.Translator translator = null;
		
		for(eu.fau.cs7.daceDS.datamodel.Translator t : sce.getTranslators()) {
			
			if(t.getID().toString().equals(instanceID)) {
				translator = t;
				break;
			}else {
				System.out.println(instanceID+"!="+t.getID());
			}
		}
		if(translator == null) {
			System.out.println("PROBLEM, didnt find my translator("+instanceID+") in scenario description");
			for(eu.fau.cs7.daceDS.datamodel.Translator t : sce.getTranslators()) {
				System.out.println("'"+t.getID()+"'");
			}
			System.exit(1);
		}
		
		return translator;
	}
	
	public static eu.fau.cs7.daceDS.datamodel.Projector getProjectorFromScenario(String instanceID, Scenario sce) {
		eu.fau.cs7.daceDS.datamodel.Projector projector = null;
		
		for(eu.fau.cs7.daceDS.datamodel.Projector p : sce.getProjectors()) {
			
			if(p.getID().toString().equals(instanceID)) {
				projector = p;
				break;
			}else {
				System.out.println(instanceID+"!="+p.getID());
			}
		}
		if(projector == null) {
			System.out.println("PROBLEM, didnt find my projector("+instanceID+") in scenario description");
			for(eu.fau.cs7.daceDS.datamodel.Projector p : sce.getProjectors()) {
				System.out.println("'"+p.getID()+"'");
			}
			System.exit(1);
		}
		
		return projector;
	}

	public static int grow(int value, int d, int seed) {
		int dr = d %100;
		int r = (int) Math.max(3,Math.abs(value)/70);
//		System.out.println("value="+value+",d="+d+",seed="+seed+",dr="+dr+",r="+r);
		return value+r;
	}	
	
	public static double growingSine(int value, int time, int s, int steplength) {
		double r = steplength *(1+ scaledSine(s+time,20,2));
		return (value+r);
	}	

}
