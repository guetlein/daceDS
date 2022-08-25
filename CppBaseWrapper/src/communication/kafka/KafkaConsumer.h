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

#include <librdkafka/rdkafkacpp.h>
#include <signal.h>

#include <csignal>
#include <iostream>
#include <stdexcept>
#include <thread>

#include "communication/Consumer.h"
#include "communication/ConsumedMessage.h"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/log.h"

using std::cout;
using std::endl;
using std::exception;
using std::string;

/**
 * Based on the cppkafka examples
 * */
namespace daceDS {
// class ExampleConsumeCb : public RdKafka::ConsumeCb {
//    public:
//     void consume_cb(RdKafka::Message &msg, void *opaque) {
//         //msg_consume(&msg, opaque);
//     }
// };
class KafkaConsumer : public Consumer {
    RdKafka::Conf *conf = 0;
    RdKafka::Conf *tconf = 0;
    bool exit_eof = false;
    volatile sig_atomic_t run = 1;
    std::vector<std::string> _topics;
    //ExampleDeliveryReportCb ex_dr_cb;

   public:
    // RdKafka::Consumer *consumer = 0;
    // RdKafka::Queue *queue = 0;
    RdKafka::KafkaConsumer *consumer = 0;
    std::thread t1;
    KafkaConsumer(std::shared_ptr<MessageHandler> hdl, bool counting = false) : Consumer(hdl, counting){};
    virtual ~KafkaConsumer() { delete consumer; };

    void msg_consume(RdKafka::Message *message, void *opaque);
    void subscribe(std::string broker, std::vector<std::string> &topic, std::string group_id, std::string loglevel = "");
    virtual void listen();
    virtual void stop();
};
}  // namespace daceDS
