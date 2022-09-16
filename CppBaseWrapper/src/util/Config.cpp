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
#include "Config.h"

#include <exception>
#include <fstream>
#include <iostream>

namespace daceDS {
Config* Config::inst = 0;

Config::Config() {}

Config* Config::getInstance() {
    if (inst == 0) {
        inst = new Config();
    }
    return inst;
}

void Config::setScenarioID(std::string s) {
    scenarioID = s;
}
void Config::setSimulatorID(std::string s) {
    simulatorID = s;
}
std::string Config::getScenarioID() {
    return scenarioID;
}

std::string Config::getSimulatorID() {
    return simulatorID;
}

std::string Config::getBaseDir() {
    return get("rootDir") + "/" + getScenarioID() + "/" + getSimulatorID();
}

std::string Config::getResourceDir() {
    return getBaseDir() + "/resource";
}

std::string Config::getOutputDir() {
    return getBaseDir() + "/output";
}

std::string Config::getLogDir() {
    // return get("logDir") + "/" + getScenarioID() + "/" + getSimulatorID() + "/";
    return get("logDir") + "/";
}

bool Config::readConfig(std::string path) {
    try {
        KDEBUG("Reading from " << path);
        std::ifstream infile(path);
        std::string line;
        configParamMap.clear();
        while (std::getline(infile, line)) {
            //skip comments
            if (line.size() > 0 && line[0] == '#')
                continue;

            //split line
            std::vector<std::string> v;
            int start = 0;
            auto pos = line.find_first_of("=", start);
            if (pos == std::string::npos) {
                continue;
            }
            //key
            std::string key, value;
            key = line.substr(start, pos - start);
            start = pos + 1;
            //value
            if (start < line.length())
                value = line.substr(start, line.length() - start);
            else
                continue;

            configParamMap[key] = value;
            KDEBUG(key << "=" << value);
        }
    } catch (const std::ifstream::failure& e) {
        KDEBUG("Exception opening/reading file");
        return false;
    }
    KDEBUG("Processed " << configParamMap.size() << " entries");

    if (configParamMap.size() == 0) {
        return false;
    }
    return true;
}

std::string Config::get(std::string key) {
    return configParamMap[key];
}

void Config::set(std::string key, std::string val) {
    configParamMap[key] = val;
}

std::string Config::getTopic(std::string channel, std::string topic) {
    return channel + "." + scenarioID + "." + topic;
}

// std::string Config::getAPITopic(std::string c) {
//     return Config::getInstance()->get("channelInteraction") + "." + scenarioID +
//            ".traffic.micro." + c;
// }

std::string Config::getInteractionTopic(std::string domain, std::string layer) {
    return Config::getInstance()->get("channelInteraction") + "." + scenarioID + "." + domain + "." + layer;
}

std::string Config::getProvisionTopic(std::string domain, std::string layer, std::string s) {
    return Config::getInstance()->get("channelProvision") + "." + scenarioID + "." + domain + "." + layer + + "." + s;
}

std::string Config::getProvisionTopic(std::string c) {
    return Config::getInstance()->get("channelProvision") + "." + scenarioID +
           ".traffic.micro." + c;
}


// //if subject is persistent we can use include the id in the topic
// std::string Config::getPersistentTopic(std::string subject, std::string entityId, std::string attribute) {
//     KDEBUG("subject=" << subject);
//     KDEBUG("entityId=" << entityId);
//     KDEBUG("attribute=" << attribute);
//     std::string t = getProvisionTopic(subject);

//     entityId = escapeSpecialChars(entityId);
//     t += "." + entityId;
//     if (attribute.size() > 0) {
//         t += "." + attribute;
//     }
//     return t;
// }

std::string Config::getPersistentTopic(std::string scope, std::string entityId, std::string element) {
    KDEBUG("scope=" << scope);
    KDEBUG("element=" << element);
    KDEBUG("entityId=" << entityId);
    std::string t = getProvisionTopic(scope);

    entityId = escapeSpecialChars(entityId);
    t += "." + entityId;
    if (element.size() > 0) {
        t += element.substr(scope.size());
    }
    return t;
}

std::string Config::getProvisionBaseTopic(std::string c) {
    return Config::getInstance()->get("channelProvision") + "." + scenarioID + "." + c;
}

std::string Config::getOrchestrationTopic(std::string c) {
    return Config::getInstance()->get("channelOrchestration") + "." + scenarioID + "." + c;
}
}