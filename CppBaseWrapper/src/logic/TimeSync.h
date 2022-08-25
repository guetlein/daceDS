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
#include <condition_variable>
#include <cstring>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "communication/Consumer.h"
#include "communication/Producer.h"
#include "communication/TimeSyncHandler.h"
#include "datamodel/SyncMsg.hh"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/log.h"

namespace daceDS {
class Consumer;
class Producer;
class TimeSyncHandler;

/**
 * Implementation of the conservative time sync mechanism.
*/
class TimeSync {
   private:
    std::shared_ptr<Producer> producer;
    std::shared_ptr<Consumer> consumer;
    std::string participantID;
    std::string syncTopic;
    int64_t syncedParticipants;
    std::shared_ptr<TimeSyncHandler> handler;

    volatile int64_t lbtsInMS = -1;
    std::map<std::string, int64_t> timeRequests;
    std::map<std::string, int64_t> expectedMessages;
    std::map<std::string, int64_t> lastAccouncedSentMessages;

    std::mutex cv_m;
    std::condition_variable cv;
    bool timeOK(int64_t t);
    bool msgsOK();

   public:
    TimeSync(std::string t, int64_t s);
    ~TimeSync(){};

    // void init( std::shared_ptr<Producer> p, std::shared_ptr<Consumer> c);

    template <class TC, class TP>
    void init() {
        KDEBUG("Initializing TimeSync");
        //todo: check TC and TP
        consumer = std::make_shared<TC>(handler);
        producer = std::make_shared<TP>();
    };

    void prepare();

    void calcLBTS();
    int64_t getLBTSInMS() { return lbtsInMS; }

    void joinTiming(int64_t time = 0L);
    void leaveTiming();
    void timeAdvance(int64_t stepInMS);
    void timeAdvanceToAbsolutePoint(int64_t newkafkatime);

    void waitForMessages();

    void handleTopicSyncMsgJoin(datamodel::SyncMsg syncMsg);
    void handleTopicSyncMsgLeave(datamodel::SyncMsg syncMsg);
    void handleTopicSyncMsgGrant(datamodel::SyncMsg syncMsg);
    void handleTopicSyncMsgRequest(datamodel::SyncMsg syncMsg);

    void tick();
};
}  // namespace daceDS
