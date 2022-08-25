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


#include "AvroHelper.h"
using namespace daceDS;

AvroHelper *AvroHelper::inst = 0;
AvroHelper::AvroHelper() {}

//from libserdes example
AvroHelper *AvroHelper::getInstance() {
    if (inst == 0) {
        inst = new AvroHelper();
        std::string errstr = "";
        Serdes::Conf *sconf = Serdes::Conf::create();

        /* Default URL */  //todo: read from cobfug
        if (sconf->set("schema.registry.url", Config::getInstance()->get(CONFIG_SCHEMAREGISTRY), errstr))
            KERROR("Conf failed: " << errstr << Config::getInstance()->get(CONFIG_SCHEMAREGISTRY));
        DS_AVRO_DBG("Config::getInstance()->get(schemaRegistry)=" << Config::getInstance()->get(CONFIG_SCHEMAREGISTRY));

        /* Default framing to CP1 */
        if (sconf->set("deserializer.framing", "cp1", errstr))
            //		if (sconf->set("deserializer.framing", "none", errstr))
            KERROR("Conf failed: " << errstr);

        if (sconf->set("serializer.framing", "cp1", errstr))
            //		if (sconf->set("deserializer.framing", "none", errstr))
            KERROR("Conf failed: " << errstr);

        if (sconf->set("debug", "all", errstr))
            KERROR("Conf failed: " << errstr);

        inst->serdes = Serdes::Avro::create(sconf, errstr);

        if(errstr.size()>0){
            KERROR("Serdes::Avro::create: " << errstr);
            KERROR("Serdes::Avro::create: " << errstr);
            KERROR("Serdes::Avro::create: " << errstr);
            KERROR("Serdes::Avro::create: " << errstr);

            exit(1);
        }

        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_MICRO, SCHEMA_DEF_MICRO);
        // AvroHelper::getInstance()->initSchema(SCHEMA_NAME_MICRO_LIST, SCHEMA_DEF_MICRO_LIST);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_MicroDetector, SCHEMA_DEF_MicroDetector);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_MICROEDGE, SCHEMA_DEF_MICROEDGE);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_RESOURCEFILE, SCHEMA_DEF_RESOURCEFILE);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_SCENARIO, SCHEMA_DEF_SCENARIO);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_STATUSMSG, SCHEMA_DEF_STATUSMSG);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_CTRLMSG, SCHEMA_DEF_CTRLMSG);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_PRIMITIVESTRING, SCHEMA_DEF_PRIMITIVESTRING);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_PRIMITIVEDOUBLE, SCHEMA_DEF_PRIMITIVEDOUBLE);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_PRIMITIVEINTEGER, SCHEMA_DEF_PRIMITIVEINTEGER);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_LIST_STRING, SCHEMA_DEF_LIST_STRING);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_LIST_DOUBLE, SCHEMA_DEF_LIST_DOUBLE);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_LIST_MICRO, SCHEMA_DEF_LIST_MICRO);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_SYNCMSG, SCHEMA_DEF_SYNCMSG);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_VEC3, SCHEMA_DEF_VEC3);
        // AvroHelper::getInstance()->initSchema(SCHEMA_NAME_RADIOMSG11P, SCHEMA_DEF_RADIOMSG11P);
        AvroHelper::getInstance()->initSchema(SCHEMA_NAME_RADIOMSG, SCHEMA_DEF_RADIOMSG);
    }
    return inst;
}

//either retrieve or upload schema definition
void AvroHelper::initSchema(std::string schemaName, std::string schemaDefinition, std::string topic) {
    std::string errstr = "";

    DS_AVRO_DBG("schemaName=" << schemaName);
    DS_AVRO_DBG("schemaDefinition=" << schemaDefinition);
    DS_AVRO_DBG("topic=" << topic);
    DS_AVRO_DBG("serdes=" << serdes);

    //check for the base definition
    schemas[schemaName] = Serdes::Schema::get(serdes, schemaName, errstr);
    if(errstr.size()>0){
        KERROR("get schema: "<<errstr);
    }
    
    if (!schemas[schemaName]) {
        Serdes::Schema *sch = Serdes::Schema::add(serdes, schemaName, schemaDefinition, errstr);
        if (!sch) {
            KERROR("Failed to get && add schema!");
            KERROR(errstr);
            exit(1);
        }
        schemas[schemaName] = sch;
    }

    //we are limited to a RecordNameStrategy with libserdes, the following is not needed right now
    // //check for full topic definition
    // if (topic.size() > 0) {
    //     std::string schemaTopicName = topic + "-value";
    //     schemas[schemaTopicName] = Serdes::Schema::get(serdes, schemaTopicName, errstr);

    //     //not in registry --> use base definition
    //     if (!schemas[schemaTopicName]) {
    //         schemas[schemaTopicName] = Serdes::Schema::add(serdes, schemaTopicName, schemaDefinition, errstr);
    //         DS_AVRO_DBG("initialized schema on topic " << schemaTopicName);
    //     } else {
    //         DS_AVRO_DBG("there is already a definition for " << topic << ": " << schemas[schemaTopicName]->definition());
    //     }
    // }
}

