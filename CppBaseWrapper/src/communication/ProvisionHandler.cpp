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
#include "ProvisionHandler.h"
using namespace daceDS;

bool ProvisionHandler::handle(ConsumedMessage* msg) {

    //decode metadata for provision channel
    TopicMetadata meta = AvroHelper::getInstance()->getTopicMetadata(msg->topic);
    ProvisionTopicMetadata pmeta = AvroHelper::getInstance()->getProvisionTopicMetadata(meta);
    DS_BASE_DBG("Got msg in KafkaProvisionConsumer: " << pmeta.meta.channelSpecific << " on topic " << msg->topic);

    //todo: this is layer independent
    //todo: topicResult -> resourceFile
    //parse different packages depending on topic
    if (pmeta.meta.channelSpecific == "scenario") {
        DS_BASE_DBG("Got scenario description");
        datamodel::Scenario r = AvroHelper::getInstance()->decodeScenario("payload", msg->payload, msg->len);
        DS_BASE_DBG("Scenario is decoded");
        api->scenario(pmeta, r);
        return true;
    }

    else if (pmeta.meta.channelSpecific == "resource") {
        DS_BASE_DBG("Received resourceFile");
        datamodel::ResourceFile r = AvroHelper::getInstance()->decodeResourceFile("payload", msg->payload, msg->len);
        DS_BASE_DBG("ResourceFile is decoded");
        api->resource(pmeta, r);
        return true;
    }
    

    return false;
}
