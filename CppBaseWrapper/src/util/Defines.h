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


#define SUMO_API_MODE SumoConnectionLibSUMO
//#define SUMO_API_MODE SumoConnectionTraCI

// #ifdef DOCKERBUILD
// #define CONFIG_PROPERTIES "/resource/config.properties"  //docker
// #else
#define CONFIG_PROPERTIES "./resource/config.properties"  //native
// #endif

#define CHANNEL_PROVISION "provision.simulation"
#define CHANNEL_INTERACTION "interaction.simulation"
#define CHANNEL_ORCHESTRATION "orchestration.simulation"

#define TOPIC_SCENARIO "scenario"
#define TOPIC_RESULT "result"
#define TOPIC_SYNCMSG "sync"
#define TOPIC_CTRLMSG "ctrl"
#define TOPIC_STATUSMSG "status"

#define SCHEMA_NAME_MICRO "eu.fau.cs7.daceDS.datamodel.Micro"
#define SCHEMA_NAME_MICRO_LIST "eu.fau.cs7.daceDS.datamodel.MicroList"
#define SCHEMA_NAME_MICROEDGE "eu.fau.cs7.daceDS.datamodel.MicroEdge"
#define SCHEMA_NAME_MicroDetector "eu.fau.cs7.daceDS.datamodel.MicroDetector"
#define SCHEMA_NAME_RESOURCEFILE "eu.fau.cs7.daceDS.datamodel.ResourceFile"
#define SCHEMA_NAME_SCENARIO "eu.fau.cs7.daceDS.datamodel.Scenario"
#define SCHEMA_NAME_STATUSMSG "eu.fau.cs7.daceDS.datamodel.Status"
#define SCHEMA_NAME_CTRLMSG "eu.fau.cs7.daceDS.datamodel.Ctrl"
#define SCHEMA_NAME_PRIMITIVEDOUBLE "eu.fau.cs7.daceDS.datamodel.PrimitiveDouble"
#define SCHEMA_NAME_PRIMITIVESTRING "eu.fau.cs7.daceDS.datamodel.PrimitiveString"
#define SCHEMA_NAME_PRIMITIVEINTEGER "eu.fau.cs7.daceDS.datamodel.PrimitiveInteger"
#define SCHEMA_NAME_LIST_STRING "eu.fau.cs7.daceDS.datamodel.ListString"
#define SCHEMA_NAME_LIST_DOUBLE "eu.fau.cs7.daceDS.datamodel.ListDouble"
#define SCHEMA_NAME_LIST_MICRO "eu.fau.cs7.daceDS.datamodel.ListMicro"
#define SCHEMA_NAME_SYNCMSG "eu.fau.cs7.daceDS.datamodel.SyncMsg"
#define SCHEMA_NAME_VEC3 "eu.fau.cs7.daceDS.datamodel.Vec3"
#define SCHEMA_NAME_RADIOMSG11P "eu.fau.cs7.daceDS.datamodel.RadioMsg11p"
#define SCHEMA_NAME_RADIOMSG "eu.fau.cs7.daceDS.datamodel.RadioMsg"

#define SCHEMA_DEF_SYNCMSG \
    "{\"type\":\"record\",\"name\":\"SyncMsg\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"Sender\",\"type\":\"string\"},{\"name\":\"Action\",\"type\":\"string\",\"default\":\"request\"},{\"name\":\"Time\",\"type\":\"long\"},{\"name\":\"Epoch\",\"type\":\"int\",\"default\":0},{\"name\":\"Messages\",\"type\":{\"type\":\"map\",\"values\":\"long\",\"default\":{}}}]}"
#define SCHEMA_DEF_MICRO \
    "{\"type\":\"record\",\"name\":\"Micro\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"vehicleID\",\"type\":\"string\"},{\"name\":\"acceleration\",\"type\":\"double\"},{\"name\":\"angle\",\"type\":\"double\"},{\"name\":\"edge\",\"type\":\"string\"},{\"name\":\"lane\",\"type\":\"int\"},{\"name\":\"position\",\"type\":{\"type\":\"record\",\"name\":\"Vec3\",\"fields\":[{\"name\":\"x\",\"type\":\"double\"},{\"name\":\"y\",\"type\":\"double\"},{\"name\":\"z\",\"type\":\"double\"}]}},{\"name\":\"positionEdge\",\"type\":\"double\"},{\"name\":\"route\",\"type\":{\"type\":\"array\",\"items\":\"string\"}},{\"name\":\"slope\",\"type\":\"double\"},{\"name\":\"speed\",\"type\":\"double\"},{\"name\":\"type\",\"type\":\"string\"}]}"