std::vector<char> AvroHelper::encodeMicro(datamodel::Micro &micro) {
    avro::ValidSchema *avro_schema;
    std::string name = SCHEMA_NAME_MICRO;
    if (schemas.count(name) == 0 || (avro_schema = schemas[name]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();

    try {
        e->init(*bin_os.get());
        avro::encode(*e, micro);
        e->flush();
    } catch (const avro::Exception &e) {
        KERROR("Avro serialization failed: " << e.what());
        exit(1);
    }

    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[name]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());
    return out;
}

std::vector<char> AvroHelper::encodeMicroEdge(datamodel::MicroEdge &micro) {
    avro::ValidSchema *avro_schema;
    std::string name = SCHEMA_NAME_MICROEDGE;
    if (schemas.count(name) == 0 || (avro_schema = schemas[name]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;
    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();

    try {
        e->init(*bin_os.get());
        avro::encode(*e, micro);
        e->flush();
    } catch (const avro::Exception &e) {
        KERROR("Avro serialization failed: " << e.what());
        exit(1);
    }
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());

    schemas[name]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());
    return out;
}

std::vector<char> AvroHelper::encodeMicroDetector(datamodel::MicroDetector &micro) {
    avro::ValidSchema *avro_schema;
    std::string name = SCHEMA_NAME_MicroDetector;
    if (schemas.count(name) == 0 || (avro_schema = schemas[name]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;
    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();

    try {
        e->init(*bin_os.get());
        avro::encode(*e, micro);
        e->flush();
    } catch (const avro::Exception &e) {
        KERROR("Avro serialization failed: " << e.what());
        exit(1);
    }
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());

    schemas[name]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());
    return out;
}

std::vector<char> AvroHelper::encodeResourceFile(datamodel::ResourceFile &rf) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_RESOURCEFILE) == 0 || (avro_schema = schemas[SCHEMA_NAME_RESOURCEFILE]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;
    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    DS_AVRO_DBG("init");
    avro::encode(*e, rf);
    e->flush();
    DS_AVRO_DBG("flush");
    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    /* Write framing */
    schemas[SCHEMA_NAME_RESOURCEFILE]->framing_write(out);
    DS_AVRO_DBG("framing");
    /* Write binary encoded Avro to output std::vector */
    out.insert(out.end(), v->begin(), v->end());
    return out;
}

datamodel::CtrlMsg AvroHelper::decodeCtrlMsg(const std::string &pfx, const void *buf, size_t len) {
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    datamodel::CtrlMsg ctrlMsg;
    avro::GenericRecord r = d->value<avro::GenericRecord>();
    ctrlMsg.Command = r.field("Command").value<std::string>();
    ctrlMsg.Sender = r.field("Sender").value<std::string>();

    if (d)
        delete d;

    return ctrlMsg;
}
datamodel::StatusMsg AvroHelper::decodeStatusMsg(const std::string &pfx, const void *buf, size_t len) {
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    datamodel::StatusMsg statusMsg;
    avro::GenericRecord r = d->value<avro::GenericRecord>();
    statusMsg.Action = r.field("Action").value<std::string>();
    statusMsg.Sender = r.field("Sender").value<std::string>();
    statusMsg.Status = r.field("Status").value<std::string>();

    if (d)
        delete d;

    return statusMsg;
}

datamodel::ResourceFile AvroHelper::decodeResourceFile(const std::string &pfx, const void *buf, size_t len) {
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    datamodel::ResourceFile resourceFile;
    avro::GenericRecord r = d->value<avro::GenericRecord>();
    resourceFile.ID = r.field("ID").value<std::string>();
    resourceFile.Type = r.field("Type").value<std::string>();

    avro::GenericUnion fileUnion = r.field("File").value<avro::GenericUnion>();
    fileUnion.selectBranch( 1 );
    avro::GenericDatum fileDatum = fileUnion.datum();
    if(fileDatum.type() ==avro::Type::AVRO_BYTES){
        resourceFile.File.set_bytes(fileDatum.value<std::vector<uint8_t>>());
    } else {
         resourceFile.File.set_null();
    } 
    DS_AVRO_DBG("Type of File field is: "<<fileDatum.type());

    avro::GenericUnion fileReferenceUnion = r.field("FileReference").value<avro::GenericUnion>();
    fileReferenceUnion.selectBranch( 1 );
    avro::GenericDatum fileReferenceDatum = fileReferenceUnion.datum();
    if(fileReferenceDatum.type() ==avro::Type::AVRO_STRING){
        resourceFile.FileReference.set_string(fileReferenceDatum.value<std::string>());
    } else {
         resourceFile.FileReference.set_null();
    } 
    DS_AVRO_DBG("Type of FileReference field is: "<<fileReferenceDatum.type());
    
    

    if (d)
        delete d;
    DS_AVRO_DBG("Decoded ResourceFile!");
    DS_AVRO_DBG("ID='" << resourceFile.ID<<"'");
    DS_AVRO_DBG("Type='" << resourceFile.Type<<"'");
    DS_AVRO_DBG("File='" << (resourceFile.File.is_null() ? "null" : "size is: "+std::to_string(resourceFile.File.get_bytes().size())) << "'");
    DS_AVRO_DBG("FileReference='" << (resourceFile.FileReference.is_null() ? "null" : resourceFile.FileReference.get_string()) << "'");

    return resourceFile;
}

// //once again another try based on https://blog.basyskom.com/2021/what-is-apache-avro-compared-to-protobuf/
// datamodel::ResourceFile AvroHelper::decodeResourceFile(const std::string &pfx, const void *buf, size_t len) {



//     // avro::GenericDatum *d = NULL;
//     // Serdes::Schema *serdes_schema = NULL;
//     // std::string errstr;
//     // serdes->deserialize(&serdes_schema, &d, buf, len, errstr);
//     // // serdes->deserialize(&serdes_schema, &rf, buf, len, errstr);

//     // datamodel::ResourceFile rf =  d->value<datamodel::ResourceFile>();
//     // // datamodel::ResourceFile* rf =  d->value<avro::GenericRecord>();
//     // DS_AVRO_DBG("serdesSchema is: "+serdes_schema->definition());
//     // DS_AVRO_DBG("Decoded ResourceFile!");
//     // DS_AVRO_DBG("ID='" << rf.ID<<"'");
//     // DS_AVRO_DBG("Type='" << rf.Type<<"'");
//     // DS_AVRO_DBG("File='" << (rf.File.is_null() ? "null" : std::to_string(rf.File.get_bytes().size())) << "'");
//     // DS_AVRO_DBG("FileReference='" << (rf.FileReference.is_null() ? "null" : rf.FileReference.get_string()) << "'");

