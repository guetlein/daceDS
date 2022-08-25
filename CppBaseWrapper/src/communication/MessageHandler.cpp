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

#include "MessageHandler.h"
using namespace daceDS;

std::mutex bufmutex;

bool MessageHandler::bufferConsumedMessage(ConsumedMessage* msg) {
    // std::unique_lock<std::mutex> lock(bufmutex);
    buffer.insert(msg);
    // lock.unlock();
    return true;
}

/*process everything that is older than time.epoch*/
void MessageHandler::processBuffer(int time, int epoch){

        // std::unique_lock<std::mutex> lock(bufmutex);
    	int64_t t = ((int64_t)1000)*time+epoch;

		KDBGCB("processing buffer for timeepoch " << t);

		for (std::set<ConsumedMessage*>::iterator it = buffer.begin(); it != buffer.end(); ){

            ConsumedMessage* msg = *it;
            
            //msg is old enough
            if(msg->st < t){
                //handle msg
		        KDBGCB("processing message on " << msg->topic << " at " << msg->st);
                handle(msg);
                //and remove it
                //free(msg->payload);
                //delete msg;
                
                delete msg;
                it = buffer.erase(it);
            } 
            //skip newer messages
            else {
                //todo: since the buffer is sorted, can we safely quit at this point?
                it++;
            }
        }	
        
    // lock.unlock();	
}

