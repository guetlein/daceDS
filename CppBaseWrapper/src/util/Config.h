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

#include <fstream>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "util/log.h"

namespace daceDS {
class Config {
   private:
    std::map<std::string, std::string> configParamMap;
    std::string scenarioID;
    std::string simulatorID;
    static Config* inst;

   public:
    Config();
    static Config* getInstance();
    void setScenarioID(std::string s);
    void setSimulatorID(std::string s);
    std::string getScenarioID();
    std::string getSimulatorID();

    bool readConfig(std::string path);

    std::string get(std::string key);
    void set(std::string key, std::string val);

    std::string getTopic(std::string channel, std::string topic);
    // std::string getAPITopic(std::string call);
    std::string getProvisionTopic(std::string c);
    std::string getProvisionBaseTopic(std::string c);
    std::string getOrchestrationTopic(std::string c);

    std::string getBaseDir();
    std::string getResourceDir();
    std::string getOutputDir();
    std::string getLogDir();

    // std::string getInteractionTopic(std::string c);
    std::string getInteractionTopic(std::string domain, std::string layer);
    std::string getProvisionTopic(std::string domain, std::string layer, std::string s);
    std::string getPersistentTopic(std::string subject, std::string entityId, std::string attribute);
};
}  // namespace daceDS
