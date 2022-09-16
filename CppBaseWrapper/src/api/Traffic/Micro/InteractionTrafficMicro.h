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

#include <exception>
#include <memory>
#include <string>
#include <map>
#include <vector>

#include "datamodel/Micro.hh"
#include "util/Config.h"
#include "util/Defines.h"

/**
* This class is derived from the traffic.micro layer definition. 
* As this is a generic interface description, this class will be 
* inherited by a wrapper for a tool that is categorized as 
* traffic.micro.
*/

#include "api/Interaction.h"

namespace daceDS {
class InteractionTrafficMicro : public Interaction {
   public:
    InteractionTrafficMicro() : Interaction(){};
    ~InteractionTrafficMicro(){};

    //scope vehicle
    class Vehicle {
       public:
        // dervived from native data model
        virtual void getAcceleration(std::string vehicleID, double& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setAcceleration(std::string vehicleID, double x) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getAngle(std::string vehicleID, double& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setAngle(std::string vehicleID, double x) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getEdge(std::string vehicleID, std::string& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setEdge(std::string vehicleID, std::string r) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getLane(std::string vehicleID, int& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setLane(std::string vehicleID, int l) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getPosition(std::string vehicleID, Vec3D& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setPosition(std::string vehicleID, Vec3D pos) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getPositionEdge(std::string vehicleID, double& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setPositionEdge(std::string vehicleID, double dist) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getRoute(std::string vehicleID, std::vector<std::string>& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setRoute(std::string vehicleID, std::vector<std::string> r) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getSlope(std::string vehicleID, double& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getSpeed(std::string vehicleID, double& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setSpeed(std::string vehicleID, double x) { throw EXCEPTION_NOT_IMPLEMENTED; };

        virtual void getType(std::string vehicleID, std::string& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };

        // additional methods
        virtual void getVehicle(std::string vehID, datamodel::Micro& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void getList(std::string edgeID, std::vector<std::string>& value, int64_t& time) { throw EXCEPTION_NOT_IMPLEMENTED; };
    };
    std::unique_ptr<Vehicle> vehicle;

    //scope edge
    class Edge {
       public:
        // dervived from native data model
        virtual std::string getEmissions(std::string edgeID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual double getFlow(std::string edgeID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual double getOccupancy(std::string edgeID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual double getSpeed(std::string edgeID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual std::vector<std::string> getVehicleList(std::string edgeID) { throw EXCEPTION_NOT_IMPLEMENTED; };

        // additional methods
        virtual std::vector<std::string> getEdgeList() { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual void setAllowedTypes(int laneID, std::vector<std::string> types) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual std::vector<std::string> getAllowedTypes(std::string edgeID, int laneID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual int getLaneNumber(std::string edgeID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual double getLaneGradient(std::string edgeID, int laneID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual double getLaneLength(std::string edgeID, int laneID) { throw EXCEPTION_NOT_IMPLEMENTED; };
        virtual double getLaneWidth(std::string edgeID, int laneID) { throw EXCEPTION_NOT_IMPLEMENTED; };

        // additional methods
    };
    std::unique_ptr<Edge> edge;

    //scope detector
    class Detector {
       public:
        // dervived from native data model
        virtual std::map<std::string, std::string> getValues(std::string detectorID) { throw EXCEPTION_NOT_IMPLEMENTED; };

        // additional methods
        virtual std::vector<std::string> getDetectorList() { throw EXCEPTION_NOT_IMPLEMENTED; };
    };
    std::unique_ptr<Detector> detector;


    //scope world
    virtual int64_t getSimTime(){throw EXCEPTION_NOT_IMPLEMENTED; };
    virtual std::string getBoundaries(){throw EXCEPTION_NOT_IMPLEMENTED; };
    virtual std::string getOffset(){throw EXCEPTION_NOT_IMPLEMENTED; };
    virtual std::string getRoadMap(){throw EXCEPTION_NOT_IMPLEMENTED; };

    //methods
    virtual void exportSim(int64_t time){throw EXCEPTION_NOT_IMPLEMENTED; };
};
}  // namespace daceDS
