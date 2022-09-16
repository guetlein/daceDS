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
#include "KafkaProducer.h"
using namespace daceDS;

void KafkaProducer::init(std::string brokers, std::vector<std::string> topics, std::string prodid, std::string loglevel) {

    id = Config::getInstance()->getScenarioID()+"_"+Config::getInstance()->getSimulatorID()+"_"+prodid;
    KDEBUG("Config metadata.broker.list=" << brokers);

    conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    // if (loglevel.size() > 0)
    //     conf->set("debug", loglevel);

    std::string errstr = "";
    //conf->set("debug", "all", errstr);
    conf->set("metadata.broker.list", brokers, errstr);
    KDBGCB(errstr);
    // conf->set("enable.idempotence", "true", errstr);  //-> exactly once
    KDBGCB(errstr);
    conf->set("client.id", id, errstr);
    KDBGCB(errstr);
    conf->set("linger.ms", "0", errstr); //latency
    KDBGCB(errstr);
    conf->set("acks", "1", errstr); //acks` must be set to `all` when `enable.idempotence` is true
    KDBGCB(errstr);
    conf->set("message.send.max.retries", "10000000", errstr);
    KDBGCB(errstr);
    conf->set("max.in.flight.requests.per.connection", "1", errstr); //important for correct ordering
    KDBGCB(errstr);
    conf->set("enable.auto.commit", "true", errstr);

    KDBGCB("Created config " << errstr);
    producer = RdKafka::Producer::create(conf, errstr);
    KDBGCB("Created producer with id="<<id<<" @" << producer);
    KDBGCB("Created producer err=" << errstr);

    for (auto topic : topics) {
        try {
            createTopic(topic);
        } catch (std::exception& e) {
            KERROR("create topic failed. maybe somebody else created it?");
        };
    }
}

bool KafkaProducer::createTopic(std::string topic) {
    KDBGCB("producer->name()" << producer->name());

    KDBGCB("create topic " << topic);
    std::string errstr = "";
    RdKafka::Conf* tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    KDBGCB("producer=" << producer);
    KDBGCB("tconf=" << tconf);
    KDBGCB("errstr=" << errstr);
    RdKafka::Topic* t = RdKafka::Topic::create(producer, topic, tconf, errstr);

    if (!t) {
        KERROR("Failed to create topic: " << errstr);
        exit(1);
    }
    KDBGCB("Created topic! " << errstr);
    topicMap[topic] = t;
    return true;
}

bool KafkaProducer::publish(std::string topic, std::string payload) {
    return publish(topic, "key", payload, -1);
}
bool KafkaProducer::publish(std::string topic, std::string payload, int64_t time) {
    return publish(topic, "key", payload, time);
}

bool KafkaProducer::publish(std::string topic, std::vector<char>& payload, bool repeatOnError) {
    return publish(topic, "key", payload, -1, repeatOnError);
}

bool KafkaProducer::publish(std::string topic, std::vector<char>& payload) {
    return publish(topic, "key", payload, -1);
}

bool KafkaProducer::publish(std::string topic, std::vector<char>& payload, int64_t time) {
    return publish(topic, "key", payload, time);
}

bool KafkaProducer::publish(std::string topic, std::string key, std::vector<char>& payload, int64_t time, bool repeatOnError) {
    if (producer == NULL) {
        KERROR("producer==NULL");
        KERROR("topic="<<topic);
        exit(1);
    }
    KDBGCB("publishing to " << topic);

    bool success = false;
    int tries = 1;
    uint32_t epoch = 0;


    //this is big endian encoding
    uint8_t timebytes[8];
    timebytes[0] = (uint8_t)((time >> 56) & 0xFF) ;
    timebytes[1] = (uint8_t)((time >> 48) & 0xFF) ;
    timebytes[2] = (uint8_t)((time >> 40) & 0XFF);
    timebytes[3] = (uint8_t)((time >> 32)& 0XFF);
    timebytes[4] = (uint8_t)((time >> 24) & 0xFF) ;
    timebytes[5] = (uint8_t)((time >> 16) & 0xFF) ;
    timebytes[6] = (uint8_t)((time >> 8) & 0XFF);
    timebytes[7] = (uint8_t)((time & 0XFF));

    KDBGCB("encoded time=" <<  time << " to " << std::hex << (int) timebytes[0] << (int) timebytes[1]<< (int) timebytes[2] << (int) timebytes[3]<< (int) timebytes[4] << (int) timebytes[5]<< (int) timebytes[6] << (int) timebytes[7] << std::dec);


    uint8_t epochbytes[4];
    epochbytes[0] = (uint8_t)((epoch >> 24) & 0xFF) ;
    epochbytes[1] = (uint8_t)((epoch >> 16) & 0xFF) ;
    epochbytes[2] = (uint8_t)((epoch >> 8) & 0XFF);
    epochbytes[3] = (uint8_t)((epoch & 0XFF));

    KDBGCB("encoded epoch=" <<  epoch << " to " << std::hex << (int) epochbytes[0] << (int) epochbytes[1]<< (int) epochbytes[2] << (int) epochbytes[3] << std::dec);

    do {
        try {
            RdKafka::Topic* t;
            if (topicMap.count(topic) == 0) {
                try {
                    createTopic(topic);
                } catch (std::exception& e) {
                    KERROR("create topic failed. maybe somebody else created it?");
                };
            }

            t = topicMap[topic];
            //todo: fixed to partition=0
            RdKafka::ErrorCode resp;
            RdKafka::Headers *headers = RdKafka::Headers::create();
            headers->add("sender", id);
            headers->add("time", ( const void *)timebytes, 8 );
            headers->add("epoch",( const void *)epochbytes, 4 );
            // headers->add("time", ( const void *)&time, 8 );
            // headers->add("epoch",( const void *)&epoch, 4 );
            
            resp = producer->produce(topic, 0,  RdKafka::Producer::RK_MSG_COPY,
                                                reinterpret_cast<char*>(payload.data()), payload.size(),
                                                const_cast<char*>(key.c_str()), key.size(),
                                                NULL, headers, NULL);

            if(resp == RdKafka::ERR_NO_ERROR){
                success = true;
                KDBGCB("published to topic=" << topic);

                Producer::publish(topic, key, payload, time);
                return true;
            }

            if (resp == RdKafka::ERR__QUEUE_FULL) { 
                // If the internal queue is full, wait for messages to be delivered and then retry. 
                KDBGCB("internal queue is full, waiting and retrying");
                producer->poll(5000/*block for max 1000ms*/);
            } else {
                KERROR(RdKafka::err2str(resp));
                return false;
            }
        } catch (std::exception& e) {
            KDBGCB(tries << ". try: Error in publish(): " << e.what());
            KDBGCB("topic " << topic << ", time " << time);
            usleep(10000 * tries++);

            if (!repeatOnError) {
                return false;
            }
        }
    } while (!success);

    Producer::publish(topic, key, payload, time, repeatOnError);

    return true;
}