#define SCHEMA_DEF_MICRO_LIST \
    "{\"type\":\"record\",\"name\":\"MicroList\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"contents\",\"type\":{\"type\":\"array\",\"items\":{\"type\":\"record\",\"name\":\"Micro\",\"fields\":[{\"name\":\"vehicleID\",\"type\":\"string\"},{\"name\":\"acceleration\",\"type\":\"double\"},{\"name\":\"angle\",\"type\":\"double\"},{\"name\":\"edge\",\"type\":\"string\"},{\"name\":\"lane\",\"type\":\"int\"},{\"name\":\"position\",\"type\":{\"type\":\"record\",\"name\":\"Vec3\",\"fields\":[{\"name\":\"x\",\"type\":\"double\"},{\"name\":\"y\",\"type\":\"double\"},{\"name\":\"z\",\"type\":\"double\"}]}},{\"name\":\"positionEdge\",\"type\":\"double\"},{\"name\":\"route\",\"type\":{\"type\":\"array\",\"items\":\"string\"}},{\"name\":\"slope\",\"type\":\"double\"},{\"name\":\"speed\",\"type\":\"double\"},{\"name\":\"type\",\"type\":\"string\"}]}}}]}"
#define SCHEMA_DEF_MICROEDGE \
    "{\"type\":\"record\",\"name\":\"MicroEdge\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"edgeID\",\"type\":\"string\"},{\"name\":\"emission\",\"type\":\"string\"},{\"name\":\"flow\",\"type\":\"double\"},{\"name\":\"occupancy\",\"type\":\"double\"},{\"name\":\"speed\",\"type\":\"double\"},{\"name\":\"vehicles\",\"type\":{\"type\":\"array\",\"items\":{\"type\":\"record\",\"name\":\"Micro\",\"fields\":[{\"name\":\"vehicleID\",\"type\":\"string\"},{\"name\":\"acceleration\",\"type\":\"double\"},{\"name\":\"angle\",\"type\":\"double\"},{\"name\":\"edge\",\"type\":\"string\"},{\"name\":\"lane\",\"type\":\"int\"},{\"name\":\"position\",\"type\":{\"type\":\"record\",\"name\":\"Vec3\",\"fields\":[{\"name\":\"x\",\"type\":\"double\"},{\"name\":\"y\",\"type\":\"double\"},{\"name\":\"z\",\"type\":\"double\"}]}},{\"name\":\"positionEdge\",\"type\":\"double\"},{\"name\":\"route\",\"type\":{\"type\":\"array\",\"items\":\"string\"}},{\"name\":\"slope\",\"type\":\"double\"},{\"name\":\"speed\",\"type\":\"double\"},{\"name\":\"type\",\"type\":\"string\"}]}}},{\"name\":\"vehicleIDs\",\"type\":{\"type\":\"array\",\"items\":\"string\"}}]}"
#define SCHEMA_DEF_MicroDetector \
    "{   \"type\": \"record\",   \"name\": \"MicroDetector\",   \"namespace\": \"eu.fau.cs7.daceDS.datamodel\",   \"fields\": [     { \"name\": \"detectorID\", \"type\": \"string\"},         { \"name\": \"values\", \"type\": {\"type\": \"map\", \"values\": [\"boolean\",\"double\",\"int\",\"string\"]}}     ] }"
#define SCHEMA_DEF_RESOURCEFILE \
    "{\"type\":\"record\",\"name\":\"ResourceFile\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"ID\",\"type\":\"string\"},{\"name\":\"Type\",\"type\":\"string\"},{\"name\":\"File\",\"type\":[\"null\",\"bytes\"],\"default\":null},{\"name\":\"FileReference\",\"type\":[\"null\",\"string\"],\"default\":null}]}"
