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


from __future__ import print_function

import math
import os
import random
import sys
import copy


try:
    sys.path.append('/carla/PythonAPI/carla/dist/carla-0.9.11-py3.8-linux-x86_64.egg')
except IndexError:
    pass
try:
    sys.path.append('/home/guetlein/Code/Extern/carla/PythonAPI/carla')
except IndexError:
    pass

import carla

from carla import ColorConverter as cc
from camera import CameraManager
from agents.navigation.behavior_agent import BehaviorAgent  # pylint: disable=import-error


class CarlaAPI(object):
    """ Class representing the surrounding environment """

    def __init__(self,xodr_path,step_length, w="", wcb=None):
        self.wrapper_cb = wcb        
        
        self.recOn = 0
    
        print("creating client")
        client = carla.Client("127.0.0.1", 2000)
        client.set_timeout(30.0)
        
        #do we know a fancy 3d map for this xodr?
        if "Town01" in xodr_path:
            xodr_path = ""
            w = "Town01"
            
            
        if(len(xodr_path)>0):
            self.xodr_path = xodr_path
            
            if os.path.exists(self.xodr_path):
                with open(self.xodr_path, encoding='utf-8') as od_file:
                    try:
                        data = od_file.read()
                    except OSError:
                        print('file could not be readed.')
                        sys.exit()
                print('load opendrive map %r.' % os.path.basename(self.xodr_path))
                vertex_distance = 2.0  # in meters
                max_road_length = 50.0 # in meters
                wall_height = 1.0      # in meters
                extra_width = 0.6      # in meters
                self.world = client.generate_opendrive_world(
                    data, carla.OpendriveGenerationParameters(
                        vertex_distance=vertex_distance,
                        max_road_length=max_road_length,
                        wall_height=wall_height,
                        additional_width=extra_width,
                        smooth_junctions=True,
                        enable_mesh_visibility=True))
        elif(len(w)>0):
            maps = client.get_available_maps()
            print(maps)
            self.world = client.load_world(w) 

        else:
            self.world = client.get_world()            
        
        settings = self.world.get_settings()
        settings.fixed_delta_seconds = step_length
        settings.synchronous_mode = True # Enables synchronous mode
        settings.substepping = True
        
        #msdt * ms need must be >= fixed_delta_seconds
        settings.max_substeps = 10
        settings.max_substep_delta_time = .01 #more accurate with .01
        #settings.max_substep_delta_time = step_length / settings.max_substeps
        
        # settings.no_rendering_mode = True
        
        print(settings)
        
        # settings.WeatherId=9
        # settings.use_offsets = False # THIS FUCKED UP EVERYTHING
        settings.use_offsets = True
        self.world.apply_settings(settings)
        self.world = client.reload_world(reset_settings=False)
        self.world.apply_settings(settings)
        
        
        self.agents = {}
        self.spawnedAgents = {}
        self.cameras = {}
        self.routes = {}
        self.removedIds = []
        self.removedVehicles = {}
        self.waypointRoutes = {}


        self.world = client.get_world() 
        self.map = None


        self.alreadAdjustedCamOnce = False   
        self.debugLabels = False
        
        self.internalID2externalID = {}
        self.sensors = {}
        
        self.stepMS = int(step_length*1000)
        self.freezeTimeOut = 100000
        
        self.sensorProb = 0.0
        self.snapshotPeriod = 120000

    def init(self):
        

        # alot
        self.map_waypoints = self.world.get_map().generate_waypoints(1)
        print("got",len(self.map_waypoints),"waypoints")
        
        self.map_topology_waypoints = self.world.get_map().get_topology()
        print("got",len(self.map_topology_waypoints),"topology_waypoints")
        
        self.map_roads_lanes = {}
        for wps in self.map_topology_waypoints:
            for wp in wps:
                if wp.road_id not in self.map_roads_lanes:
                    self.map_roads_lanes[wp.road_id] = set()
                self.map_roads_lanes[wp.road_id].add(wp.lane_id)
            
        print("map:",self.map_roads_lanes)

  
        self.currentRoadId = {}
        self.currentLaneId = {}
        
        ########################
        ### TL
        #######################
        #todo: tmp fix to speed up things: we are setting all tls to green
        tls = self.world.get_map().get_all_landmarks_of_type('1000001')
        #alternative: for tl in world.get_actors().filter('traffic.traffic_light*'):
        for tl in tls:
            t = self.world.get_traffic_light(tl)
            t.set_state(carla.TrafficLightState.Green)
            t.set_green_time(4000.0)


        while self.map == None:
            try:
                self.map = self.world.get_map()
            except Exception as e:
                print(e, flush=True)
        
        
    def labelWPs(self):
        #all waypoints
        print("print wps")
        for wp in self.map_waypoints:
            p1 = wp.transform.location
            self.world.debug.draw_string(p1, "Road "+str(wp.road_id), draw_shadow=False,	 color=carla.Color(r=0, g=0, b=255), life_time=100, persistent_lines=True)
            # self.world.debug.draw_point(p1,  size=1,  color=carla.Color(r=255, g=0, b=0), life_time=0)
            
        #topo waypoints
        print("print map_topology_waypoints")
        i = 0
        for wp in self.map_topology_waypoints:
            x = carla.Color(r=(254-i*30)%255, g=(i*30)%255, b=0)
            p1 = wp[0].transform.location
            # self.world.debug.draw_string(p1, 'o', draw_shadow=False,	 color=carla.Color(r=0, g=255, b=0), life_time=100, persistent_lines=True)
            self.world.debug.draw_string(p1, "Beginning Road "+str(wp[0].road_id), draw_shadow=False,	 color=x, life_time=100, persistent_lines=True)
            # print(wp[0].road_id,"is at",p1)
            
            p2 = wp[1].transform.location
            # self.world.debug.draw_string(p2, 'o', draw_shadow=False,	 color=carla.Color(r=0, g=255, b=0), life_time=100, persistent_lines=True)
            self.world.debug.draw_string(p1, "End Road "+str(wp[1].road_id), draw_shadow=False,	 color=x, life_time=100, persistent_lines=True)
            # print(wp[1].road_id,"is at",p2)
        
            self.world.debug.draw_arrow(p1, p2, thickness=0.1, arrow_size=0.1, color=x, life_time=100)        
            
            i+=1
            
            
    
    def labelRoads(self):    
        for road,lanes in self.map_roads_lanes.items():
            for lane in lanes:
                wp = self.map.get_waypoint_xodr(road, lane, 0.0)
                l=wp.transform.location
                l.x+=lane
                l.y+=lane
                if self.debugLabels == True:
                    self.world.debug.draw_string(l, "road"+str(road)+"."+str(lane), draw_shadow=True, color=carla.Color(r=0, g=0, b=0), life_time=100, persistent_lines=True)

           
    
    def getWPLaneEntry(self, road_id, lane_id):
        
        wp = self.map.get_waypoint_xodr(road_id, lane_id, 0) 
        
        if(lane_id>0): 
            wps = wp.previous_until_lane_start(1)
            for i in range(len(wps)):
                if(wps[-i-1].road_id == road_id): #last element was on another road for some examples
                    wp = wps[-i-1]
                    return wp

        return wp  
         
    
    def getWPLaneExit(self, road_id, lane_id):
        
        wp = self.map.get_waypoint_xodr(road_id, lane_id, 0) 
        
        if(lane_id<0): 
            wps = wp.next_until_lane_end(1)
            for i in range(len(wps)):
                if(wps[-i-1].road_id == road_id): #last element was on another road for some examples
                    wp = wps[-i-1]
                    return wp

        return wp    
        
    def destroy(self):
        """Destroys all actors"""
        # actors = [self.player]
        # for actor in actors:
        #     if actor is not None:
        #         actor.destroy()
        print("todo: destroy()")
                
    
    def adjustCam(self, transform):
        
        print("adjustCam",transform, flush=True)
        spectator = self.world.get_spectator()  
        camTransform = carla.Transform()

        #from top
        camTransform.location.x = transform.location.x 
        camTransform.location.y = transform.location.y 
        camTransform.location.z = transform.location.z  + 50
        camTransform.rotation = carla.Rotation(-90.000000, 0, 0) # von oben
        
        
        spectator.set_transform(camTransform)
        print("adjusted cam to",camTransform, flush=True)
    
    
    
    def createSensors(self, vehicle, vehicleId):
        sensors = []
        
        
        cam = CameraManager(vehicle, pos="trough_windshield", vid=vehicleId)
        cam.set_sensor(0, force_respawn=True)
        sensors.append(cam)
        
        cam2 = CameraManager(vehicle, pos="bird", vid=vehicleId)
        cam2.set_sensor(0, force_respawn=True)
        sensors.append(cam2)
        
        cam3= CameraManager(vehicle, pos="above_with_half_veh_towards_forward", vid=vehicleId)
        cam3.set_sensor(0, force_respawn=True)
        sensors.append(cam3)
        
        
        
        depthCam = CameraManager(vehicle, pos="on_hood_directed_forward", vid=vehicleId)
        depthCam.set_sensor(3)
        sensors.append(depthCam)
        
        
        
        segCam = CameraManager(vehicle, pos="on_hood_directed_forward", vid=vehicleId)
        segCam.set_sensor(5)
        sensors.append(segCam)
        
        
        
        lidar1 = CameraManager(vehicle, pos="on_hood_directed_forward", vid=vehicleId)
        lidar1.set_sensor(6)
        sensors.append(lidar1)   
        
        return sensors
    
    
    def removeSensors(self, aid):
        for sensor in self.sensors[aid]:
            try:
                print("destroying sensor ", sensor, flush=True)
                sensor.destroy()
            except Exception as e:
                print(e, flush=True)
    
    
    def createVehicle(self, submicro):
        
        #######################################
        ### find spawn position
        #######################################
        startTransform = None
        try:
            startTransform = carla.Transform()
            
            #if we have a raw location, we could use it
            # if(submicro['position'] != None):
            if(False):                      
                
                loc = submicro['position']
                rot = submicro['rotation']
                
                startTransform.location.x = loc['x'] #+ 4 * math.cos(math.radians(rot['z'])) #useful if we start directly at edge:
                startTransform.location.y = loc['y'] #+ 4 * math.sin(math.radians(rot['z'])) #useful if we start directly at edge:
                startTransform.location.z = loc['z']
                startTransform.rotation.pitch = rot['y']
                startTransform.rotation.yaw =  rot['z']
                startTransform.rotation.roll = rot['x']
                
                print("spawning", submicro["vehicleID"], " at ", startTransform, " based on raw location", flush=True)
            
            #otherwise we retrieve a WP that is at the beginning of the current road
            else:
                startTransform = self.getWPLaneEntry(submicro['lane'].split("-")[0],submicro['lane'].split("-")[1]).transform
                print("spawning", submicro["vehicleID"], " at ", startTransform, " based on lane entry WP", flush=True)
            
            startTransform.location.z += 2.0
 
        except Exception as e:
            print(e, flush=True)
            print(submicro, flush=True)
            print("failed trying to find loc for wp",(submicro['lane'].split("-")[0],submicro['lane'].split("-")[1]), flush=True)
            return
            
        if self.alreadAdjustedCamOnce == False:
            self.alreadAdjustedCamOnce = True
            self.adjustCam(startTransform)
        # self.world.debug.draw_string(startTransform.location, "X", draw_shadow=True, color=carla.Color(r=254, g=0, b=0), life_time=100, persistent_lines=True)

        

        #######################################
        ### make a list of locations for route
        #######################################
        route_locations = []
        try:
            for waypoint in submicro['route']:
                ll = carla.Location()
                
                    
                wp = self.map.get_waypoint_xodr(waypoint['roadId'], waypoint['laneId'],0)
                wps = self.getWPLaneEntry(waypoint['roadId'], waypoint['laneId'])
                wpe = self.getWPLaneExit(waypoint['roadId'], waypoint['laneId'])
                print("adding estimated exit of road=", waypoint['roadId'], "lane=",waypoint['laneId'],"->",wpe," to route")
                
                #label it in the world
                if self.debugLabels == True:
                    self.world.debug.draw_string(wps.transform.location, str(waypoint['roadId'])+" entry", draw_shadow=True, color=carla.Color(r=4, g=0, b=240), life_time=100, persistent_lines=True)
                    self.world.debug.draw_string(wpe.transform.location, str(waypoint['roadId'])+" exit", draw_shadow=True, color=carla.Color(r=4, g=0, b=240), life_time=100, persistent_lines=True)
                ll = wpe.transform.location   
                route_locations.append(ll)
                
            
        except Exception as e:
            print(e)
            return
            
        #######################################
        ### create the vehicle
        #######################################
        vehicle = None
        try:
            #spawn vehicle at start position
            #veh_list = self.world.get_blueprint_library().filter('vehicle.*')
            veh_list = ['vehicle.audi.etron', 'vehicle.bmw.grandtourer', 'vehicle.citroen.c3', 'vehicle.mercedes-benz.coupe', 'vehicle.nissan.micra']                 
            blueprint = self.world.get_blueprint_library().filter(random.choice(veh_list))[0]
            # blueprint = random.choice(self.world.get_blueprint_library().filter('vehicle.*'))
            vehicle = self.world.spawn_actor(blueprint, startTransform) 
            
            #register route
            self.routes[vehicle.id] = route_locations
            
            #register current road
            self.currentRoadId[vehicle.id] = sm['lane'].split("-")[0]
            self.currentLaneId[vehicle.id] = sm['lane'].split("-")[1]
            print("spawned veh!")
            
            #add agent and first stop
            agent = BehaviorAgent(vehicle)
            self.spawnedAgents[vehicle.id] = agent
            self.internalID2externalID[vehicle.id] = submicro['vehicleID']
            destination = route_locations.pop(0)
            self.setNewTarget(agent,startTransform.location, destination)
            
            #set initial speed
            # forward_vec = vehicle.get_transform().get_forward_vector()
            forward_vec = startTransform.get_forward_vector()
            velocity_vec = submicro['speed'] * forward_vec
            vehicle.set_target_velocity(velocity_vec)
            
            #todo: not working, overwritten by driving dynamics?
            ## set gear
            # self.setGear(vehicle.id, 3)
            
        except Exception as e:
            print(e)
            print("failed to create vehicle",e, flush=True)
            return
        


       
        
        #######################################
        ### add sensors?
        #######################################
        
        self.collision_sensor = self.world.spawn_actor(self.world.get_blueprint_library().find('sensor.other.collision'), carla.Transform(), attach_to=vehicle)
        self.collision_sensor.listen(lambda event: self.collisionListener(event))
        print("created coll sensor!")
        
        #var1: have persistent sensors and enable recording from time to time
        #var2: create and delete sensors from time to time <- some bug freezes whole simulation with var2
        self.sensors[vehicle.id] = []   
        
        if(random.random()<=self.sensorProb):
            # vehicle.id is internal id, submicro['vehicleID'] is external id
            self.sensors[vehicle.id] = self.createSensors(vehicle, submicro['vehicleID']) #var1

        
        print("finished with createVehicle!")
        
    
    
        return vehicle.id
        
    def collisionListener(self, event):
        
        #https://carla.readthedocs.io/en/latest/ref_sensors/#semantic-segmentation-camera
        print("collision with actor of type:", event.other_actor.semantic_tags)
        
        self.removeVehicle(event.actor.id)
        
    
    
    def setNewTarget(self, agent, cur, dest):
        try:
                
            print("trying to create path between",cur, "and",dest, flush=True)
            agent.set_destination(cur, dest, clean=True)   
         
            
            # label dest and planned mini-hops
            if self.debugLabels == True:
                self.world.debug.draw_string(dest, "S", draw_shadow=True, color=carla.Color(r=254, g=0, b=0), life_time=10, persistent_lines=True)
                for ele in agent.get_local_planner().waypoints_queue:
                    p = ele[0].transform.location
                    self.world.debug.draw_string(p, "Q", draw_shadow=True, color=carla.Color(r=254, g=0, b=0), life_time=100, persistent_lines=True)
                
        except Exception as e:
            print(e)
            print("failed to set_destination, cur=",cur,"dest=",dest, flush=True)
            return
            



    def prepareStep(self):
        # print("prepare step")
        for aid in self.removedIds:
            self.removeVehicle(aid)
        self.removedIds.clear()
        
        
        toRemove = []
        for agentid,agent in self.agents.items():
            
  
            try:
                agent.update_information()
            except Exception as e:
                print("failed to update info for agent",agentid,e, flush=True)
                    
            
            try:
                #do we need a new sub target?
                if len(agent.get_local_planner().waypoints_queue) == 0:
                    #reached point, drive to next
                    print("reached sub target, remaing wps in route:",len(self.routes[agentid]))
                    
                    if len(self.routes[agentid])>0:   
                        print("reached loc:",agent.vehicle.get_location(), flush=True)
                        nextStop = self.routes[agentid].pop(0)
                        self.setNewTarget(agent, agent.vehicle.get_location(), nextStop)         

                    else:
                        print("All elements of route reached, destroying actor",agentid, flush=True)
                        toRemove.append(agentid)
                        continue
                        
    
                control = agent.run_step(debug=False)
                agent.vehicle.apply_control(control) #Applies a control object on the next tick, containing driving parameters such as throttle, steering or gear shifting.
                
            except Exception as e:
                print("failed to update agent",agentid,e, flush=True)
                
        
        for aid in toRemove:
            self.removeVehicle(aid)
            
                
                
     
    
    def step(self):
        self.world.tick(self.freezeTimeOut)
    
    
