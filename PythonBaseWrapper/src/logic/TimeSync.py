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

import sys
sys.path.insert(1, '../communication/')
from KafkaProducer import KafkaProducer
from KafkaConsumer import KafkaConsumer

 
syncMsgSchemaPath = "/daceDS/Datamodel/AvroDefinitions/SyncMsg.avsc"
import time

import re


class TimeSync():
    
    def __init__(self, broker, registry, topic, myID, initNoParticipants=2, logging=True, timeoutHandler=None):    
        self.myID = myID
        self.topic = topic
        self.joinStr = {
        'Sender' : self.myID,
        'Action' : 'join',
        'Time' : 0,
        'Epoch' : 0,
        'Messages' : {}
        }

        self.logging = logging
        self.broker = broker
        self.registry = registry
        
        # self.producer = KafkaProducerKafkian(broker, registry, myID, useAvro=True, schemaPath=syncMsgSchemaPath)
        # self.consumer = KafkaConsumerKafkian(broker, registry, [self.topic], myID)
        self.producer = KafkaProducer(broker, registry, myID, useAvro=True, schemaPath=syncMsgSchemaPath)
        self.consumer = KafkaConsumer(broker, registry, [self.topic], myID)

        self.lbts = 0
        self.currentLocalTime = 0
        self.expectedReceiveCount = {}
        self.expectedPattern = []
        self.participants = {}
        self.initNoParticipants = initNoParticipants

        #need to keep track of relevant messages, better do that inside the avroconsumer/producer class
        self.actualReceivedMsgCount = {}
        self.actualSentMsgCount = {}

        self.lastAnnouncedSentMessages = {}
        self.timeoutHandler = timeoutHandler
        
        self.receiveCountErrors = {}
        
        self.inferedTopics = []
        
        self.max_error_count = 100


    def refreshLBTS(self):
        if len(self.participants) > 0:
            self.lbts = min(int(x) for x in self.participants.values())


    def processSyncMsg(self, msg):
        sender = msg['Sender']
        if (msg['Action'] == 'join' and (sender not in self.participants)):
            self.participants[sender] = msg['Time']
        elif (msg['Action'] == 'join'):
             print(sender,"has already joined, ignoring request")
        elif (msg['Action'] == 'leave' and (sender in self.participants)):
            self.participants.pop(sender)
        elif (msg['Action'] == 'request' and (sender in self.participants)):
            self.participants[sender] = msg['Time']
            
            for topic in msg['Messages']:
                if(self.logging == True):
                    print("a message was announced on ",topic, flush=True)
                if topic in self.expectedReceiveCount:
                    self.expectedReceiveCount[topic] += msg['Messages'][topic]
                    continue
                
                for pattern in self.expectedPattern:
                    if(pattern.match(topic)):
                        if(self.logging == True):
                            print("found new topic that matches my pattern: " + topic, flush=True)
                        self.expectedReceiveCount[topic] = msg['Messages'][topic]
                        self.inferedTopics.append(topic)
                        self.refreshLBTS()
                        return
                    else:
                        if(self.logging == True):
                            print(topic,"does not match pattern: " + pattern.pattern, flush=True)
                         
                #neither pattern nor topic list is matching                    
                if(self.logging == True):
                    print("ignoring " + topic + ("is not in my list of interest"), flush=True)
                    print("currently in interest list:")
                    for t in self.expectedReceiveCount:
                        print(t)
        elif sender not in self.participants:
            print('participants',sender,'is no official member',self.participants)
            
        else:
            print('unknown request: ', msg) 
            print('participants: ', self.participants)
            print('lbts: ', self.lbts, flush=True)
            
        self.refreshLBTS()


    def getSentMsgDiff(self):
        diff = {}
        copyOfSentMessages = self.actualSentMsgCount.copy()
        for topic in copyOfSentMessages:
            if topic in self.lastAnnouncedSentMessages:
                d = copyOfSentMessages[topic] - self.lastAnnouncedSentMessages[topic]
                if d > 0 :
                    diff[topic] = d
            else:
                diff[topic] = copyOfSentMessages[topic]
        
        self.lastAnnouncedSentMessages = copyOfSentMessages
        return diff


    def timeOK(self, t):
        if t <= self.lbts:
            if(self.logging == True):
                print("time OK? YES!",t, ', actual=', self.lbts, flush=True)
            return True
        else:
            if(self.logging == True):
                print("time OK? NO!",t, ', actual=', self.lbts, flush=True)
            return False
    
    def msgCountOK(self):
        # print("msgCountOK(self)", flush=True)
        for topic, number in self.expectedReceiveCount.items():
            if number == 0 :                    
                continue
            if topic not in self.actualReceivedMsgCount:
                if(self.logging == True):
                    print("never received a message on ",topic, "supposed to have", number, flush=True)
                    
                if topic not in self.receiveCountErrors:
                    self.receiveCountErrors[topic]=1
                else:
                    self.receiveCountErrors[topic] = self.receiveCountErrors[topic]+1
                    
                if ((self.timeoutHandler is not None) and (self.receiveCountErrors[topic] % self.max_error_count == 0)):
                    print("error count is",self.receiveCountErrors[topic],", calling timeouthandler", flush=True)
                    self.timeoutHandler()
                    print("timeouthandler returned", flush=True)
                    
                return False
            
            if self.actualReceivedMsgCount[topic] < number:
                if(self.logging == True):
                    print("msgCount OK? NO!",self.actualReceivedMsgCount, '/', self.expectedReceiveCount, flush=True)
                return False
        
        if(self.logging == True):
            print("msgCount OK? YES!",self.actualReceivedMsgCount, '/', self.expectedReceiveCount, flush=True)
        return True


