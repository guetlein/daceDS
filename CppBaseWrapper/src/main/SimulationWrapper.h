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
#pragma once

#include <limits.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include <algorithm>
#include <chrono>
#include <cwchar>
#include <iostream>
#include <locale>
#include <map>
#include <string>

#include "api/Provision.h"
#include "communication/Consumer.h"
#include "communication/Producer.h"
#include "logic/SimulationControl.h"
#include "logic/TimeSync.h"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/Utils.h"
#include "util/log.h"

namespace daceDS {
class SimulationControl;
class Provision;

class SimulationWrapper {
    
   public:
    SimulationWrapper(std::string s, std::string t) : scenarioID(s), simulatorID(t){};
    virtual ~SimulationWrapper(){};

    virtual void runWrapper(){};
    virtual void endWrapper(){};
    virtual void killWrapper(){};

    std::shared_ptr<Producer> producer;
    std::shared_ptr<Provision> provision;
    std::shared_ptr<TimeSync> timeSync;
    std::shared_ptr<SimulationControl> ctrl;

    std::unique_ptr<Consumer> provisionConsumer;
    std::unique_ptr<Consumer> orchestrationConsumer;
    std::unique_ptr<Consumer> interactionConsumer;

    std::string getID() { return scenarioID + "_" + simulatorID; }
    std::string getID(std::string suf) { return getID() + suf; }

    std::string scenarioID = "not_initialized";   //has to be known before resources can be received
    std::string simulatorID = "not_initialized";  //has to be known before resources can be received
};
}
