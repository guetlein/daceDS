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

#ifndef CPP_MICRO_HH_2650593976__H_
#define CPP_MICRO_HH_2650593976__H_

#include <sstream>

#include "Vec3.hh"
#include "avro/Decoder.hh"
#include "avro/Encoder.hh"
#include "avro/Specific.hh"
#include "boost/any.hpp"

namespace daceDS {
namespace datamodel {
struct Micro {
    std::string vehicleID;
    double acceleration;
    double angle;
    std::string edge;
    int32_t lane;
    Vec3 position;
    double positionEdge;
    std::vector<std::string> route;
    double slope;
    double speed;
    std::string type;
    Micro() : vehicleID(std::string()),
              acceleration(double()),
              angle(double()),
              edge(std::string()),
              lane(int32_t()),
              position(Vec3()),
              positionEdge(double()),
              route(std::vector<std::string>()),
              slope(double()),
              speed(double()),
              type(std::string()) {}
};

}  // namespace datamodel
}  // namespace daceDS
namespace avro {
template <>
struct codec_traits<daceDS::datamodel::Micro> {
    static void encode(Encoder& e, const daceDS::datamodel::Micro& v) {
        avro::encode(e, v.vehicleID);
        avro::encode(e, v.acceleration);
        avro::encode(e, v.angle);
        avro::encode(e, v.edge);
        avro::encode(e, v.lane);
        avro::encode(e, v.position);
        avro::encode(e, v.positionEdge);
        avro::encode(e, v.route);
        avro::encode(e, v.slope);
        avro::encode(e, v.speed);
        avro::encode(e, v.type);
    }
    static void decode(Decoder& d, daceDS::datamodel::Micro& v) {
        if (avro::ResolvingDecoder* rd =
                dynamic_cast<avro::ResolvingDecoder*>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                 it != fo.end(); ++it) {
                switch (*it) {
                    case 0:
                        avro::decode(d, v.vehicleID);
                        break;
                    case 1:
                        avro::decode(d, v.acceleration);
                        break;
                    case 2:
                        avro::decode(d, v.angle);
                        break;
                    case 3:
                        avro::decode(d, v.edge);
                        break;
                    case 4:
                        avro::decode(d, v.lane);
                        break;
                    case 5:
                        avro::decode(d, v.position);
                        break;
                    case 6:
                        avro::decode(d, v.positionEdge);
                        break;
                    case 7:
                        avro::decode(d, v.route);
                        break;
                    case 8:
                        avro::decode(d, v.slope);
                        break;
                    case 9:
                        avro::decode(d, v.speed);
                        break;
                    case 10:
                        avro::decode(d, v.type);
                        break;
                    default:
                        break;
                }
            }
        } else {
            avro::decode(d, v.vehicleID);
            avro::decode(d, v.acceleration);
            avro::decode(d, v.angle);
            avro::decode(d, v.edge);
            avro::decode(d, v.lane);
            avro::decode(d, v.position);
            avro::decode(d, v.positionEdge);
            avro::decode(d, v.route);
            avro::decode(d, v.slope);
            avro::decode(d, v.speed);
            avro::decode(d, v.type);
        }
    }
};

}  // namespace avro
#endif