#############################################
#####         public methods
############################################    

    def addExpectedTopic(self, topic):
        if topic not in self.expectedReceiveCount:
            self.expectedReceiveCount[topic] = 0
        else:
            print("topic ", topic, " was already marked as expected");

    def addExpectedPattern(self, pattern):
        self.expectedPattern.append(re.compile(pattern))
                                        
    def joinTiming(self):  
        errorCount = 0
        self.producer.produce(topic=self.topic, value=self.joinStr)
        while(len(self.participants) < self.initNoParticipants):
            print("waiting for others to join: ", len(self.participants),'/', self.initNoParticipants, flush=True)
            msg = self.consumer.poll(.1)
            if(msg != None):
                print("topic",msg.topic())
                print("value",msg.value())
                self.processSyncMsg(msg.value())
            errorCount+=1
            
            # if (errorCount%103==0):
            #     self.producer.produce(topic=self.topic, value=self.joinStr)
            if (errorCount%5==0 and len(self.participants) == 0):
                self.consumer.stop()
                time.sleep(1)
                self.consumer = KafkaConsumerKafkian(self.broker, self.registry, [self.topic], self.myID+str(errorCount))
            
                
        print("all expected participants have joined: ", len(self.participants),'/', self.initNoParticipants, flush=True)
    
    def timeAdvance(self, timestep):
        t=self.currentLocalTime + timestep
        
        syncmsg = {
        'Sender' : self.myID,
        'Action' : 'request',
        'Epoch' : 0,
        'Messages' : self.getSentMsgDiff(),
        'Time' : t
        }
        self.producer.produce(topic=self.topic, value=syncmsg)
        if(self.logging == True):
            print("< < < sent out time msg", syncmsg, flush=True)
                
                
        #wait until others are ready
        while True:
            if (self.timeOK(t)):
                break
            try:
                if(self.logging == True):
                    print(".", end='',flush=True)
                inMsg = self.consumer.poll(.1)
                if inMsg == None:
                    continue
                if(self.logging == True):
                    print("> > > received in ", self.topic, inMsg.value(), flush=True)
                self.processSyncMsg(inMsg.value())
            except Exception as e:
                #self.consumer = KafkaConsumer(self.broker, self.registry, [self.topic], self.myID)
                print("timeAdvanceException",e, flush=True)
                
        #wait until received all announced messages
        while True:
            # print("self.msgCountOK()", flush=True)
            if self.msgCountOK():
                break
            time.sleep(.1)
                
        self.currentLocalTime = t
        # print("currentLocalTime is",t, flush=True)


    def leaveTiming(self):
        msg = {}
        msg['Sender'] = self.myID
        msg['Action'] = 'leave'
        msg['Time'] = -1
        msg['Epoch'] = 0
        msg['Messages'] =  {}
        self.producer.produce(topic=self.topic, value=msg)


    def notifiyAboutSentMessage(self, topic, count=1):
        if topic not in self.actualSentMsgCount:
            self.actualSentMsgCount[topic] = count
        else:
            self.actualSentMsgCount[topic] += count


    def notifiyAboutReceivedMessage(self, topic, count=1):
        if topic not in self.actualReceivedMsgCount:
            self.actualReceivedMsgCount[topic] = count
        else:
            self.actualReceivedMsgCount[topic] += count
        print("received",self.actualReceivedMsgCount[topic],"messages in",topic)


