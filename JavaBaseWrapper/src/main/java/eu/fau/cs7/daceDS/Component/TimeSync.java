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
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.Map;

import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Kafka.ConsumerImplKafka;
import eu.fau.cs7.daceDS.Kafka.ConsumerCallbackImplKafka;
import eu.fau.cs7.daceDS.Kafka.ProducerImplKafka;
import eu.fau.cs7.daceDS.datamodel.SyncMsg;

/**
 * Implementation of the conservative time synchronization mechanism.
 * 
 * @author guetlein
 *
 */
public class TimeSync implements ConsumerCallbackImplKafka {
	static public String scenarioID = "";
	static Logger logger = Logger.getLogger(TimeSync.class.getName());
	static private String ID = "";
	static ArrayList<String> topics = new ArrayList<String>();

	static private Producer<SyncMsg> writer;
	static private Consumer<SyncMsg> reader;
	static HashMap<String, SyncMsg> participants = new HashMap<String, SyncMsg>();
	static public int receiveCount = 0;
	static public int sentCount = 0;
	static private Object lock = Long.valueOf(2L);
	static private long localtime;
	static private int expectedNoInitialParticipants;
	static private HashMap<String, Long> expectedMsgs = new HashMap<String, Long>();
	static private long lbts = 0;
	static HashMap<String, Long>  lastAccouncedSentMessages  = new HashMap<String, Long>();
	static private int noParticipants;
	private static long lbep;

	public void init(String scID, String participantID, int noParticipants) {
		ID = "KafkaTimeClient:" + scID + ":" + participantID;
		System.out.println("TimeclientStarted with id=" + ID + " on scen=" + scID);
		logger = Logger.getLogger(ID);
		scenarioID = scID;
		expectedNoInitialParticipants = noParticipants;
		writer = new ProducerImplKafka<SyncMsg>(scID + "_" + participantID + "_timesyncwriter");
		writer.init();
		reader = new ConsumerImplKafka<SyncMsg>(scID + "_" + participantID + "_timesyncreader");
		topics.add(Config.get("channelOrchestration") + "." + scenarioID + "." + Config.get("topicSyncMsg"));
		reader.init(topics, (ConsumerCallback)this);
		logger.info("TimeClient up and runnning with "+noParticipants+" participants!");
		// todo: instead of polling upon time advance, may be faster to subscribe in
		// seperate thread
	}

	public static boolean timeAdvance(long targettime) {
		return timeAdvance(targettime, 0);
	}

	//todo: keep in mind: we are receiving our own messages with the current library!
	public static boolean timeAdvance(long targettime, int targetepoch) {
//		long targettime = localtime + step;
//		System.out.println("timeAdvance() to " + targettime + "." + targetepoch + ", while lbts=" + lbts+ " lbep=" + lbep);
		//SyncMsg sync = new SyncMsg(ID,ActionType.REQUEST,targettime,epoch,mySentMsgList);

//		System.out.println("size of KafkaWriter.getSentCounting()="+KafkaWriter.getSentCounting().size());
		HashMap<CharSequence, Long>  diff = new HashMap<CharSequence, Long>();
		// HashMap<String, Long>  diff = new HashMap<String, Long>();
		HashMap<String, Long>  sent = (HashMap<String, Long>)ProducerImplKafka.getSentCounting().clone(); //todo: only a shallow copy - anyway, other iteration would be better
		for(Entry<String, Long> entry : sent.entrySet()) {
			CharSequence cTopic = entry.getKey();
			// String cTopic = entry.getKey();
			String topic = entry.getKey();
			if(lastAccouncedSentMessages.containsKey(topic)){
				long d = entry.getValue() - lastAccouncedSentMessages.get(topic);
				if (d>0){
					diff.put(cTopic,d);
				} else {
//					System.out.println("doing nothing, d=0 entry.getValue()="+entry.getValue()+"   lastAccouncedSentMessages.get(topic)="+lastAccouncedSentMessages.get(topic));
				}
			} else {
				diff.put(cTopic,entry.getValue());
			}
		} 


//		System.out.println("size of diff="+diff.size());
//		for(CharSequence s : diff.keySet()){
//			System.out.println(s + ":"+diff.get(s));
//		}
	
		lastAccouncedSentMessages = sent;
		
		SyncMsg sync = new SyncMsg(ID,"request",targettime,targetepoch,diff);

		writer.publish(topics.get(0), sync, lbts);

		calcLBTS();
//		int errorCount=0;
		while (!(timeOK (targettime, lbts, targetepoch, lbep) && receivedMsgsOK())) {
			reader.poll(100);
//			errorCount++;
//			if(errorCount>6000) {
//				return false;
//			}
//			try {
//				Thread.sleep(1);
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
		}
		localtime = targettime;
		return true;
//		System.out.println("localtime is " + targettime);
	}

