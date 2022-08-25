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
#include <utils/traci/TraCIAPI.h>

#include <algorithm>
#include <cwchar>
#include <map>
#include <mutex>
#include <sstream>

#include "../SumoWrapper.h"
#include "datamodel/Micro.hh"
#include "datamodel/MicroDetector.hh"
#include "datamodel/MicroEdge.hh"
#include "datamodel/Observer.hh"
#include "util/Config.h"
#include "util/Defines.h"
#include "util/Utils.h"
#include "util/log.h"

/*

Während InteractionImpl erstmal nur implementiert was durchs interface vorgebenen wird, gibt es hier mehr funktionalität. bspw das verbinden mit sumo, zeitsync etc.

Damit es beim Zugriff keine RaceConditions gibt, wird auf die TraCI schnittstelle ausschließlich über ein Objekt dieser Klasse zugegriffen.

Das bedeutet u.a. : 
(a) Wenn ein Interaktionsrequest empfangen wird, wird der aufruf hierher delegiert
(b) Wenn die Zeitsyncro einen progress erwartet, wird das hier realisiert
(c) Wenn die Steuerungslogik abfragt, ob fahrzeuge verschickt werden müssen, würd das hier realisiert
(d) Wenn ein Observer werte braucht, wird das hier realisiert
*/

// struct StringObservation{
// 	c::Observer o;
// 	string value;
// };

// struct DoubleObservation{
// 	c::Observer o;
// 	double value;
// };

namespace daceDS {

class SumoWrapper;
class SumoConnectionTraCI {
    // private:

    // 	enum APICALL{
    // 		MOVETOXY,
    // 		GETNOVEHICLES,
    // 		ADDVEHICLE,
    // 		GETSPEED,
    // 		GETPOSITION,
    // 		GETVEHICLEIDS
    // 	};

    // 	const std::string INSTANCE_TYPE = "sumo";
    // 	const int LOGLEVEL = 4;

    // int stepLength = 0;

    // bool notransfer = false;

    // int scaling = 1;
    // int scalingCounter = 0;

    // std::string lastedge = "none";
    // std::string federateName = "anon";

    // std::vector<std::string> announcedCongestions;

    // std::string getEdgeFromLane(std::string lane);
    // int pid, status;

   private:
    int64_t sumoTime = 0;
    std::vector<std::string> respLinks;
    std::vector<std::string> outgoingLinks;

    std::unique_ptr<TraCIAPI> traci;

    int port;
    int64_t int64_t routecount = 0;  //used to interfer names for new routes
    int64_t failedAdoptions = 0;

    bool responsibleForWholeMap = false;

    void succAdoption(std::string vehID, std::string currentLane);

    int adopted = 0;
    int saveStateCounter = 0;

    std::map<std::string, short> borderVehicles;

   public:
    std::map<std::string, std::string> activeGhosts;


    std::vector<datamodel::Micro> getVehiclesNewOnBorderEdge2();

    //called by SumoSimulationControl
    SumoConnectionTraCI(int port);
    ~SumoConnectionTraCI(){};
    bool connect();
    virtual void closeSumo();
    void setResponsibleForWholeMap(bool resp);
    bool getResponsibleForWholeMap();
    void setRespLinks(std::vector<std::string>& links);
    void setOutgoingLinks(std::vector<std::string>& links);
    std::vector<std::string> getRespLinks();
    virtual void setTime(int64_t fedTime);
    std::vector<std::string> getVehiclesNewOnBorderEdge();
    int64_t getSumoTime() { return sumoTime; };
    bool adopt(double time, datamodel::Micro vehicle);

    //std::string getGenericValue(std::string obj, std::string param);
    void getDetectorValue(std::string id, std::string param, double& value);

    std::vector<std::string> getUpdatedRoute(std::string vehID);

    bool release(std::string vehID, std::string currentEdge);
    void cleanGhosts();
    bool isVehicleGhost(std::string vehID);
    bool isAvailableVehicle(std::string vehID);

    void updateGhost(datamodel::Micro& vehicle){};

    std::string getNewSaveStatePath();

    // 		std::string getexepath();

    // 		//sync
    // 		virtual void setOrder(int i);

    // 		bool isResponsibleFor(std::string link);
    // 		bool isBorderLink(std::string link);
    // 		bool isActiveGhost(std::string vehID);
    // 		vector<std::string> getCongestedEdges();
    // 		vector<std::string> vehOnBorderEdge();
    // 		void addBorder(std::string link);
    // 		void removeBorder(std::string link);
    // 		void replaceBorderLinks(vector<std::string> links);
    // 		void replaceRespLinks(vector<std::string> links);
    // 		void addBorderLinks(list<std::string>* links);
    // 		void addRespLinks(list<std::string>* links);

    // 		//vector<TrafficInfo> getTrafficInfo();
    // 		virtual std::vector<VehicleStruct> getVehicles();
    // 		virtual int getNoVehicles();

    // 		virtual void printAllVehicles();
    // 		void setConfigKeyValue(std::string key, std::string val);

    // 		//incoming
    // //		void adoptWithScaling(std::string vehID, std::vector<std::string> route, std::string laneID, std::string destination, double speed);
    // 		virtual void adoptWithSpatialDistribution(std::string vehID, std::vector<std::string> route, std::string laneID, std::string destination, double speed, int srcScaling);
    // 		virtual void ghostFeedback(double time, std::string vehID, std::string lane, double position, double speed, int srcScaling);
    // 		virtual VehicleStruct getVehData(std::string vehID);

    // 		virtual void succAdoption(std::string vehID, std::string currentLane);

    // 		//modifiy
    // 		virtual void moveTo(std::string id, double x, double y);
    // 		virtual std::string processRequest(int requestType, std::vector<std::string> requestParams);
    // 		virtual std::vector<std::string> getVehicleIDs();
    // 		virtual double getSpeed(std::string vehicle);
    // 		virtual libsumo::TraCIPosition getPosition(std::string vehicle);

    // 		//logging
    // 		void log(int level, std::string s);
    // 		void log(int level, std::string s, int64_t int64_t i);
    // //		void log(int level, char *s);

    // 		//other
    // 		string getInstanceType(){return INSTANCE_TYPE;};
    // 		// virtual void observe(datamodel::Observer o);

    // 		//refc2020 nov
    // 		bool connect();

    // 		//new observers nov 2020
    // 		//regarding vehicles

    class Vehicle {
        SumoConnectionTraCI* parent;

       public:
        Vehicle(SumoConnectionTraCI* p) : parent(p){};

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
        SumoConnectionTraCI* parent;

       public:
        Edge(SumoConnectionTraCI* p) : parent(p){};
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
        SumoConnectionTraCI* parent;

       public:
        Detector(SumoConnectionTraCI* p) : parent(p) {
            e1.reset(new SumoConnectionTraCI::Detector::E1(this));
            e3.reset(new SumoConnectionTraCI::Detector::E3(this));
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
