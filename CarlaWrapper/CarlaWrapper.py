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


import logging
import sys
import time
import configparser

sys.path.append('../PythonBaseWrapper/src/logic')
sys.path.append('../PythonBaseWrapper/src/communication')
from TimeSync import TimeSync
from KafkaConsumer import KafkaConsumer
from KafkaProducer import KafkaProducer
from CarlaAPI import CarlaAPI

config = configparser.ConfigParser()
config.read('config.properties')
broker = config["general"]["kafkaBroker"]
registry = config["general"]["schemaRegistry"]
baseDir = "/daceDS/CarlaWrapper/tmp/"
submicroSchemaPath = "/daceDS/Datamodel/AvroDefinitions/SubMicro_flat.avsc"

import json


observable_roads = ["22_1", "22_-1", "23_1", "23_-1", "24_1",
"24_-1","4_1","4_-1","17_1","17_-1","18_1","18_-1","18_1","18_-1"]


class CarlaWrapper():
    def __init__(self, scenarioID, instanceID):
        # self.regexTopic = ["^provision\.simulation\."+scenarioID+"\.traffic\.submicro\.(\S)+\.edge\.(\S)+\.vehicles"]       
        self.regexTopic = ["^provision\.simulation\."+scenarioID+"\.traffic\.submicro\.lane\.(\S)+\.vehicles"]       
        self.topicPre = "provision.simulation."+scenarioID+".traffic.submicro."
        self.timeTopic = "orchestration.simulation."+scenarioID+".sync"
        self.scetopic = ['provision.simulation.'+scenarioID+'.scenario']
        self.statusTopic = 'orchestration.simulation.'+scenarioID+'.status'
        self.restopic = ['provision.simulation.'+scenarioID+'.resource']
        
        self.kid = scenarioID+"."+instanceID
        self.scenarioID = scenarioID
        self.instanceID = instanceID
        self.adoptList = []        
        
        self.producer = KafkaProducer(broker, registry,  self.kid, useAvro=True, schemaPath=submicroSchemaPath)
        self.statusProducer = KafkaProducer(broker, registry,  self.kid, useAvro=False)
        self.jsonProducer = KafkaProducer(broker, registry,  self.kid+"j", useAvro=False)
        self.consumer = None
        
        self.waitCounter = 0
        
        self.idMapping = {}
        self.roadMapFile = None
        self.sim = None
        
        self.demoMode = False
        
        
    def klog(self, msg):
        self.statusProducer.produce(self.statusTopic, self.instanceID+": "+msg)
        
    def publishObservation(self, topic, value):
        # print("trying to publish on topic="+topic+": "+value)
        self.jsonProducer.produce(topic, value, timestamp=self.timeSync.currentLocalTime)
        
    def prepare(self):
        
        print("preparing")
            
        self.klog("started")
        
        #################################
        ####### 1. get more info ########
        if(self.demoMode==False):
            print("fetching sce", flush=True)
            # self.sceConsumer = KafkaConsumerKafkian(kafkaHost, schemaHost, self.scetopic, self.kid+".sce")
            self.sceConsumer = KafkaConsumer(broker, registry, self.scetopic, self.kid+".sce")
            # sceConsumer.subscribe(scetopic)
            self.sce = None
            while self.sce == None:
                try:
                    msg = self.sceConsumer.poll(1)
                    if(msg is not None):
                        print("got return from poll", flush=True)
                        self.sce = msg.value()
                    else:
                        print("got null return from poll", flush=True)
                except Exception as e:    
                    print("Unexpected error:", sys.exc_info()[0], flush=True)
                    print("Unexpected error:", e, flush=True)
                    time.sleep(1)
                    print(".",end='', flush=True)
                    #sceConsumer = KafkaConsumer(kafkaHost, schemaHost, scetopic, kid+"_sce") #dies when topic is not existing, recreate each time
                print("polling for sce", flush=True)
            self.sceConsumer.stop()
            print("got sce", flush=True)
        else:
            print("DEMO MODE! using /daceDS/Scenario/Demo/Town01_Carla/Carla.sce")
            with open('/daceDS/Scenario/Demo/Town01_Carla/Carla.sce') as json_file:
                self.sce = json.load(json_file)
            print(self.sce)
            
        
        
        ##get resources
        for sim in self.sce['BBs']:
            if sim['ID'] != self.instanceID:
                continue
            self.sim = sim
            for resID,resType in sim["Resources"].items():
                if resType == "RoadMap":
                    self.roadMapFile = resID
                    
        if self.sim == None:
            print("no sim desc was found")
            sys.exit(1)
            
        if self.roadMapFile == None:
            print("no xodr was anounced")
            sys.exit(1)
            
            
        self.klog("initialized")
            
        
        self.klog("waiting for resources")
            
        #fetching resources
        print("fetching roadmap",self.roadMapFile, flush=True)
        if(self.demoMode==False):
            self.resConsumer = KafkaConsumer(broker, registry, self.restopic, self.kid+".res")
            while True:
                try:
                    res = self.resConsumer.poll(1)
                    if (res is None):
                        continue
                    resource = res.value()
                    print("received resource!",flush=True)
                    #print(resource,flush=True)
                    l = -1
                    if ((resource['File'] is None) and (resource['FileReference'] is None)):
                        continue
                    if (resource['File'] is not None) :
                        l = len(resource['File'])
                        
                    print(" id=",resource['ID']," type=",resource['Type'],"file=",l,"bytes",flush=True)
                    if((resource['ID'] ==  self.roadMapFile) and (resource["Type"] == "RoadMap")): #would take sumo maps aswell
                        print("saving to disk",flush=True)
                        if(resource['File']!=None ):
                            resBytes = resource['File']
            
                            newFile = open(baseDir+self.roadMapFile, "wb")
                            for byte in resBytes:
                                newFile.write(byte.to_bytes(1, byteorder='big'))
                
                            newFile.close()
                        
                            
                        
                        break
                except Exception as e:    
                    print("Unexpected error:", sys.exc_info()[0], flush=True)
                    print("Unexpected error:", e, flush=True)
                    time.sleep(1)
                    print(".",end='', flush=True)
            
            #resConsumer.stop()
        
            
        print("got roadmap", flush=True)
                            
        ###############################
        ####### 2. start timing #######
        self.timeSync = TimeSync(broker, registry, self.timeTopic, self.kid+".time", self.sce['Execution']['SyncedParticipants'], logging=False, timeoutHandler=self.handleTimeout)
        
        for p in self.regexTopic:
            self.timeSync.addExpectedPattern(p)
        
        if(self.demoMode==False):
            print("calling joinTiming", flush=True)
            self.timeSync.joinTiming()
            print("done", flush=True)
            
        
    
    def handleTimeout(self):
        print("handleTimeout", flush=True)
        self.startMainConsumer()
        
    def startMainConsumer(self):
        if(self.consumer is not None):
            self.consumer.stop()
            time.sleep(1)
            self.waitCounter+=1
            
        if(len(self.timeSync.inferedTopics) > 0):
            print("(re)subscribing to", self.timeSync.inferedTopics)
            self.consumer = KafkaConsumer(broker, registry, self.timeSync.inferedTopics,  self.kid+"_consumer",cb=self.processMsg)
        else:
            print("(re)subscribing to", self.regexTopic)
            self.consumer = KafkaConsumer(broker, registry, self.regexTopic,  self.kid+"_consumer",cb=self.processMsg)
            
        self.consumer.listenInBG()
        
    def processMsg(self, inMsg):
        if inMsg != None:
            print("== > > > received in ", inMsg.topic(), ", time =", inMsg.timestamp()[1], inMsg.value(), flush=True)
            #notify timesync about received msg
              
            #example = provision.simulation.1632490892177.traffic.micro.sumo0.edge.1---2e0---2e00.vehicles 
            timestamp = inMsg.timestamp()[1]# 0 should be TIMESTAMP_CREATE_TIME
            topicBody = inMsg.topic()[len(self.topicPre):]
            subject,tmp = topicBody.split(".", 1)
            if subject == "road":
                edge,attribute = tmp.split(".", 1)
                #special chars
                edge = edge.replace("---2e",".")
                realtopic = self.topicPre+subject+"."+edge+"."+attribute
                print(inMsg.topic(),"->",realtopic)
                self.timeSync.notifiyAboutReceivedMessage(realtopic) 
                
            else:
                
                self.timeSync.notifiyAboutReceivedMessage(realtopic) 
                print(subject,"not supported jet")
                return
            
            submicro = inMsg.value()
            self.adoptList.append(submicro)

            
    def run(self):              
        try:
            
            self.klog("simulating")
        
            ### create api bridge and run  ######
            self.api = None
            stepLengthMs = self.sim['StepLength']
            stepLengthS = self.sim['StepLength']/1000
            mapFile = baseDir+self.roadMapFile
            print("intantiating apibridge")
            self.api = CarlaAPI(mapFile, stepLengthS)
            
            self.startMainConsumer()
            
            print("running init")
            self.api.init()
            self.api.labelRoads() #in rendered world
            
            self.api.registerObservers(observable_roads)
            
            #while True:
            end = self.sce['SimulationEnd']*1000
            print("running from",self.timeSync.currentLocalTime,"to",end)
            
            while self.timeSync.currentLocalTime < end:
                
                # 1. ask to proceed
                # todo: check sce to get step length of 2th smallest instance
                # let's assume it's 1000ms as a first step
                sssl = 1000
                sssl_factor = int(sssl / stepLengthMs)
                self.timeSync.timeAdvance(sssl)      
                
                # correct way:
                # self.timeSync.timeAdvance(stepLengthMs)                
                # if(self.timeSync.currentLocalTime%10000 == 0):
                #     print(str(self.timeSync.currentLocalTime/1000),"s", flush=True)
                    
                
                    
                # 2. now, we are sure that we received all messages related to old t     
                #add vehicle
                # print("sstage 2", flush=True)     
                for submicro in self.adoptList:
                    # print("creating",submicro['vehicleID'], flush=True)
                    vid = self.api.createVehicle(submicro)
                    self.idMapping[vid] = submicro['vehicleID']
                self.adoptList.clear()
    
    
    
                # 3. calc step
                ### VARIANT ###
                ### in order to speed things up, we will do multiple steps until next sssl at once
                ### that means we need to update positions in prepreateStep() and step()
                
                for mini_step in range(sssl_factor):
                    # 3.1. make preparations   
                    # print("prepareStep", flush=True)  
                    try:
                        self.api.prepareStep()
                    except Exception as e:
                        print("prepareStep catched exception ")
                        print(e)     
                        
                    # 3.2. make the sim step simulate
                    # print("step", flush=True)     
                    try:
                        #todo: compute all steps until next sssl at once
                        self.api.step()
                    except Exception as e:
                        print("step catched exception ")
                        print(e)   
                    

                # 4. process
                # print("processStep", flush=True)     
                try:
                    self.api.processStep()
                except Exception as e:
                    print("processStep catched exception ")
                    print(e)
                    
                # 5. observe
                # print("observe", flush=True)     
                vids = ""
                speeds = ""
                tmp = ""
                try:
                    #todo: add parameterization for multiple observers, currently this is just the number of vehicles on roads
                    # observations = self.api.observe(self.timeSync.currentLocalTime)
                    # for road,obs in observations.items():
                    #     obs_topic = self.topicPre + "road."+road
                        
                    #     vids = '['
                    #     speeds = '['
                    #     first = True
                    #     for ob in obs:
                    #         tmp = ob
                    #         if first == False:
                    #             speeds += ", "
                    #             vids += ", "
                    #         else:
                    #             first = False
                    #         speeds += str(ob["speed"])
                    #         vids += "\"" + ob["vehicleID"] + "\""
                    #     speeds +=']'
                    #     vids +=']'
                    #     val = "{ \"vehicles\" : " + vids +", \"speeds\" : " + speeds + "}"
                        
                        # self.publishObservation(obs_topic, val)
                    pass
                except Exception as e:
                    print("observe catched exception ")
                    print(e)                
                    print(vids,speeds,tmp)                
                
                
                # 6. post step tasks 
                try:
                    self.api.postStep(self.timeSync.currentLocalTime)
                except Exception as e:
                    print("postStep catched exception ")
                    print(e)
                    
                    
                    
                # # 7. send out
                # smlDict = {}
                # try:
                #     for vid,vehicle in self.api.getRemovedVehicles().items():
                #         print("sending out", vid,"(", self.idMapping[vid],")")
                #         submicro = self.api.getSubMicro(vid,self.idMapping[vid])         
                #         edge = submicro['edge']
                #         if edge not in smlDict:
                #             smlDict[edge] = []
                #         smlDict[edge].append(submicro)
                # except Exception as e:
                #     print("failed to create liste of to-remove vehicles",smlDict,e)    
                    
                # try:    
                #     for edge, sml in smlDict.items():
                #         submicrolist = {}
                #         submicrolist['contents'] = sml
                #         topic =  self.topicPre + self.instanceID + ".edge." + edge + ".vehicles" 
                #         self.producer.publish(topic, sml, edge, self.timeSync.currentLocalTime)
                        
                #     self.api.clearRemovedVehicles()
                # except Exception as e:
                #     print("failed to send out removed vehicles",e)    
                    
            
            self.klog("finished")
    
        except Exception as e:
            print("main loop catched exception - vvvvvvvv")
            print(e)
            print("main loop catched exception - ^^^^^^^")
                    
        finally:
            if self.api is not None:
                self.api.destroy()
            if self.consumer is not None:
                self.consumer.stop()
            # if self.jsonProducer is not None:
            #     self.jsonProducer.stop()
            # if self.statusProducer is not None:
            #     self.statusProducer.stop()
            if self.timeSync is not None:
                self.timeSync.leaveTiming()
            #todo: reset to async mode and delete all actors? benefit?


def main():
    if (len(sys.argv) > 1):
        scenarioID = sys.argv[1]
        instanceID = sys.argv[2]
    else:
        print("No SceID & SimID provided, using demo input")

        from datetime import datetime
        date_time = datetime.now()
        scenarioID = "demo"+date_time.strftime("%m%d%H%M%S")
        instanceID = "carla0"#+date_time.strftime("%m/%d/%Y, %H:%M:%S")

    
    try:
        ctrl = CarlaWrapper(scenarioID,instanceID)
        
        if scenarioID[0:4] == "demo":
            ctrl.demoMode = True
            
        ctrl.prepare()
        ctrl.run()

    except KeyboardInterrupt:
        print('\nCancelled by user. Bye!')


if __name__ == '__main__':
    main()