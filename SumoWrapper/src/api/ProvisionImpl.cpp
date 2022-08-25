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
#include "ProvisionImpl.h"
using namespace daceDS;

std::vector<std::string> Provision::methods = {
    "scenario",
    "resource"};

/*****************
 * Handler methods
 * ******************/

void ProvisionImpl::scenario(ProvisionTopicMetadata& pmeta, datamodel::Scenario& scenario) {
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

    ///////////////////
    // extract required resources
    resources.clear();

    // // Is there a global map file?
    // if (sce->RoadMap != "") {
    //     ResourceFileMeta rfm;
    //     rfm.path = sce->RoadMap;
    //     rfm.type = STR_TYPE_ROADMAP;
    //     rfm.received = false;
    //     resources.push_back(rfm);
    //     KDEBUG("added global road map: " << sce->RoadMap);
    // }

    // // input that match my layer
    // KDEBUG("ScenarioDescription.Inputs size=" << sce->Inputs.size());
    // for (int i = 0; i < sce->Inputs.size(); i++) {
    //     datamodel::Input input = sce->Inputs[i];
    //     if (input.layer != sim->Layer) {
    //         continue;
    //     }
    //     ResourceFileMeta rfm;
    //     rfm.path = input.path;
    //     rfm.type = STR_TYPE_INPUT;
    //     rfm.received = false;
    //     resources.push_back(rfm);
    //     KDEBUG("added layer input: " << input.path);
    // }

    // resources for my instance
    KDEBUG("sim->resources size=" << sim->resources.size());
    for (std::pair<std::string, std::string> res : sim->resources) {
        ResourceFileMeta rfm;
        rfm.path = res.first;
        //in case this is a reference we will strip the id to the filename at this point
        if(rfm.path.find(":") != std::string::npos){
            rfm.path = rfm.path.substr(rfm.path.find_last_of("/\\") + 1);
        }
        
        rfm.type = res.second;
        rfm.received = false;

        //if there is already a global file with the same path, we will replace it
        //remove old
        resources.erase(
            std::remove_if(resources.begin(), resources.end(), [&](ResourceFileMeta const& existingRfm) {
                return existingRfm.path == rfm.path;
            }),
            resources.end());
        //add new
        resources.push_back(rfm);
        KDEBUG("added instance resource: " << rfm.path);

        std::string rL = "";
        for (ResourceFileMeta& r : getResources()) {
            rL += r.path + "," + r.type + ",received=" + std::to_string(r.received) + "; ";
        }
        KDEBUG("total resource list is now: [" << rL << "]");
    }

    ctrl->notifySceChange();
}

void ProvisionImpl::resource(ProvisionTopicMetadata& pmeta, datamodel::ResourceFile& rf) {
    KDEBUG("ResourceFile received ");
    if (!sce) {  //before we dont have scenario file, we dont know which resource files are relevant for us
        KERROR("Got ResourceFile before sce");
        exit(1);
    }

    //check if this rf is on our list
    ResourceFileMeta* resourceFileMeta = 0;
    for (auto& rfm : resources) {
        if (rfm.path == rf.ID) {
            resourceFileMeta = &rfm;
            break;
        }
    }
    if (!resourceFileMeta) {
        KDEBUG(rf.ID << " is not for me. only interested in: ");
        for (ResourceFileMeta rfm : resources) {
            KDEBUG(rfm.path);
        }
        return;
    }

    //get destination path
    std::string destinationPath = Config::getInstance()->get("rootDir") + "/" + Config::getInstance()->getScenarioID() + "/" + Config::getInstance()->getSimulatorID() + "/resource/" + rf.ID;
    KDEBUG("destination is " << destinationPath);

    //we have two cases: file is embedded directly or there is only a reference
    //is it just a reference; do we need to fetch resource?

    bool validFile =  !rf.File.is_null() && rf.File.get_bytes().size()>0;
    bool validFileReference = !rf.FileReference.is_null() && rf.FileReference.get_string().size()>0;

    //do we use a file reference?
    if (!validFile && validFileReference) {

        std::string ref = rf.FileReference.get_string();
        if(ref.rfind("s3://", 0) == 0){        
            KDEBUG(rf.ID << " file field is null, querying file from s3 storage");
            std::string cmd = "./retrieveFileFromDB.sh " + ref + " " + destinationPath;
            system(cmd.c_str());
        } 
        else if(ref.rfind("file://", 0) == 0){        
            KDEBUG(rf.ID << " file field is null, querying file from local storage");
            std::string cmd = "./retrieveFileFromDisk.sh " + ref + " " + destinationPath;
            system(cmd.c_str());
        } 
        else {
            KERROR("CANNOT HANDLE THIS FILE REFERENCE: " << ref );
        }
    } 
    //or already got the payload?
    else if ( validFile ){
        KDEBUG(rf.ID << " file reference field is null, writing bytes from File struct to disk");
        std::vector<unsigned char> f = rf.File.get_bytes();
        std::ofstream file(destinationPath, std::ios::out | std::ios::binary);  //todo: check if dir exists
        file.write(reinterpret_cast<const char*>(f.data()), f.size());
    } else {
        KERROR("ERROR " << rf.ID << " file field and file reference field is null!");
        return;
    }

    KDEBUG("setting receive status to true for '" << resourceFileMeta->path << "'");
    resourceFileMeta->received = true;
}

void ProvisionImpl::Vehicle::traffic(ProvisionTopicMetadata& pmeta, datamodel::Micro& m) {

    KDEBUG("adding " << m.vehicleID << " on edge " << m.edge << " to processing buffer");
    addIncomingVehicle(m);
}

/***********************
* Scenario
************************/

/* Return when SCE was received */
//todo: remove busy wait
std::shared_ptr<datamodel::Scenario> ProvisionImpl::waitForScenario() {
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

std::vector<ResourceFileMeta> ProvisionImpl::getResources() {
    return resources;
}

/* returns when all announced resources have been received. */
//todo: remove busy wait
void ProvisionImpl::waitForResources() {
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

std::string ProvisionImpl::getFileByType(std::string type) {
    for (ResourceFileMeta rfm : resources) {
        if (rfm.type == type && rfm.received)  //if resource is of type and already received
            return rfm.path;
    }
    return "";
}
