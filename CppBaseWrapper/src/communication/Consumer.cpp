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
#include "Consumer.h"
using namespace daceDS;

std::map<std::string, int64_t> Consumer::countingReceiveCounter;
std::vector<std::string> Consumer::countingExpectedTopics;
std::vector<std::string> Consumer::countingExpectedPatterns;

Consumer::Consumer(std::shared_ptr<MessageHandler> hdl, bool isCounting = false) {
    handler = hdl;
    counting = isCounting;
};

void Consumer::subscribe(std::string broker,
                         std::vector<std::string> &topics,
                         std::string group_id,
                         std::string loglevel) {
    if (!isCounting()) {
        return;
    }
    for (std::string topic : topics) {
        countingReceiveCounter[topic] = 0;
        if (topic[0] == '^') {
            KDBGCB("added " << topic << " to the list of expected patterns");
            Consumer::countingExpectedPatterns.push_back(topic);
        } else {
            KDBGCB("added " << topic << " to the list of expected topics");
            Consumer::countingExpectedTopics.push_back(topic);  //todo: delete this list and set countingreceivecounter[t]=0?
        }
    }
}

bool Consumer::process(ConsumedMessage* cmsg) {

    // if we are not counting our messages, there is also no need for buffering them
    // process them right away and delete rdkafkamsg and cmsg afterwards
    if (!isCounting()) {
        bool suc = handler->handle(cmsg);
        delete cmsg;
        return suc;
    }

    bool suc = handler->bufferConsumedMessage(cmsg);

    std::string topic = cmsg->topic;
    // std::string topic = getExpectation(cmsg.topic);
    if (Consumer::countingReceiveCounter.count(topic) == 0)
        Consumer::countingReceiveCounter[topic] = 0;
    Consumer::countingReceiveCounter[topic]++;

    if (timeSync) {
        timeSync->tick();
        KDBGCB("ticking");
    }

    return suc;
}

std::string Consumer::getExpectation(std::string topic) {
    std::string t = "";
    if (std::find(Consumer::countingExpectedTopics.begin(), Consumer::countingExpectedTopics.end(), topic) != Consumer::countingExpectedTopics.end()) {
        t = topic;
    } else {
        for (std::string pattern : Consumer::countingExpectedPatterns) {
            if (std::regex_match(topic, std::regex(pattern))) {
                KDBGCB("yes! (pattern " << pattern << " matches)");
                t = pattern;
                break;
            }
        }
    }
    KDBGCB("Expectation for " << topic << " is " << t);
    return t;
}

bool Consumer::isExpectedTopic(std::string topic) {
    KDBGCB("is " << topic << " expected?");
    if (std::find(Consumer::countingExpectedTopics.begin(), Consumer::countingExpectedTopics.end(), topic) != Consumer::countingExpectedTopics.end()) {
        KDBGCB("yes!");
        return true;
    }
    for (std::string pattern : Consumer::countingExpectedPatterns) {
        if (std::regex_match(topic, std::regex(pattern))) {
            KDBGCB("yes! (pattern " << pattern << " matches)");
            return true;
        }
    }
    KDBGCB("no!");
    return false;
}