//     // avro::ValidSchema* schema = serdes_schema->object();


//     avro::ValidSchema schema;
//     try {
//         schema = avro::compileJsonSchemaFromString(SCHEMA_DEF_RESOURCEFILE);
//     } catch (std::exception &ex) {
//         KERROR("Failed to compile schema:" << ex.what());
//         exit(1);
//     }

//     // Create input stream and a validating binary decoder
//     auto in = avro::memoryInputStream(buf, len);
//     auto decoder = avro::binaryDecoder();

//     decoder->init(*in);
//     auto validatingDecoder = avro::validatingDecoder(schema, decoder);

//     datamodel::ResourceFile decoded;

//     try {
//         // avro::decode(*validatingDecoder, decoded);
//         datamodel::ResourceFile::decode(*validatingDecoder, decoded);
//     } catch (std::exception &ex) {
//         KERROR("Decode failed with:" << ex.what());
//         exit(1);
//     }

//     DS_AVRO_DBG("Decoded ResourceFile!");
//     DS_AVRO_DBG("ID='" << decoded.ID<<"'");
//     DS_AVRO_DBG("Type='" << decoded.Type<<"'");
//     DS_AVRO_DBG("File='" << (decoded.File.is_null() ? "null" : std::to_string(decoded.File.get_bytes().size())) << "'");
//     DS_AVRO_DBG("FileReference='" << (decoded.FileReference.is_null() ? "null" : decoded.FileReference.get_string()) << "'");
//     return decoded;
// }