// old one: "{\"type\":\"record\",\"name\":\"ResourceFile\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"ID\",\"type\":\"string\"},{\"name\":\"Type\",\"type\":\"string\"},{\"name\":\"File\",\"type\":\"bytes\"}]}"
#define SCHEMA_DEF_SCENARIO \
    "{\"type\":\"record\",\"name\":\"Scenario\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"scenarioID\",\"type\":\"string\"},{\"name\":\"domainReferences\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}},{\"name\":\"simulationStart\",\"type\":\"long\"},{\"name\":\"simulationEnd\",\"type\":\"long\"},{\"name\":\"execution\",\"type\":{\"type\":\"record\",\"name\":\"Execution\",\"fields\":[{\"name\":\"randomSeed\",\"type\":\"long\"},{\"name\":\"constraints\",\"type\":\"string\"},{\"name\":\"priority\",\"type\":\"long\"},{\"name\":\"syncedParticipants\",\"type\":\"long\"}]}},{\"name\":\"buildingBlocks\",\"type\":{\"type\":\"array\",\"items\":{\"type\":\"record\",\"name\":\"BB\",\"fields\":[{\"name\":\"instanceID\",\"type\":\"string\"},{\"name\":\"domain\",\"type\":\"string\"},{\"name\":\"layer\",\"type\":\"string\"},{\"name\":\"type\",\"type\":\"string\"},{\"name\":\"stepLength\",\"type\":\"int\"},{\"name\":\"isExternal\",\"type\":\"boolean\"},{\"name\":\"parameters\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}},{\"name\":\"resources\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}},{\"name\":\"results\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}},{\"name\":\"responsibilities\",\"type\":{\"type\":\"array\",\"items\":\"string\"}},{\"name\":\"synchronized\",\"type\":\"boolean\"},{\"name\":\"observers\",\"type\":{\"type\":\"array\",\"items\":{\"type\":\"record\",\"name\":\"Observer\",\"fields\":[{\"name\":\"task\",\"type\":\"string\"},{\"name\":\"element\",\"type\":\"string\"},{\"name\":\"filter\",\"type\":\"string\"},{\"name\":\"period\",\"type\":\"int\"},{\"name\":\"trigger\",\"type\":\"string\"},{\"name\":\"type\",\"type\":\"string\"}]}}}]}}},{\"name\":\"translators\",\"type\":{\"type\":\"array\",\"items\":{\"type\":\"record\",\"name\":\"Translator\",\"fields\":[{\"name\":\"translatorID\",\"type\":\"string\"},{\"name\":\"domain\",\"type\":\"string\"},{\"name\":\"type\",\"type\":\"string\"},{\"name\":\"layerA\",\"type\":\"string\"},{\"name\":\"responsibilitiesA\",\"type\":{\"type\":\"array\",\"items\":\"string\"}},{\"name\":\"layerB\",\"type\":\"string\"},{\"name\":\"responsibilitiesB\",\"type\":{\"type\":\"array\",\"items\":\"string\"}},{\"name\":\"resources\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}},{\"name\":\"parameters\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}}]}}},{\"name\":\"projectors\",\"type\":{\"type\":\"array\",\"items\":{\"type\":\"record\",\"name\":\"Projector\",\"fields\":[{\"name\":\"projectorID\",\"type\":\"string\"},{\"name\":\"type\",\"type\":\"string\"},{\"name\":\"domainA\",\"type\":\"string\"},{\"name\":\"layerA\",\"type\":\"string\"},{\"name\":\"domainB\",\"type\":\"string\"},{\"name\":\"layerB\",\"type\":\"string\"},{\"name\":\"resources\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}},{\"name\":\"parameters\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}}]}}}]}"
#define SCHEMA_DEF_STATUSMSG \
    "{\"type\":\"record\",\"name\":\"StatusMsg\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"Sender\",\"type\":\"string\"},{\"name\":\"Action\",\"type\":\"string\"},{\"name\":\"Status\",\"type\":\"string\"}]}"
