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
#include "KafkaConsumer.h"
using namespace daceDS;
std::function<void()> on_signal;

void signal_handler(int) { on_signal(); }
void KafkaConsumer::subscribe(std::string broker,
                              std::vector<std::string> &topics,
                              std::string group_id, std::string loglevel) {

    group_id = Config::getInstance()->getScenarioID()+"_"+Config::getInstance()->getSimulatorID()+"_"+group_id;
    KDBGCB("Group id is " << group_id);
    KDBGCB("Brokers: " << broker);

    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    std::string errstr = "";
    tconf->set("auto.offset.reset", "earliest", errstr);
    tconf->set("topic.auto.offset.reset", "earliest", errstr);
    //tconf->set("enable.auto.commit", "true", errstr);
    tconf->set("request.required.acks", "1", errstr);

    conf->set("metadata.broker.list", broker, errstr);
    conf->set("default_topic_conf", tconf, errstr);
    conf->set("enable.partition.eof", "false", errstr);
    conf->set("group.id", group_id, errstr);
    conf->set("client.id", group_id, errstr);
    conf->set("acks", "1", errstr);
    conf->set("message.send.max.retries", "10000000", errstr);
    conf->set("max.in.flight.requests.per.connection", "1", errstr); //important for correct ordering
    conf->set("enable.auto.commit", "true", errstr);

    conf->set("topic.metadata.refresh.interval.ms", "1000", errstr);
    conf->set("metadata.max.age.ms", "1000", errstr);
    conf->set("allow.auto.create.topics", "true", errstr);
    conf->set("auto.offset.reset", "earliest", errstr);
    conf->set("max.partition.fetch.bytes", "10000000", errstr);
    conf->set("socket.blocking.max.ms", "6000", errstr);
    conf->set("fetch.min.bytes", "1", errstr); //latency
    conf->set("fetch.wait.max.ms", "1", errstr); //latency?
    //conf->set("debug", "all", errstr);

    // consumer = RdKafka::Consumer::create(conf, errstr);

    conf->set("default_topic_conf", tconf, errstr);
    KDBGCB("Created config " << errstr);

    consumer = RdKafka::KafkaConsumer::create(conf, errstr);

    if (!consumer) {
        KERROR("Failed to create consumer: " << errstr);
        exit(1);
    }

    KDBGCB("Created consumer " << consumer->name());
    //conf->set("dr_cb", &ex_dr_cb, errstr);
    //queue = RdKafka::Queue::create(consumer);

    KDBGCB("topic list contains number of entries: "<< topics.size());
    for (auto topicStr : topics) {
        KDBGCB("Subscribing to " << topicStr);
        //     RdKafka::Topic *topic = RdKafka::Topic::create(consumer,
        //                                                    topicStr,
        //                                                    tconf,
        //                                                    errstr);
        //     if (!topic) {
        //         KERROR("Failed to create topic: " << errstr);
        //         exit(1);
        //     }
        //     int32_t partition = 0;
        //     int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
        //     //RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset, queue);
        //     RdKafka::ErrorCode resp = consumer->subscribe(topic, partition, start_offset, queue);
        //     if (resp != RdKafka::ERR_NO_ERROR) {
        //         KERROR("Failed to start consumer: " << RdKafka::err2str(resp));
        //         exit(1);
        //     }
    }
    RdKafka::ErrorCode resp = consumer->subscribe(topics);
    _topics = topics;

    if (resp != RdKafka::ERR_NO_ERROR) {
        KERROR("Failed to start consumer: " << RdKafka::err2str(resp));
        exit(1);
    }

    Consumer::subscribe(broker, topics, group_id, loglevel);

    // on_signal = [&]() { KDEBUG("on_signal"); };
    // signal(SIGINT, signal_handler);
    // signal(SIGABRT, signal_handler);
    KDBGCB("Starting listener thread");
    t1 = std::thread(&KafkaConsumer::listen, this);
    //t1.detach();
}

