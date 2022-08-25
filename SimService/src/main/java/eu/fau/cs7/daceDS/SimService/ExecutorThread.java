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
package eu.fau.cs7.daceDS.SimService;

import java.io.IOException;
import org.apache.log4j.Logger;

import eu.fau.cs7.daceDS.Component.Config;
import eu.fau.cs7.daceDS.datamodel.Scenario;


/**
 * Used for the execution of a process (i.e. simulator).
 * 
 * @author guetlein
 */
public class ExecutorThread extends Thread{

	Scenario scenario;
	String basePath;
	String callString;
	String killString;
	String type;
	String id;
	Process proc = null;
	long pid = -1;
	boolean running = false;
	int exitVal = -1;

	Logger logger = Logger.getLogger(ExecutorThread.class.getName());
	
	ExecutorThread(String basePath, String type, String id, Scenario scenario){
		this.basePath  = basePath;
		this.scenario  = scenario;
		this.type = type;
		this.id = id;
		logger  = Logger.getLogger(ExecutorThread.class.getName()+"."+type+"."+id);		
		callString = basePath+"/"+type+"/"+Config.EXECUTABLE;
		callString += " " + scenario.getScenarioID() + " " + id;
		
		killString = basePath+"/"+type+"/"+Config.EXECUTABLE_KILL;
		killString += " " + scenario.getScenarioID() + " " + id;
	}
	
	
	public void runKillScript() {
		logger.info("Killstring is: " + killString);
		Runtime rt = null;
		Process proc = null;
		try {
			rt = Runtime.getRuntime();
			proc = rt.exec(killString);
			long exitVal = proc.waitFor();
			logger.info(id + ": Exit value: " + exitVal);
		} catch (IOException | InterruptedException e) {
			if(proc != null) proc.destroyForcibly();
		}
	}
	
	//todo: very prototypical
	public void run() {
		logger.info("Callstring is: " + callString);
		Runtime rt = null;
		try {
			running = true;
			rt = Runtime.getRuntime();
			proc = rt.exec(callString);
			pid = proc.pid();
			
			exitVal = proc.waitFor();
			
			running = false;
			logger.info(id + ": Exit value: " + exitVal);
		} catch (IOException | InterruptedException e) {
			if(proc != null) proc.destroyForcibly();
			logger.error(callString + " --> Process exception: " + e.getLocalizedMessage());
			running = false;
		}
		runKillScript();
	}

	public void terminate() {
		if(proc != null && running) {
			proc.destroy();
		}
	}
	
	public long getPID(){
		return pid;
	}
	public long getExitVal(){
		return exitVal;
	}
	public boolean isRunning(){
		return running;
	}
}
