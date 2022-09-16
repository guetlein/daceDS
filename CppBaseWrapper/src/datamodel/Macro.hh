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


#ifndef CPP_MACRO_HH_1303895446__H_
#define CPP_MACRO_HH_1303895446__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct Macro {
    std::string roadID;
    double flow;
    double density;
    double speed;
    Macro() :
        roadID(std::string()),
        flow(double()),
        density(double()),
        speed(double())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::Macro> {
    static void encode(Encoder& e, const daceDS::datamodel::Macro& v) {
        avro::encode(e, v.roadID);
        avro::encode(e, v.flow);
        avro::encode(e, v.density);
        avro::encode(e, v.speed);
    }
    static void decode(Decoder& d, daceDS::datamodel::Macro& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.roadID);
                    break;
                case 1:
                    avro::decode(d, v.flow);
                    break;
                case 2:
                    avro::decode(d, v.density);
                    break;
                case 3:
                    avro::decode(d, v.speed);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.roadID);
            avro::decode(d, v.flow);
            avro::decode(d, v.density);
            avro::decode(d, v.speed);
        }
    }
};

}
#endif