datamodel::Scenario AvroHelper::decodeScenario(const std::string &pfx, const void *buf, size_t len) {
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    DS_AVRO_DBG("calling serdes ");
    serdes->deserialize(&schema, &d, buf, len, errstr);

    DS_AVRO_DBG("building obj");

    datamodel::Scenario scenario;
    avro::GenericRecord r = d->value<avro::GenericRecord>();
    avro::GenericRecord r2 = d->value<avro::GenericRecord>();
    scenario.scenarioID = r.field("scenarioID").value<std::string>();

    DS_AVRO_DBG("scenarioid=" << scenario.scenarioID);

    // scenario.RoadMap = r.field("RoadMap").value<std::string>();
    // DS_AVRO_DBG("RoadMap=" << scenario.RoadMap);

    scenario.simulationStart = r.field("simulationStart").value<int64_t>();
    DS_AVRO_DBG("SimulationStart=" << scenario.simulationStart);
    scenario.simulationEnd = r.field("simulationEnd").value<int64_t>();
    DS_AVRO_DBG("SimulationEnd=" << scenario.simulationEnd);

    // for (avro::GenericDatum rr : r.field("Inputs").value<avro::GenericArray>().value()) {
    //     avro::GenericRecord rl = rr.value<avro::GenericRecord>();
    //     DS_AVRO_DBG("GenericRecord=");
    //     datamodel::Input input;
    //     input.layer = rl.field("Layer").value<std::string>();
    //     DS_AVRO_DBG("Layer=" << input.layer);
    //     input.path = rl.field("Path").value<std::string>();
    //     input.processing = rl.field("Processing").value<std::string>();
    //     scenario.Inputs.push_back(input);
    // }

    DS_AVRO_DBG("Execution ");
    //Segfault:
    //	datamodel::Execution execution = r.field("Execution").value<datamodel::Execution>();
    avro::GenericRecord aexecution = r.field("execution").value<avro::GenericRecord>();
    datamodel::Execution execution;
    execution.constraints = aexecution.field("constraints").value<std::string>();
    execution.priority = aexecution.field("priority").value<int64_t>();
    execution.randomSeed = aexecution.field("randomSeed").value<int64_t>();
    execution.syncedParticipants = aexecution.field("syncedParticipants").value<int64_t>();
    DS_AVRO_DBG("Execution SyncedParticipants=" << execution.SyncedParticipants);

    scenario.execution = execution;

    //optinal fields are handed with boost::any -> sim.LayerParams.get_array() instead of sim.LayerPArams

    for (avro::GenericDatum rr : r.field("buildingBlocks").value<avro::GenericArray>().value()) {
        DS_AVRO_DBG("BB");
        datamodel::BB sim;
        avro::GenericRecord rl = rr.value<avro::GenericRecord>();
        sim.instanceID = rl.field("instanceID").value<std::string>();

        DS_AVRO_DBG("instanceID=" << sim.instanceID);

        sim.type = rl.field("type").value<std::string>();
        DS_AVRO_DBG("type=" << sim.type);
        sim.stepLength = rl.field("stepLength").value<int>();
        DS_AVRO_DBG("stepLength=" << sim.stepLength);
        sim.layer = rl.field("layer").value<std::string>();
        DS_AVRO_DBG("layer=" << sim.layer);
        sim.domain = rl.field("domain").value<std::string>();
        DS_AVRO_DBG("domain=" << sim.domain);

        avro::GenericMap::Value vp = rl.field("parameters").value<avro::GenericMap>().value();
        for (std::pair<std::string, avro::GenericDatum> p : vp) {
            DS_AVRO_DBG(" Key=" << p.first);
            DS_AVRO_DBG(" Value=" << p.second.value<std::string>());
            sim.parameters[p.first] = p.second.value<std::string>();
        }

        vp = rl.field("resources").value<avro::GenericMap>().value();
        for (std::pair<std::string, avro::GenericDatum> p : vp) {
            DS_AVRO_DBG(" Key=" << p.first);
            DS_AVRO_DBG(" Value=" << p.second.value<std::string>());
            sim.resources[p.first] = p.second.value<std::string>();
        }

        vp = rl.field("results").value<avro::GenericMap>().value();
        for (std::pair<std::string, avro::GenericDatum> p : vp) {
            DS_AVRO_DBG(" Key=" << p.first);
            DS_AVRO_DBG(" Value=" << p.second.value<std::string>());
            sim.results[p.first] = p.second.value<std::string>();
        }

        sim.synchronized = rl.field("synchronized").value<bool>();
        sim.isExternal = rl.field("isExternal").value<bool>();
        DS_AVRO_DBG("Synchronized=" << sim.synchronized);
        // avro::GenericRecord atiming = rl.field("Timing").value<avro::GenericRecord>();
        // datamodel::Timing timing;
        // timing.Constrained = atiming.field("Constrained").value<bool>();
        // DS_AVRO_DBG("Constrained=" << timing.Constrained);
        // timing.Lookahead = atiming.field("Lookahead").value<int64_t>();
        // DS_AVRO_DBG("Lookahead=" << timing.Lookahead);
        // timing.Regulating = atiming.field("Regulating").value<bool>();
        // DS_AVRO_DBG("Regulating=" << timing.Regulating);
        // sim.Timing = timing;

        for (avro::GenericDatum rrr : rl.field("responsibilities").value<avro::GenericArray>().value()) {
            std::string resp = rrr.value<std::string>();
            DS_AVRO_DBG("resp" << resp);
            sim.responsibilities.push_back(resp);
        }

        // for (avro::GenericDatum rrr : rl.field("Borders").value<avro::GenericArray>().value()) {
        //     std::string border = rrr.value<std::string>();
        //     DS_AVRO_DBG("border" << border);
        //     sim.Borders.push_back(border);
        // }

        for (avro::GenericDatum rrr : rl.field("observers").value<avro::GenericArray>().value()) {
            DS_AVRO_DBG("Observers: ");
            datamodel::Observer ob;
            avro::GenericRecord obr = rrr.value<avro::GenericRecord>();
            ob.task = obr.field("task").value<std::string>();
            DS_AVRO_DBG(" task=" << ob.task);
            ob.element = obr.field("element").value<std::string>();
            DS_AVRO_DBG(" element=" << ob.element);
            ob.filter = obr.field("filter").value<std::string>();
            DS_AVRO_DBG(" filter=" << ob.filter);
            ob.period = obr.field("period").value<int32_t>();
            DS_AVRO_DBG(" period=" << ob.period);
            ob.trigger = obr.field("trigger").value<std::string>();
            DS_AVRO_DBG(" trigger=" << ob.trigger);
            ob.type = obr.field("type").value<std::string>();
            DS_AVRO_DBG(" type=" << ob.type);
            sim.observers.push_back(ob);
        }

        // sim.CustomParams = rl.field("CustomParams").value<std::string>();
        // DS_AVRO_DBG(" CustomParams=" << sim.CustomParams);
        // avro::GenericMap::Value cpams = rl.field("CustomParams").value<avro::GenericMap>().value();
        // for (std::pair<std::string, avro::GenericDatum> p : cpams) {
        //     DS_AVRO_DBG(" Key=" << p.first);
        //     DS_AVRO_DBG(" Value=" << p.second.value<std::string>());
        //     sim.CustomParams[p.first] = p.second.value<std::string>();
        // }
        scenario.buildingBlocks.push_back(sim);
    }

    for (avro::GenericDatum rr : r.field("translators").value<avro::GenericArray>().value()) {
        avro::GenericRecord rl = rr.value<avro::GenericRecord>();
        datamodel::Translator translator;
        translator.translatorID = rl.field("translatorID").value<std::string>();
        translator.domain = rl.field("domain").value<std::string>();
        translator.layerA = rl.field("layerA").value<std::string>();
        translator.layerB = rl.field("layerB").value<std::string>();

        auto vp = rl.field("resources").value<avro::GenericMap>().value();
        for (std::pair<std::string, avro::GenericDatum> p : vp) {
            DS_AVRO_DBG(" Key=" << p.first);
            DS_AVRO_DBG(" Value=" << p.second.value<std::string>());
            translator.resources[p.first] = p.second.value<std::string>();
        }
        vp = rl.field("parameters").value<avro::GenericMap>().value();
        for (std::pair<std::string, avro::GenericDatum> p : vp) {
            DS_AVRO_DBG(" Key=" << p.first);
            DS_AVRO_DBG(" Value=" << p.second.value<std::string>());
            translator.parameters[p.first] = p.second.value<std::string>();
        }

        for (avro::GenericDatum rrr : rl.field("responsibilitiesA").value<avro::GenericArray>().value()) {
            std::string resp = rrr.value<std::string>();
            DS_AVRO_DBG("respA: " << resp);
            translator.responsibilitiesA.push_back(resp);
        }        
        for (avro::GenericDatum rrr : rl.field("responsibilitiesB").value<avro::GenericArray>().value()) {
            std::string resp = rrr.value<std::string>();
            DS_AVRO_DBG("respB: " << resp);
            translator.responsibilitiesB.push_back(resp);
        }

        scenario.translators.push_back(translator);
    }


    
    //todo: if projectors are needed add logic here


    if (d)
        delete d;

    return scenario;
}

