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


#ifndef CPP_SCENARIO_HH_293442187__H_
#define CPP_SCENARIO_HH_293442187__H_


#include <sstream>
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#include "BB.hh"
#include "Translator.hh"
#include "Projector.hh"

namespace daceDS {
namespace datamodel {
struct Execution {
    int64_t randomSeed;
    std::string constraints;
    int64_t priority;
    int64_t syncedParticipants;
    Execution() :
        randomSeed(int64_t()),
        constraints(std::string()),
        priority(int64_t()),
        syncedParticipants(int64_t())
        { }
};

struct Scenario {
    std::string scenarioID;
    std::map<std::string, std::string > domainReferences;
    int64_t simulationStart;
    int64_t simulationEnd;
    Execution execution;
    std::vector<BB > buildingBlocks;
    std::vector<Translator > translators;
    std::vector<Projector > projectors;
    Scenario() :
        scenarioID(std::string()),
        domainReferences(std::map<std::string, std::string >()),
        simulationStart(int64_t()),
        simulationEnd(int64_t()),
        execution(Execution()),
        buildingBlocks(std::vector<BB >()),
        translators(std::vector<Translator >()),
        projectors(std::vector<Projector >())
        { }
};

}
}
namespace avro {
template<> struct codec_traits<daceDS::datamodel::Execution> {
    static void encode(Encoder& e, const daceDS::datamodel::Execution& v) {
        avro::encode(e, v.randomSeed);
        avro::encode(e, v.constraints);
        avro::encode(e, v.priority);
        avro::encode(e, v.syncedParticipants);
    }
    static void decode(Decoder& d, daceDS::datamodel::Execution& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.randomSeed);
                    break;
                case 1:
                    avro::decode(d, v.constraints);
                    break;
                case 2:
                    avro::decode(d, v.priority);
                    break;
                case 3:
                    avro::decode(d, v.syncedParticipants);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.randomSeed);
            avro::decode(d, v.constraints);
            avro::decode(d, v.priority);
            avro::decode(d, v.syncedParticipants);
        }
    }
};

template<> struct codec_traits<daceDS::datamodel::Scenario> {
    static void encode(Encoder& e, const daceDS::datamodel::Scenario& v) {
        avro::encode(e, v.scenarioID);
        avro::encode(e, v.domainReferences);
        avro::encode(e, v.simulationStart);
        avro::encode(e, v.simulationEnd);
        avro::encode(e, v.execution);
        avro::encode(e, v.buildingBlocks);
        avro::encode(e, v.translators);
        avro::encode(e, v.projectors);
    }
    static void decode(Decoder& d, daceDS::datamodel::Scenario& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.scenarioID);
                    break;
                case 1:
                    avro::decode(d, v.domainReferences);
                    break;
                case 2:
                    avro::decode(d, v.simulationStart);
                    break;
                case 3:
                    avro::decode(d, v.simulationEnd);
                    break;
                case 4:
                    avro::decode(d, v.execution);
                    break;
                case 5:
                    avro::decode(d, v.buildingBlocks);
                    break;
                case 6:
                    avro::decode(d, v.translators);
                    break;
                case 7:
                    avro::decode(d, v.projectors);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.scenarioID);
            avro::decode(d, v.domainReferences);
            avro::decode(d, v.simulationStart);
            avro::decode(d, v.simulationEnd);
            avro::decode(d, v.execution);
            avro::decode(d, v.buildingBlocks);
            avro::decode(d, v.translators);
            avro::decode(d, v.projectors);
        }
    }
};

}
#endif
