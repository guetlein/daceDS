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

#include "Consumer.h"
#include "communication/AvroHelper.h"
#include "datamodel/SyncMsg.hh"
#include "logic/TimeSync.h"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/log.h"

namespace daceDS {
class TimeSync;

class TimeSyncHandler : public MessageHandler {
    std::shared_ptr<TimeSync> timeSync;

   public:
    TimeSyncHandler(std::shared_ptr<TimeSync> t) : timeSync(t), MessageHandler(){};
    virtual ~TimeSyncHandler(){};

    bool handle(ConsumedMessage* msg) override;
};
}  // namespace daceDS
