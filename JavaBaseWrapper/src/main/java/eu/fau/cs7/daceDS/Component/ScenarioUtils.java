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
package eu.fau.cs7.daceDS.Component;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.file.Files;
import java.nio.file.Path;

import org.apache.avro.Schema;
import org.apache.avro.io.Decoder;
import org.apache.avro.io.DecoderFactory;
import org.apache.avro.specific.SpecificDatumReader;

import eu.fau.cs7.daceDS.datamodel.BB;
import eu.fau.cs7.daceDS.datamodel.Macro;
import eu.fau.cs7.daceDS.datamodel.Scenario;

public class ScenarioUtils {
	
	public static <A> A getObjectFromJSON(String schemaStr, String json) {
		try {
			Schema.Parser schemaParser = new Schema.Parser();
			Schema schema = schemaParser.parse(schemaStr);
			DecoderFactory decoderFactory = new DecoderFactory();
			Decoder decoder;
		
			decoder = decoderFactory.jsonDecoder(schema, json);
            System.out.println(decoder);
            
            SpecificDatumReader<A> reader =
			            new SpecificDatumReader<A>(schema);
            A a = reader.read(null, decoder);
			return a;			      
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return null;
	}	
	public static Macro getMacroFromJSON(String json) {
		try {
		    String schemaStr = Macro.getClassSchema().toString();
			Schema.Parser schemaParser = new Schema.Parser();
			Schema schema = schemaParser.parse(schemaStr);
			DecoderFactory decoderFactory = new DecoderFactory();
			Decoder decoder;
		
			decoder = decoderFactory.jsonDecoder(schema, json);
            System.out.println(decoder);
            
            SpecificDatumReader<Macro> reader =
			            new SpecificDatumReader<Macro>(schema);
            Macro macro = reader.read(null, decoder);
			return macro;			      
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		return null;
	}	
	
	public static Scenario getScenarioFromJSON(String genericRecordStr) {
		try {
			
		    String schemaStr = Scenario.getClassSchema().toString();
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

	public static BB getBBFromScenario(String bbID, Scenario sce) {
		BB bb = null;
		
		for(BB t : sce.getBuildingBlocks()) {
			if(t.getInstanceID().toString().equals(bbID)) {
				bb = t;
				break;
			}else {
				System.out.println(bbID+"!="+t.getInstanceID());
			}
		}
		if(bb == null) {
			System.out.println("PROBLEM, didnt find my simulator("+bbID+") in scenario description");
			for(BB t : sce.getBuildingBlocks()) {
				System.out.println("'"+t.getInstanceID()+"'");
			}
			System.exit(1);
		}
		
		return bb;
	}


	public static eu.fau.cs7.daceDS.datamodel.Translator getTranslatorFromScenario(String instanceID, Scenario sce) {
		eu.fau.cs7.daceDS.datamodel.Translator translator = null;
		
		for(eu.fau.cs7.daceDS.datamodel.Translator t : sce.getTranslators()) {
			
			if(t.getTranslatorID().toString().equals(instanceID)) {
				translator = t;
				break;
			}else {
				System.out.println(instanceID+"!="+t.getTranslatorID());
			}
		}
		if(translator == null) {
			System.out.println("PROBLEM, didnt find my translator("+instanceID+") in scenario description");
			for(eu.fau.cs7.daceDS.datamodel.Translator t : sce.getTranslators()) {
				System.out.println("'"+t.getTranslatorID()+"'");
			}
			System.exit(1);
		}
		
		return translator;
	}
	
	public static eu.fau.cs7.daceDS.datamodel.Projector getProjectorFromScenario(String instanceID, Scenario sce) {
		eu.fau.cs7.daceDS.datamodel.Projector projector = null;
		
		for(eu.fau.cs7.daceDS.datamodel.Projector p : sce.getProjectors()) {
			
			if(p.getProjectorID().toString().equals(instanceID)) {
				projector = p;
				break;
			}else {
				System.out.println(instanceID+"!="+p.getProjectorID());
			}
		}
		if(projector == null) {
			System.out.println("PROBLEM, didnt find my projector("+instanceID+") in scenario description");
			for(eu.fau.cs7.daceDS.datamodel.Projector p : sce.getProjectors()) {
				System.out.println("'"+p.getProjectorID()+"'");
			}
			System.exit(1);
		}
		
		return projector;
	}

	
	public static byte[] longToBytes(long x) {
	    ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
	    buffer.putLong(x);
	    return buffer.array();
	}
	
	public static byte[] intToBytes(int x) {
	    ByteBuffer buffer = ByteBuffer.allocate(Integer.BYTES);
	    buffer.putInt(x);
	    return buffer.array();
	}

	public static long bytesToLong(byte[] bytes) {
	    ByteBuffer buffer = ByteBuffer.allocate(Long.BYTES);
	    buffer.put(bytes);
	    buffer.flip();
	    return buffer.getLong();
	}

	public static int bytesToInt(byte[] bytes) {
	    ByteBuffer buffer = ByteBuffer.allocate(Integer.BYTES);
	    buffer.put(bytes);
	    buffer.flip();
	    return buffer.getInt();
	}
	
}
