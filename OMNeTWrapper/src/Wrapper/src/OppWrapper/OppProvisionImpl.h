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
#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "OppWrapper.h"
#include "api/Provision.h"
#include "main/SimulationWrapper.h"
#include "util/Config.h"
#include "util/Defines.h"

namespace daceDS {

class OppProvisionImpl : public Provision {
   public:
    OppProvisionImpl(std::shared_ptr<SimulationWrapper> w) : Provision(w) {
    };
    ~OppProvisionImpl(){};

    void resource(ProvisionTopicMetadata& pmeta, datamodel::ResourceFile& r);
    void scenario(ProvisionTopicMetadata& pmeta, datamodel::Scenario& r);


    // void handleTopicMicro(int64_t, int64_t, daceDS::datamodel::Micro micro);
    void handleTopicSendRadioMsg(daceDS::datamodel::RadioMsg msg);


    std::shared_ptr<datamodel::Scenario> sce;
    std::shared_ptr<datamodel::BB> sim;
    std::map<std::pair<std::string, std::string>, bool> receivedResources;
    std::vector<ResourceFileMeta> resources;
    std::vector<ResourceFileMeta> getResources();

    std::shared_ptr<datamodel::Scenario> waitForScenario();
    std::shared_ptr<datamodel::BB> getSim() { return sim; };
    std::shared_ptr<datamodel::Scenario> getScenario() { return sce; };
    void waitForResources();
    std::string getFileByType(std::string type);

};
}  // namespace daceDS
