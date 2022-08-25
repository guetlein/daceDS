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
#include <string>
#include <vector>

#include "util/log.h"

/**
 * Generic producer class. Use a custom producer that inherits, 
 * in order to connect specific middlewares such as Kafka.
*/
namespace daceDS {
class Producer {
    bool counting = false;
    
   protected:
     std::string id;

   public:
    Producer(bool c = false) : counting(c){};
    ~Producer(){};

    virtual void init(std::string broker, std::vector<std::string> topics, std::string i, std::string loglevel = ""){};
    virtual bool createTopic(std::string topic) { return false; };

    virtual bool publish(std::string topic, std::string payload);
    virtual bool publish(std::string topic, std::string payload, int64_t time);
    virtual bool publish(std::string topic, std::string key, std::string payload, int64_t time, bool repeatOnError = false);

    virtual bool publish(std::string topic, std::vector<char>& payload, bool repeatOnError);
    virtual bool publish(std::string topic, std::vector<char>& payload);
    virtual bool publish(std::string topic, std::vector<char>& payload, int64_t time);
    virtual bool publish(std::string topic, std::string key, std::vector<char>& payload, int64_t time, bool repeatOnError = false);

    bool isCounting() { return counting; };
    static std::map<std::string, int64_t> countingSentMessages;
};
}  // namespace daceDS