#define SCHEMA_DEF_CTRLMSG \
    "{\"type\":\"record\",\"name\":\"CtrlMsg\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"Sender\",\"type\":\"string\"},{\"name\":\"Command\",\"type\":\"string\"}]}"
#define SCHEMA_DEF_RADIOMSG11P \
    "{\"type\":\"record\",\"name\":\"RadioMsg11p\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"Sender\",\"type\":\"string\"},{\"name\":\"Params\",\"type\":\"string\"},{\"name\":\"Type\",\"type\":\"string\"},{\"name\":\"Payload\",\"type\":\"string\"}]}"
#define SCHEMA_DEF_RADIOMSG \
    "{\"type\":\"record\",\"name\":\"RadioMsg\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"sender\",\"type\":\"string\",\"default\":\"\"},{\"name\":\"sendTime\",\"type\":\"long\",\"default\":0},{\"name\":\"receiver\",\"type\":\"string\",\"default\":\"\"},{\"name\":\"receiveTime\",\"type\":\"long\",\"default\":0},{\"name\":\"data\",\"type\":{\"type\":\"map\",\"values\":\"string\",\"default\":{}}}]}"
#define SCHEMA_DEF_PRIMITIVEDOUBLE \
    "{\"type\":\"double\"}"
#define SCHEMA_DEF_PRIMITIVEINTEGER \
    "{\"type\":\"int\"}"
#define SCHEMA_DEF_PRIMITIVESTRING \
    "{\"type\":\"string\"}"
#define SCHEMA_DEF_LIST_STRING \
    "{\"type\":\"array\",\"items\": {\"name\":\"elem\",\"type\":\"string\"}}"
#define SCHEMA_DEF_LIST_DOUBLE \
    "{\"type\":\"array\",\"items\": {\"name\":\"elem\",\"type\":\"double\"}}"
#define SCHEMA_DEF_LIST_MICRO \
    "{ \"type\": \"array\", \"items\": { \"type\": \"record\", \"name\": \"Micro\", \"namespace\": \"eu.fau.cs7.daceDS.datamodel\", \"fields\": [ { \"name\": \"vehicleID\", \"type\": \"string\"}, { \"name\": \"acceleration\",\"type\": \"double\"}, { \"name\": \"angle\", \"type\": \"double\"}, { \"name\": \"edge\", \"type\": \"string\"}, { \"name\": \"lane\", \"type\": \"int\"}, { \"name\": \"position\", \"type\": { \"type\": \"record\", \"name\": \"Vec3\", \"namespace\": \"eu.fau.cs7.daceDS.datamodel\", \"fields\": [ { \"name\": \"x\", \"type\": \"double\"}, { \"name\": \"y\", \"type\": \"double\"}, { \"name\": \"z\", \"type\": \"double\"} ] }}, { \"name\": \"positionEdge\", \"type\": \"double\"}, { \"name\": \"route\", \"type\": {\"type\": \"array\", \"items\": \"string\"}}, { \"name\": \"slope\", \"type\": \"double\"}, { \"name\": \"speed\", \"type\": \"double\"}, { \"name\": \"type\", \"type\": \"string\"} ] } }"
#define SCHEMA_DEF_VEC3 \
    "{\"type\":\"record\",\"name\":\"Vec3\",\"namespace\":\"eu.fau.cs7.daceDS.datamodel\",\"fields\":[{\"name\":\"x\",\"type\":\"double\"},{\"name\":\"y\",\"type\":\"double\"},{\"name\":\"z\",\"type\":\"double\"}]}"

#define CONFIG_KAFKABROKER "kafkaBroker"
#define CONFIG_KAFKALOGLEVEL "kafkaLogLevel"
#define CONFIG_SCHEMAREGISTRY "schemaRegistry"

#define DOMAIN_COMMUNICATION "communication"
#define DOMAIN_TRAFFIC "traffic"
#define LAYER_11P "11p"
#define LAYER_MICRO "micro"

#define ERROR_DOUBLE -42.0
#define ERROR_INTEGER -42
#define ERROR_STRING "ERROR"

#define STR_TYPE_ROADMAP "RoadMap"
#define STR_TYPE_INPUT "Input"
#define WAITING_TIME 1000000

