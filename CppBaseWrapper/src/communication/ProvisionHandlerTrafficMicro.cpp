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
#include "ProvisionHandlerTrafficMicro.h"
using namespace daceDS;

bool ProvisionHandlerTrafficMicro::handle(ConsumedMessage* msg) {

    ProvisionHandler::handle(msg);
    
    //todo: this is layer specific
    if (pmeta.meta.channelSpecific.find("edge") != std::string::npos) {
        DS_BASE_DBG("Received traffic with key: " << msg->key);
        datamodel::Micro micro = AvroHelper::getInstance()->decodeMicro("payload", msg->payload, msg->len);
        DS_BASE_DBG("decoded micro");
        DS_BASE_DBG("micro.vehicleID="<<micro.vehicleID);
        api->Provision::vehicle->traffic(pmeta, micro);
    } else {
        DS_BASE_DBG(pmeta.meta.channelSpecific << " is not supported");
        return false;
    }

    return true;
}
