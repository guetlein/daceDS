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
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

import java.util.HashMap;


/** 
 * A generic consumer class that lets a callback function process messages 
 * that are received.
 *  
 * @author guetlein
 *
 * @param <T>
 */
public abstract class Consumer<T>
{
	protected String id;
	protected static final int DEFAULT_TIMEOUT = 50;    
	protected ConsumerCallback cb;
	protected Thread t;	
	protected boolean countingConsumer = true;
	protected boolean isListening;
	protected static HashMap<String, Long> receivedCounting = new HashMap<String, Long>();
	protected static List<String> subscribedTopics = new ArrayList<String>();
	protected static List<String> countingSubscribedTopics = new ArrayList<String>();
	protected static List<Pattern> subscribedPatterns = new ArrayList<Pattern>();//Pattern.compile("undefined");
	protected static List<Pattern> countingSubscribedPatterns = new ArrayList<Pattern>();//Pattern.compile("undefined");

	public Consumer(String id) {	this.id = id;};

	public Consumer(String id, boolean consuming) {		this.countingConsumer = consuming;
	this.id = id;};

	public  abstract void init(List<String> topics, ConsumerCallback callback);
	
	public abstract  void initPlainString(List<String> topics, ConsumerCallback callback);
	
	public abstract  void init(List<String> topics, ConsumerCallback callback, boolean plainString);

	public  abstract void initPattern(String pattern, ConsumerCallback callback);
	
	public  abstract void initPattern(String pattern, ConsumerCallback callback, boolean fetchLatest, boolean autopoll);

	public  abstract void init(List<String> topics, ConsumerCallback callback,  boolean fetchLatest, boolean autopoll, boolean plainString);

	public abstract void poll();

	public  abstract void poll(int timeout);
	
	public  abstract void close();


	//////////////////////////////////
	//counting

	public static List<String> getSubscribedTopics() {
		return subscribedTopics;
	}
	public static List<String> getCountingSubscribedTopics() {
		return countingSubscribedTopics;
	}

	public static String getSubscribedPattern() {
		String pstr = "";
		for(Pattern p : subscribedPatterns) {
			pstr+=p.pattern()+",";
		}
		return pstr;
	}
	public static String getCountingSubscribedPattern() {
		String pstr = "";
		for(Pattern p : countingSubscribedPatterns) {
			pstr+=p.pattern()+",";
		}
		return pstr;
	}

	public static HashMap<String, Long> getReceivedCounting(){
		return receivedCounting;
	}

	public static boolean hasSubscribed(String topic) {
		boolean t = subscribedTopics.contains(topic);
		if (t) return true;

		for(Pattern p : subscribedPatterns) {
			Matcher m = p.matcher(topic);
			if (m.matches()) 
				return true;
		}
		
		return false;
	}

	public static boolean hasSubscribedCounting(String topic) {
		boolean t = countingSubscribedTopics.contains(topic);
		if (t) return true;

		for(Pattern p : countingSubscribedPatterns) {
			Matcher m = p.matcher(topic);
			if (m.matches()) 
				return true;
		}

		return false;
	}

}
