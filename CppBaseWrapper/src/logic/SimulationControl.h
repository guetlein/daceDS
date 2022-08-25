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

#include <stdio.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>
#include <list>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "api/Provision.h"
#include "communication/Producer.h"
#include "datamodel/Observer.hh"
#include "logic/Observer.h"
#include "logic/TimeSync.h"
#include "main/SimulationWrapper.h"
#include "util/Config.h"
#include "util/Defines.h"

#include "communication/ProvisionHandler.h"

using namespace std;

namespace daceDS {

class SimulationWrapper;
class Provision;
class ProvisionHandler;

/**
 * Base SimulationControler class, might be inherited and extended by wrappers.
*/
class SimulationControl {
   protected:
    std::vector<std::shared_ptr<Observer>> observers;
    std::shared_ptr<Producer> producer;
    std::shared_ptr<SimulationWrapper> wrapper;
    std::shared_ptr<Provision> provision;
    int64_t localSimulatorTimeMs;


    std::shared_ptr<ProvisionHandler> phdl;

   public:
    SimulationControl(){};
    virtual ~SimulationControl(){};

    virtual void init(std::shared_ptr<Producer> p, std::shared_ptr<SimulationWrapper> w, std::shared_ptr<ProvisionHandler> ph) {
        producer = p;
        wrapper = w;
        phdl = ph;
    };
    virtual void run(){};
    virtual void close(){};

    virtual void addObserver(datamodel::Observer o);
    virtual void runObservers();
};

}  // namespace daceDS
