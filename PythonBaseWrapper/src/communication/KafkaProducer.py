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

from AvroProducerConsumer import AvroProducerStrKey
from confluent_kafka import Producer
from struct import *

class KafkaProducer():
    
    def __init__(self, broker, registry, producerID,useAvro=True,schema="",schemaPath=""):    
        self.producerID = producerID
        self.broker = broker
        self.registry = registry

        self.producerConf = {'bootstrap.servers': broker,'schema.registry.url': registry, 'acks' : '1', 'enable.idempotence' : 'false', 
                             'linger.ms' : '0'}
        if(useAvro):
            if len(schema)>0:
                self.producer = AvroProducerStrKey(self.producerConf, default_value_schema=schema)
            elif len(schemaPath)>0:
                schema = avro.load(schemaPath)
                self.producer = AvroProducerStrKey(self.producerConf, default_value_schema=schema)
        else:

            self.producerConf = {'bootstrap.servers': broker, 'acks' : '1', 'enable.idempotence' : 'false', 'linger.ms' : '0'}
            self.producer = Producer(self.producerConf)

    getbinary = lambda x, n: format(x, 'b').zfill(n)

    def produce(self, topic, value, key="empty",timestamp=-1):
        # print("< < < preparing headers", flush=True)
        headers = {}
        headers["sender"] = self.producerID
        headers["time"] = pack('!q',timestamp)
        headers["epoch"] =  pack('!i',0)
        # print("< < < publishing to", topic, flush=True)
        self.producer.produce(topic=topic, value=value, key=key, headers=headers)
        # self.producer.produce(topic=topic, value=value, key=key)
        self.producer.flush()



