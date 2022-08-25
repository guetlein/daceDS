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

#include <rdkafkacpp.h>

#include <iostream>
#include <map>
#include <stdexcept>

#include "communication/Producer.h"
#include "util/Config.h"
#include "util/log.h"

using std::cin;
using std::cout;
using std::endl;
using std::exception;
using std::getline;
using std::string;

/**
 * Based on the cppkafka examples
 * */

namespace daceDS {
class KafkaProducer : public Producer {
    RdKafka::Producer* producer;
    RdKafka::Conf* conf;
    std::map<std::string, RdKafka::Topic*> topicMap;
    static std::map<std::string, int64_t> sentMessages;

   public:
    KafkaProducer(bool counting = false) : Producer(counting){};
    ~KafkaProducer() { delete producer; };

    void init(std::string broker, std::vector<std::string> topics, string i, string loglevel = "");
    bool createTopic(std::string topic);

    virtual bool publish(std::string topic, std::string payload);
    virtual bool publish(std::string topic, std::string payload, int64_t time);
    virtual bool publish(std::string topic, std::string key, std::string payload, int64_t time, bool repeatOnError = false);

    bool publish(std::string topic, std::vector<char>& payload, bool repeatOnError);
    bool publish(std::string topic, std::vector<char>& payload);
    bool publish(std::string topic, std::vector<char>& payload, int64_t time);
    bool publish(std::string topic, std::string key, std::vector<char>& payload, int64_t time, bool repeatOnError = false);

};
}  // namespace daceDS
