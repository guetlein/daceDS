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
#include <memory>
#include <string>
#include <filesystem>
//#include <codecvt>
#include <limits.h>
#include <stdio.h>
// #include <utils/traci/TraCIAPI.h>
#include <libsumo/Simulation.h>
#include <libsumo/Edge.h>
#include <libsumo/Vehicle.h>
#include <libsumo/Lane.h>
#include <libsumo/InductionLoop.h>
#include <libsumo/Route.h>


#include <algorithm>
#include <cwchar>
#include <map>
#include <mutex>
#include <sstream>
#include <set>

#include "../SumoWrapper.h"
#include "datamodel/Micro.hh"
#include "datamodel/MicroDetector.hh"
#include "datamodel/MicroEdge.hh"
#include "datamodel/Observer.hh"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/Utils.h"
#include "util/log.h"



namespace daceDS {

class SumoWrapper;
class SumoConnectionLibSUMO {

   private:
    int64_t sumoTime = 0;
    std::vector<std::string> respLinks;
    std::vector<std::string> outgoingLinks;
    std::set<std::string> ghosts; //set because of search in O(log)
    std::set<std::string> nextRoundGhosts; 
    std::set<std::string> currentVehicleIDSet;
    std::map<std::string, std::string> ghostRoutes;
    std::map<std::string, std::string> ghostRouteIDs;

    // std::unique_ptr<TraCIAPI> traci; //libsumo

    int port;
    int64_t routecount = 0;  //used to interfer names for new routes
    int64_t failedAdoptions = 0;

    bool responsibleForWholeMap = false;

    void succAdoption(std::string vehID, std::string currentLane);

    int adopted = 0;
    int saveStateCounter = 0;

    std::map<std::string, short> borderVehicles;

    bool ghostingEnabled = true;

   public:


    std::vector<datamodel::Micro> getAndReleaseLeavingVehicles();

    //called by SumoSimulationControl
    SumoConnectionLibSUMO();
    // SumoConnectionLibSUMO(int port);
    ~SumoConnectionLibSUMO(){};
    bool connect();
    virtual void closeSumo();
    void setResponsibleForWholeMap(bool resp);
    bool getResponsibleForWholeMap();
    void setRespLinks(std::vector<std::string>& links);
    void setOutgoingLinks(std::vector<std::string>& links);
    std::vector<std::string> getRespLinks();
    bool isRespLink(std::string edge);


    virtual void setTime(int64_t fedTime);
    virtual void proceedSim(int32_t steps = 0);
    std::vector<std::string> getVehiclesNewOnBorderEdge();
    int64_t getSumoTime() { return sumoTime; };
    bool adopt(double time, datamodel::Micro vehicle);

    //std::string getGenericValue(std::string obj, std::string param);
    void getDetectorValue(std::string id, std::string param, double& value);

    std::vector<std::string> getUpdatedRoute(std::string vehID);

    void cleanGhosts();
    bool isVehicleGhost(std::string vehID);
    bool isVehiclePresentInSimulation(std::string vehID);
    void updateGhost(datamodel::Micro& vehicle);
    void removeAllGhostsFromSim();
    std::map<std::string, std::vector<std::string>> freshlyAdoptedVehicleIDs;
    std::vector<datamodel::Micro> SumoConnectionLibSUMO::getAndReleaseFreshlyAdoptedVehicles();

    std::string getNewSaveStatePath();

    void setGhostingDisabled();
    bool getGhostingEnabled();




    class Vehicle {
        SumoConnectionLibSUMO* parent;

       public:
        Vehicle(SumoConnectionLibSUMO* p) : parent(p){};

        void getAcceleration(std::string vehicleID, double& value, int64_t& time);
        void setAcceleration(std::string vehicleID, double x);

        void getAngle(std::string vehicleID, double& value, int64_t& time);
        void setAngle(std::string vehicleID, double x);