//Todo: why do we need to do the decodings in this way? Shouldn't this work in a smarter way using libserdes?
datamodel::Micro AvroHelper::decodeMicro(const std::string &pfx, const void *buf, size_t len) {
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr = "";

    datamodel::Micro micro;

    try {

        serdes->deserialize(&schema, &d, buf, len, errstr);

        if(errstr.size()>0){
        KERROR("error deserializing: " << errstr);
            exit(1);
            return micro;   
        }
        
        avro::GenericRecord r = d->value<avro::GenericRecord>();
        DS_AVRO_DBG("created generic record");

        try {
            DS_AVRO_DBG("vehicleID");
            micro.vehicleID = r.field("vehicleID").value<std::string>();
            DS_AVRO_DBG("vehicleID" << micro.vehicleID);

        } catch (const avro::Exception &e) {
            DS_AVRO_DBG("get vehicle id failed: " << e.what());
            micro.vehicleID = "unkown";
        }

        try {
            DS_AVRO_DBG("route");
            for (avro::GenericDatum rr : r.field("route").value<avro::GenericArray>().value()) {
                micro.route.push_back(rr.value<std::string>());
            }
        } catch (const avro::Exception &e) {
            KERROR("get route failed: " << e.what());
        }

        try {
            micro.edge = r.field("edge").value<std::string>();
            DS_AVRO_DBG("edge" << micro.edge);
        } catch (const avro::Exception &e) {
            KERROR("get edge failed: " << e.what());
            micro.edge = "unkown";
        }

        try {
            micro.lane = r.field("lane").value<int32_t>();
            DS_AVRO_DBG("lane" << micro.lane);
        } catch (const avro::Exception &e) {
            KERROR("get lane failed: " << e.what());
            micro.lane = 0;
        }

        try {
            avro::GenericRecord position = r.field("position").value<avro::GenericRecord>();
            micro.position.x = position.field("x").value<double>();
            micro.position.y = position.field("y").value<double>();
            micro.position.z = position.field("z").value<double>();
        } catch (const avro::Exception &e) {
            KERROR("get position failed: " << e.what());
            micro.position.x = 0;
            micro.position.y = 0;
            micro.position.z = 0;
        }

        try {
            micro.positionEdge = r.field("positionEdge").value<double>();
            DS_AVRO_DBG("positionEdge" << micro.positionEdge);
        } catch (const avro::Exception &e) {
            KERROR("get lane failed: " << e.what());
            micro.positionEdge = 0;
        }

        try {
            micro.speed = r.field("speed").value<double>();
            DS_AVRO_DBG("speed" << micro.speed);
        } catch (const avro::Exception &e) {
            KERROR("get lane failed: " << e.what());
            micro.speed = 0;
        }


        try {
            micro.acceleration = r.field("acceleration").value<double>();
            DS_AVRO_DBG("acceleration" << micro.acceleration);
        } catch (const avro::Exception &e) {
            KERROR("get lane failed: " << e.what());
            micro.lane = 0;
        }


        try {

            micro.angle = r.field("angle").value<double>();
            DS_AVRO_DBG("angle" << micro.angle);
        } catch (const avro::Exception &e) {
                KERROR("get angle failed: " << e.what());
                micro.angle = 0;
        }


        try {

            micro.slope = r.field("slope").value<double>();
            DS_AVRO_DBG("slope" << micro.slope);
        } catch (const avro::Exception &e) {
                KERROR("get slope failed: " << e.what());
                micro.slope = 0;
        }



        try {

            micro.type = r.field("type").value<std::string>();
            DS_AVRO_DBG("type" << micro.type);
        } catch (const avro::Exception &e) {
                KERROR("get type failed: " << e.what());
                micro.type = "unkown";
        }

        DS_AVRO_DBG("success decoding micro");
    }
    catch (std::exception &e) {
            KERROR("decode micro failed: " << e.what());
    }

    if (d)
        delete d;

    return micro;
}

std::vector<datamodel::Micro> AvroHelper::decodeListMicro(const std::string &pfx, const void *buf, size_t len) {
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;
    std::vector<datamodel::Micro> list;
    serdes->deserialize(&schema, &d, buf, len, errstr);

    datamodel::Micro micro;
    for (avro::GenericDatum m : d->value<avro::GenericArray>().value()) {
        avro::GenericRecord r = m.value<avro::GenericRecord>();
        datamodel::Micro micro;
        micro.vehicleID = r.field("vehicleID").value<std::string>();

        for (avro::GenericDatum rr : r.field("route").value<avro::GenericArray>().value()) {
            micro.route.push_back(rr.value<std::string>());
        }

        micro.edge = r.field("edge").value<std::string>();
        micro.lane = r.field("lane").value<int32_t>();

        avro::GenericRecord position = r.field("position").value<avro::GenericRecord>();
        micro.position.x = position.field("x").value<double>();
        micro.position.y = position.field("y").value<double>();
        micro.position.z = position.field("z").value<double>();

        micro.positionEdge = r.field("positionEdge").value<double>();
        micro.speed = r.field("speed").value<double>();
        micro.acceleration = r.field("acceleration").value<double>();
        micro.angle = r.field("angle").value<double>();
        micro.type = r.field("type").value<std::string>();

        list.push_back(micro);
    }
    if (d)
        delete d;

    return list;
}

datamodel::MicroList AvroHelper::decodeMicroList(const std::string &pfx, const void *buf, size_t len) {
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;
    std::vector<datamodel::Micro> list;
    serdes->deserialize(&schema, &d, buf, len, errstr);

    datamodel::MicroList microList;

    avro::GenericRecord gr = d->value<avro::GenericRecord>();
    //for (avro::GenericDatum m : d->value<avro::GenericArray>().value()) {
    for (avro::GenericDatum m : gr.field("contents").value<avro::GenericArray>().value()) {
        avro::GenericRecord r = m.value<avro::GenericRecord>();
        datamodel::Micro micro;
        micro.vehicleID = r.field("vehicleID").value<std::string>();

        for (avro::GenericDatum rr : r.field("route").value<avro::GenericArray>().value()) {
            micro.route.push_back(rr.value<std::string>());
        }

        micro.edge = r.field("edge").value<std::string>();
        micro.lane = r.field("lane").value<int32_t>();

        avro::GenericRecord position = r.field("position").value<avro::GenericRecord>();
        micro.position.x = position.field("x").value<double>();
        micro.position.y = position.field("y").value<double>();
        micro.position.z = position.field("z").value<double>();

        micro.positionEdge = r.field("positionEdge").value<double>();
        micro.speed = r.field("speed").value<double>();
        micro.acceleration = r.field("acceleration").value<double>();
        micro.angle = r.field("angle").value<double>();
        micro.type = r.field("type").value<std::string>();

        microList.contents.push_back(micro);
    }
    if (d)
        delete d;

    return microList;
}

