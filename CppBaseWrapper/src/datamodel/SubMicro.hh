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


#ifndef CPP_SUBMICRO_HH_2650593976__H_
#define CPP_SUBMICRO_HH_2650593976__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "Vec3.hh"
#include "SubMicroVehicleControl.hh"
#include "Sensor.hh"

namespace daceDS {
namespace datamodel {
struct SubMicro {
    std::string vehicleID;
    Vec3 acceleration;
    Vec3 position;
    Vec3 rotation;
    std::vector<Vec3> route;
    std::vector<Sensor> sensor;
    std::string type;
    SubMicroVehicleControl vehicleControl;
    Vec3 velocity;
    SubMicro() :
        vehicleID(std::string()),
        acceleration(Vec3()),
        position(Vec3()),
        rotation(Vec3()),
        route(std::vector<Vec3>()),
        sensor(std::vector<sSensor>()),
        type(std::string()),
        vehicleControl(SubMicroVehicleControl()),
        velocity(Vec3())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::SubMicro> {
    static void encode(Encoder& e, const daceDS::datamodel::SubMicro& v) {
        avro::encode(e, v.vehicleID);
        avro::encode(e, v.acceleration);
        avro::encode(e, v.position);
        avro::encode(e, v.rotation);
        avro::encode(e, v.route);
        avro::encode(e, v.sensor);
        avro::encode(e, v.type);
        avro::encode(e, v.vehicleControl);
        avro::encode(e, v.velocity);
    }
    static void decode(Decoder& d, daceDS::datamodel::SubMicro& v) {
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
                    avro::decode(d, v.acceleration);
                    break;
                case 2:
                    avro::decode(d, v.position);
                    break;
                case 3:
                    avro::decode(d, v.rotation);
                    break;
                case 4:
                    avro::decode(d, v.route);
                    break;
                case 5:
                    avro::decode(d, v.sensor);
                    break;
                case 6:
                    avro::decode(d, v.type);
                    break;
                case 7:
                    avro::decode(d, v.vehicleControl);
                    break;
                case 8:
                    avro::decode(d, v.velocity);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.vehicleID);
            avro::decode(d, v.acceleration);
            avro::decode(d, v.position);
            avro::decode(d, v.rotation);
            avro::decode(d, v.route);
            avro::decode(d, v.sensor);
            avro::decode(d, v.type);
            avro::decode(d, v.vehicleControl);
            avro::decode(d, v.velocity);
        }
    }
};

}
#endif
