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
#include "SumoConnectionTraCI.h"
using namespace daceDS;

SumoConnectionTraCI::SumoConnectionTraCI(int p) {
    traci = std::make_unique<TraCIAPI>();
    port = p;

    vehicle.reset(new SumoConnectionTraCI::Vehicle(this));
    edge.reset(new SumoConnectionTraCI::Edge(this));
    detector.reset(new SumoConnectionTraCI::Detector(this));
}

bool SumoConnectionTraCI::connect() {
    //connect SUMO
    int numTries = 0;
    while (numTries < 50) {
        numTries++;
        //wait a little for instance to be up and running
        usleep(numTries * 1000 * 1000u);
        string host = Config::getInstance()->get("sumoHost");
        if (host.size() == 0) {
            KERROR("sumoHost is not set, falling back to localhost");
            host = "127.0.0.1";
        }
        try {
            KDEBUG(numTries << ". try! connecting to " << host << ":" << port);
            traci->connect(host, port);
            KDEBUG("success");
            return true;
        } catch (tcpip::SocketException &e) {
            KERROR("Socket error while connecting: " << e.what());
        } catch (exception &e) {
            KERROR("Error while connecting: " << e.what());
        }
    }

    KERROR("Could not connect to SUMO!");
    return false;
}

mutex tracimutex;

void SumoConnectionTraCI::setRespLinks(vector<std::string> &items) {
    respLinks.insert(respLinks.end(), items.begin(), items.end());
}

void SumoConnectionTraCI::setResponsibleForWholeMap(bool resp) {
    responsibleForWholeMap = resp;
}
bool SumoConnectionTraCI::getResponsibleForWholeMap() {
    return responsibleForWholeMap;
}

vector<std::string> SumoConnectionTraCI::getRespLinks() {
    return respLinks;
}

void SumoConnectionTraCI::closeSumo() {
    try {
        traci->close();    
    } catch (tcpip::SocketException e) {
    } catch (const std::exception &e) {
    }
}

void SumoConnectionTraCI::setTime(int64_t fedTimeInMS) {
    unique_lock<mutex> lock(tracimutex);
    try {
        KDEBUG("trying to set sumo time via traci->simulationStep to " << fedTimeInMS);
        double fedTimeInS = fedTimeInMS / 1000.0;
        traci->simulationStep(fedTimeInS);              //in seconds
        sumoTime = traci->simulation.getCurrentTime();  // in ms
        KDEBUG("done");
    } catch (tcpip::SocketException e) {
        KERROR("SocketException; " << e.what());
        exit(42);
    } catch (const std::exception &e) {
        KERROR("Exception; " << e.what());
        exit(42);
    }

    lock.unlock();
    return;
}

int64_t SumoConnectionTraCI::getTime() {
    unique_lock<mutex> lock(tracimutex);
    int64_t value = ERROR_INTEGER;
    try {
        KDEBUG("trying to get sumo time via traci");
        value = traci->simulation.getCurrentTime();  // in ms
        KDEBUG("done");
    } catch (tcpip::SocketException e) {
        KERROR("SocketException; " << e.what());
        exit(42);
    } catch (const std::exception &e) {
        KERROR("Exception; " << e.what());
        exit(42);
    }

    lock.unlock();
    return value;
}


// //outgoing
std::vector<std::string> SumoConnectionTraCI::getVehiclesNewOnBorderEdge() {
    std::vector<std::string> newBorderVehicles;

    if (responsibleForWholeMap) {
        KDEBUG("vehOnBorderEdge() --> resp for whole map, returning empty list");
        return newBorderVehicles;
    }

    unique_lock<mutex> lock(tracimutex);
    //only conceptual - check all vehicles
    try {
        std::vector<std::string> currentVehicles = traci->vehicle.getIDList();
        //		CARDEBUG("Currently avail. cars: " << currentVehicles.size());

        for (std::string vehID : currentVehicles) {
            //			KDEBUG( "traci call done" );
            string currentLane = traci->vehicle.getLaneID(vehID);
            string currentEdge = traci->lane.getEdgeID(currentLane);

            //don't send out vehicles multiple times.
            //vehc is (1) on border and (2) unknown = not processed yet
            if (find(outgoingLinks.begin(), outgoingLinks.end(), currentEdge) != outgoingLinks.end() && borderVehicles.find(vehID) == borderVehicles.end()) {
                //				KDEBUG (vehID+" is on border" );
                borderVehicles[vehID] = 0;  //mark vehicle as processed;
                newBorderVehicles.push_back(vehID);
                traci->vehicle.setColor(vehID, libsumo::TraCIColor(255, 255, 255, 155));
            }
            //vehc is (1) not on border and (2) known = processed and gone
            else if (find(outgoingLinks.begin(), outgoingLinks.end(), currentEdge) == outgoingLinks.end() &&
                     borderVehicles.find(vehID) != borderVehicles.end()) {
                //				KDEBUG (vehID+" already gone" );
                borderVehicles.erase(vehID);
            } else {
                //				KDEBUG (vehID+" not on border" );
            }
        }
    } catch (tcpip::SocketException e) {
        KERROR("Wrapper: vehOnBorderEdge: SocketException");
    }

    lock.unlock();
    return newBorderVehicles;
}


