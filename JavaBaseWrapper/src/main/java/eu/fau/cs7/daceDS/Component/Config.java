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
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

import org.apache.kafka.clients.consumer.ConsumerConfig;
import org.apache.kafka.clients.producer.ProducerConfig;
import org.apache.kafka.common.serialization.StringSerializer;
import org.apache.kafka.common.serialization.StringDeserializer;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.datamodel.BB;
import io.confluent.kafka.serializers.KafkaAvroDeserializer;
import io.confluent.kafka.serializers.KafkaAvroDeserializerConfig;
import io.confluent.kafka.serializers.KafkaAvroSerializer;
import io.confluent.kafka.serializers.KafkaAvroSerializerConfig;
import io.confluent.kafka.serializers.subject.RecordNameStrategy;
import io.confluent.kafka.serializers.AbstractKafkaSchemaSerDeConfig;

/**
 * Provides constants, as well as parameters that are imported from the config file,
 * and some basic helper methods.
 * 
 * @author guetlein
 *
 */
public class Config
{      
	static Properties prop = new Properties();
	static Logger logger = Logger.getLogger("Config");;
	static final String PROPERTIES_FILE_NAME = "config.properties";
	public static final int DEFAULT_WAIT_MS = 1000;
	public static final int DEFAULT_POLL_MS = 500;

	/* paths */
	public static final String EXECUTABLE = "run.sh";
	public static final String EXECUTABLE_KILL = "kill.sh";
	public static final String DIR_ROOT = "rootDir";
	public static final String DIR_EXECROOT = "executablesRoot";
	public static final String DIR_OUTPUT = "outputDirectory";
	public static final String DIR_RESOURCE = "resourceDirectory";
	public static final String DIR_DEFINITION = "definitionDirectory";

	/* property names */
	public static final String EXECUTABLES_ROOT = "executablesRoot";
	public static final String LOG_DIR = "logDir";
	public static final String LOG_FILE = "logFile";
	public static final String SCHEMA_REGISTRY = "schemaRegistry";
	public static final String KAFKA_BROKER = "kafkaBroker";
	public static final String REQUEST_MAX_AGE = "requestMaxAge";

	/* topic property names */
	public static final String CHANNEL_ORCHESTRATION = "channelOrchestration";
	public static final String CHANNEL_INTERACTION = "channelInteraction";
	public static final String CHANNEL_PROVISION = "channelProvision";
	public static final String TOPIC_SCENARIO = "topicScenario";
	public static final String TOPIC_CTRL = "topicCtrl";
	public static final String TOPIC_STATUS = "topicStatus";

	/* topic related */
	public static final String DOMAIN_TRAFFIC = "traffic";
	public static final String WILDCARD = "([^\\\\.]+)";
	
	/* observers */
	public static final String OBSERVER_ATTRIBUTE_EMPTY = "";
	public static final String OBSERVER_ATTRIBUTE_EDGE = "edge";

	/* resources */
	public static final String RESOURCE_TRAFFIC = "Traffic";
	public static final String RESOURCE_ROADMAP = "RoadMap";
	
	/* serialization */
	public static final String SERIALIZATION_AVRO = "avro";
	public static final String SERIALIZATION_JSON = "json";
	
	public static void readConfig()
	{
		boolean suc = false;
		try {
			prop = new Properties();
			InputStream inputStream = readExternalConfig();
			prop.load(inputStream);
			suc = true;

		} catch (Exception e) {
			System.out.println("No external config next to .jar, using embedded default config ");
			prop = new Properties();
			try {
				InputStream inputStream = readInteralConfig();
				prop.load(inputStream);
				suc = true;
			} catch (IOException e1) {
				System.out.println("Internal Config not found");
			}
		} 

		if (suc){
			System.out.println("Config found!");
			for (Object key: prop.keySet()) {
//				System.out.println(key + ": " + prop.getProperty(key.toString()));
			}
			System.setProperty(LOG_DIR, Config.get(LOG_DIR));
		} else{

			System.out.println("Config NOT found!");
		}

		logger = Logger.getLogger("Config");
	}

