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
#include <mutex>
#include <stdexcept>

#include "datamodel/Micro.hh"
#include "datamodel/ResourceFile.hh"
#include "datamodel/Scenario.hh"
#include "datamodel/BB.hh"
#include "main/SimulationWrapper.h"
#include "util/Config.h"
#include "util/Defines.h"


namespace daceDS {
class SimulationWrapper;
class Provision {
   protected:
    std::shared_ptr<SimulationWrapper> wrapper;

   public:
    static std::vector<std::string> methods;
    static std::vector<std::string> getMethods() {
        return methods;
    }

    int bla = 42;
    Provision(std::shared_ptr<SimulationWrapper> w) : wrapper(w){};
    Provision(){};
    ~Provision(){};

    //this is layer independent
    virtual void resource(ProvisionTopicMetadata& pmeta, datamodel::ResourceFile& r) { throw EXCEPTION_NOT_IMPLEMENTED; };
    virtual void scenario(ProvisionTopicMetadata& pmeta, datamodel::Scenario& r) { throw EXCEPTION_NOT_IMPLEMENTED; };

};
}  // namespace daceDS
