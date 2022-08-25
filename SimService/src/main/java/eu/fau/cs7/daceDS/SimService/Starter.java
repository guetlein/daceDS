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
import java.text.SimpleDateFormat;
import java.util.Calendar;

import eu.fau.cs7.daceDS.Component.Config;


public class Starter{

	public static void main(String[] args) {

		SimpleDateFormat dfDate  = new SimpleDateFormat("yyyy.MM.dd_HH:mm:ss");
		Calendar c = Calendar.getInstance(); 
		String date=dfDate.format(c.getTime());
		System.setProperty(Config.LOG_FILE, "/tmp/SimulationService_"+date+".txt");
		Config.readConfig();
		System.setProperty(Config.LOG_DIR, Config.get(Config.LOG_DIR));
		System.setProperty(Config.LOG_FILE, Config.get(Config.LOG_DIR)+"/SimulationService_"+date+".txt");
		
		SimulationService.main(args);

	}
}
