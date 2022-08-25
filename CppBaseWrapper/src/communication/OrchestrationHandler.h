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

#include <csignal>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "MessageHandler.h"
#include "api/Traffic/Micro/Orchestration.h"
#include "communication/AvroHelper.h"
#include "datamodel/CtrlMsg.hh"
#include "datamodel/StatusMsg.hh"
#include "datamodel/SyncMsg.hh"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/log.h"

namespace daceDS {
class OrchestrationHandler : public MessageHandler {
    std::shared_ptr<Orchestration> api;

   public:
    OrchestrationHandler(std::shared_ptr<Orchestration> o) : MessageHandler() { api = o; };
    virtual ~OrchestrationHandler(){};

    bool handle(ConsumedMessage* msg) override;
};
}  // namespace daceDS
