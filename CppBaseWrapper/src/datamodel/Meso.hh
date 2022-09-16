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


#ifndef CPP_MESO_HH_1303895446__H_
#define CPP_MESO_HH_1303895446__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct Meso {
    std::string vehicleID;
    std::string edge;
    std::string type;
    std::string route;
    Meso() :
        vehicleID(std::string()),
        edge(std::string()),
        type(std::string()),
        route(std::string())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::Meso> {
    static void encode(Encoder& e, const daceDS::datamodel::Meso& v) {
        avro::encode(e, v.vehicleID);
        avro::encode(e, v.edge);
        avro::encode(e, v.type);
        avro::encode(e, v.route);
    }
    static void decode(Decoder& d, daceDS::datamodel::Meso& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.vehicleID);
                    break;
                case 1:
                    avro::decode(d, v.edge);
                    break;
                case 2:
                    avro::decode(d, v.type);
                    break;
                case 3:
                    avro::decode(d, v.route);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.vehicleID);
            avro::decode(d, v.edge);
            avro::decode(d, v.type);
            avro::decode(d, v.route);
        }
    }
};

}
#endif