	private static InputStream readInteralConfig() throws FileNotFoundException {
		InputStream  inputStream = Config.class.getClassLoader().getResourceAsStream(PROPERTIES_FILE_NAME);
		if(inputStream!=null)
			return inputStream;
		else {
			throw new FileNotFoundException("Property file '" + PROPERTIES_FILE_NAME + "' not found in the classpath.");
		}
	}

	public static InputStream readExternalConfig() throws FileNotFoundException {
		File jarPath=new File(Config.class.getProtectionDomain().getCodeSource().getLocation().getPath());
		String propertiesPath=jarPath.getParentFile().getAbsolutePath();

		logger.info("Trying to read config from: " + propertiesPath+"/"+PROPERTIES_FILE_NAME );
		InputStream  inputStream = new FileInputStream(propertiesPath+"/"+PROPERTIES_FILE_NAME);
		return inputStream;
	}
	
	public static String get(String p) {
		String property = prop.getProperty(p);
		if(property == null) {
			logger.error("Property "+p+" NOT found! Returning empty string.");
			return "";
		}
		return property;
	}		
	
	public static long getLong(String p) {
		String property = prop.getProperty(p);
		if(property == null ) {
			logger.error("Property "+p+" NOT found!");
			return -1;
		}
		try {
			Long l = Long.valueOf(property);
			return l;
		} catch (NumberFormatException e) {
			logger.error("Property "+p+" NOT a number!");
			return -1;
		}
	}
	
	public static String getStatusTopic(String scenarioID) {
		return Config.get(CHANNEL_ORCHESTRATION)+"."+ scenarioID+".status";
	}
	
	public static String getAPITopic(String scenarioID, String domain,String layer, String simulator, String p) {
		return Config.get(CHANNEL_INTERACTION)+"."+ scenarioID+"."+domain+"."+layer+"."+simulator+"."+p;
	}	
	

	public static String getProvisionTopic(String scenarioID, String domain, String layer, String p) {
		return Config.get(CHANNEL_PROVISION)+"."+ scenarioID+"."+domain+"."+layer+"."+p;
	}
	
	public static String getProvisionTopic(String scenarioID, String p) {
		return Config.get(CHANNEL_PROVISION)+"."+ scenarioID+"."+p;
	}	
	public static String getProvisionTopic(String scenarioID, BB bb, String p) {
		return Config.get(CHANNEL_PROVISION)+"."+ scenarioID+"."+bb.getDomain()+"."+bb.getLayer()+"."+p;
	}

	public static String getResultTopic(String scenarioID) {
		return Config.get(CHANNEL_PROVISION)+"."+ scenarioID+".result";
	}

	//todo: everything else
	public static String escapeSpecialChars(String topic) {
		return topic.replaceAll("\\.", "---2e");
	}	

	//todo: everything else
	public static String deescapeSpecialChars(String topic) {
		return topic.replaceAll("---2e", ".");
	}	
	
	//todo: move to file
	public static Properties getProperties(String groupID, boolean offsetLatest) {
		return getProperties(groupID, offsetLatest, false);
	}
	
	public static Properties getProperties(String groupID, boolean offsetLatest, boolean plainStringValue) {
		Properties kafkaProps = new Properties();
		kafkaProps.put(KafkaAvroDeserializerConfig.SPECIFIC_AVRO_READER_CONFIG, true);
		kafkaProps.put(KafkaAvroDeserializerConfig.SCHEMA_REGISTRY_URL_CONFIG, Config.get(SCHEMA_REGISTRY));	      //todo: avoid schema registry?
		kafkaProps.put(ConsumerConfig.BOOTSTRAP_SERVERS_CONFIG, Config.get(KAFKA_BROKER));
		kafkaProps.put(ConsumerConfig.GROUP_ID_CONFIG, groupID);
		kafkaProps.put(ConsumerConfig.KEY_DESERIALIZER_CLASS_CONFIG,  StringDeserializer.class.getName());
		if(plainStringValue) {
			kafkaProps.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, StringDeserializer.class.getName());
		} else {
			kafkaProps.put(ConsumerConfig.VALUE_DESERIALIZER_CLASS_CONFIG, KafkaAvroDeserializer.class.getName());
		}
		kafkaProps.put(ConsumerConfig.ENABLE_AUTO_COMMIT_CONFIG, "true");
		kafkaProps.put(ConsumerConfig.ISOLATION_LEVEL_CONFIG, "read_committed");
		kafkaProps.put(ConsumerConfig.MAX_POLL_RECORDS_CONFIG, 300);
		kafkaProps.put(ConsumerConfig.MAX_POLL_INTERVAL_MS_CONFIG, 75000);
		kafkaProps.put(ConsumerConfig.METADATA_MAX_AGE_CONFIG, 1000);
		kafkaProps.put(AbstractKafkaSchemaSerDeConfig.VALUE_SUBJECT_NAME_STRATEGY, RecordNameStrategy.class.getName()); //libserdes does not support anything else
		