// //outgoing
std::vector<datamodel::Micro> SumoConnectionTraCI::getVehiclesNewOnBorderEdge2() {
    std::vector<datamodel::Micro> newBorderVehicles;

    if (responsibleForWholeMap) {
        KDEBUG("vehOnBorderEdge() --> resp for whole map, returning empty list");
        return newBorderVehicles;
    }

    // unique_lock<mutex> lock(tracimutex);
    //only conceptual - check all vehicles
    try {

        for (std::string currentEdge : outgoingLinks) {
            std::vector<std::string> currentVehicles = traci->edge.getLastStepVehicleIDs(currentEdge);
            for (std::string vehID : currentVehicles) {

                //don't send out vehicles multiple times.
                //vehc is (1) on border and (2) unknown = not processed yet
                if (find(outgoingLinks.begin(), outgoingLinks.end(), currentEdge) != outgoingLinks.end() && borderVehicles.find(vehID) == borderVehicles.end()) {
                    //				KDEBUG (vehID+" is on border" );
                    borderVehicles[vehID] = 0;  //mark vehicle as processed;
                    datamodel::Micro m;
                    int64_t t;
                    vehicle->getVehicle(vehID, m, t);
                    newBorderVehicles.push_back(m);
                    // traci->vehicle.setColor(vehID, libsumo::TraCIColor(255, 255, 255, 155));
                }
                //vehc is (1) not on border and (2) known = processed and gone
                else if (find(outgoingLinks.begin(), outgoingLinks.end(), currentEdge) == outgoingLinks.end() &&
                        borderVehicles.find(vehID) != borderVehicles.end()) {
                    //				KDEBUG (vehID+" already gone" );
                    borderVehicles.erase(vehID);
                } else {
                    //				KDEBUG (vehID+" not on border" );
                }
            }
        }
    } catch (tcpip::SocketException e) {
        KERROR("Wrapper: vehOnBorderEdge: SocketException");
    }

    // lock.unlock();
    return newBorderVehicles;
}

void SumoConnectionTraCI::setOutgoingLinks(vector<std::string> &items) {
    outgoingLinks.insert(outgoingLinks.end(), items.begin(), items.end());
    KDEBUG("i am now responsible for " << outgoingLinks.size() << " borders");
}

bool SumoConnectionTraCI::adopt(double time, datamodel::Micro vehicle) {
    auto route = vehicle.route;
    auto vehID = vehicle.vehicleID;
    auto position = vehicle.positionEdge;
    //todo: check all params!
    if (route.empty()) {
        KDEBUG("No route given: Nothing to do");
        return true;
    }

    KDEBUG("-------------------------------------");

    string currentEdge = route[0];
    string destinationEdge = route[route.size() - 1];

    //todo: right now we only have one lane per edge
    //parent->traci->edge.getLaneNumber(currentEdge)
    string currentLane = currentEdge + "_" + to_string(vehicle.lane);

    KDEBUG("currentEdge    : " << currentEdge);
    KDEBUG("currentLane   : " << currentLane);
    KDEBUG("destinationEdge: " << destinationEdge);
    KDEBUG("speed		  : " << vehicle.speed);
    KDEBUG("route.begin()  : " << route[0]);
    KDEBUG("route.end()	  : " << route[route.size() - 1]);

    //check that carID is unknown in local world
    unique_lock<mutex> lock(tracimutex);

    try {
        KDEBUG("FIRST API CALL");
        std::vector<std::string> idlist = traci->vehicle.getIDList();
        KDEBUG("FIRST API CALL DONE");

        for (std::string knownVehID : idlist) {
            if (knownVehID.compare(vehID) == 0) {
                KDEBUG("already know this car - ignoring");
                //todo: ghosting
                lock.unlock();
                return true;
            }
        }
        string routeID = "route_gen_" + to_string(routecount++);
        //needed modification in sendcommand():sumo/utils/traci
        KDEBUG("adding route " << routeID << ": ");
        KDEBUG("adding route " << route[0]);
        traci->route.add(routeID, route);
        KDEBUG("adding route done");

        //todo: at the moment we got only cars of on single type: vtype0
        KDEBUG("adding vehicle " << vehID << ": " << routeID << " with " << vehicle.speed);
        traci->vehicle.add(vehID, routeID, vehicle.type, "-1", "first", "base", to_string(vehicle.speed), "current", "max", to_string(vehicle.speed), "", "", "", 0, 0);
        KDEBUG("adding vehicle done");

        //we are setting the vehicle to a fixed position, corresponding to its previous representation
        if (position < 0 || position > traci->lane.getLength(currentLane))
            position = 0;
        KDEBUG("lane position: " << position);
        KDEBUG("moving vehicle");
        traci->vehicle.moveTo(vehID, currentLane, position);  //corresponding to the doc, this is instantly forced and therefore leads to consistent states!!!
        //		parent->traci->vehicle.setSpeedMode(0);
        //		parent->traci->vehicle.setSpeed(vehID, speed);

        KDEBUG("adjusting vehicle speed");
        traci->vehicle.setSpeed(vehID, vehicle.speed);  //wichtig! sonst wird auf max beschleunigt

    } catch (tcpip::SocketException e) {
        KERROR(">>> could not add vehicle and route for" << vehID);
        KERROR(e.what());
        KERROR(">>> trying with own route ");
        try {
            std::vector<std::string> ownroute;
            ownroute.push_back(currentEdge);
            string routeID = "route_gen_" + to_string(routecount++);
            traci->route.add(routeID, ownroute);
            traci->vehicle.add(vehID, routeID, vehicle.type, "-1", "first", "base", to_string(vehicle.speed), "current", "max", to_string(vehicle.speed), "", "", "", 0, 0);
            traci->vehicle.moveTo(vehID, currentLane, position);  //corresponding to the doc, this is instantly forced and therefore leads to consistent states!!!
            traci->vehicle.changeTarget(vehID, destinationEdge);
            KDEBUG("new route is: ");
            ownroute = traci->vehicle.getRoute(vehID);
            for (std::string edge : ownroute) {
                KDEBUG(edge << "->");
            }

            traci->vehicle.setSpeed(vehID, vehicle.speed);

        } catch (tcpip::SocketException e) {
            KERROR(">>> could not add route ");
            KERROR(e.what());
            failedAdoptions++;

            KDEBUG("-------------------------------------");
            lock.unlock();
            return true;
        }
    } catch (const std::exception &e) {
        KDEBUG("adoption: Exception at adding");
        KDEBUG(e.what());
    }

    KDEBUG("succesfully added " << vehID);
    succAdoption(vehID, currentLane);

    KDEBUG("-------------------------------------");
    lock.unlock();
    return true;
}

