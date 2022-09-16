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
#include "api/Interaction.h"
#include "main/SimulationWrapper.h"
#include "util/Config.h"
#include "util/Defines.h"

namespace daceDS {

class OppInteractionImpl : public Interaction {
   public:
    OppInteractionImpl(std::shared_ptr<SimulationWrapper> w) : Interaction() {
        wrapper = w;
    };
    virtual ~OppInteractionImpl(){};

    std::shared_ptr<SimulationWrapper> wrapper;

    void handleInteractionPositionUpdate(int64_t, int64_t, daceDS::datamodel::InteractionMsg msg);
    void handleInteractionSendRadioMsg(daceDS::datamodel::RadioMsg msg);

};
}  // namespace daceDS
