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
#include "OrchestrationHandler.h"
using namespace daceDS;

bool OrchestrationHandler::handle(ConsumedMessage* msg) {
    //decode metadata for orchestration channel
    TopicMetadata meta = AvroHelper::getInstance()->getTopicMetadata(msg->topic);
    OrchestrationTopicMetadata ometa = AvroHelper::getInstance()->getOrchestrationTopicMetadata(meta);

    KERROR("orch msg on " << msg->topic);
    if (ometa.command == Command::CONTROL) {
        datamodel::CtrlMsg ctrl = AvroHelper::getInstance()->decodeCtrlMsg("payload", msg->payload, msg->len);
        api->ctrl(ometa, ctrl);
    } else if (ometa.command == Command::STATUS) {
        datamodel::StatusMsg state = AvroHelper::getInstance()->decodeStatusMsg("payload", msg->payload, msg->len);
        api->status(ometa, state);
    } else {
        KERROR("no valid command on topic=" << msg->topic);
        KERROR("no valid command on topic=" << msg->topic);
        KERROR("no valid command on topic=" << msg->topic);
        return false;
    }

    return true;
}
