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


#ifndef CPP_MICROEDGE_HH_3424181540__H_
#define CPP_MICROEDGE_HH_3424181540__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "Micro.hh"

namespace daceDS {
namespace datamodel {
struct MicroEdge {
    std::string edgeID;
    std::string emission;
    double flow;
    double occupancy;
    double speed;
    std::vector<Micro> vehicles;
    std::vector<std::string > vehicleIDs;
    MicroEdge() :
        edgeID(std::string()),
        emission(std::string()),
        flow(double()),
        occupancy(double()),
        speed(double()),
        vehicles(std::vector<Micro>()),
        vehicleIDs(std::vector<std::string >())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::MicroEdge> {
    static void encode(Encoder& e, const daceDS::datamodel::MicroEdge& v) {
        avro::encode(e, v.edgeID);
        avro::encode(e, v.emission);
        avro::encode(e, v.flow);
        avro::encode(e, v.occupancy);
        avro::encode(e, v.speed);
        avro::encode(e, v.vehicles);
        avro::encode(e, v.vehicleIDs);
    }
    static void decode(Decoder& d, daceDS::datamodel::MicroEdge& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.edgeID);
                    break;
                case 1:
                    avro::decode(d, v.emission);
                    break;
                case 2:
                    avro::decode(d, v.flow);
                    break;
                case 3:
                    avro::decode(d, v.occupancy);
                    break;
                case 4:
                    avro::decode(d, v.speed);
                    break;
                case 5:
                    avro::decode(d, v.vehicles);
                    break;
                case 6:
                    avro::decode(d, v.vehicleIDs);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.edgeID);
            avro::decode(d, v.emission);
            avro::decode(d, v.flow);
            avro::decode(d, v.occupancy);
            avro::decode(d, v.speed);
            avro::decode(d, v.vehicles);
            avro::decode(d, v.vehicleIDs);
        }
    }
};

}
#endif
