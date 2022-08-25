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


namespace daceDS {
    
class ConsumedMessage {


    public:

    std::string topic = "x";
    std::string key= "x";
    int64_t timestamp = 0;
    char * payload;
    size_t len = 0;
    std::string sender= "x";
    int64_t st = 0;
    ConsumedMessage(std::string topic, std::string key, int64_t timestamp, std::string sender, size_t len, int64_t st, void * payloadSrc) : topic(topic), key(key), timestamp(timestamp), sender(sender), len(len), st(st) {
        payload = new char[len];
        memcpy(payload, payloadSrc, len);
    };
    ~ConsumedMessage(){ delete[] payload;};
};
}