TopicMetadata AvroHelper::getTopicMetadata(std::string topic) {
    TopicMetadata meta;

    std::stringstream ss(topic);

    //get Channel
    std::string channel;
    getline(ss, channel, '.');
    std::string sim;
    getline(ss, sim, '.');
    std::string scenarioID;
    getline(ss, scenarioID, '.');
    std::string domain;
    getline(ss, domain, '.');
    std::string layer;
    getline(ss, layer, '.');

    // disables instance id from topics
    // //instance optional:
    // std::string tmp;
    // getline(ss, tmp, '.');
    // std::string itn = "instanceID=";
    // if (tmp.substr(0, itn.length()) == itn) {
    //     meta.instanceID = tmp;
    // } else {
    //     meta.channelSpecific = tmp + meta.channelSpecific;
    // }

    //followup is channel specific:
    std::string specific;
    getline(ss, specific);
    meta.channelSpecific = specific;

    if (channel == "interaction")
        meta.channel = Channel::INTERACTION;
    else if (channel == "provision")
        meta.channel = Channel::PROVISION;
    else if (channel == "orchestration")
        meta.channel = Channel::ORCHESTRATION;
    else {
        meta.channelSpecific = channel + meta.channelSpecific;
    }

    meta.scenarioID = scenarioID;
    if (domain == "traffic")
        meta.domain = Domain::TRAFFIC;
    else {
        meta.channelSpecific = domain + meta.channelSpecific;
    }

    if (layer == "micro")
        meta.layer = Layer::MICRO;
    else if (layer == "meso")
        meta.layer = Layer::MESO;
    else if (layer == "macro")
        meta.layer = Layer::MACRO;
    else {
        meta.channelSpecific = layer + meta.channelSpecific;
    }
    return meta;
}

OrchestrationTopicMetadata AvroHelper::getOrchestrationTopicMetadata(TopicMetadata m) {
    OrchestrationTopicMetadata om;
    om.meta = m;
    if (m.channelSpecific == "sync")
        om.command = Command::SYNC;
    else if (m.channelSpecific == "status")
        om.command = Command::STATUS;
    else if (m.channelSpecific == "ctrl")
        om.command = Command::CONTROL;
    else {
        KERROR(m.channelSpecific << "' is not defined");
    }
    return om;
}

//todo: this does not work with my current (flexivel) layer definition
InteractionTopicMetadata AvroHelper::getInteractionTopicMetadata(TopicMetadata m) {
    InteractionTopicMetadata im;
    im.meta = m;

    std::stringstream ss(m.channelSpecific);
    std::string scope;
    getline(ss, scope, '.');
    if (scope == "vehicle") im.scope = Scope::VEHICLE;
    else if (scope == "edge") im.scope = Scope::EDGE;
    else if (scope == "detector") im.scope = Scope::DETECTOR;
    else im.scope = Scope::BASE;

    //entity & attribute are mostly there, but optional
    std::string s1;
    std::string s2, s3;
    std::string entity = "", attribute = "", method = "";
    if(im.scope == Scope::BASE)
        s1 == scope;
    else
        getline(ss, s1, '.');
    getline(ss, s2, '.');
    getline(ss, s3, '.');

    //entity is astd::string identifier
    getline(ss, im.entity, '.');

    if (s3.size() > 0) {
        entity = s1;
        attribute = s2;
        method = s3;
    } else if (s2.size() > 0) {
        method = s2;
        //s1 entity or attribute?
        if (isValidAttributeStr(s1)) {
            attribute = s1;
        } else {
            entity = s1;
        }

    } else if (s1.size() > 0) {
        method = s1;
    } else {
        KERROR("error parsing interactiontopicmetdata");
        exit(1);
    }

    if (s2.size() > 0) {
        attribute = s1;
        if (attribute == "acceleration") im.attribute = Attribute::ACCELERATION;
        if (attribute == "angle") im.attribute = Attribute::ANGLE;
        if (attribute == "edge") im.attribute = Attribute::EDGE;
        if (attribute == "lane") im.attribute = Attribute::LANE;
        if (attribute == "position") im.attribute = Attribute::POSITION;
        if (attribute == "positionEdge") im.attribute = Attribute::POSITIONEDGE;
        if (attribute == "route") im.attribute = Attribute::ROUTE;
        if (attribute == "speed") im.attribute = Attribute::SPEED;
        if (attribute == "slope") im.attribute = Attribute::SLOPE;
        if (attribute == "type") im.attribute = Attribute::TYPE;
        method = s2;

    } else {
        method = s1;
    }
    if (method == "get")
        im.method = Method::GET;
    else if (method == "set")
        im.method = Method::SET;
    else if (method == "ret")
        im.method = Method::RET;
    else if (method == "err")
        im.method = Method::ERR;
    else {
        im.method = Method::ACT;
        im.action = method;
    }

    return im;


}


InteractionTopicMetadata2 AvroHelper::getInteractionTopicMetadata2(TopicMetadata m) {
    InteractionTopicMetadata2 im;
    im.meta = m;
    DS_AVRO_DBG("channelspecfic is = " << m.channelSpecific);
    std::vector<std::string> tokens = split(m.channelSpecific, '.');

    DS_AVRO_DBG("got " << tokens.size() << " tokens out of it");
    if(tokens.size()>0){
        std::string method = tokens.back();
        tokens.pop_back();
        if (method == "get")
            im.method = Method::GET;
        else if (method == "set")
            im.method = Method::SET;
        else if (method == "ret")
            im.method = Method::RET;
        else if (method == "err")
            im.method = Method::ERR;
        else {
            im.method = Method::ACT;
            im.action = method;
        }
    } else {
        return im;
    }

    im.hierarchy = tokens;

    return im;
}

ProvisionTopicMetadata AvroHelper::getProvisionTopicMetadata(TopicMetadata m) {
    ProvisionTopicMetadata pm;
    pm.meta = m;

    return pm;
}