void KafkaConsumer::msg_consume(RdKafka::Message *message, void *opaque) {
    switch (message->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;

        case RdKafka::ERR_NO_ERROR:
            /* Real message */
            KDBGCB("Received on topic="
                   << message->topic_name() << " @ time="
                   << message->timestamp().timestamp << " @ offset="
                   << message->offset());
            {
                try {
                    std::string key = "";
                    std::string sender = "";
                    int64_t st = 0;
                    
                    RdKafka::Headers* headers = message->headers();
                    if (headers) {
                        int64_t time = -1;
                        uint32_t epoch = -1;
                        std::vector<RdKafka::Headers::Header> hdrs = headers->get_all();
                        for (size_t i = 0; i < hdrs.size(); i++) {
                            const RdKafka::Headers::Header hdr = hdrs[i];

                            if (hdr.value() != NULL){
                                if(hdr.key() == "sender"){
                                    sender =  static_cast<const char *>(hdr.value());

                                    //is this our own message? -> ignore it
                                    if(sender.find(Config::getInstance()->getSimulatorID())  != std::string::npos ){
                                        KDBGCB("ignoring own message! sender=" << sender<< ", own id="<<Config::getInstance()->getSimulatorID());
                                        consumer->commitSync();
                                        return;
                                    } 
                                    else {
                                        KDBGCB("accepting message! sender=" << sender << ", own id="<<Config::getInstance()->getSimulatorID());
                                    }
                                }
                                else if(hdr.key() == "time"){
                                    // time = *reinterpret_cast<const int64_t*>(hdr.value());     
                                    const uint8_t *array = reinterpret_cast<const uint8_t *>(hdr.value());
                                    KDBGCB("received time: " << std::hex << (int) array[0] << (int) array[1]<< (int) array[2] << (int) array[3]<< (int) array[4] << (int) array[5]<< (int) array[6] << (int) array[7] << std::dec);

                                    time = 
                                        static_cast<int64_t>(array[7]) |
                                        static_cast<int64_t>(array[6]) << 8 |
                                        static_cast<int64_t>(array[5]) << 16 |
                                        static_cast<int64_t>(array[4]) << 24 |
                                        static_cast<int64_t>(array[3]) << 32 |
                                        static_cast<int64_t>(array[2]) << 40 |
                                        static_cast<int64_t>(array[1]) << 48 |
                                        static_cast<int64_t>(array[0]) << 56;          
                                    KDBGCB("decoded to " << time) ;
                                }
                                else if(hdr.key() == "epoch"){
                                    // epoch = *reinterpret_cast<const int*>(hdr.value()); 
                                    const uint8_t *array = reinterpret_cast<const uint8_t *>(hdr.value());                                    
                                    KDBGCB("received epoch: " << std::hex << (int) array[0] << (int) array[1]<< (int) array[2] << (int) array[3]<< std::dec);

                                    epoch = 
                                        static_cast<uint32_t>(array[3]) << 0 |
                                        static_cast<uint32_t>(array[2]) << 8 |
                                        static_cast<uint32_t>(array[1]) << 16 |
                                        static_cast<uint32_t>(array[0]) << 24;     
                                    KDBGCB("decoded to " << epoch) ;
                                }
                            }
                        }

                        if (message->key()) {
                            key = *message->key();
                        }
                


                        if(time < 0 || epoch < 0){
                            KDBGCB ("time or epoch of received msg below 0");
                            // exit(1);
                        } else {
                            st =  time*1000+epoch;
                        }
                        KDBGCB("HEADERS: sender=" << sender << ", time="<<time<<", epoch="<< epoch<<", st="<< st);
                        

                    } else { 
                        KERROR ("no headers in msg on topic "<< message->topic_name());
                    }

                    ConsumedMessage* cmsg = new ConsumedMessage(message->topic_name(),
                                                                key, 
                                                                (message->timestamp()).timestamp,
                                                                sender,
                                                                message->len(),
                                                                st,
                                                                message->payload());
                    bool p = process(cmsg);
                } 
                catch(std::exception &e)  {
                    KERROR("msg_consume ERROR: " << e.what());
                }
            }
            consumer->commitSync();

            break;

        case RdKafka::ERR__PARTITION_EOF:
            /* Last message */
            KDBGCB("Consumed EOF MSG");
            if (exit_eof) {
                run = 0;
            } else{
                KDBGCB("--> Currently ignoring EOF MSG");
            }
            break;

        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
        case RdKafka::ERR_UNKNOWN_TOPIC_OR_PART:
            KERROR(message->errstr());
            //run = 0;
            //consumer->subscribe(_topics);
            break;

        default:
            /* Errors */
            KERROR("Consume failed: " << std::to_string(message->err()) << " " << message->errstr());
            //consumer->subscribe(_topics);
            exit(1);
    }
}

void KafkaConsumer::listen() {
    KDBGCB("Starting to listen on topics: ");
    for (auto t : _topics) {
        KDEBUG(t);
    }

    usleep(1000 * 1000);
    while (run) {
        // RdKafka::Message *msg = consumer->consume(queue, 1000);
        try {
            RdKafka::Message *msg = consumer->consume(10);

            msg_consume(msg, NULL);
            delete msg;

        }  catch  (std::exception &e)  {
            KERROR(e.what());
        }

        //consumer->poll(1); //poll should never be used with RdKafka::KafkaConsumer
    }

    KDBGCB("Finished listening on topics: ");
    for (auto t : _topics) {
        KDEBUG(t);
    }
}

void KafkaConsumer::stop() {
    run = 0;
    t1.join();
    consumer->close();
}