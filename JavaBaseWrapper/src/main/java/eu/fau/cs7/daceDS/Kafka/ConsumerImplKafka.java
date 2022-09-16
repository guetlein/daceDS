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
package eu.fau.cs7.daceDS.Kafka;
import java.time.Duration;
import java.util.List;
import java.util.regex.Pattern;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;
import org.apache.kafka.common.header.Header;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.Component.Consumer;
import eu.fau.cs7.daceDS.Component.ConsumerCallback;
import eu.fau.cs7.daceDS.Component.ScenarioUtils;

/** 
 * A generic Kafka consumer class that lets a callback function process messages 
 * that are received.
 *  
 * @author guetlein
 *
 * @param <T>
 */
public class ConsumerImplKafka<T> extends Consumer<T>
{
	public final static Logger logger = Logger.getLogger(ConsumerImplKafka.class.getName());
	protected KafkaConsumer<String, T> consumer; 
	ConsumerCallbackImplKafka cb;

	public ConsumerImplKafka(String id){
		super(id);
	}

	public ConsumerImplKafka(String id, boolean consuming){
		super(id,consuming);
	}

	public  void init(List<String> topics, ConsumerCallback callback) {
		init(topics, callback, false);
	}	
	
	public  void initPlainString(List<String> topics, ConsumerCallback callback) {
		init(topics, callback, true, true, true);
	}
	
	public  void init(List<String> topics, ConsumerCallback callback, boolean plainString) {
		init(topics, callback, false, false, plainString);
	}

	public  void initPattern(String pattern, ConsumerCallback callback) {
		initPattern(pattern, callback, false, false);
	}
	
	public  void initPattern(String pattern, ConsumerCallback callback, boolean fetchLatest, boolean autopoll)
	{
		this.cb = (ConsumerCallbackImplKafka) callback;
		boolean plainString = false;
		consumer = new KafkaConsumer<String, T>(Config.getProperties(id, fetchLatest, plainString));   
		logger.info(Config.getProperties(id, fetchLatest, true).toString());
		consumer.subscribe(Pattern.compile(pattern));
		logger.info(id + (countingConsumer ? " is counting and " : "" ) + " started subscribing to pattern : " + pattern);

		if(autopoll) {
			isListening = true;
			Runnable r = new Runnable() {
				public void run() {
					logger.info("autopolling enabled");
					while(isListening) {
						poll();
					}
				}
			};
			t = new Thread(r);
			t.start();
		}

		Pattern p = Pattern.compile(pattern);
		subscribedPatterns.add(p);
		logger.info("subscribed to:" + p.pattern());
		if(countingConsumer){
			countingSubscribedPatterns.add(Pattern.compile(pattern));
		}
		
	}

	public  void init(List<String> topics, ConsumerCallback callback,  boolean fetchLatest, boolean autopoll, boolean plainString)
	{
		this.cb = (ConsumerCallbackImplKafka) callback;
		consumer = new KafkaConsumer<String, T>(Config.getProperties(id, fetchLatest, plainString));    
		consumer.subscribe(topics);
		logger.info(id + " started subscribing: ");
		for(String s : topics) {
			logger.info(s+", ");
		}

		if(autopoll) {
			isListening = true;
			Runnable r = new Runnable() {
				public void run() {
					logger.info("autopolling enabled");
					while(isListening) {
						poll();
					}
				}
			};
			t = new Thread(r);
			t.start();
		}

		subscribedTopics.addAll(topics);
		logger.info("subscribed to:");
		for(String t : ConsumerImplKafka.getSubscribedTopics()){
			logger.info(t);
		}

		if(countingConsumer){
			countingSubscribedTopics.addAll(topics);
		}
	}

	public void poll() {
		poll(DEFAULT_TIMEOUT);
	}

	public void poll(int timeout) {
		ConsumerRecords<String, T> Trawmsg  = consumer.poll(Duration.ofMillis(timeout));

		for (ConsumerRecord<String, T> r : Trawmsg) {	
			T msg = r.value();
			if(msg == null) {
				logger.error("received null object");
				continue;
			}
			logger.debug("received msg");
			cb.receive(r, getTime(r), getEpoch(r), getSender(r));

			if(countingConsumer){
				logger.debug("added msg to counting log");
				receivedCounting.merge(r.topic(), 1L, Long::sum); 
			}
		}
	}
	
	public void close() {
		try {
			isListening = false;
			consumer.unsubscribe();
			consumer.close();
		} catch (Exception e) {}
	}

	public static long getTime(ConsumerRecord record) {
		Header h = record.headers().lastHeader("time");
		if(h== null) {
			return -1;
		}
		return ScenarioUtils.bytesToLong(h.value());
	}

	public static int getEpoch(ConsumerRecord record) {		
		Header h = record.headers().lastHeader("epoch");
		if(h== null) {
			return 0;
		}
		return ScenarioUtils.bytesToInt(h.value());
	}
	public static String getSender(ConsumerRecord record) {		
		Header h = record.headers().lastHeader("sender");
		if(h== null) {
			return "";
		}
		return h.value().toString();
	}

}
