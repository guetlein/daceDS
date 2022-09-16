/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef CPP_OBSERVER_HH_3631740942__H_
#define CPP_OBSERVER_HH_3631740942__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct Observer {
    std::string task;
    std::string element;
    std::string filter;
    int32_t period;
    std::string trigger;
    std::string type;
    Observer() :
        task(std::string()),
        element(std::string()),
        filter(std::string()),
        period(int32_t()),
        trigger(std::string()),
        type(std::string())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::Observer> {
    static void encode(Encoder& e, const daceDS::datamodel::Observer& v) {
        avro::encode(e, v.task);
        avro::encode(e, v.element);
        avro::encode(e, v.filter);
        avro::encode(e, v.period);
        avro::encode(e, v.trigger);
        avro::encode(e, v.type);
    }
    static void decode(Decoder& d, daceDS::datamodel::Observer& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.task);
                    break;
                case 1:
                    avro::decode(d, v.element);
                    break;
                case 2:
                    avro::decode(d, v.filter);
                    break;
                case 3:
                    avro::decode(d, v.period);
                    break;
                case 4:
                    avro::decode(d, v.trigger);
                    break;
                case 5:
                    avro::decode(d, v.type);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.task);
            avro::decode(d, v.element);
            avro::decode(d, v.filter);
            avro::decode(d, v.period);
            avro::decode(d, v.trigger);
            avro::decode(d, v.type);
        }
    }
};

}
#endif
