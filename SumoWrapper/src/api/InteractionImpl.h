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

#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <locale>
#include <string>
//#include <codecvt>
#include <api/Traffic/Micro/InteractionTrafficMicro.h>
#include <limits.h>
#include <stdio.h>
// #include <utils/traci/TraCIAPI.h>
#include <libsumo/Simulation.h>

#include <algorithm>
#include <cwchar>
#include <map>
#include <mutex>
#include <sstream>

#ifdef USING_TRACI
    #include "../api/SumoConnectionTraCI.h"
#else
    #include "../api/SumoConnectionLibSUMO.h"
#endif

#include "datamodel/Micro.hh"
#include "datamodel/Observer.hh"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/Utils.h"
#include "util/log.h"

namespace daceDS {
#ifdef USING_TRACI
    class SumoConnectionTraCI;
#else
    class SumoConnectionLibSUMO;
#endif


/**
 * Implements the interface, main use: map between layer interface and tool interface
 * e.g.: micro.vehicle.road -> sumo.vehicle.edge
 * little complicated compound calls (e.g. setAngle()) are just delegated to sumoConnection 
 * and realized there. bc complexity is needed there anyway (e.g. adopt vehicle, set time, ...)
*/
class InteractionImpl : public daceDS::InteractionTrafficMicro {

    #ifdef USING_TRACI
        std::shared_ptr<SumoConnectionTraCI> sumoConnection;
    #else
        std::shared_ptr<SumoConnectionLibSUMO> sumoConnection;
    #endif

    

   public:

   #ifdef USING_TRACI
        InteractionImpl(std::shared_ptr<SumoConnectionTraCI> s) : sumoConnection(s) {
            this->vehicle.reset(new daceDS::InteractionImpl::Vehicle(s));
            this->edge.reset(new daceDS::InteractionImpl::Edge(s));
        };
    #else
        InteractionImpl(std::shared_ptr<SumoConnectionLibSUMO> s) : sumoConnection(s) {
            this->vehicle.reset(new daceDS::InteractionImpl::Vehicle(s));
            this->edge.reset(new daceDS::InteractionImpl::Edge(s));
        };
    #endif



    ~InteractionImpl(){};

    class Vehicle : public daceDS::InteractionTrafficMicro::Vehicle {
        #ifdef USING_TRACI
            std::shared_ptr<SumoConnectionTraCI> sumoConnection;
        #else
            std::shared_ptr<SumoConnectionLibSUMO> sumoConnection;
        #endif

    

       public:
        
        #ifdef USING_TRACI
             Vehicle(std::shared_ptr<SumoConnectionTraCI> s) : sumoConnection(s) {}
        #else
             Vehicle(std::shared_ptr<SumoConnectionLibSUMO> s) : sumoConnection(s) {}
        #endif
        //derived from native datamodel
        void getAcceleration(std::string vehicleID, double& value, int64_t& time);
        void setAcceleration(std::string vehicleID, double x);

        void getAngle(std::string vehicleID, double& value, int64_t& time);
        void setAngle(std::string vehicleID, double x);

        void getEdge(std::string vehicleID, std::string& value, int64_t& time);
        void setEdge(std::string vehicleID, std::string r);

        void getLane(std::string vehicleID, int& value, int64_t& time);
        void setLane(std::string vehicleID, int l);

        void getPosition(std::string vehicleID, Vec3D& value, int64_t& time);
        void setPosition(std::string vehicleID, Vec3D pos);

        void getPositionEdge(std::string vehicleID, double& value, int64_t& time);
        void setPositionEdge(std::string vehicleID, double dist);

        void getRoute(std::string vehicleID, std::vector<std::string>& value, int64_t& time);
        void setRoute(std::string vehicleID, std::vector<std::string> r);

        void getSlope(std::string vehicleID, double& value, int64_t& time);

        void getSpeed(std::string vehicleID, double& value, int64_t& time);
        void setSpeed(std::string vehicleID, double x);

        void getType(std::string vehicleID, std::string& value, int64_t& time);

        // additional methods
        void getVehicle(std::string vehID, datamodel::Micro& value, int64_t& time);
        void getList(std::vector<std::string>& value, int64_t& time);
    };

    class Edge : public InteractionTrafficMicro::Edge {
        #ifdef USING_TRACI
            std::shared_ptr<SumoConnectionTraCI> sumoConnection;
        #else
            std::shared_ptr<SumoConnectionLibSUMO> sumoConnection;
        #endif

       public:
        #ifdef USING_TRACI
             Edge(std::shared_ptr<SumoConnectionTraCI> s) : sumoConnection(s) {}
        #else
             Edge(std::shared_ptr<SumoConnectionLibSUMO> s) : sumoConnection(s) {}
        #endif
       

        std::string getEmissions(std::string edgeID);
        double getFlow(std::string edgeID);
        double getOccupancy(std::string edgeID);
        double getSpeed(std::string edgeID);
        std::vector<std::string> getVehicleIDs(std::string edgeID);

        std::vector<std::string> getEdgeList();

        // additional methods
        void setAllowedTypes(std::string edgeID, int laneID, std::vector<std::string> types);
        std::vector<std::string> getAllowedTypes(std::string edgeID, int laneID);
        int getLaneNumber(std::string edgeID);
        double getLaneGradient(std::string edgeID, int laneID);
        double getLaneLength(std::string edgeID, int laneID);
        double getLaneWidth(std::string edgeID, int laneID);

    };

   

    //scope detector
    class Detector : public InteractionTrafficMicro::Detector {
        #ifdef USING_TRACI
            std::shared_ptr<SumoConnectionTraCI> sumoConnection;
        #else
            std::shared_ptr<SumoConnectionLibSUMO> sumoConnection;
        #endif

        public:
        // dervived from native data model
        std::map<std::string, std::string> getValues(std::string detectorID);

        // additional methods
        std::vector<std::string> getDetectorList();
    };


    //scope world
    int64_t getSimTime();
    std::string getBoundaries();
    std::string getOffset();
    std::string getRoadMap();

    //methods
    void exportSim(int64_t time = -1);
};
}  // namespace daceDS