        void getEdgeID(std::string vehicleID, std::string& value, int64_t& time);
        void setEdgeID(std::string vehicleID, std::string r);

        void getLaneIndex(std::string vehicleID, int& value, int64_t& time);
        void setLaneIndex(std::string vehicleID, int l);

        void getPosition(std::string vehicleID, Vec3D& value, int64_t& time);
        void setPosition(std::string vehicleID, Vec3D pos);

        void getLanePosition(std::string vehicleID, double& value, int64_t& time);
        void setLanePosition(std::string vehicleID, double dist);

        void getRoute(std::string vehicleID, std::vector<std::string>& value, int64_t& time);
        void setRoute(std::string vehicleID, std::vector<std::string> r);

        void getSlope(std::string vehicleID, double& value, int64_t& time);

        void getSpeed(std::string vehicleID, double& value, int64_t& time);
        void setSpeed(std::string vehicleID, double x);

        void getVehicleClass(std::string vehicleID, std::string& value, int64_t& time);

        void getList(std::vector<std::string>& value, int64_t& time);
        void getVehicle(std::string vehID, daceDS::datamodel::Micro& value, int64_t& time);
    };
    std::unique_ptr<Vehicle> vehicle;

    class Edge {
       protected:
        SumoConnectionLibSUMO* parent;

       public:
        Edge(SumoConnectionLibSUMO* p) : parent(p){};
        std::vector<std::string> getIDList();
        std::string getEmissions(std::string edgeID);
        double getFlow(std::string edgeID);
        double getOccupancy(std::string edgeID);
        double getSpeed(std::string edgeID);
        std::vector<std::string> getVehicleIDs(std::string edgeID);
        std::vector<datamodel::Micro> getVehicles(std::string edgeID);

        void getList(std::vector<std::string>& value, int64_t& time);
        daceDS::datamodel::MicroEdge getEdge(std::string id);

        void setAllowedTypes(std::string edgeID, int laneID, std::vector<std::string> types);
        std::vector<std::string> getAllowedTypes(std::string edgeID, int laneID);
        int getLaneNumber(std::string edgeID);
        double getLaneGradient(std::string edgeID, int  laneID);
        double getLaneLength(std::string edgeID, int  laneID);
        double getLaneWidth(std::string edgeID, int  laneID);
    };
    std::unique_ptr<Edge> edge;

    class Detector {
       protected:
        SumoConnectionLibSUMO* parent;

       public:
        Detector(SumoConnectionLibSUMO* p) : parent(p) {
            e1.reset(new SumoConnectionLibSUMO::Detector::E1(this));
            e3.reset(new SumoConnectionLibSUMO::Detector::E3(this));
        };

        class E1 {
            Detector* parent;

           public:
            E1(Detector* p) : parent(p){};
            std::vector<std::string> getIDs();
            int getLastStepVehicleNumber(std::string id);
            double getLastStepMeanSpeed(std::string id);
            double getLastStepOccupancy(std::string id);
            double getLastStepMeanLength(std::string id);
            double getTimeSinceDetection(std::string id);
            std::vector<std::string> getLastStepVehicleIDs(std::string id);

            daceDS::datamodel::MicroDetector getDetector(std::string id);
        };

        class E3 {
            Detector* parent;

           public:
            E3(Detector* p) : parent(p){};
            std::vector<std::string> getIDs();
            int getLastStepVehicleNumber(std::string id);
            double getLastStepMeanSpeed(std::string id);
            int getLastStepHaltingNumber(std::string id);
            std::vector<std::string> getLastStepVehicleIDs(std::string id);

            daceDS::datamodel::MicroDetector getDetector(std::string id);
        };
        std::unique_ptr<E1> e1;
        std::unique_ptr<E3> e3;
    };
    std::unique_ptr<Detector> detector;


    //world
    void exportSim(int64_t time);
    int64_t getTime();
    std::string getBoundaries();
    std::string getOffset();
    std::string getRoadMap();
};
}  // namespace daceDS
