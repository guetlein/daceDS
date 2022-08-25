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
#include "Producer.h"
using namespace daceDS;

std::map<std::string, int64_t> Producer::countingSentMessages;

bool Producer::publish(std::string topic, std::string payload) {
    return false;
}

bool Producer::publish(std::string topic, std::string payload, int64_t time) {
    return false;
}

bool Producer::publish(std::string topic, std::string key, std::string payload, int64_t time, bool repeatOnError) {
    if (!isCounting()) {
        KDBGCB("Published to " << topic << " --> not counting");
        return false;
    }

    KDBGCB("Published to " << topic << " --> counting");
    if (Producer::countingSentMessages.count(topic) == 0) {
        Producer::countingSentMessages[topic] = 0;
    }
    Producer::countingSentMessages[topic]++;
    return false;
}

bool Producer::publish(std::string topic, std::vector<char>& payload, bool repeatOnError) {
    return false;
}

bool Producer::publish(std::string topic, std::vector<char>& payload) {
    return false;
}

bool Producer::publish(std::string topic, std::vector<char>& payload, int64_t time) {
    return false;
}

bool Producer::publish(std::string topic, std::string key, std::vector<char>& payload, int64_t time, bool repeatOnError) {
    if (!isCounting()) {
        KDBGCB("Published to " << topic << " --> not counting");
        return false;
    }

    KDBGCB("Published to " << topic << " --> counting");
    if (Producer::countingSentMessages.count(topic) == 0) {
        Producer::countingSentMessages[topic] = 0;
    }
    Producer::countingSentMessages[topic]++;
    return false;
}
