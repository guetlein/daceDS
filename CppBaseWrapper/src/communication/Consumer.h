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

#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "MessageHandler.h"
#include "ConsumedMessage.h"
#include "logic/TimeSync.h"
#include "util/Defines.h"

/**
 * Generic consumer class. Use a custom consumer that inherits, 
 * in order to connect specific middlewares such as Kafka.
*/
namespace daceDS {
    
class TimeSync;

class Consumer {
    /*counting*/
    bool counting = false;
    std::shared_ptr<TimeSync> timeSync;

   public:
    Consumer(std::shared_ptr<MessageHandler> hdl, bool isCounting = false);
    virtual ~Consumer(){};

    virtual void subscribe(std::string broker, std::vector<std::string> &topic, std::string group_id, std::string loglevel = "");

    virtual void stop(){};
    virtual void listen(){};
    virtual bool process(ConsumedMessage* cmsg);

    std::shared_ptr<MessageHandler> handler;

    /*counting*/
    static std::map<std::string, int64_t> countingReceiveCounter;
    static std::vector<std::string> countingExpectedTopics;
    static std::vector<std::string> countingExpectedPatterns;
    static std::string getExpectation(std::string topic);
    bool isCounting() { return counting; };
    void setTimeSync(std::shared_ptr<TimeSync> t) { timeSync = t; };
    static bool isExpectedTopic(std::string topic);
};
}  // namespace daceDS
