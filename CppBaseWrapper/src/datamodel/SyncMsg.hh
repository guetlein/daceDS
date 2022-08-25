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


#ifndef CPP_SYNCMSG_HH_1303895446__H_
#define CPP_SYNCMSG_HH_1303895446__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"

namespace daceDS {
namespace datamodel {
struct SyncMsg {
    std::string Sender;
    std::string Action;
    int64_t Time;
    int32_t Epoch;
    std::map<std::string, int64_t > Messages;
    SyncMsg() :
        Sender(std::string()),
        Action(std::string()),
        Time(int64_t()),
        Epoch(int32_t()),
        Messages(std::map<std::string, int64_t >())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::SyncMsg> {
    static void encode(Encoder& e, const daceDS::datamodel::SyncMsg& v) {
        avro::encode(e, v.Sender);
        avro::encode(e, v.Action);
        avro::encode(e, v.Time);
        avro::encode(e, v.Epoch);
        avro::encode(e, v.Messages);
    }
    static void decode(Decoder& d, daceDS::datamodel::SyncMsg& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.Sender);
                    break;
                case 1:
                    avro::decode(d, v.Action);
                    break;
                case 2:
                    avro::decode(d, v.Time);
                    break;
                case 3:
                    avro::decode(d, v.Epoch);
                    break;
                case 4:
                    avro::decode(d, v.Messages);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.Sender);
            avro::decode(d, v.Action);
            avro::decode(d, v.Time);
            avro::decode(d, v.Epoch);
            avro::decode(d, v.Messages);
        }
    }
};

}
#endif
