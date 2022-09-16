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


#ifndef CPP_INTERACTIONMSG_HH_662141847__H_
#define CPP_INTERACTIONMSG_HH_662141847__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct InteractionMsg {
    std::string CallID;
    std::string MethodID;
    std::map<std::string, std::string > Input;
    std::map<std::string, std::string > Output;
    InteractionMsg() :
        CallID(std::string()),
        MethodID(std::string()),
        Input(std::map<std::string, std::string >()),
        Output(std::map<std::string, std::string >())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::InteractionMsg> {
    static void encode(Encoder& e, const daceDS::datamodel::InteractionMsg& v) {
        avro::encode(e, v.CallID);
        avro::encode(e, v.MethodID);
        avro::encode(e, v.Input);
        avro::encode(e, v.Output);
    }
    static void decode(Decoder& d, daceDS::datamodel::InteractionMsg& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.CallID);
                    break;
                case 1:
                    avro::decode(d, v.MethodID);
                    break;
                case 2:
                    avro::decode(d, v.Input);
                    break;
                case 3:
                    avro::decode(d, v.Output);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.CallID);
            avro::decode(d, v.MethodID);
            avro::decode(d, v.Input);
            avro::decode(d, v.Output);
        }
    }
};

}
#endif
