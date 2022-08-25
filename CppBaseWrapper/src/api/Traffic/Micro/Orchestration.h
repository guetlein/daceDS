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

#include "main/SimulationWrapper.h"
#include "util/Config.h"
#include "util/Defines.h"

/**
* This class provides the generic orchestration interface description. 
*/
//todo: split into layer (in)dependent parts

namespace daceDS {
class Orchestration {
   protected:
    std::shared_ptr<SimulationWrapper> wrapper;

   public:
    static std::vector<std::string> methods;
    static std::vector<std::string> getMethods() {
        return methods;
    }

    Orchestration(SimulationWrapper* w) : wrapper(w){};
    ~Orchestration(){};

    //this is layer independent
    virtual void ctrl(OrchestrationTopicMetadata& ometa, datamodel::CtrlMsg& ctrl) { throw EXCEPTION_NOT_IMPLEMENTED; };
    virtual void status(OrchestrationTopicMetadata& ometa, datamodel::StatusMsg& state) { throw EXCEPTION_NOT_IMPLEMENTED; };
};
}  // namespace daceDS