	static boolean timeOK(long targetTime, long actualTime, long targetEpoch, long actualEpoch){
//		logger.info("targetTime=" + targetTime+", actualTime=" + actualTime + ", targetEpoch=" + targetEpoch + ", actualEpoch=" + actualEpoch);
//		logger.info("timeOK=" + ((targetTime == actualTime && targetEpoch <= actualEpoch) || (targetTime < actualTime)));
		return (targetTime == actualTime && targetEpoch <= actualEpoch) || (targetTime < actualTime);
		//todo: use noParticipants and epochs
	}
	static boolean receivedMsgsOK(){
		HashMap<String, Long> received = ConsumerImplKafka.getReceivedCounting();
		for(Entry<String, Long> entry : expectedMsgs.entrySet()) {
			String expectedTopic = entry.getKey();
			Long expectedNum = entry.getValue();
			if(!received.containsKey(expectedTopic) || received.get(expectedTopic) < expectedNum  ){
				long n = received.containsKey(expectedTopic) ? received.get(expectedTopic) : 0;
				System.out.println("receivedMsgsOK? No: topic=" +expectedTopic + "  "+n+"/"+expectedNum);
				return false;
			}
			long n = received.containsKey(expectedTopic) ? received.get(expectedTopic) : 0;
//			System.out.println("receivedMsgsOK? Yes: topic=" +expectedTopic + "  "+n+"/"+expectedNum);
		}
		
		return true;
	}

	public void leave() {
		//SyncMsg sync = new SyncMsg(ID,ActionType.LEAVE,localtime,0,mySentMsgList);
		SyncMsg sync = new SyncMsg(ID,"leave",localtime,0,new HashMap<CharSequence, Long>());
		// SyncMsg sync = new SyncMsg(ID,"leave",localtime,0,new HashMap<String, Long>());
		writer.publish(topics.get(0), sync, lbts);
	}

	public void close(){
		reader.close();
		writer.close();
	}

	public void join() {
		
		SyncMsg sync = new SyncMsg(ID,"join",localtime,0,new HashMap<CharSequence, Long>());
		writer.publish(topics.get(0), sync, lbts);

		while (participants.size() < expectedNoInitialParticipants) {
			//SyncMsg sync = new SyncMsg(ID,ActionType.JOIN,localtime,0,mySentMsgList);
//			SyncMsg sync = new SyncMsg(ID,"join",localtime,0,new HashMap<CharSequence, Long>());
			// SyncMsg sync = new SyncMsg(ID,"join",localtime,0,new HashMap<String, Long>());
//			writer.publish(topics.get(0), sync);
			reader.poll();

//			logger.debug("participants.size() < expectedNoInitialParticipants:    " + participants.size() +" < " + expectedNoInitialParticipants);
//			System.out.println("participants.size() < expectedNoInitialParticipants:    " + participants.size() +" < " + expectedNoInitialParticipants);
			try {Thread.sleep(1000);} catch (InterruptedException e) {	e.printStackTrace();}
		}   	    
//		logger.info("Stepping in at " + lbts);
	}

	@Override
	public <T> void receive(ConsumerRecord r, long time, int epoch, String hsender) {
//		public <T> void receive(String topic, String key, T o, long timestamp) {
		
		SyncMsg syncMsg = (SyncMsg) r.value();
//		logger.debug("received action="+syncMsg.getAction().toString()+ " from "+syncMsg.getSender().toString() + " with time=" + syncMsg.getTime());
	
		String action =  syncMsg.getAction().toString();
		String sender = syncMsg.getSender().toString();
		if(action.equals("request") || action.equals("join") ) {
			synchronized (lock) {
				participants.put(syncMsg.getSender().toString(), syncMsg);
		}
				//we now switch from received incremental topic counts to stored absolute counts
				updateExpectedMessages(syncMsg.getSender().toString(), syncMsg.getMessages());
				
		} 
		else if(action.equals("leave") ){
				if (participants.containsKey(sender)){
					participants.remove(sender);
				}
		}	

		calcLBTS(); //why not only updating for current sender? taking care of leavers?
	}

	void updateExpectedMessages(String sender, Map<CharSequence,Long> msgs){
		synchronized (lock) {
		for(Entry<CharSequence,Long> e : msgs.entrySet()){
				String topic = e.getKey().toString();
				Long num = e.getValue();

				//use only topics that are counted
				if(ConsumerImplKafka.hasSubscribedCounting(topic)) {
					long old = 0;
					if (expectedMsgs.get(topic) != null){
						old = expectedMsgs.get(topic);
					}
					long n = expectedMsgs.merge(topic, num, Long::sum);
//					logger.info(sender + " has announced "+num+" message on " + topic + " --> now " +  n + " in total (before "+ old +")");
				}
				else{
//					System.out.println("ignoring that " + sender + " has sent a message on " + topic + " --> i did not subscribe to that");
//					System.out.println("i only subscribed to");
//					for(String t : KafkaReader.getCountingSubscribedTopics()){
//						System.out.println(t);
//					}
//					System.out.println(KafkaReader.getCountingSubscribedPattern());
				} 
			}
		}
	}

	static void calcLBTS(){
		synchronized (lock) {
				long mintime = -1;
				int minepoch = -1;

				for(SyncMsg s : participants.values()) {
					
					if (mintime == -1 || s.getTime() < mintime ) {
						mintime = s.getTime();
						minepoch = s.getEpoch();
						
					}
					if (s.getTime() == mintime && s.getEpoch() < minepoch){
						minepoch = s.getEpoch();
					}

				}				
				lbts = mintime;
				lbep = minepoch;
				noParticipants = participants.size();
//				logger.info("lbts=" + lbts + ", lbep=" + lbep);
		}
	}
	public static long getLocaltime(){
		return localtime;
	}
}
