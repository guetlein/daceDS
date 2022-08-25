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


#ifndef CPP_TRANSLATOR_HH_3631740942__H_
#define CPP_TRANSLATOR_HH_3631740942__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct Translator {
    std::string translatorID;
    std::string domain;
    std::string type;
    std::string layerA;
    std::vector<std::string > responsibilitiesA;
    std::string layerB;
    std::vector<std::string > responsibilitiesB;
    std::map<std::string, std::string > resources;
    std::map<std::string, std::string > parameters;
    Translator() :
        translatorID(std::string()),
        domain(std::string()),
        type(std::string()),
        layerA(std::string()),
        responsibilitiesA(std::vector<std::string >()),
        layerB(std::string()),
        responsibilitiesB(std::vector<std::string >()),
        resources(std::map<std::string, std::string >()),
        parameters(std::map<std::string, std::string >())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::Translator> {
    static void encode(Encoder& e, const daceDS::datamodel::Translator& v) {
        avro::encode(e, v.translatorID);
        avro::encode(e, v.domain);
        avro::encode(e, v.type);
        avro::encode(e, v.layerA);
        avro::encode(e, v.responsibilitiesA);
        avro::encode(e, v.layerB);
        avro::encode(e, v.responsibilitiesB);
        avro::encode(e, v.resources);
        avro::encode(e, v.parameters);
    }
    static void decode(Decoder& d, daceDS::datamodel::Translator& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.translatorID);
                    break;
                case 1:
                    avro::decode(d, v.domain);
                    break;
                case 2:
                    avro::decode(d, v.type);
                    break;
                case 3:
                    avro::decode(d, v.layerA);
                    break;
                case 4:
                    avro::decode(d, v.responsibilitiesA);
                    break;
                case 5:
                    avro::decode(d, v.layerB);
                    break;
                case 6:
                    avro::decode(d, v.responsibilitiesB);
                    break;
                case 7:
                    avro::decode(d, v.resources);
                    break;
                case 8:
                    avro::decode(d, v.parameters);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.translatorID);
            avro::decode(d, v.domain);
            avro::decode(d, v.type);
            avro::decode(d, v.layerA);
            avro::decode(d, v.responsibilitiesA);
            avro::decode(d, v.layerB);
            avro::decode(d, v.responsibilitiesB);
            avro::decode(d, v.resources);
            avro::decode(d, v.parameters);
        }
    }
};

}
#endif