void AvroHelper::decodeBaseType(ConsumedMessage &msg, std::string &s1) {
    const void *buf = msg.payload;
    size_t len = msg.len;

    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);
    s1 = d->value<std::string>();

    if (d)
        delete d;

    return;
}

void AvroHelper::decodeBaseType(ConsumedMessage &msg, double &d1, double &d2, double &d3) {
    const void *buf = msg.payload;
    size_t len = msg.len;

    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    avro::GenericRecord r = d->value<avro::GenericRecord>();
    d1 = r.field("d1").value<double>();
    d2 = r.field("d2").value<double>();
    d3 = r.field("d3").value<double>();

    if (d)
        delete d;

    return;
}

void AvroHelper::decodeBaseType(ConsumedMessage &msg, double &d1, double &d2) {
    const void *buf = msg.payload;
    size_t len = msg.len;

    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    avro::GenericRecord r = d->value<avro::GenericRecord>();
    d1 = r.field("d1").value<double>();
    d2 = r.field("d2").value<double>();

    if (d)
        delete d;

    return;
}
void AvroHelper::decodeBaseType(ConsumedMessage &msg, double &d1) {
    const void *buf = msg.payload;
    size_t len = msg.len;

    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    d1 = d->value<double>();

    if (d)
        delete d;

    return;
}

void AvroHelper::decodeBaseType(ConsumedMessage &msg, int &i1) {
    const void *buf = msg.payload;
    size_t len = msg.len;

    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    i1 = d->value<int>();

    if (d)
        delete d;

    return;
}

void AvroHelper::decodeBaseType(ConsumedMessage &msg, std::vector<std::string> &sl) {
    const void *buf = msg.payload;
    size_t len = msg.len;

    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    avro::GenericRecord r = d->value<avro::GenericRecord>();
    for (avro::GenericDatum rr : r.field("strings").value<avro::GenericArray>().value()) {
        std::string str = rr.value<std::string>();
        sl.push_back(str);
    }
}

void AvroHelper::decodeVec3D(ConsumedMessage &msg, Vec3D &v3d) {
    const void *buf = msg.payload;
    size_t len = msg.len;

    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    serdes->deserialize(&schema, &d, buf, len, errstr);

    avro::GenericRecord r = d->value<avro::GenericRecord>();
    v3d.x = r.field("x").value<double>();
    v3d.y = r.field("y").value<double>();
    v3d.z = r.field("z").value<double>();

    if (d)
        delete d;

    return;
}

std::vector<char> AvroHelper::encodeVec3D(Vec3D &v3d) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_VEC3) == 0 || (avro_schema = schemas[SCHEMA_NAME_VEC3]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    datamodel::Vec3 v3;
    v3.x = v3d.x;
    v3.y = v3d.y;
    v3.z = v3d.z;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, v3);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_VEC3]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

//hier
std::vector<char> AvroHelper::encodeBaseType(double &d) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_PRIMITIVEDOUBLE) == 0 || (avro_schema = schemas[SCHEMA_NAME_PRIMITIVEDOUBLE]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }
    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, d);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_PRIMITIVEDOUBLE]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

std::vector<char> AvroHelper::encodeBaseType(int &d) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_PRIMITIVEINTEGER) == 0 || (avro_schema = schemas[SCHEMA_NAME_PRIMITIVEINTEGER]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, d);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_PRIMITIVEINTEGER]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

std::vector<char> AvroHelper::encodeBaseType(std::string &d) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_PRIMITIVESTRING) == 0 || (avro_schema = schemas[SCHEMA_NAME_PRIMITIVESTRING]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, d);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_PRIMITIVESTRING]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

std::vector<char> AvroHelper::encodeBaseType(std::vector<std::string> &vec) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_LIST_STRING) == 0 || (avro_schema = schemas[SCHEMA_NAME_LIST_STRING]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, vec);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_LIST_STRING]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

std::vector<char> AvroHelper::encodeBaseType(std::vector<double> &vec) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_LIST_DOUBLE) == 0 || (avro_schema = schemas[SCHEMA_NAME_LIST_DOUBLE]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, vec);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_LIST_DOUBLE]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

std::vector<char> AvroHelper::encodeListMicro(std::vector<datamodel::Micro> &vec) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_LIST_MICRO) == 0 || (avro_schema = schemas[SCHEMA_NAME_LIST_MICRO]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, vec);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_LIST_MICRO]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

std::vector<char> AvroHelper::encodeMicroList(std::vector<datamodel::Micro> &vec) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_MICRO_LIST) == 0 || (avro_schema = schemas[SCHEMA_NAME_MICRO_LIST]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, vec);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    schemas[SCHEMA_NAME_MICRO_LIST]->framing_write(out);
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

//from libserdes example
static int json2avro(Serdes::Schema *schema, const std::string &json,
                     avro::GenericDatum **datump) {
    avro::ValidSchema *avro_schema = schema->object();
    std::istringstream iss(json);
    std::auto_ptr<avro::InputStream> json_is = avro::istreamInputStream(iss);
    avro::DecoderPtr json_decoder = avro::jsonDecoder(*avro_schema);
    avro::GenericDatum *datum = new avro::GenericDatum(*avro_schema);

    try {
        json_decoder->init(*json_is);
        avro::decode(*json_decoder, *datum);

    } catch (const avro::Exception &e) {
        KERROR("JSON to Avro transformation failed: " << e.what());
        return -1;
    }

    *datump = datum;
    return 0;
}

std::vector<char> AvroHelper::encodeSyncMsg(datamodel::SyncMsg &syncMsg) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_SYNCMSG) == 0 || (avro_schema = schemas[SCHEMA_NAME_SYNCMSG]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }

    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, syncMsg);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    /* Write framing */
    //	schemas[TOPIC_SYNCMSG]->framing_write(out);
    schemas[SCHEMA_NAME_SYNCMSG]->framing_write(out);
    /* Write binary encoded Avro to output std::vector */
    out.insert(out.end(), v->begin(), v->end());

    return out;
}