		if(offsetLatest) {
			kafkaProps.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG, "latest");	
		} else {
			kafkaProps.put(ConsumerConfig.AUTO_OFFSET_RESET_CONFIG, "earliest");	
		}
		
		return kafkaProps;
	}

	//todo: move to file
	public static Properties getProducerProperties(String id) {
		return getProducerProperties(id, false, false);
	}
	public static Properties getProducerProperties(String id, boolean exactlyOnce) {
		return getProducerProperties(id, exactlyOnce, false);
	}
		public static Properties getProducerProperties(String id, boolean exactlyOnce, boolean plainStringValue) {
		Properties kafkaProducerProps = new Properties();
		kafkaProducerProps.put(KafkaAvroSerializerConfig.SCHEMA_REGISTRY_URL_CONFIG, Config.get(Config.SCHEMA_REGISTRY));	   
		kafkaProducerProps.put(ProducerConfig.BOOTSTRAP_SERVERS_CONFIG, Config.get(Config.KAFKA_BROKER));
		kafkaProducerProps.put(ProducerConfig.MAX_REQUEST_SIZE_CONFIG, 10485760);
		kafkaProducerProps.put(ProducerConfig.KEY_SERIALIZER_CLASS_CONFIG, StringSerializer.class.getName());
		if(plainStringValue) {
			kafkaProducerProps.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG,  StringSerializer.class.getName());	
			
		} else {
			kafkaProducerProps.put(ProducerConfig.VALUE_SERIALIZER_CLASS_CONFIG,  KafkaAvroSerializer.class.getName());		
		}
		kafkaProducerProps.put("linger.ms", 0);		
		kafkaProducerProps.put("acks", "1");		
		kafkaProducerProps.put("max.in.flight.requests.per.connection", "1");		
		
		kafkaProducerProps.put("enable.idempotence", "false");		
//		kafkaProducerProps.put("message.max.bytes", "" + 1024 * 1024 * 100);
		kafkaProducerProps.put(AbstractKafkaSchemaSerDeConfig.VALUE_SUBJECT_NAME_STRATEGY, RecordNameStrategy.class.getName()); //libserdes does not support anything else
		
		if(exactlyOnce) {
			System.out.println("exactly once is enabled");
			kafkaProducerProps.put(ProducerConfig.TRANSACTIONAL_ID_CONFIG, id);
		}
		kafkaProducerProps.put(ProducerConfig.MAX_BLOCK_MS_CONFIG, 6000); //was 6000//related to the transactional problem
		return kafkaProducerProps;
	}

		public static String getRootDir(){
			return get(DIR_ROOT);
		}
		public static String getLogDir(){
			return get(LOG_DIR);
		}
	public static String getExecutablesRootDir(){
		return get(DIR_EXECROOT);
	}

	public static String getBaseDir(String sceID, String simID){
		return get(DIR_ROOT) +"/"+sceID+"/"+simID;
	}

	public static String getResourceDir(String sceID, String simID){
		return getBaseDir(sceID,simID)+"/"+get(DIR_RESOURCE);
	}
	public static String getOutputDir(String sceID, String simID){
		return getBaseDir(sceID,simID)+"/"+get(DIR_OUTPUT);
	}

	public static String getInstanceLogDir(String scenarioID, String simulatorID) {
//		return getLogDir()+"/"+scenarioID+"/"+simulatorID;
		return getLogDir();
	}
}	
