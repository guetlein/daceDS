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
#include "TimeSyncHandler.h"
using namespace daceDS;

bool TimeSyncHandler::handle(ConsumedMessage* msg) {
    DS_SYNC_DBG("got msg in KafkaTimeSyncConsumer on topic=" << msg->topic);
    datamodel::SyncMsg sync = AvroHelper::getInstance()->decodeSyncMsg("payload", msg->payload, msg->len);
    // if(sync.Action==datamodel::ActionType::REQUEST){
    if (sync.Action == "request") {
        timeSync->handleTopicSyncMsgRequest(sync);
    }
    // if(sync.Action==datamodel::ActionType::JOIN){
    else if (sync.Action == "join") {
        timeSync->handleTopicSyncMsgJoin(sync);
    }
    // if(sync.Action==datamodel::ActionType::LEAVE){
    else if (sync.Action == "leave") {
        timeSync->handleTopicSyncMsgLeave(sync);
    } else {
        return false;
    }

    return true;
}
