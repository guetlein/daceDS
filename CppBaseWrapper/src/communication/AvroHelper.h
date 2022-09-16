/*******************************************************************************
 * Copyright 2021 Moritz Gütlein
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License.  You may obtain a copy
 * of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations under
 * the License.
 ******************************************************************************/
#pragma once


#include <algorithm>
#include <avro/Compiler.hh>
#include <avro/Decoder.hh>
#include <avro/Encoder.hh>
#include <avro/Exception.hh>
#include <avro/Generic.hh>
#include <avro/Specific.hh>
#include <avro/Stream.hh>
#include <avro/ValidSchema.hh>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

#include <libserdes/serdes-avro.h>
#include <libserdes/serdes.h>
#include <libserdes/serdescpp-avro.h>
#include <libserdes/serdescpp.h>
#include <stdlib.h>


#include "datamodel/CtrlMsg.hh"
#include "datamodel/Micro.hh"
#include "datamodel/MicroDetector.hh"
#include "datamodel/MicroEdge.hh"
#include "datamodel/MicroList.hh"
#include "datamodel/RadioMsg.hh"
#include "datamodel/ResourceFile.hh"
#include "datamodel/Scenario.hh"
#include "datamodel/StatusMsg.hh"
#include "datamodel/SyncMsg.hh"
#include "datamodel/InteractionMsg.hh"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/Utils.h"
#include "util/log.h"
#include "communication/ConsumedMessage.h"
/**
 * Based on libserdes examples.
 * Why do we need to the decodings in this way? Shouldn't this work in a smarter way using libserdes?
 * Todo: REWRITE EVERYTHING
 * */
namespace daceDS {
class AvroHelper {
   private:
    static AvroHelper* inst;
    AvroHelper();

   public:
    static AvroHelper* getInstance();
    void initSchema(std::string schema, std::string definition, std::string topic = "");
    std::map<std::string, Serdes::Schema*> schemas;

    ~AvroHelper() { delete AvroHelper::inst; };
    Serdes::Avro* serdes;
    static Serdes::Conf* sconf;

    datamodel::ResourceFile decodeResourceFile(const std::string& pfx, const void* buf, size_t len);
    datamodel::Scenario decodeScenario(const std::string& pfx, const void* buf, size_t len);
    datamodel::Micro decodeMicro(const std::string& pfx, const void* buf, size_t len);
    std::vector<datamodel::Micro> decodeListMicro(const std::string& pfx, const void* buf, size_t len);
    datamodel::MicroList decodeMicroList(const std::string& pfx, const void* buf, size_t len);
    datamodel::StatusMsg decodeStatusMsg(const std::string& pfx, const void* buf, size_t len);
    datamodel::CtrlMsg decodeCtrlMsg(const std::string& pfx, const void* buf, size_t len);
    void decodeGenericRecord(const std::string& pfx, const void* buf, size_t len);

    std::vector<char> encodeMicro(datamodel::Micro& micro);
    std::vector<char> encodeListMicro(std::vector<datamodel::Micro>& micro);
    std::vector<char> encodeMicroList(std::vector<datamodel::Micro>& micro);
    std::vector<char> encodeMicroEdge(datamodel::MicroEdge& micro);
    std::vector<char> encodeMicroDetector(datamodel::MicroDetector& micro);

    std::vector<char> encodeResourceFile(datamodel::ResourceFile& rf);
    TopicMetadata getTopicMetadata(std::string t);
    OrchestrationTopicMetadata getOrchestrationTopicMetadata(TopicMetadata m);
    InteractionTopicMetadata getInteractionTopicMetadata(TopicMetadata m);
    InteractionTopicMetadata2 getInteractionTopicMetadata2(TopicMetadata m);
    ProvisionTopicMetadata getProvisionTopicMetadata(TopicMetadata m);

    std::vector<char> encodeBaseType(double& d);
    std::vector<char> encodeBaseType(int& i);
    std::vector<char> encodeBaseType(std::string& s);
    std::vector<char> encodeBaseType(std::vector<std::string>& v);
    std::vector<char> encodeBaseType(std::vector<double>& v);

    void decodeBaseType(ConsumedMessage& msg, std::string& s);
    void decodeBaseType(ConsumedMessage& msg, double& d1);
    void decodeBaseType(ConsumedMessage& msg, double& d1, double& d2);
    void decodeBaseType(ConsumedMessage& msg, double& d1, double& d2, double& d3);
    void decodeBaseType(ConsumedMessage& msg, int& i1);
    void decodeBaseType(ConsumedMessage& msg, std::vector<std::string>& sl);

    void decodeVec3D(ConsumedMessage& msg, Vec3D& v3d);
    std::vector<char> encodeVec3D(Vec3D& v3d);

    std::vector<char> encodeSyncMsg(datamodel::SyncMsg& syncMsg);
    datamodel::SyncMsg decodeSyncMsg(const std::string& pfx, const void* buf, size_t len);

    // std::vector<char> encodeRadioMsg11p(datamodel::RadioMsg11p& msg);
    // datamodel::RadioMsg11p decodeRadioMsg11p(const std::string& pfx, const void* buf, size_t len);
    std::vector<char> encodeRadioMsg(datamodel::RadioMsg& msg);
    datamodel::RadioMsg decodeRadioMsg(const std::string& pfx, const void* buf, size_t len);


    std::vector<char> encodeInteractionMsg(datamodel::InteractionMsg& msg);
    datamodel::InteractionMsg decodeInteractionMsg(const std::string& pfx, const void* buf, size_t len);
};

}  // namespace daceDS
