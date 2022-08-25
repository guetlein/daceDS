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


#ifndef CPP_PROJECTOR_HH_2432675606__H_
#define CPP_PROJECTOR_HH_2432675606__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct Projector {
    std::string projectorID;
    std::string type;
    std::string domainA;
    std::string layerA;
    std::string domainB;
    std::string layerB;
    std::map<std::string, std::string > resources;
    std::map<std::string, std::string > parameters;
    Projector() :
        projectorID(std::string()),
        type(std::string()),
        domainA(std::string()),
        layerA(std::string()),
        domainB(std::string()),
        layerB(std::string()),
        resources(std::map<std::string, std::string >()),
        parameters(std::map<std::string, std::string >())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::Projector> {
    static void encode(Encoder& e, const daceDS::datamodel::Projector& v) {
        avro::encode(e, v.projectorID);
        avro::encode(e, v.type);
        avro::encode(e, v.domainA);
        avro::encode(e, v.layerA);
        avro::encode(e, v.domainB);
        avro::encode(e, v.layerB);
        avro::encode(e, v.resources);
        avro::encode(e, v.parameters);
    }
    static void decode(Decoder& d, daceDS::datamodel::Projector& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.projectorID);
                    break;
                case 1:
                    avro::decode(d, v.type);
                    break;
                case 2:
                    avro::decode(d, v.domainA);
                    break;
                case 3:
                    avro::decode(d, v.layerA);
                    break;
                case 4:
                    avro::decode(d, v.domainB);
                    break;
                case 5:
                    avro::decode(d, v.layerB);
                    break;
                case 6:
                    avro::decode(d, v.resources);
                    break;
                case 7:
                    avro::decode(d, v.parameters);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.projectorID);
            avro::decode(d, v.type);
            avro::decode(d, v.domainA);
            avro::decode(d, v.layerA);
            avro::decode(d, v.domainB);
            avro::decode(d, v.layerB);
            avro::decode(d, v.resources);
            avro::decode(d, v.parameters);
        }
    }
};

}
#endif