bool KafkaProducer::publish(std::string topic, std::string key, std::string payload, int64_t time, bool repeatOnError) {
    if (producer == NULL) {
        KERROR("producer==NULL");
        KERROR("topic="<<topic);
        exit(1);
    }
    KDBGCB("Publishing " << payload << " to " << topic);
    bool success = false;
    int tries = 1;
    int epoch = 0;

    //this is big endian encoding
    uint8_t timebytes[8];
    timebytes[0] = (uint8_t)((time >> 56) & 0xFF) ;
    timebytes[1] = (uint8_t)((time >> 48) & 0xFF) ;
    timebytes[2] = (uint8_t)((time >> 40) & 0XFF);
    timebytes[3] = (uint8_t)((time >> 32)& 0XFF);
    timebytes[4] = (uint8_t)((time >> 24) & 0xFF) ;
    timebytes[5] = (uint8_t)((time >> 16) & 0xFF) ;
    timebytes[6] = (uint8_t)((time >> 8) & 0XFF);
    timebytes[7] = (uint8_t)((time & 0XFF));

    uint8_t epochbytes[4];
    epochbytes[0] = (uint8_t)((epoch >> 24) & 0xFF) ;
    epochbytes[1] = (uint8_t)((epoch >> 16) & 0xFF) ;
    epochbytes[2] = (uint8_t)((epoch >> 8) & 0XFF);
    epochbytes[3] = (uint8_t)((epoch & 0XFF));


    do {
        try {
            RdKafka::Topic* t;
            if (topicMap.count(topic) == 0) {
                try {
                    createTopic(topic);
                } catch (std::exception& e) {
                    KERROR("create topic failed. maybe somebody else created it?");
                };
            }
            t = topicMap[topic];
            //currently fixed to partition=0
            RdKafka::ErrorCode resp;
            RdKafka::Headers *headers = RdKafka::Headers::create();
            headers->add("sender", "sumo");
            headers->add("time", ( const void *)timebytes, 8 );
            headers->add("epoch",( const void *)epochbytes, 4 );

            resp = producer->produce(topic, 0, RdKafka::Producer::RK_MSG_COPY,
                                                    const_cast<char*>(payload.c_str()), payload.size(),
                                                    const_cast<char*>(key.c_str()), key.size(),
                                                    NULL, headers, NULL);
            
            if(resp == RdKafka::ERR_NO_ERROR){
                success = true;
                KDBGCB("published to topic=" << topic);
                Producer::publish(topic, key, payload, time, repeatOnError);
                return true;
            }
            if (resp == RdKafka::ERR__QUEUE_FULL) { 
                // If the internal queue is full, wait for messages to be delivered and then retry. 
                KERROR("queue is full");
                producer->poll(1000/*block for max 1000ms*/);
            } else {
                KERROR(RdKafka::err2str(resp));
                KERROR("failed to publish");
                return false;
            }
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            KERROR(tries << ". try: Error in publish()");

            KERROR("topic " << topic << ", time " << time);
            usleep(10000 * tries++);
            if (!repeatOnError) {
                return false;
            }
        }
    } while (!success);

    Producer::publish(topic, key, payload, time, repeatOnError);
    return true;
}
