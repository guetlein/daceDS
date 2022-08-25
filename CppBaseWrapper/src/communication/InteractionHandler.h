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
#include "api/Traffic/Micro/Interaction.h"
#include "communication/AvroHelper.h"
#include "communication/Producer.h"
#include "communication/ConsumedMessage.h"
#include "cppkafka/configuration.h"
#include "cppkafka/consumer.h"
#include "cppkafka/utils/consumer_dispatcher.h"
#include "datamodel/SyncMsg.hh"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/log.h"

namespace daceDS {

class Producer;

class InteractionHandler : public MessageHandler {
    std::shared_ptr<Interaction> api;
    std::shared_ptr<Producer> producer;

   public:
    InteractionHandler(std::shared_ptr<Interaction> i, std::shared_ptr<Producer> p);
    ~InteractionHandler(){};

    bool handle(ConsumedMessage* msg) override;
    void parseScopeVehicle(ConsumedMessage* msg, InteractionTopicMetadata2& imeta);
    void parseScopeWorld(ConsumedMessage* msg, InteractionTopicMetadata2& imeta);
};
}  // namespace daceDS
