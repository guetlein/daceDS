#!/usr/bin/env python3
#
# Copyright 2021 Moritz Gütlein
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
#   http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
# 

from confluent_kafka import avro
import csv
from confluent_kafka.avro.serializer import SerializerError
import json
from confluent_kafka import KafkaError
from AvroProducerConsumer import AvroConsumerStrKey
from confluent_kafka import Consumer
import threading
 

class KafkaConsumer():
    
    def __init__(self, broker, registry, topics, consumerID,avro=True,cb=0):    
        self.consumerID = consumerID
        self.broker = broker
        self.registry = registry
        self.topics = topics
        self.cb = cb
        self.avro = avro

        if(self.avro):
            self.consumerConf = {'bootstrap.servers': broker,'schema.registry.url': registry,'group.id': consumerID,'client.id': consumerID,'auto.offset.reset' : "earliest", 'max.poll.interval.ms' : 3000000}
            self.consumer = AvroConsumerStrKey(self.consumerConf)
        else:
            self.consumerConf = {'bootstrap.servers': broker,'group.id': consumerID,'client.id': consumerID,'auto.offset.reset' : "earliest", 'max.poll.interval.ms' : 3000000}
            self.consumer = Consumer(self.consumerConf)
        self.running = True
        
        self._subscribe(topics)        
        self.th = None

    def translate_to_topic_notation(edge_id):
        edge_id_copy = edge_id
        conform_edge_id = ""
        while edge_id_copy.find("---") != -1:
            conform_edge_id = conform_edge_id + edge_id_copy[:edge_id_copy.find("---")] + "---" + format(ord("-"), "x") + "---" + format(ord("-"), "x") + "---" + format(ord("-"), "x")
            edge_id_copy = edge_id_copy[edge_id_copy.find("---") + 3:]
    
        conform_edge_id = conform_edge_id + edge_id_copy
    
        return conform_edge_id.translate({ord(sign): "---" + format(ord(sign), "x") for sign in conform_edge_id if sign.isalnum() is False and sign != "-"})


    def run(self):
        self._subscribe(self.topics)
        while(self.running):
            try:
                inMsg = self.consumer.poll(10)
                if inMsg != None and not(inMsg.error()):
                    if(self.cb):
                        self.cb(inMsg)
                    else:
                        pass
                        # print("> > > received msg! topic =", inMsg.topic(), ", time =", inMsg.timestamp()[1],", key =", inMsg.key(), ", value =", inMsg.value(), flush=True)

            except SerializerError as e:
                print("Message deserialization failed for {}: {}".format(inMsg, e), flush=True)  
                
    def listenInBG(self):
       self.th = threading.Thread(target=self.run)
       self.th.start()
           

    # def listen(self):
    #     self.consumer.subscribe(self.topics)
    #     while(self.running):
    #         try:
    #             inMsg = self.consumer.poll(10)
    #             if inMsg != None and not(inMsg.error()):
    #                 if(self.cb):
    #                     self.cb(inMsg)
    #                 else:
    #                     print("> > > received msg! topic =", inMsg.topic(), ", time =", inMsg.timestamp()[1],", key =", inMsg.key(), ", value =", inMsg.value(), flush=True)

    #         except SerializerError as e:
    #             print("Message deserialization failed for {}: {}".format(inMsg, e), flush=True)



    def poll(self,timeout):
        try:
            msg = self.consumer.poll(timeout)
            if msg is None:
                return None
            if(msg.error()):
                
                print("got an error, restarting consumers")
                
                self.consumer.close()
                if(self.avro):
                    self.consumer = AvroConsumerStrKey(self.consumerConf)
                else:
                    self.consumer = Consumer(self.consumerConf)
                    
                self._subscribe(self.topics)   
                return None
            return msg
        except:
            print("got an exception, restarting consumers")
            self.consumer.close()
            if(self.avro):
                self.consumer = AvroConsumerStrKey(self.consumerConf)
            else:
                self.consumer = Consumer(self.consumerConf)
                
            self._subscribe(self.topics)   
            return None

    def _subscribe(self,topics):
        self.consumer.subscribe(topics)
        print("subscribed to:")
        for t in topics:        
            print(t)

    def stop(self):
        self.running = False
        self.running = False
        self.consumer.close()

                