##################
### process the last simulation step. mainly check if vehicles are going to be sent out
    
    def processStep(self):
        #there is no sourrounding for a new vehicle, we need to wait fpr the first tick
        for agentid,agent in self.spawnedAgents.items():
            self.agents[agentid] = agent
        self.spawnedAgents.clear()    
        
            
        
        #update road and lane registry
        toTransfer = []
        for agentid,agent in self.agents.items():
            agent_wp = self.map.get_waypoint(agent.vehicle.get_location())
            self.currentRoadId[agentid] = agent_wp.road_id
            self.currentLaneId[agentid] = agent_wp.lane_id
            
            
            # #do we need to abandon a vehicle
            # if not self.isResponsible(agent_wp.road_id, agent_wp.lane_id):
            #     toTransfer.append(agentid)
     
        
        for aid in toTransfer:
            self.transferVehicle(aid)
        
     
        
     
    def postStep(self, timeInMS = 0):

        # #make a picture
        # if (self.recOn > 0):
        #     #var 1: disnable recording of existing sensors
        #     self.recOn -= 1
        #     if self.recOn == 0:
        #         for agentid,sensor_list in self.sensors.items():
        #             for sensor in sensor_list:
        #                 sensor.disable_recording()
                        
        #var 2: remove sensors
        
        # if (timeInMS % self.snapshotPeriod == 10000):
        #     print("deleting image sensors", flush=True)
        #     for agentid,agent in self.agents.items():
        #         print("agent",agentid,"has vehicle",agent.vehicle.id," of type:", agent.vehicle.semantic_tags)
        #         try:
        #             self.removeSensors(agentid)   
        #             self.sensors[agentid] = []         
        #         except Exception as e:        
        #             print(e, flush=True)
            
        if (timeInMS > 100000 and timeInMS % 1000 == 0):
            # #var 1: enable recording of existing sensors
            # self.recOn = 2
            # for agentid,sensor_list in self.sensors.items():
            #     for sensor in sensor_list:
            #         sensor.enable_recording(timeInMS)
                    
                    
            #var 2: create sensors in every period
            print("creating image sensors")
            self.recOn = 5
            for agentid,agent in self.agents.items():
                try:
                    vid = self.internalID2externalID[agentid]
                    print("agent",agentid,"has vehicle",agent.vehicle.id," with extid", vid, " type:", agent.vehicle.semantic_tags)
                    
                    self.sensors[agentid] = self.createSensors(agent.vehicle, vid)                
                    for sensor in self.sensors[agentid]:
                        sensor.enable_recording(timeInMS)
                except Exception as e:
                    print("problem when creating sensors for agent",agentid,":",e)
                    pass
            
        
        return
        # for agentid,agent in self.agents.items():            
            # rstr=""
            # for wp in self.getRoute(agentid):
            #     rstr += str(wp.road_id)+" "
                
            # target_wp = None
            # tar = "empty"
            # if len(agent.get_local_planner().waypoints_queue)>0:
            #    target_wp = agent.get_local_planner().waypoints_queue[0][0]
            #    tar = target_wp.transform.location
            # print("veh=",agentid, "edge=",  agent_wp.road_id, "lane", agent_wp.lane_id, "loc", agent.vehicle.get_location(), "route=",rstr, 
            #       "target=", target_wp.road_id, tar, flush=True)
            
            # print("veh=",agentid,"velocity=",self.getVelocity(agentid),"rotation=",self.getRotation(agentid),"ctrl=",self.getVehCtrl(agentid),"speed=",self.getSpeed(agentid))

        
    
    
    
    def registerObservers(self, observable_roads):
        self.observable_roads = observable_roads
        
    #todo: add parameterization for multiple observers
    def observe(self,timeInMS):
        
        
        # if timeInMS % 60000 != 0:
        #     return {}
        
    
        print("observing t="+str(timeInMS)+ " with "+str(len(self.agents.keys()))+" agents", flush=True)
        
        vehsOnActiveRoads = {}
        for agentid in self.agents.keys():
            sm = self.getSubMicro(agentid)
            # print(sm['position'])
            # print(sm['rotation'])
            
            r = sm['lane'].split("-")[0]
            l = sm['lane'].split("-")[1]
            rl=str(r)+"_"+str(l)
            # print("road:",rl)
            
            if rl not in vehsOnActiveRoads:
                vehsOnActiveRoads[rl] = []
            
            vehsOnActiveRoads[rl].append(sm)
            

        # for r,cnt in vehsOnActiveRoads.items():
        #     print("t="+str(timeInMS)+", road="+r+", count="+str(cnt))

        
        obs = {}
        for road in self.observable_roads:
            if road in vehsOnActiveRoads:
                obs[road] = vehsOnActiveRoads[road]
            else:
                obs[road] = []
                
        return obs
    
    
        
    def getRoute(self, vid):
        wps =[]
        for loc in self.routes[vid]:
            wps.append(self.map.get_waypoint(loc))
        return wps
    
    #in km/h
    def getSpeed(self, vid):
        return self.agents[vid].speed     
    
    def getAcceleration(self, vid):
        return self.agents[vid].vehicle.get_acceleration()
    
    def getPosition(self, vid):
        return self.agents[vid].vehicle.get_location()
    
    def getRotation(self, vid):
        return self.agents[vid].vehicle.get_transform().rotation
    
    def getVelocity(self, vid):
        return self.agents[vid].vehicle.get_velocity()
    
    def getRoad(self, vid):
        return  self.currentRoadId[vid]
    
    def getLane(self, vid):
        return self.currentLaneId[vid]
    
    def getVehCtrl(self, vid):
        return self.agents[vid].vehicle.get_control()

    def getSubMicro(self, vid):
        submicro = {}
        try:
            submicro['vehicleID'] = self.internalID2externalID[vid]
            submicro['route'] = self.getRoute(vid)
            submicro['speed'] = self.getSpeed(vid)
            submicro['acceleration'] = self.getAcceleration(vid)
            submicro['position'] = self.getPosition(vid)
            submicro['rotation'] = self.getRotation(vid)
            submicro['velocity'] = self.getVelocity(vid)
            submicro['lane'] = self.getRoad(vid)+"-"+self.getLane(vid)
            
            ctrl = self.getVehCtrl(vid)
            submicro['throttle'] = ctrl.throttle
            submicro['brake'] = ctrl.brake
            submicro['gear'] = ctrl.gear
            submicro['steer'] = ctrl.steer
        except Exception as e:
            print("failed to getsubmicro for",vid,e, flush=True)
        
        
        print("speed=", self.getSpeed(vid), "km/h,    vel=", self.getVelocity(vid), "m/s   allowed=", self.agents[vid].speed_limit)
        return submicro
    
    
    #todo: problem how to cover little helper roads that are unkown to domain?
    def isResponsible(self, road, lane):
        if( road == 24 and lane == 1):
            return True
        return False
    
    def setGear(self, vid, g):
        ctrl = self.getVehCtrl(vid)
        ctrl.gear=g
        self.agents[vid].vehicle.apply_control(ctrl)
        
        
        
    def transferVehicle(self, aid):
        
        if (self.wrapper_cb != None):
            sm = self.getSubMicro(aid)
            self.wrapper_cb.sendOut(sm)
            
        #first step: remove instantly
        #todo: implement a passive ghost feature
            
        self.removeVehicle(aid)
        
        
    def removeVehicle(self, aid):

        try:
            self.collision_sensor.destroy()
            for sensor in self.sensors(aid):
                sensor.destroy()
        except:
            pass
        
        try:
            self.agents.pop(aid).vehicle.destroy()
            self.routes.pop(aid)
        except:
            pass