void SumoConnectionTraCI::succAdoption(std::string vehID, std::string currentLane) {
    adopted++;

    //	//for performance there can only be one ghost per lane, namely the last incoming one
    //	//todo: this is slow, bidirectional map?
    //	for(std::map< std::string, std::string>::iterator it = activeGhosts.begin(); it != activeGhosts.end();)
    //	{
    //		if(it->second == currentLane){
    //			it = activeGhosts.erase(it);
    //		}
    //		else
    //		{
    //			it++;
    //		}
    //	}
    //	KDEBUG("***********ADDED " << vehID << " as ghost");
    //activeGhosts[vehID] = currentLane;
}

daceDS::datamodel::MicroDetector SumoConnectionTraCI::Detector::E1::getDetector(std::string id) {
    daceDS::datamodel::MicroDetector ret;
    ret.detectorID = id;
    ret.values["lastStepVehicleNumber"].set_int(getLastStepVehicleNumber(id));
    ret.values["lastStepMeanSpeed"].set_double(getLastStepMeanSpeed(id));
    ret.values["lastStepOccupancy"].set_double(getLastStepOccupancy(id));
    ret.values["lastStepMeanLength"].set_double(getLastStepMeanLength(id));
    ret.values["timeSinceDetection"].set_double(getTimeSinceDetection(id));
    ret.values["lastStepVehicleIDs"].set_string(concat(getLastStepVehicleIDs(id), ';'));
    return ret;
}

