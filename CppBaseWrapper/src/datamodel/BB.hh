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


#ifndef CPP_BB_HH_568901365__H_
#define CPP_BB_HH_568901365__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "Observer.hh"

namespace daceDS {
namespace datamodel {
struct BB {
    std::string instanceID;
    std::string domain;
    std::string layer;
    std::string type;
    int32_t stepLength;
    bool isExternal;
    std::map<std::string, std::string > parameters;
    std::map<std::string, std::string > resources;
    std::map<std::string, std::string > results;
    std::vector<std::string > responsibilities;
    bool synchronized;
    std::vector<Observer > observers;
    BB() :
        instanceID(std::string()),
        domain(std::string()),
        layer(std::string()),
        type(std::string()),
        stepLength(int32_t()),
        isExternal(bool()),
        parameters(std::map<std::string, std::string >()),
        resources(std::map<std::string, std::string >()),
        results(std::map<std::string, std::string >()),
        responsibilities(std::vector<std::string >()),
        synchronized(bool()),
        observers(std::vector<Observer > ())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::BB> {
    static void encode(Encoder& e, const daceDS::datamodel::BB& v) {
        avro::encode(e, v.instanceID);
        avro::encode(e, v.domain);
        avro::encode(e, v.layer);
        avro::encode(e, v.type);
        avro::encode(e, v.stepLength);
        avro::encode(e, v.isExternal);
        avro::encode(e, v.parameters);
        avro::encode(e, v.resources);
        avro::encode(e, v.results);
        avro::encode(e, v.responsibilities);
        avro::encode(e, v.synchronized);
        avro::encode(e, v.observers);
    }
    static void decode(Decoder& d, daceDS::datamodel::BB& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.instanceID);
                    break;
                case 1:
                    avro::decode(d, v.domain);
                    break;
                case 2:
                    avro::decode(d, v.layer);
                    break;
                case 3:
                    avro::decode(d, v.type);
                    break;
                case 4:
                    avro::decode(d, v.stepLength);
                    break;
                case 5:
                    avro::decode(d, v.isExternal);
                    break;
                case 6:
                    avro::decode(d, v.parameters);
                    break;
                case 7:
                    avro::decode(d, v.resources);
                    break;
                case 8:
                    avro::decode(d, v.results);
                    break;
                case 9:
                    avro::decode(d, v.responsibilities);
                    break;
                case 10:
                    avro::decode(d, v.synchronized);
                    break;
                case 11:
                    avro::decode(d, v.observers);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.instanceID);
            avro::decode(d, v.domain);
            avro::decode(d, v.layer);
            avro::decode(d, v.type);
            avro::decode(d, v.stepLength);
            avro::decode(d, v.isExternal);
            avro::decode(d, v.parameters);
            avro::decode(d, v.resources);
            avro::decode(d, v.results);
            avro::decode(d, v.responsibilities);
            avro::decode(d, v.synchronized);
            avro::decode(d, v.observers);
        }
    }
};

}
#endif
