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

import java.util.HashMap;
import java.util.List;


/**
 * A generic producer class.
 *  
 * @author guetlein
 *
 * @param <T>
 */
public abstract class  Producer<T>
{

	protected long time = -1;
	protected static long step = 1;
	protected String id;
    
    protected static HashMap<String, Long> sentCounting = new HashMap<String, Long>();
	protected boolean countingProducer = false;
	protected boolean exactlyOnce = false; 
	protected boolean plainStringValue = true; 


	public Producer(String id){
		this.id = id;
	}

	public Producer(String id, boolean counting){
		this.id = id;
		this.countingProducer = counting;
	}

	public void setExactlyOnce(boolean b){
		this.exactlyOnce = b;
	}


	public abstract void init();
	
	public abstract void initPlainString();

	public abstract void initTopicNamingStrategy();

	public abstract void createTopics(List<String> topicNames);

	public abstract boolean publish(String topic, T payload, long time);

	public abstract boolean publish(String topic, String key, T payload, long time, int epoch, String sender);
	
	public static HashMap<String, Long> getSentCounting(){
		return sentCounting;
	}

	public abstract void close();
}
