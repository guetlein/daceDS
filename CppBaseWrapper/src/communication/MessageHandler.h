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

#include <set>
#include "../util/log.h"
#include "../util/Defines.h"
#include "ConsumedMessage.h"
#include <librdkafka/rdkafkacpp.h>

#include <mutex>
/**
 * Each consumer uses a single messageHandler implementation to process incoming messages.
 * Used to implement handlers for orchestration, interaction, and provision channel.
*/
namespace daceDS {

   
class MessageHandler {
   public:
    MessageHandler(){};
    virtual ~MessageHandler(){};

    virtual bool handle(ConsumedMessage* msg) = 0;
    
    // struct cm_compare {
    //     bool operator() (const ConsumedMessage& a, const ConsumedMessage& b) const {
    //         return 1;
    //         if(a.st == b.st) {
    //             if(a.topic == b.topic) { 
    //                 if(a.key == b.key) { 
    //                     return a.sender < b.sender;
    //                 }
    //                 return a.key < b.key;
    //             }
    //             return a.topic < b.topic;

    //         }
    //         return a.st < b.st;
    //     }
    // };

    struct cm_compare {
        bool operator() (const ConsumedMessage* a, const ConsumedMessage* b) const {
            return 1;
            if(a->st == b->st) {
                if(a->topic == b->topic) { 
                    if(a->key == b->key) { 
                        return a->sender < b->sender;
                    }
                    return a->key < b->key;
                }
                return a->topic < b->topic;

            }
            return a->st < b->st;
        }
    };

    std::set<ConsumedMessage*, cm_compare> buffer;
    bool bufferConsumedMessage(ConsumedMessage* msg);
    void processBuffer(int time, int epoch);
};
}  // namespace daceDS