#define RESULTS_STR_RETURNMODE "returnMode"
#define RESULTS_STR_RETURNMODE_ALL "all"
#define RESULTS_STR_RETURNMODE_RESOURCES "resources"
#define RESULTS_STR_RETURNMODE_LOGS "logs"
#define RESULTS_STR_RETURNMODE_LISTED "listed"
#define RESULTS_STR_RETURNMODE_NONE "none"

#define EXCEPTION_NOT_IMPLEMENTED std::runtime_error(std::string("Call to non-implemented function: '") + __PRETTY_FUNCTION__ + "'")

#include <cstddef>
#include <exception>
#include <string>
#include <vector>

namespace daceDS {

namespace Constants {
#include <string>
const std::string STR_ORCHESTRATION_CONSUMER = "_orchestration_consumer";
const std::string STR_INTERACTION_CONSUMER = "_interaction_consumer";
const std::string STR_PROVISION_CONSUMER = "_interaction_consumer";
const std::string STR_SCENARIO_CONSUMER = "_scenario_consumer";
const std::string STR_TIMESYNC_ID = "_timesync";
const std::string STR_SERTYPE_AVRO = "avro";
const std::string STR_SERTYPE_JSON = "json";
const std::string STR_MAGIC = "---";
const std::string STR_MAGIC_REPLACE = "---2d---2d---2d";
const std::string STR_FILTER_ALL_TOKEN = "";
const char CHAR_SPLIT_TOKEN = ' ';
const char CHAR_IGNORE_TOKEN = ':';
}  // namespace Constants

enum class Channel {
    ORCHESTRATION,
    INTERACTION,
    PROVISION
};

enum class Domain {
    NULLDOMAIN,
    TRAFFIC
};

enum class Layer {
    NULLLAYER,
    MICRO,
    MESO,
    MACRO
};

enum class Scope {
    NULLSCOPE,
    BASE,
    VEHICLE,
    EDGE,
    DETECTOR
};

enum class Attribute {
    NULLATTRIBUTE,
    ACCELERATION,
    ANGLE,
    EDGE,
    LANE,
    POSITION,
    POSITIONEDGE,
    ROUTE,
    SLOPE,
    SPEED,
    TYPE
};

enum class Command {
    SYNC,
    STATUS,
    TERMINATE,
    CONTROL,
    DIE
};

enum class Method {
    GET,
    SET,
    RET,
    ERR,
    ACT
};

enum class ReturnMode {
    ALL,
    RESOURCES,
    LOGS,
    LISTED,
    NONE
};



typedef struct TopicMetadata {
    Channel channel;
    std::string scenarioID = "";
    Domain domain = Domain::NULLDOMAIN;
    Layer layer = Layer::NULLLAYER;
    std::string instanceID = "";
    std::string channelSpecific;
    int64_t time = 0;
} TopicMetadata;

typedef struct OrchestrationTopicMetadata {
    TopicMetadata meta;
    Command command;

} OrchestrationTopicMetadata;

typedef struct InteractionTopicMetadata {
    TopicMetadata meta;
    Scope scope;
    std::string entity = "";
    Attribute attribute;
    Method method;
    std::string action = "";

} InteractionTopicMetadata;

typedef struct InteractionTopicMetadata2 {
    TopicMetadata meta;
    std::vector<std::string> hierarchy;
    Method method;
    std::string action = "";
} InteractionTopicMetadata2;

typedef struct ProvisionTopicMetadata {
    TopicMetadata meta;
    Scope scope;
    Attribute attribute;
    Method method;

} ProvisionTopicMetadata;

static std::vector<std::string> validAttributes = {
    "acceleration",
    "angle",
    "edge",
    "emissions",
    "lane",
    "position",
    "route",
    "speedMode",
    "genericParameter",
    "vehicleIDs",
    "boundaries",
    "edges",
    "speed",
    "density",
    "flow",
    "emissions"};

typedef struct Vec3D {
    double x, y, z;

} Vec3D;

typedef struct ResourceFileMeta {
    std::string path;
    std::string type;
    bool received;
} ResourceFileMeta;

}  // namespace daceDS
