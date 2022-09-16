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
#include "OppProvisionImpl.h"


std::vector<std::string> daceDS::Provision::methods = {
    "scenario",
    "resource"};

/*****************
 * Handler methods
 * ******************/

void daceDS::OppProvisionImpl::scenario(ProvisionTopicMetadata& pmeta, datamodel::Scenario& scenario) {
    if (Config::getInstance()->getScenarioID() != scenario.scenarioID) {
        KERROR("ERROR scenarioIDs must not change " << Config::getInstance()->getScenarioID() << " --> " << scenario.scenarioID);
        exit(EXIT_FAILURE);
    }

    //currently we only process the initial sce --> need further resource management, simple res.clear() is not enough
    if (sce) return;

    KDEBUG("Scenario received, updating current config... ");
    sce = std::make_shared<datamodel::Scenario>(scenario);

    ///////////////////
    //extract description for this simulator instance

    if (sim) sim.reset();
    for (datamodel::BB s : sce->buildingBlocks) {
        if (Config::getInstance()->getSimulatorID() != s.instanceID) {
            continue;
        }
        sim = std::make_shared<datamodel::BB>(s);
    }

    if (!sim) {
        KERROR("ATTENTION! My simulator is currently not listed in the scenario config");
        exit(EXIT_FAILURE);
    }

}

void daceDS::OppProvisionImpl::resource(ProvisionTopicMetadata& pmeta, datamodel::ResourceFile& rf) {
    KDEBUG("ResourceFile received ");
}


/***********************
* Scenario
************************/

/* Return when SCE was received */
//todo: remove busy wait
std::shared_ptr<daceDS::datamodel::Scenario> daceDS::OppProvisionImpl::waitForScenario() {
    KDEBUG("wait until scenario file is received on scenariotopic");
    while (!sce) {
        usleep(WAITING_TIME);
        KDEBUG("waiting for scenario file");
    }
    return sce;
}

/*****************
 * Resources
 *****************/

std::vector<daceDS::ResourceFileMeta> daceDS::OppProvisionImpl::getResources() {
    return resources;
}

/* returns when all announced resources have been received. */
//todo: remove busy wait
void daceDS::OppProvisionImpl::waitForResources() {
    KDEBUG("will wait for resources ");

    bool stillwaiting = true;
    while (stillwaiting) {
        stillwaiting = false;
        for (ResourceFileMeta& rfm : getResources()) {
            // KDEBUG(rfm.path << " received: " << rfm.received);
            if (!rfm.received) {
                usleep(WAITING_TIME);
                KDEBUG("waiting for '" << rfm.path << "'");
                stillwaiting = true;
            }
        }
    }

    //at this point we have all files or at least file references
}

std::string daceDS::OppProvisionImpl::getFileByType(std::string type) {
    for (ResourceFileMeta rfm : resources) {
        if (rfm.type == type && rfm.received)  //if resource is of type and already received
            return rfm.path;
    }
    return "";
}


// void daceDS::OppProvisionImpl::handleTopicMicro(int64_t t , int64_t st, daceDS::datamodel::Micro micro){
//     (std::dynamic_pointer_cast<OppWrapper>(wrapper))->handleTopicMicro(t, st, micro);
// }

void daceDS::OppProvisionImpl::handleTopicSendRadioMsg(daceDS::datamodel::RadioMsg msg){
    (std::dynamic_pointer_cast<OppWrapper>(wrapper))->handleTopicSendRadioMsg(msg);
}