std::vector<std::string> SumoConnectionTraCI::Detector::E1::getIDs() {
    std::vector<std::string> ret;
    unique_lock<mutex> lock(tracimutex);
    try {
        ret = parent->parent->traci->inductionloop.getIDList();
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return ret;
}

int SumoConnectionTraCI::Detector::E1::getLastStepVehicleNumber(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->inductionloop.getLastStepVehicleNumber(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1;
}
double SumoConnectionTraCI::Detector::E1::getLastStepMeanSpeed(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->inductionloop.getLastStepMeanSpeed(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1.0;
}
double SumoConnectionTraCI::Detector::E1::getLastStepOccupancy(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->inductionloop.getLastStepOccupancy(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1.0;
}
double SumoConnectionTraCI::Detector::E1::getLastStepMeanLength(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->inductionloop.getLastStepMeanLength(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1.0;
}
double SumoConnectionTraCI::Detector::E1::getTimeSinceDetection(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->inductionloop.getTimeSinceDetection(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1.0;
}
std::vector<std::string> SumoConnectionTraCI::Detector::E1::getLastStepVehicleIDs(std::string id) {
    std::vector<std::string> ret;
    unique_lock<mutex> lock(tracimutex);
    try {
        ret = parent->parent->traci->inductionloop.getLastStepVehicleIDs(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return ret;
}

daceDS::datamodel::MicroDetector SumoConnectionTraCI::Detector::E3::getDetector(std::string id) {
    daceDS::datamodel::MicroDetector ret;
    ret.detectorID = id;
    ret.values["lastStepVehicleNumber"].set_int(getLastStepVehicleNumber(id));
    ret.values["lastStepMeanSpeed"].set_double(getLastStepMeanSpeed(id));
    ret.values["lastStepOccupancy"].set_double(getLastStepHaltingNumber(id));
    ret.values["lastStepVehicleIDs"].set_string(concat(getLastStepVehicleIDs(id), ';'));
    return ret;
}

std::vector<std::string> SumoConnectionTraCI::Detector::E3::getIDs() {
    std::vector<std::string> ret;
    unique_lock<mutex> lock(tracimutex);
    try {
        ret = parent->parent->traci->multientryexit.getIDList();
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return ret;
}
int SumoConnectionTraCI::Detector::E3::getLastStepVehicleNumber(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->multientryexit.getLastStepVehicleNumber(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1;
}
double SumoConnectionTraCI::Detector::E3::getLastStepMeanSpeed(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->multientryexit.getLastStepMeanSpeed(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1.0;
}

int SumoConnectionTraCI::Detector::E3::getLastStepHaltingNumber(std::string id) {
    unique_lock<mutex> lock(tracimutex);
    try {
        return parent->parent->traci->multientryexit.getLastStepHaltingNumber(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return -1;
}
std::vector<std::string> SumoConnectionTraCI::Detector::E3::getLastStepVehicleIDs(std::string id) {
    std::vector<std::string> ret;
    unique_lock<mutex> lock(tracimutex);
    try {
        ret = parent->parent->traci->multientryexit.getLastStepVehicleIDs(id);
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }
    lock.unlock();
    return ret;
}

//used by getVehicle
//todo: is this alright?
std::vector<std::string> SumoConnectionTraCI::getUpdatedRoute(std::string vehID) {
    std::vector<std::string> updatedroute;
    unique_lock<mutex> lock(tracimutex);

    try {
        string currentLane, currentEdge;
        currentLane = traci->vehicle.getLaneID(vehID);
        currentEdge = traci->lane.getEdgeID(currentLane);
        int routeIndex = traci->vehicle.getRouteIndex(vehID);
        std::vector<std::string> completeRoute = traci->vehicle.getRoute(vehID);
        KDEBUG("routeIndex= " << routeIndex);
        KDEBUG("completeRoute= " << concat(completeRoute, ' '));
        //get subroute from current edge till destination
        if (routeIndex == -1) {
            KDEBUG(" --> using complete route ");
            updatedroute = completeRoute;
        } else {
            KDEBUG(" --> using part of route ");
            for (int i = routeIndex; i < completeRoute.size(); i++) {
                updatedroute.push_back(completeRoute[i]);
            }
        }

        KDEBUG("updatedroute= " << concat(updatedroute, ' '));
        KDEBUG("size= " << updatedroute.size());
    } catch (tcpip::SocketException &e) {
        KERROR("SocketException: release:  " << e.what());
        KERROR("SocketException: release:  [" + vehID);
    }

    lock.unlock();
    return updatedroute;
}

bool SumoConnectionTraCI::isAvailableVehicle(std::string vehID) {
    auto l = traci->vehicle.getIDList();
    auto av = std::find(l.begin(), l.end(), vehID) != l.end();
    KDEBUG("isAvailable? " << vehID << " = " << av);
    return av;
}

void SumoConnectionTraCI::cleanGhosts() {
    unique_lock<mutex> lock(tracimutex);

    KDEBUG("cleaning ghosts...");

    std::string oldghosts = "";
    for (auto s : activeGhosts) {
        oldghosts += s.first + ",";
    }
    KDEBUG("old ghosts: [" << oldghosts + "]");

    // Erase members that satisfy needs_removing(itr)
    for (std::map<std::string, std::string>::const_iterator itr = activeGhosts.cbegin(); itr != activeGhosts.cend();)
        itr = (!isAvailableVehicle(itr->first)) ? activeGhosts.erase(itr) : std::next(itr);

    std::string newghosts = "";
    for (auto s : activeGhosts) {
        newghosts += s.first + ",";
    }
    KDEBUG("new ghosts: [" << newghosts + "]");

    lock.unlock();
}

bool SumoConnectionTraCI::isVehicleGhost(std::string vehID) {
    return activeGhosts.count(vehID) > 0;
}

bool SumoConnectionTraCI::release(std::string vehID, std::string currentEdge) {
    KDEBUG("releasing " << vehID << " @ sumotime:" << sumoTime);
    unique_lock<mutex> lock(tracimutex);

    try {
        // string currentLane = traci->vehicle.getLaneID(vehID);
        // string currentEdge = traci->lane.getEdgeID(currentLane);
        //let vehicle disappear
        traci->vehicle.changeTarget(vehID, currentEdge); // traci->vehicle.remove(vehID);
        
        // libsumo::TraCIColor c;
        // c.r = (unsigned char)199;
        // c.g = (unsigned char)199;
        // c.b = (unsigned char)199;
        // c.a = (unsigned char)255;
        // traci->vehicle.setColor(vehID, c);

        //mark vehID as ghost

        KDEBUG("marked " << vehID << " as ghost");
        activeGhosts[vehID] = currentEdge;
    } catch (tcpip::SocketException &e) {
        KERROR("SocketException: release:  " << e.what());
        KERROR("SocketException: release:  [" + vehID);
        lock.unlock();
        return false;
    }
    lock.unlock();
    return true;
}

void SumoConnectionTraCI::Vehicle::getList(std::vector<std::string> &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);

    try {
        value = parent->traci->vehicle.getIDList();
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR(" SumoConnectionTraCI::getVehicleList(): failed");
    } catch (const std::exception &e) {
        KERROR(" SumoConnectionTraCI::getVehicleList(): failed");
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getAcceleration(std::string vehID, double &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_DOUBLE;
    try {
        KDEBUG("SumoConnectionTraCI::getVehicleAcceleration");
        value = parent->traci->vehicle.getAcceleration(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleAcceleration:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getAngle(std::string vehID, double &value, int64_t& time) {
    KDEBUG("SumoConnectionTraCI::getVehicleAngle");
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_DOUBLE;
    try {
        value = parent->traci->vehicle.getAngle(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleAngle:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getEdgeID(std::string vehID, std::string &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_STRING;
    try {
        value = parent->traci->vehicle.getRoadID(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleRoadID:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getLaneIndex(std::string vehID, int &value, int64_t& time) {
    KDEBUG("SumoConnectionTraCI::getVehicleLane");
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_INTEGER;
    try {
        value = parent->traci->vehicle.getLaneIndex(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleLane:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getPosition(std::string vehID, Vec3D &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);
    value = {ERROR_DOUBLE, ERROR_DOUBLE, ERROR_DOUBLE};
    try {
        auto p = parent->traci->vehicle.getPosition3D(vehID);
        value.x = p.x;
        value.y = p.y;
        value.z = p.z;
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehiclePosition:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}
void SumoConnectionTraCI::Vehicle::getLanePosition(std::string vehID, double &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_DOUBLE;
    try {
        value = parent->traci->vehicle.getLanePosition(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getLanePosition:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}
void SumoConnectionTraCI::Vehicle::getRoute(std::string vehID, std::vector<std::string> &value, int64_t& time) {
    KDEBUG("SumoConnectionTraCI::getVehicleRoute");
    unique_lock<mutex> lock(tracimutex);
    try {
        value = parent->traci->vehicle.getRoute(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleRoute:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getSlope(std::string vehID, double &value, int64_t& time) {
    KDEBUG("SumoConnectionTraCI::getSlope");
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_DOUBLE;
    try {
        value = parent->traci->vehicle.getSlope(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getSlope:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}
void SumoConnectionTraCI::Vehicle::getSpeed(std::string vehID, double &value, int64_t& time) {
    KDEBUG("SumoConnectionTraCI::getVehicleSpeed");
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_DOUBLE;
    try {
        value = parent->traci->vehicle.getSpeed(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleSpeed:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getVehicleClass(std::string vehID, std::string &value, int64_t& time) {
    KDEBUG("SumoConnectionTraCI::getVehicleType");
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_STRING;
    try {
        value = parent->traci->vehicle.getVehicleClass(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleType:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionTraCI::Vehicle::getVehicle(std::string vehID, datamodel::Micro &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);
    try {
        value.acceleration = parent->traci->vehicle.getAcceleration(vehID);
        value.angle = parent->traci->vehicle.getAngle(vehID);
        value.edge = parent->traci->vehicle.getRoadID(vehID);
        value.lane = parent->traci->vehicle.getLaneIndex(vehID);
        auto pos = parent->traci->vehicle.getPosition(vehID);
        value.position.x = pos.x;
        value.position.y = pos.y;
        value.position.z = pos.z;
        value.positionEdge = parent->traci->vehicle.getLanePosition(vehID);

        lock.unlock();
        auto r = parent->getUpdatedRoute(vehID);
        value.route = r;
        lock = unique_lock<mutex>(tracimutex);

        value.speed = parent->traci->vehicle.getSpeed(vehID);
        value.vehicleID = vehID;
        value.type = parent->traci->vehicle.getTypeID(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicle:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

//regarding roads

void SumoConnectionTraCI::Edge::getList(std::vector<std::string> &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);

    try {
        value = parent->traci->edge.getIDList();
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR(" SumoConnectionTraCI::getVehicleList(): failed");
    } catch (const std::exception &e) {
        KERROR(" SumoConnectionTraCI::getVehicleList(): failed");
    }
    lock.unlock();
}

datamodel::MicroEdge SumoConnectionTraCI::Edge::getEdge(std::string edgeID) {
    datamodel::MicroEdge edge;

    edge.edgeID = edgeID;
    //edge.detector = parent->traci->edg.getAngle(vehID);
    edge.emission = getEmissions(edgeID);
    edge.flow = getFlow(edgeID);
    //eigtl keine statischen attribute?
    // vehicle.laneNo.x = pos.x;
    // vehicle.laneGradient.y = pos.y;
    // vehicle.laneLength.z = pos.z;
    // vehicle.laneWidth = parent->traci->vehicle.getLanePosition(vehID);
    edge.occupancy = getOccupancy(edgeID);
    edge.speed = getSpeed(edgeID);
    edge.vehicles = getVehicles(edgeID);
    edge.vehicleIDs = getVehicleIDs(edgeID);

    return edge;
}

std::vector<std::string> SumoConnectionTraCI::Edge::getIDList() {
    unique_lock<mutex> lock(tracimutex);
    std::vector<std::string> ret;
    try {
        ret = parent->traci->edge.getIDList();
    } catch (tcpip::SocketException &e) {
        KERROR(" SumoConnectionTraCI::getRoadList(): failed");
    } catch (const std::exception &e) {
        KERROR(" SumoConnectionTraCI::getRoadList(): failed");
    }
    lock.unlock();
    return ret;
}

std::string SumoConnectionTraCI::Edge::getEmissions(std::string edgeID) {
    unique_lock<mutex> lock(tracimutex);
    std::string emissionstr = ERROR_STRING;
    try {
        emissionstr = "NOx=" + to_string(parent->traci->edge.getNOxEmission(edgeID));
        emissionstr += ", CO2=" + to_string(parent->traci->edge.getCO2Emission(edgeID));
        emissionstr += ", CO=" + to_string(parent->traci->edge.getCOEmission(edgeID));
        emissionstr += ", HC=" + to_string(parent->traci->edge.getHCEmission(edgeID));
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getRoadEmissions:  " << e.what());
        KERROR("SocketException: " + edgeID);
    }
    lock.unlock();
    return emissionstr;
}
double SumoConnectionTraCI::Edge::getFlow(std::string edgeID) {
    unique_lock<mutex> lock(tracimutex);
    double ret = ERROR_DOUBLE;
    try {
        ret = parent->traci->edge.getLastStepVehicleNumber(edgeID);  //todo: too simple
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getFlow:  " << e.what());
        KERROR("SocketException: " + edgeID);
    }
    lock.unlock();
    return ret;
}

double SumoConnectionTraCI::Edge::getOccupancy(std::string edgeID) {
    unique_lock<mutex> lock(tracimutex);
    double ret = ERROR_DOUBLE;
    try {
        ret = parent->traci->edge.getLastStepOccupancy(edgeID);  //todo: too simple
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getOccupancy:  " << e.what());
        KERROR("SocketException: " + edgeID);
    }
    lock.unlock();
    return ret;
}

double SumoConnectionTraCI::Edge::getSpeed(std::string edgeID) {
    unique_lock<mutex> lock(tracimutex);
    double ret = ERROR_DOUBLE;
    try {
        ret = parent->traci->edge.getLastStepMeanSpeed(edgeID);  //todo: too simple
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getSpeed:  " << e.what());
        KERROR("SocketException: " + edgeID);
    }
    lock.unlock();
    return ret;
}

//ignore ghosts!
std::vector<std::string> SumoConnectionTraCI::Edge::getVehicleIDs(std::string edgeID) {
    unique_lock<mutex> lock(tracimutex);
    std::vector<std::string> ret;
    try {
        ret = parent->traci->edge.getLastStepVehicleIDs(edgeID);

        KDEBUG(ret.size() << " vehicles on " << edgeID);
        //remove ghosts from list
        ret.erase(std::remove_if(ret.begin(), ret.end(), [this](std::string vehID) {
                      return parent->isVehicleGhost(vehID);
                  }),
                  ret.end());

        KDEBUG("    " << ret.size() << " vehicles after filtering ghosts");

    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getRoadVehicleList:  " << e.what());
        KERROR("SocketException: " + edgeID);
    }
    lock.unlock();
    return ret;
}

std::vector<datamodel::Micro> SumoConnectionTraCI::Edge::getVehicles(std::string edgeID) {
    std::vector<datamodel::Micro> ret;
    for (std::string vehID : getVehicleIDs(edgeID)) {
        datamodel::Micro m;
        int64_t t;
        parent->vehicle->getVehicle(vehID, m, t);
        ret.push_back(m);
    }
    return ret;
}


        void SumoConnectionTraCI::Edge::setAllowedTypes(std::string edgeID, int  laneID,  std::vector<std::string> types){
            unique_lock<mutex> lock(tracimutex);
                std::string laneIDStr = edgeID+std::to_string(laneID);
            try {
                parent->traci->lane.setAllowed(laneIDStr, types);
            } catch (libsumo::TraCIException &e) {
                KERROR("TraCIException, setAllowedTypes, " << laneIDStr <<", "<< e.what());
            }
            lock.unlock();
            return;
        }

        std::vector<std::string> SumoConnectionTraCI::Edge::getAllowedTypes(std::string edgeID,  int laneID){
            unique_lock<mutex> lock(tracimutex);
            std::vector<std::string> ret;
                std::string laneIDStr = edgeID+std::to_string(laneID);
            try {
                ret = parent->traci->lane.getAllowed(laneIDStr);
            } catch (libsumo::TraCIException &e) {
                KERROR("TraCIException, getAllowedTypes, " << laneIDStr <<", "<< e.what());
            }
            lock.unlock();
            return ret;
        }

        int SumoConnectionTraCI::Edge::getLaneNumber(std::string edgeID){
            unique_lock<mutex> lock(tracimutex);
            int ret = ERROR_INTEGER;
            try {
                ret = parent->traci->edge.getLaneNumber(edgeID);
            } catch (libsumo::TraCIException &e) {
                KERROR("TraCIException, getLaneNumber, " << edgeID <<", "<< e.what());
            }
            lock.unlock();
            return ret;
        }

        double SumoConnectionTraCI::Edge::getLaneGradient(std::string edgeID,  int laneID){
            unique_lock<mutex> lock(tracimutex);
            double ret = ERROR_DOUBLE;
            std::string laneIDStr = edgeID+std::to_string(laneID);
            try {                
                ret = std::stod(parent->traci->lane.getParameter(laneIDStr, "slope"));
            } catch (libsumo::TraCIException &e) {
                KERROR("TraCIException, getLaneGradient, " << laneIDStr <<", "<< e.what());
            }
            lock.unlock();
            return ret;
        }

        double SumoConnectionTraCI::Edge::getLaneLength(std::string edgeID,  int laneID){
            unique_lock<mutex> lock(tracimutex);
            double ret = ERROR_DOUBLE;
                std::string laneIDStr = edgeID+std::to_string(laneID);
            try {
                ret = parent->traci->lane.getLength(laneIDStr);
            } catch (libsumo::TraCIException &e) {
                KERROR("TraCIException, getLaneLength, " << laneIDStr <<", "<< e.what());
            }
            lock.unlock();
            return ret;
        }

        double SumoConnectionTraCI::Edge::getLaneWidth(std::string edgeID,  int laneID){
            unique_lock<mutex> lock(tracimutex);
            double ret = ERROR_DOUBLE;
                std::string laneIDStr = edgeID+std::to_string(laneID);
            try {
                ret = parent->traci->lane.getWidth(laneIDStr);
            } catch (libsumo::TraCIException &e) {
                KERROR("TraCIException, getLaneWidth, " << laneIDStr <<", "<< e.what());
            }
            lock.unlock();
            return ret;
        }



/*
##### SETTERS #####
*/
void SumoConnectionTraCI::Vehicle::setAcceleration(std::string vehID, double s) {
    KERROR("SumoConnectionTraCI::setVehicleAcceleration not implemented");
    return;
}
void SumoConnectionTraCI::Vehicle::setAngle(std::string vehID, double angle) {
    KDEBUG("SumoConnectionTraCI::setVehicleAcceleration " << vehID << " " << angle);

    auto edge = "";  //getVehicleRoadID(vehID);
    auto lane = 0;   //getVehicleLane(vehID);
    Vec3D pos;
    int64_t t;
    getPosition(vehID, pos, t);
    int keepRoute = 1;  //1: closest position within the existing route will be used - otherwise route will be overwritten
    unique_lock<mutex> lock(tracimutex);
    KDEBUG("SumoConnectionTraCI::setVehicleAcceleration pos = " << pos.x << "|" << pos.y);

    try {
        /* traci docum.: edgeID and lane are optional placement hints to resolve ambiguities */
        parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, angle, keepRoute);
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: setVehicleAcceleration:  " << e.what());
        KERROR("SocketException: " + vehID);
    }

    lock.unlock();
    return;
}
void SumoConnectionTraCI::Vehicle::setEdgeID(std::string vehID, std::string edge) {
    KDEBUG("SumoConnectionTraCI::setRoad " << vehID << " " << edge);
    unique_lock<mutex> lock(tracimutex);
    int lane = -1;
    double lanePosition = 0;
    auto pos = parent->traci->simulation.convert2D(edge, lanePosition);
    int keeproute = 2;  //2: can move anywhere, but route may be overwritten

    KDEBUG("SumoConnectionTraCI::setRoad pos " << pos.x << " " << pos.y);
    try {
        /* traci docum.: edgeID and lane are optional placement hints to resolve ambiguities */
        parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, libsumo::INVALID_DOUBLE_VALUE, keeproute);
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: setRoad:  " << e.what());
        KERROR("SocketException: " + vehID);
    }

    lock.unlock();
    return;
}
void SumoConnectionTraCI::Vehicle::setLaneIndex(std::string vehID, int lane) {
    KDEBUG("SumoConnectionTraCI::setVehicleLane " << vehID << " " << lane);
    std::string edge;
    double angle;
    Vec3D pos;
    int64_t t;
    getEdgeID(vehID, edge, t);
    getAngle(vehID, angle, t);
    getPosition(vehID, pos, t);
    int keepRoute = 1;  //1: closest position within the existing route will be used - otherwise route will be overwritten
    unique_lock<mutex> lock(tracimutex);

    try {
        parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, angle, keepRoute);
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: setVehicleLane:  " << e.what());
        KERROR("SocketException: " + vehID);
    }

    lock.unlock();
    return;
}

void SumoConnectionTraCI::Vehicle::setPosition(std::string vehID, Vec3D pos) {
    KDEBUG("SumoConnectionTraCI::setVehiclePosition " << vehID << " " << pos.x << "|" << pos.y);
    unique_lock<mutex> lock(tracimutex);
    string edge = "";
    int lane = 0;
    double angle = libsumo::INVALID_DOUBLE_VALUE;
    int keeproute = 2;  //2: can move anywhere, but route may be overwritten

    try {
        /* traci docum.: edgeID and lane are optional placement hints to resolve ambiguities */
        parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, angle, keeproute);
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: setVehiclePosition:  " << e.what());
        KERROR("SocketException: " + vehID);
    }

    lock.unlock();
    return;
}

void SumoConnectionTraCI::Vehicle::setLanePosition(std::string vehID, double x) {
    KDEBUG("SumoConnectionTraCI::setVehiclePositionEdge " << vehID << " " << x);
    std::string edge;
    int lane;
    int64_t t;
    getEdgeID(vehID, edge, t);
    getLaneIndex(vehID, lane, t);
    int keepRoute = 1;  //1: closest position within the existing route will be used - otherwise route will be overwritten
    unique_lock<mutex> lock(tracimutex);
    string laneID = edge + "_" + to_string(lane);

    KDEBUG("SumoConnectionTraCI::setVehiclePositionEdge laneID " << laneID);
    try {
        parent->traci->vehicle.moveTo(vehID, laneID, x);
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: setVehiclePositionEdge:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();

    return;
}

void SumoConnectionTraCI::Vehicle::setRoute(std::string vehID, std::vector<std::string> x) {
    KDEBUG("SumoConnectionTraCI::setVehicleRoute " << vehID);
    if (x.empty()) return;
    //veh needs to be on the inital edge
    std::string edge;
    int64_t t;
    getEdgeID(vehID, edge, t);
    if (edge != x[0]) {
        setEdgeID(vehID, x[0]);
    }
    unique_lock<mutex> lock(tracimutex);

    try {
        parent->traci->vehicle.setRoute(vehID, x);
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: setVehicleRoute:  " << e.what());
        KERROR("SocketException: " + vehID);
    }

    lock.unlock();
    return;
}

void SumoConnectionTraCI::Vehicle::setSpeed(std::string vehID, double s) {
    KDEBUG("SumoConnectionTraCI::setVehicleSpeed " << vehID << " " << s);
    unique_lock<mutex> lock(tracimutex);

    try {
        parent->traci->vehicle.setSpeed(vehID, s);
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: setVehicleSpeed:  " << e.what());
        KERROR("SocketException: " + vehID);
    }

    lock.unlock();
    return;
}


std::string SumoConnectionTraCI::getNewSaveStatePath(){
    return Config::getInstance()->getOutputDir()+"/state_"+std::to_string(getSumoTime())+"_"+std::to_string(saveStateCounter++)+".sbx";
}

void SumoConnectionTraCI::exportSim(int64_t time){
    unique_lock<mutex> lock(tracimutex);

    //do we want it now?
    if(time < 0){
        const std::string path = getNewSaveStatePath();
        try {
            traci->simulation.saveState(path); 
            KDEBUG("saved snapshot to " << path);
        
        } catch (libsumo::TraCIException &e) {
            KERROR("TraCIException: exportSim: " << path << ", " << e.what());
        }

        lock.unlock();
        return;
    } else {
        //use the best fit
        KDEBUG("SumoConnectionTraCI::exportSim @ " << time << ", let's have a look at existing snapshots... ");
        for (const auto & entry : std::filesystem::directory_iterator(Config::getInstance()->getOutputDir()))
            std::cout << entry.path() << std::endl;
    }

}

std::string SumoConnectionTraCI::getBoundaries(){
    unique_lock<mutex> lock(tracimutex);
    std::string value = "";
    try {
        for (auto vec : traci->simulation.getNetBoundary() )
            value += vec.getString() +" ";
    } catch (libsumo::TraCIException &e) {
        KERROR("TraCIException: getBoundaries: " << e.what());
    }

    lock.unlock();
    return value;

}

std::string SumoConnectionTraCI::getOffset() {  
    return "getOffset is currently not supported by SUMO";
}

std::string SumoConnectionTraCI::getRoadMap() { 
    return "getRoadMap is currently not supported by SUMO, please have a look at the .sce";
}