datamodel::SyncMsg AvroHelper::decodeSyncMsg(const std::string &pfx, const void *buf, size_t len) {
    //DS_AVRO_DBG("decodeSyncMsg");
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    //DS_AVRO_DBG("serdes->deserialize");
    serdes->deserialize(&schema, &d, buf, len, errstr);

    datamodel::SyncMsg syncMsg;

    avro::GenericRecord r = d->value<avro::GenericRecord>();
    syncMsg.Time = r.field("Time").value<int64_t>();
    syncMsg.Epoch = r.field("Epoch").value<int>();
    // avro::GenericEnum ge = r.field("Action").value<avro::GenericEnum>();
    // syncMsg.Action = static_cast<datamodel::ActionType>(ge.value());
    syncMsg.Action = r.field("Action").value<std::string>();
    syncMsg.Sender = r.field("Sender").value<std::string>();

    avro::GenericMap messages = r.field("Messages").value<avro::GenericMap>();

    for (std::pair<std::string, avro::GenericDatum> entry : messages.value()) {
        syncMsg.Messages[entry.first] = entry.second.value<int64_t>();
    }
    // syncMsg.Messages = messages.value();  <-- does this also work ?

    if (d)
        delete d;

    //DS_AVRO_DBG("decodeSyncMsg done");

    return syncMsg;
}

// std::vector<char> AvroHelper::encodeRadioMsg11p(datamodel::RadioMsg11p &msg) {
//     avro::ValidSchema *avro_schema;
//     if (schemas.count(SCHEMA_NAME_RADIOMSG11P) == 0 || (avro_schema = schemas[SCHEMA_NAME_RADIOMSG11P]->object()) == 0) {
//         KERROR("schema is not initialized");
//         exit(1);
//     }
//     std::vector<char> out;
//     std::string errstr;

//     avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
//     std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
//     e->init(*bin_os.get());
//     avro::encode(*e, msg);
//     e->flush();

//     /* Extract written bytes. */
//     boost::shared_ptr<std::vector<uint8_t>> v;
//     v = avro::snapshot(*bin_os.get());
//     /* Write framing */
//     schemas[SCHEMA_NAME_RADIOMSG11P]->framing_write(out);
//     /* Write binary encoded Avro to output std::vector */
//     out.insert(out.end(), v->begin(), v->end());

//     return out;
// }
// datamodel::RadioMsg11p AvroHelper::decodeRadioMsg11p(const std::string &pfx, const void *buf, size_t len) {
//     DS_AVRO_DBG("decodeRadioMsg11p");
//     std::string out;
//     avro::GenericDatum *d = NULL;
//     Serdes::Schema *schema = NULL;
//     std::string errstr;

//     DS_AVRO_DBG("serdes->deserialize");
//     serdes->deserialize(&schema, &d, buf, len, errstr);

//     datamodel::RadioMsg11p msg;
//     avro::GenericRecord r = d->value<avro::GenericRecord>();
//     msg.Sender = r.field("Sender").value<std::string>();
//     msg.Params = r.field("Params").value<std::string>();
//     msg.Type = r.field("Type").value<std::string>();
//     msg.Payload = r.field("Payload").value<std::string>();

//     if (d)
//         delete d;

//     DS_AVRO_DBG("decodeRadioMsg11p done");

//     return msg;
// }

std::vector<char> AvroHelper::encodeRadioMsg(datamodel::RadioMsg &msg) {
    avro::ValidSchema *avro_schema;
    if (schemas.count(SCHEMA_NAME_RADIOMSG) == 0 || (avro_schema = schemas[SCHEMA_NAME_RADIOMSG]->object()) == 0) {
        KERROR("schema is not initialized");
        exit(1);
    }
    std::vector<char> out;
    std::string errstr;

    avro::EncoderPtr e = avro::validatingEncoder(*avro_schema, avro::binaryEncoder());
    std::auto_ptr<avro::OutputStream> bin_os = avro::memoryOutputStream();
    e->init(*bin_os.get());
    avro::encode(*e, msg);
    e->flush();

    /* Extract written bytes. */
    boost::shared_ptr<std::vector<uint8_t>> v;
    v = avro::snapshot(*bin_os.get());
    /* Write framing */
    schemas[SCHEMA_NAME_RADIOMSG]->framing_write(out);
    /* Write binary encoded Avro to output std::vector */
    out.insert(out.end(), v->begin(), v->end());

    return out;
}
datamodel::RadioMsg AvroHelper::decodeRadioMsg(const std::string &pfx, const void *buf, size_t len) {
    DS_AVRO_DBG("decodeRadioMsg");
    std::string out;
    avro::GenericDatum *d = NULL;
    Serdes::Schema *schema = NULL;
    std::string errstr;

    DS_AVRO_DBG("serdes->deserialize");
    serdes->deserialize(&schema, &d, buf, len, errstr);

    datamodel::RadioMsg msg;
    avro::GenericRecord r = d->value<avro::GenericRecord>();
    msg.sender = r.field("sender").value<std::string>();
    DS_AVRO_DBG("msg.sender "<<msg.sender);
    msg.sendTime = r.field("sendTime").value<int64_t>();
    DS_AVRO_DBG("msg.sendTime "<<msg.sendTime);
    msg.receiver = r.field("receiver").value<std::string>();
    DS_AVRO_DBG("msg.receiver "<<msg.receiver);
    msg.receiveTime = r.field("receiveTime").value<int64_t>();
    DS_AVRO_DBG("msg.receiveTime "<<msg.receiveTime);
    avro::GenericMap::Value vp = r.field("data").value<avro::GenericMap>().value();
    for (std::pair<std::string, avro::GenericDatum> p : vp) {
        msg.data[p.first] = p.second.value<std::string>();
    }

    if (d)
        delete d;

    DS_AVRO_DBG("decodeRadioMsg done");

    return msg;
}
