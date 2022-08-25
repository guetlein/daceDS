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

#include <libsumo/TraCIDefs.h>
#include <stdio.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <thread>
#include <fstream>
#include <vector>

#include "../SumoWrapper.h"
#include "../api/ProvisionImpl.h"

#ifdef USING_TRACI
    #include "../api/SumoConnectionTraCI.h"
#else
    #include "../api/SumoConnectionLibSUMO.h"
#endif

#include "../api/SumoObserver.h"
#include "communication/kafka/KafkaProducer.h"
#include "datamodel/Observer.hh"
#include "logic/TimeSync.h"
#include "util/Config.h"
#include "util/Defines.h"

namespace daceDS {
class SumoWrapper;
class KafkaProducer;
class Observer;
class ProvisionImpl;
class SUMO_API_MODE;


class SumoSimulationControl : public SimulationControl {

    std::shared_ptr<SUMO_API_MODE> sumoConnection;

    bool sceChanged = false;
    bool done = false;
    
    int64_t    totalT = 0;
    int64_t    totalS = 0;
    int64_t    totalP = 0;
    int64_t    totalO = 0;
    int64_t    totalOutg = 0;

    int sentVehicles = 0;
    int adoptedVehicles = 0;
    int failedAdoptions = 0;
    std::vector<std::string> sentVehicleIDs;
    std::vector<std::string> adoptedVehicleIDs;
    std::vector<std::string> failedAdoptionIDs;

    
   public:
    SumoSimulationControl(){};
    ~SumoSimulationControl(){};

    void init(std::shared_ptr<Producer> p, std::shared_ptr<SumoWrapper> w, std::shared_ptr<ProvisionImpl> pr, int port, std::shared_ptr<ProvisionHandler> phdl);
    void run();
    void close();

    void processInteractions(int64_t lbts);
    void processProvisions(int64_t lbts);
    void processOutgoing(int64_t simulatorTimeMs);

    map<std::string, string> ghostsToRemove;
    std::vector<std::string> freshlyAdoptedVehicleIDs;
    std::shared_ptr<ProvisionImpl> provision;

    std::vector<std::shared_ptr<Observer>> observers;
    void addObserver(datamodel::Observer o);
    void runObservers(int64_t simulatorTimeMs);

    void notifySceChange();
    void processDescription();
    std::shared_ptr<datamodel::Scenario> sce;
    //std::shared_ptr<datamodel::TrafficSimulator> sim;

    auto getSumoConnection() { return sumoConnection; };
    
    std::vector<std::string> outgoingLinks;
    void setOutgoingLinks(std::vector<std::string> o){outgoingLinks=o;};
    void provideGhostUpdates(int64_t simulatorTimeMs);

    void writeVehicleData(int64_t simulatorTimeMs);

};
}  // namespace daceDS
