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
#include "SumoConnectionLibSUMO.h"
using namespace daceDS;



SumoConnectionLibSUMO::SumoConnectionLibSUMO() {
    vehicle.reset(new SumoConnectionLibSUMO::Vehicle(this));
    edge.reset(new SumoConnectionLibSUMO::Edge(this));
    detector.reset(new SumoConnectionLibSUMO::Detector(this));
}

bool SumoConnectionLibSUMO::connect() {
    return true;
}


void SumoConnectionLibSUMO::setGhostingDisabled(){ 
    ghostingEnabled = false;
}

bool SumoConnectionLibSUMO::getGhostingEnabled(){ 
    return ghostingEnabled;
}


mutex tracimutex;
void SumoConnectionLibSUMO::setOutgoingLinks(vector<std::string> &items) {
    outgoingLinks.insert(outgoingLinks.end(), items.begin(), items.end());
    KDEBUG("i am now responsible for " << outgoingLinks.size() << " borders");
}
void SumoConnectionLibSUMO::setRespLinks(vector<std::string> &items) {
    respLinks.insert(respLinks.end(), items.begin(), items.end());
}

void SumoConnectionLibSUMO::setResponsibleForWholeMap(bool resp) {
    responsibleForWholeMap = resp;
}
bool SumoConnectionLibSUMO::getResponsibleForWholeMap() {
    return responsibleForWholeMap;
}

bool SumoConnectionLibSUMO::isRespLink(std::string edge) {
    return std::find(respLinks.begin(), respLinks.end(), edge) != respLinks.end();
}

vector<std::string> SumoConnectionLibSUMO::getRespLinks() {
    return respLinks;
}

void SumoConnectionLibSUMO::closeSumo() {
    libsumo::Simulation::close();
}

void SumoConnectionLibSUMO::setTime(int64_t fedTimeInMS) {
    try{
        libsumo::Simulation::step(fedTimeInMS); //rodo ms or secs?
        //we need this in every iteration, so update right now
        auto vehList = libsumo::Vehicle::getIDList();
        currentVehicleIDSet.clear();
        std::copy(vehList.begin(),
                vehList.end(),
                std::inserter(currentVehicleIDSet, currentVehicleIDSet.begin()));
        int gso = ghosts.size();

        ghosts.insert(nextRoundGhosts.begin(), nextRoundGhosts.end());
        DSTRAFFIC_CTRL_DBG("added " <<  nextRoundGhosts.size() << " vehicles to ghosts list ("<<gso<<"->"<<ghosts.size()<<")");
        nextRoundGhosts.clear();

    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
        return false;
    }
}
void SumoConnectionLibSUMO::proceedSim(int steps) {
    try{
        for(int i=0;i<steps;i++){
            libsumo::Simulation::step(); 
        }
        //we need this in every iteration, so update right now
        auto vehList = libsumo::Vehicle::getIDList();
        currentVehicleIDSet.clear();
        std::copy(vehList.begin(),
                vehList.end(),
                std::inserter(currentVehicleIDSet, currentVehicleIDSet.begin()));
        int gso = ghosts.size();
        ghosts.insert(nextRoundGhosts.begin(), nextRoundGhosts.end());

        DSTRAFFIC_CTRL_DBG("added " <<  nextRoundGhosts.size() << " vehicles to ghosts list ("<<gso<<"->"<<ghosts.size()<<")");

        nextRoundGhosts.clear();

    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
        return false;
    }
}

int64_t SumoConnectionLibSUMO::getTime() {
    return libsumo::Simulation::getCurrentTime(); // in ms
}

/*************************
*   RELEASE VEHICLES
**************************/


/*
* Finds vehicles that entered an outgoing link 
* - returns them
* - changes their destination to the outgoing link (in order to disappear after leaving the outgoing link)
* - register them as a ghost
*/
std::vector<datamodel::Micro> SumoConnectionLibSUMO::getAndReleaseLeavingVehicles() {
    std::vector<datamodel::Micro> newBorderVehicles;


    for (std::string outgoingEdge : outgoingLinks) {
        std::vector<std::string> outgoingVehicles = libsumo::Edge::getLastStepVehicleIDs(outgoingEdge); 
        for (std::string vehID : outgoingVehicles) {

            //if the vehicle is already processed (i.e., put in the ghosts list) we skip it
            if (isVehicleGhost(vehID)){
                DSTRAFFIC_CTRL_DBG(vehID << " is in ghosts list");
                continue;
            } else {
                DSTRAFFIC_CTRL_DBG(vehID << " is not in ghosts list");
            }

            DSTRAFFIC_CTRL_DBG ( "adding new vehicle to ghosts list: " << vehID);

            datamodel::Micro m;
            int64_t t;
            vehicle->getVehicle(vehID, m, t);
            newBorderVehicles.push_back(m);



            nextRoundGhosts.insert(vehID); // needed for both ghost modes, as we still wawnt to ignore vehs that we already transferred

     
            if(getGhostingEnabled()){
                //since sumo allows only for checking the attributes of the n-1th step we only have the adoption of adding new vehicles all the time for incorp. ghost feedback
                //therefore we need a new route to let the vehicle end its trip in this instance's world after the outgoing edge
                DSTRAFFIC_CTRL_DBG ( "creating route for ghost" );
                string routeID = "route_gen_" + to_string(routecount++);
                libsumo::Route::add(routeID, {outgoingEdge});
                ghostRouteIDs[vehID] = routeID;         
                ghostRoutes[vehID] = outgoingEdge;       
            } 
            //if ghosting is disabled, we simply let the veh disappear after border edge
            else {
                libsumo::Vehicle::changeTarget(vehID, outgoingEdge);
            }
        }
    }
    return newBorderVehicles;
}

bool SumoConnectionLibSUMO::isVehiclePresentInSimulation(std::string vehID) {
    auto av = std::find(currentVehicleIDSet.begin(), currentVehicleIDSet.end(), vehID) != currentVehicleIDSet.end();
    DSTRAFFIC_CTRL_DBG(vehID << " in  currentVehicleIDSet of size "<<  currentVehicleIDSet.size() << ": " <<av);
    return av;
}

//
void SumoConnectionLibSUMO::cleanGhosts() {

    KDEBUG("cleaning ghosts...");
    int oldAmount = ghosts.size();
    // std::string oldghosts = "";
    // for (auto s : ghosts) {
    //     oldghosts += s + ",";
    // }
    // KDEBUG("old ghosts: [" << oldghosts + "]");

    for (auto it = ghosts.begin(); it != ghosts.end(); ) {
        if (!isVehiclePresentInSimulation(*it)) {
            DSTRAFFIC_CTRL_DBG(*it << " not present anymore");
            it = ghosts.erase(it);
        }
        else {
            DSTRAFFIC_CTRL_DBG(*it << " is still present");
            ++it;
        }
    }
    int newAmount = ghosts.size();

    // std::string newghosts = "";
    // for (auto s : ghosts) {
    //     newghosts += s + ",";
    // }
    // KDEBUG("new ghosts: [" << newghosts + "]");


    DSTRAFFIC_CTRL_DBG("removed " << (oldAmount-newAmount) << " ghosts. checked " << oldAmount << " ghosts.");

}

bool SumoConnectionLibSUMO::isVehicleGhost(std::string vehID) {
    return ghosts.count(vehID) > 0;
}


void SumoConnectionLibSUMO::removeAllGhostsFromSim() {
    int removed = 0;
    for (auto it = ghosts.begin(); it != ghosts.end(); ) {
        try{
            libsumo::Vehicle::remove(*it);   
            ++it; 
            removed++;
        } catch (libsumo::TraCIException& e){
            KERROR(e.what());
            DSTRAFFIC_CTRL_DBG ("simulatorTimeMs=" << getTime() <<": could not find '"<<*it<<"'. will ignore it in the future. ");
 
            ghostRoutes.erase(*it);
            ghostRouteIDs.erase(*it);
            it = ghosts.erase(it);
        }
    }

    DSTRAFFIC_CTRL_DBG ("simulatorTimeMs=" << getTime() <<": removed "<< removed<<" ghosts from last iteration ");
}

void SumoConnectionLibSUMO::updateGhost(datamodel::Micro& vehicle) {
    try{
        DSTRAFFIC_CTRL_DBG ("simulatorTimeMs=" << getTime() <<": updating ghost of "<<vehicle.vehicleID<<", setting pos to "<<vehicle.position.x<<"|"<<vehicle.position.y<<", speed to "<< vehicle.speed);

        //variant 1: does not work: still uses the own calculations with diff results
        // libsumo::Vehicle::moveTo(vehicle.vehicleID, vehicle.edge + "_" + std::to_string(vehicle.lane), vehicle.positionEdge);  //according to the doc, this is instantly forced 
        // libsumo::Vehicle::setSpeed(vehicle.vehicleID, vehicle.speed);

        // //variant 2: one step behind
        // libsumo::Vehicle::moveToXY(vehicle.vehicleID, vehicle.edge, vehicle.lane, vehicle.position.x, vehicle.position.y, vehicle.angle, 2); //2:="ignore local road topology -> force position"
        // libsumo::Vehicle::setSpeed(vehicle.vehicleID, vehicle.speed);


        //variant 5: sim to adoption: WORKS!
        auto vehID = vehicle.vehicleID;
        string currentLane = ghostRoutes[vehID] + "_" + to_string(vehicle.lane);
        // libsumo::Vehicle::remove(vehID); // will do that globalle before updating
        libsumo::Vehicle::add(vehID, ghostRouteIDs[vehID], vehicle.type, "now", to_string(vehicle.lane), to_string(vehicle.positionEdge), to_string(vehicle.speed), "current", "max", to_string(vehicle.speed), "", "", "", 0, 0);
        libsumo::Vehicle::setAcceleration(vehID, vehicle.acceleration, 0);
        libsumo::Vehicle::moveTo(vehID, currentLane, vehicle.positionEdge); //if not moveto or movetoxy is called, vehicle is added after next simstep call
        libsumo::Vehicle::setSpeed(vehID, vehicle.speed);

    } catch (libsumo::TraCIException& e){
        KERROR(e.what());
        ghosts.erase(vehicle.vehicleID);
        KERROR("have problems with the processing the ghost of "<<vehicle.vehicleID<<"! removed id from ghosts set in order to ignore it from now on");
    }
}


//this returns fresh vehicles that are still on their adoption link
// in order to provide updates of them for other instances that might have ghosted them
std::vector<datamodel::Micro> SumoConnectionLibSUMO::getAndReleaseFreshlyAdoptedVehicles() {
    std::vector<datamodel::Micro> vehicles;
    for (auto& [edge, vehIDs] : freshlyAdoptedVehicleIDs){
        for (auto it = vehIDs.begin(); it != vehIDs.end(); ) {
            
            datamodel::Micro m;
            int64_t t;
            vehicle->getVehicle(*it, m, t);
            
            //is vehicle not fresh anymore?
            if (edge != m.edge) {
                it = vehIDs.erase(it);
            }
            else {
                ++it;
                vehicles.push_back(m);
            }
        }
    }
    return vehicles;
}

bool SumoConnectionLibSUMO::adopt(double time, datamodel::Micro vehicle) {
    try {
        
        auto route = vehicle.route;
        auto vehID = vehicle.vehicleID;
        auto position = vehicle.positionEdge;
        //todo: check all params!
        if (route.empty()) {
            DSTRAFFIC_TRANSFER_DBG("No route given: Nothing to do");
            return true;
        }


        string currentEdge = route[0];
        string destinationEdge = route[route.size() - 1];

        //todo: right now we only have one lane per edge
        //parent->traci->edge.getLaneNumber(currentEdge)
        string currentLane = currentEdge + "_" + to_string(vehicle.lane);

        DSTRAFFIC_TRANSFER_DBG("currentEdge    : " << currentEdge);
        DSTRAFFIC_TRANSFER_DBG("currentLane   : " << currentLane);
        DSTRAFFIC_TRANSFER_DBG("destinationEdge: " << destinationEdge);
        DSTRAFFIC_TRANSFER_DBG("x: " << vehicle.position.x);
        DSTRAFFIC_TRANSFER_DBG("y: " << vehicle.position.y);
        DSTRAFFIC_TRANSFER_DBG("speed		  : " << vehicle.speed);
        DSTRAFFIC_TRANSFER_DBG("route.begin()  : " << route[0]);
        DSTRAFFIC_TRANSFER_DBG("route.end()	  : " << route[route.size() - 1]);


        if (isVehiclePresentInSimulation(vehID)) {
            DSTRAFFIC_TRANSFER_DBG("already know this car - ignoring");
            return false;
        }

        string routeID = "route_gen_" + to_string(routecount++);
        //needed modification in sendcommand():sumo/utils/traci
        // DSTRAFFIC_TRANSFER_DBG("adding route " << routeID << ": ");
        // DSTRAFFIC_TRANSFER_DBG("adding route " << route[0]);
        libsumo::Route::add(routeID, route);//traci->route.add(routeID, route);
        // DSTRAFFIC_TRANSFER_DBG("adding route done");

        //todo: at the moment we got only cars of on single type: vtype0

        std::string posStr = to_string(position);
        std::string laneStr = to_string(vehicle.lane);
        std::string speedStr = to_string(vehicle.speed);

        if ( vehicle.speed < 0){
            speedStr = "max"; // sim standard
        }

        //bringt uns garnix, weil wir danach ja moveto noch brauchen        
        // if ( position < 0){
        //     posStr = "base"; // sim standard
        // }
        // if ( vehicle.lane < 0){
        //     laneStr = "best"; // sim standard
        // }

        DSTRAFFIC_TRANSFER_DBG("adding vehicle " << vehID << ": rid=" << routeID << ", type="<<vehicle.type<<", departLane="<<laneStr<<", departPos="<<posStr<< ", speed=" << speedStr);

        libsumo::Vehicle::add(vehID, routeID, vehicle.type, "now", laneStr, posStr , speedStr);


        libsumo::Vehicle::setAcceleration(vehID, vehicle.acceleration, 0);
        libsumo::Vehicle::moveTo(vehID, currentLane, position); //if not moveto or movetoxy is called, vehicle is added after next simstep call
        libsumo::Vehicle::setSpeed(vehID, -1);


        DSTRAFFIC_TRANSFER_DBG("succesfully added " << vehID);
        succAdoption(vehID, currentLane);

        //do we already know the edge?
        if(freshlyAdoptedVehicleIDs.count(currentEdge) == 0){
            freshlyAdoptedVehicleIDs[currentEdge] = std::vector<std::string>();
        }
        freshlyAdoptedVehicleIDs[currentEdge].push_back(vehID);
        // KDEBUG("-------------------------------------");
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
        return false;
    }

    return true;
}

void SumoConnectionLibSUMO::succAdoption(std::string vehID, std::string currentLane) {
    adopted++;
}

daceDS::datamodel::MicroDetector SumoConnectionLibSUMO::Detector::E1::getDetector(std::string id) {
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

std::vector<std::string> SumoConnectionLibSUMO::Detector::E1::getIDs() {
    return libsumo::InductionLoop::getIDList();
}

int SumoConnectionLibSUMO::Detector::E1::getLastStepVehicleNumber(std::string id) {
    return libsumo::InductionLoop::getLastStepVehicleNumber(id);
}
double SumoConnectionLibSUMO::Detector::E1::getLastStepMeanSpeed(std::string id) {
    return libsumo::InductionLoop::getLastStepMeanSpeed(id);
}
double SumoConnectionLibSUMO::Detector::E1::getLastStepOccupancy(std::string id) {
    return libsumo::InductionLoop::getLastStepOccupancy(id);
}
double SumoConnectionLibSUMO::Detector::E1::getLastStepMeanLength(std::string id) {
    return libsumo::InductionLoop::getLastStepMeanLength(id);
}
double SumoConnectionLibSUMO::Detector::E1::getTimeSinceDetection(std::string id) {
    return libsumo::InductionLoop::getTimeSinceDetection(id);
}
std::vector<std::string> SumoConnectionLibSUMO::Detector::E1::getLastStepVehicleIDs(std::string id) {
    return libsumo::InductionLoop::getLastStepVehicleIDs(id);
}

daceDS::datamodel::MicroDetector SumoConnectionLibSUMO::Detector::E3::getDetector(std::string id) {
    daceDS::datamodel::MicroDetector ret;
    ret.detectorID = id;
    ret.values["lastStepVehicleNumber"].set_int(getLastStepVehicleNumber(id));
    ret.values["lastStepMeanSpeed"].set_double(getLastStepMeanSpeed(id));
    ret.values["lastStepOccupancy"].set_double(getLastStepHaltingNumber(id));
    ret.values["lastStepVehicleIDs"].set_string(concat(getLastStepVehicleIDs(id), ';'));
    return ret;
}

std::vector<std::string> SumoConnectionLibSUMO::Detector::E3::getIDs() {
    std::vector<std::string> ret;
    // unique_lock<mutex> lock(tracimutex);
    // try {
    //     ret = parent->parent->traci->multientryexit.getIDList();
    // } catch (libsumo::TraCIException &e) {
    //     KERROR(e.what());
    // }
    // lock.unlock();
    return ret;
}
int SumoConnectionLibSUMO::Detector::E3::getLastStepVehicleNumber(std::string id) {
    // unique_lock<mutex> lock(tracimutex);
    // try {
    //     return parent->parent->traci->multientryexit.getLastStepVehicleNumber(id);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR(e.what());
    // }
    // lock.unlock();
    return -1;
}
double SumoConnectionLibSUMO::Detector::E3::getLastStepMeanSpeed(std::string id) {
    // unique_lock<mutex> lock(tracimutex);
    // try {
    //     return parent->parent->traci->multientryexit.getLastStepMeanSpeed(id);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR(e.what());
    // }
    // lock.unlock();
    return -1.0;
}

int SumoConnectionLibSUMO::Detector::E3::getLastStepHaltingNumber(std::string id) {
    // unique_lock<mutex> lock(tracimutex);
    // try {
    //     return parent->parent->traci->multientryexit.getLastStepHaltingNumber(id);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR(e.what());
    // }
    // lock.unlock();
    return -1;
}
std::vector<std::string> SumoConnectionLibSUMO::Detector::E3::getLastStepVehicleIDs(std::string id) {
    std::vector<std::string> ret;
    // unique_lock<mutex> lock(tracimutex);
    // try {
    //     ret = parent->parent->traci->multientryexit.getLastStepVehicleIDs(id);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR(e.what());
    // }
    // lock.unlock();
    return ret;
}

//used by getVehicle
//todo: is this alright?
std::vector<std::string> SumoConnectionLibSUMO::getUpdatedRoute(std::string vehID) {
    std::vector<std::string> updatedroute;
    // unique_lock<mutex> lock(tracimutex);

    try {
        string currentLane, currentEdge;
        currentLane = libsumo::Vehicle::getLaneID(vehID);//traci->vehicle.getLaneID(vehID);
        currentEdge = libsumo::Lane::getEdgeID(currentLane); // traci->lane.getEdgeID(currentLane);
        int routeIndex = libsumo::Vehicle::getRouteIndex(vehID); // traci->vehicle.getRouteIndex(vehID);
        std::vector<std::string> completeRoute = libsumo::Vehicle::getRoute(vehID); //traci->vehicle.getRoute(vehID);
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
    } catch (libsumo::TraCIException &e) {
        KERROR(e.what());
    }

    // lock.unlock();
    return updatedroute;
}


void SumoConnectionLibSUMO::Vehicle::getList(std::vector<std::string> &value, int64_t& time) {
    value = libsumo::Vehicle::getIDList();
    time = parent->getSumoTime();
}

void SumoConnectionLibSUMO::Vehicle::getAcceleration(std::string vehID, double &value, int64_t& time) {
    value = libsumo::Vehicle::getAcceleration(vehID);
    time = parent->getSumoTime();
}

void SumoConnectionLibSUMO::Vehicle::getAngle(std::string vehID, double &value, int64_t& time) {
    value = libsumo::Vehicle::getAngle(vehID);
    time = parent->getSumoTime();
}

void SumoConnectionLibSUMO::Vehicle::getEdgeID(std::string vehID, std::string &value, int64_t& time) {
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_STRING;
    try {
        value = libsumo::Vehicle::getRoadID(vehID);//parent->traci->vehicle.getRoadID(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("libsumo::TraCIException  " << e.what());
    }
    lock.unlock();
}

void SumoConnectionLibSUMO::Vehicle::getLaneIndex(std::string vehID, int &value, int64_t& time) {
    KDEBUG("SumoConnectionLibSUMO::getVehicleLane");
    unique_lock<mutex> lock(tracimutex);
    value = ERROR_INTEGER;
    try {
        value = libsumo::Vehicle::getLaneIndex(vehID);//parent->traci->vehicle.getLaneIndex(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("SocketException: getVehicleLane:  " << e.what());
        KERROR("SocketException: " + vehID);
    }
    lock.unlock();
}

void SumoConnectionLibSUMO::Vehicle::getPosition(std::string vehID, Vec3D &value, int64_t& time) {
    auto p = libsumo::Vehicle::getPosition3D(vehID);
    value.x = p.x;
    value.y = p.y;
    value.z = p.z;
}

void SumoConnectionLibSUMO::Vehicle::getLanePosition(std::string vehID, double &value, int64_t& time) {
    value = libsumo::Vehicle::getLanePosition(vehID);
    time = parent->getSumoTime();
}
void SumoConnectionLibSUMO::Vehicle::getRoute(std::string vehID, std::vector<std::string> &value, int64_t& time) {
    value = libsumo::Vehicle::getRoute(vehID);
    time = parent->getSumoTime();
}

void SumoConnectionLibSUMO::Vehicle::getSlope(std::string vehID, double &value, int64_t& time) {
    value = libsumo::Vehicle::getSlope(vehID);
    time = parent->getSumoTime();
}
void SumoConnectionLibSUMO::Vehicle::getSpeed(std::string vehID, double &value, int64_t& time) {
    value = libsumo::Vehicle::getSpeed(vehID);
    time = parent->getSumoTime();
}

void SumoConnectionLibSUMO::Vehicle::getVehicleClass(std::string vehID, std::string &value, int64_t& time) {
    value = libsumo::Vehicle::getVehicleClass(vehID);
    time = parent->getSumoTime();
}

void SumoConnectionLibSUMO::Vehicle::getVehicle(std::string vehID, datamodel::Micro &value, int64_t& time) {

    try {
        value.acceleration = libsumo::Vehicle::getAcceleration(vehID);
        value.angle =libsumo::Vehicle::getAngle(vehID);
        value.edge = libsumo::Vehicle::getRoadID(vehID);
        value.lane = libsumo::Vehicle::getLaneIndex(vehID);
        auto pos = libsumo::Vehicle::getPosition(vehID);
        value.position.x = pos.x;
        value.position.y = pos.y;
        value.position.z = pos.z;
        value.positionEdge = libsumo::Vehicle::getLanePosition(vehID);

        auto r = libsumo::Vehicle::getRoute(vehID);
        value.route = r;

        value.speed = libsumo::Vehicle::getSpeed(vehID);
        value.vehicleID = vehID;
        value.type = libsumo::Vehicle::getTypeID(vehID);
        time = parent->getSumoTime();
    } catch (libsumo::TraCIException &e) {
        KERROR("libsumo::TraCIException " << e.what());
    }
}

//regarding roads

void SumoConnectionLibSUMO::Edge::getList(std::vector<std::string> &value, int64_t& time) {
    // unique_lock<mutex> lock(tracimutex);

    // try {
        value = libsumo::Edge::getIDList();//parent->traci->edge.getIDList();
        time = parent->getSumoTime();
    // } catch (libsumo::TraCIException &e) {
    //     KERROR(" SumoConnectionLibSUMO::getVehicleList(): failed");
    // } catch (const std::exception &e) {
    //     KERROR(" SumoConnectionLibSUMO::getVehicleList(): failed");
    // }
    // lock.unlock();
}

datamodel::MicroEdge SumoConnectionLibSUMO::Edge::getEdge(std::string edgeID) {
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

std::vector<std::string> SumoConnectionLibSUMO::Edge::getIDList() {
    return libsumo::Edge::getIDList();
}

std::string SumoConnectionLibSUMO::Edge::getEmissions(std::string edgeID) {
    std::string emissionstr = "NOx=" + to_string(libsumo::Edge::getNOxEmission(edgeID));
        emissionstr += ", CO2=" + to_string(libsumo::Edge::getCO2Emission(edgeID));
        emissionstr += ", CO=" + to_string(libsumo::Edge::getCOEmission(edgeID));
        emissionstr += ", HC=" + to_string(libsumo::Edge::getHCEmission(edgeID));
    return emissionstr;
}
double SumoConnectionLibSUMO::Edge::getFlow(std::string edgeID) {
    return libsumo::Edge::getLastStepVehicleNumber(edgeID);  //todo: too simple
}

double SumoConnectionLibSUMO::Edge::getOccupancy(std::string edgeID) {
    return libsumo::Edge::getLastStepOccupancy(edgeID);
}

double SumoConnectionLibSUMO::Edge::getSpeed(std::string edgeID) {
    return libsumo::Edge::getLastStepMeanSpeed(edgeID);;
}

//ignore ghosts!
std::vector<std::string> SumoConnectionLibSUMO::Edge::getVehicleIDs(std::string edgeID) {
    std::vector<std::string> ret = libsumo::Edge::getLastStepVehicleIDs(edgeID);

    KDEBUG(ret.size() << " vehicles on " << edgeID);
    //remove ghosts from list
    ret.erase(std::remove_if(ret.begin(), ret.end(), [this](std::string vehID) {
                    return parent->isVehicleGhost(vehID);
                }),
                ret.end());

    KDEBUG("    " << ret.size() << " vehicles after filtering ghosts");

    return ret;
}

std::vector<datamodel::Micro> SumoConnectionLibSUMO::Edge::getVehicles(std::string edgeID) {
    std::vector<datamodel::Micro> ret;
    for (std::string vehID : getVehicleIDs(edgeID)) {
        datamodel::Micro m;
        int64_t t;
        parent->vehicle->getVehicle(vehID, m, t);
        ret.push_back(m);
    }
    return ret;
}


        void SumoConnectionLibSUMO::Edge::setAllowedTypes(std::string edgeID, int  laneID,  std::vector<std::string> types){
            // unique_lock<mutex> lock(tracimutex);
            //     std::string laneIDStr = edgeID+std::to_string(laneID);
            // try {
            //     parent->traci->lane.setAllowed(laneIDStr, types);
            // } catch (libsumo::TraCIException &e) {
            //     KERROR("TraCIException, setAllowedTypes, " << laneIDStr <<", "<< e.what());
            // }
            // lock.unlock();
            return;
        }

        std::vector<std::string> SumoConnectionLibSUMO::Edge::getAllowedTypes(std::string edgeID,  int laneID){
            // unique_lock<mutex> lock(tracimutex);
            std::vector<std::string> ret;
            //     std::string laneIDStr = edgeID+std::to_string(laneID);
            // try {
            //     ret = parent->traci->lane.getAllowed(laneIDStr);
            // } catch (libsumo::TraCIException &e) {
            //     KERROR("TraCIException, getAllowedTypes, " << laneIDStr <<", "<< e.what());
            // }
            // lock.unlock();
            return ret;
        }

        int SumoConnectionLibSUMO::Edge::getLaneNumber(std::string edgeID){
            // unique_lock<mutex> lock(tracimutex);
            int ret = ERROR_INTEGER;
            // try {
            //     ret = parent->traci->edge.getLaneNumber(edgeID);
            // } catch (libsumo::TraCIException &e) {
            //     KERROR("TraCIException, getLaneNumber, " << edgeID <<", "<< e.what());
            // }
            // lock.unlock();
            return ret;
        }

        double SumoConnectionLibSUMO::Edge::getLaneGradient(std::string edgeID,  int laneID){
            // unique_lock<mutex> lock(tracimutex);
            double ret = ERROR_DOUBLE;
            // std::string laneIDStr = edgeID+std::to_string(laneID);
            // try {                
            //     ret = std::stod(parent->traci->lane.getParameter(laneIDStr, "slope"));
            // } catch (libsumo::TraCIException &e) {
            //     KERROR("TraCIException, getLaneGradient, " << laneIDStr <<", "<< e.what());
            // }
            // lock.unlock();
            return ret;
        }

        double SumoConnectionLibSUMO::Edge::getLaneLength(std::string edgeID,  int laneID){
            // unique_lock<mutex> lock(tracimutex);
            double ret = ERROR_DOUBLE;
            //     std::string laneIDStr = edgeID+std::to_string(laneID);
            // try {
            //     ret = parent->traci->lane.getLength(laneIDStr);
            // } catch (libsumo::TraCIException &e) {
            //     KERROR("TraCIException, getLaneLength, " << laneIDStr <<", "<< e.what());
            // }
            // lock.unlock();
            return ret;
        }

        double SumoConnectionLibSUMO::Edge::getLaneWidth(std::string edgeID,  int laneID){
            // unique_lock<mutex> lock(tracimutex);
            double ret = ERROR_DOUBLE;
            //     std::string laneIDStr = edgeID+std::to_string(laneID);
            // try {
            //     ret = parent->traci->lane.getWidth(laneIDStr);
            // } catch (libsumo::TraCIException &e) {
            //     KERROR("TraCIException, getLaneWidth, " << laneIDStr <<", "<< e.what());
            // }
            // lock.unlock();
            return ret;
        }



/*
##### SETTERS #####
*/
void SumoConnectionLibSUMO::Vehicle::setAcceleration(std::string vehID, double s) {
    KERROR("SumoConnectionLibSUMO::setVehicleAcceleration not implemented");
    return;
}
void SumoConnectionLibSUMO::Vehicle::setAngle(std::string vehID, double angle) {
    // KDEBUG("SumoConnectionLibSUMO::setVehicleAcceleration " << vehID << " " << angle);

    // auto edge = "";  //getVehicleRoadID(vehID);
    // auto lane = 0;   //getVehicleLane(vehID);
    // Vec3D pos;
    // int64_t t;
    // getPosition(vehID, pos, t);
    // int keepRoute = 1;  //1: closest position within the existing route will be used - otherwise route will be overwritten
    // unique_lock<mutex> lock(tracimutex);
    // KDEBUG("SumoConnectionLibSUMO::setVehicleAcceleration pos = " << pos.x << "|" << pos.y);

    // try {
    //     /* traci docum.: edgeID and lane are optional placement hints to resolve ambiguities */
    //     parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, angle, keepRoute);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("SocketException: setVehicleAcceleration:  " << e.what());
    //     KERROR("SocketException: " + vehID);
    // }

    // lock.unlock();
    return;
}
void SumoConnectionLibSUMO::Vehicle::setEdgeID(std::string vehID, std::string edge) {
    KDEBUG("SumoConnectionLibSUMO::setRoad " << vehID << " " << edge);
    // unique_lock<mutex> lock(tracimutex);
    int lane = -1;
    double lanePosition = 0;
    //auto pos = libsumo::Simulation::Convert2D(edge,lanePosition);//parent->traci->simulation.convert2D(edge, lanePosition);
    libsumo::TraCIPosition pos = libsumo::Simulation::convert2D(edge,  lanePosition, 0, false);
    int keeproute = 2;  //2: can move anywhere, but route may be overwritten

    KDEBUG("SumoConnectionLibSUMO::setRoad pos " << pos.x << " " << pos.y);
    // try {
        /* traci docum.: edgeID and lane are optional placement hints to resolve ambiguities */
        libsumo::Vehicle::moveToXY(vehID, edge, lane, pos.x, pos.y, libsumo::INVALID_DOUBLE_VALUE, keeproute);
        // parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, libsumo::INVALID_DOUBLE_VALUE, keeproute);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("SocketException: setRoad:  " << e.what());
    //     KERROR("SocketException: " + vehID);
    // }

    // lock.unlock();
    return;
}
void SumoConnectionLibSUMO::Vehicle::setLaneIndex(std::string vehID, int lane) {
    // KDEBUG("SumoConnectionLibSUMO::setVehicleLane " << vehID << " " << lane);
    // std::string edge;
    // double angle;
    // Vec3D pos;
    // int64_t t;
    // getEdgeID(vehID, edge, t);
    // getAngle(vehID, angle, t);
    // getPosition(vehID, pos, t);
    // int keepRoute = 1;  //1: closest position within the existing route will be used - otherwise route will be overwritten
    // unique_lock<mutex> lock(tracimutex);

    // try {
    //     parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, angle, keepRoute);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("SocketException: setVehicleLane:  " << e.what());
    //     KERROR("SocketException: " + vehID);
    // }

    // lock.unlock();
    // return;
}

void SumoConnectionLibSUMO::Vehicle::setPosition(std::string vehID, Vec3D pos) {
    // KDEBUG("SumoConnectionLibSUMO::setVehiclePosition " << vehID << " " << pos.x << "|" << pos.y);
    // unique_lock<mutex> lock(tracimutex);
    // string edge = "";
    // int lane = 0;
    // double angle = libsumo::INVALID_DOUBLE_VALUE;
    // int keeproute = 2;  //2: can move anywhere, but route may be overwritten

    // try {
    //     /* traci docum.: edgeID and lane are optional placement hints to resolve ambiguities */
    //     parent->traci->vehicle.moveToXY(vehID, edge, lane, pos.x, pos.y, angle, keeproute);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("SocketException: setVehiclePosition:  " << e.what());
    //     KERROR("SocketException: " + vehID);
    // }

    // lock.unlock();
    // return;
}

void SumoConnectionLibSUMO::Vehicle::setLanePosition(std::string vehID, double x) {
    // KDEBUG("SumoConnectionLibSUMO::setVehiclePositionEdge " << vehID << " " << x);
    // std::string edge;
    // int lane;
    // int64_t t;
    // getEdgeID(vehID, edge, t);
    // getLaneIndex(vehID, lane, t);
    // int keepRoute = 1;  //1: closest position within the existing route will be used - otherwise route will be overwritten
    // unique_lock<mutex> lock(tracimutex);
    // string laneID = edge + "_" + to_string(lane);

    // KDEBUG("SumoConnectionLibSUMO::setVehiclePositionEdge laneID " << laneID);
    // try {
    //     parent->traci->vehicle.moveTo(vehID, laneID, x);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("SocketException: setVehiclePositionEdge:  " << e.what());
    //     KERROR("SocketException: " + vehID);
    // }
    // lock.unlock();

    // return;
}

void SumoConnectionLibSUMO::Vehicle::setRoute(std::string vehID, std::vector<std::string> x) {
    // KDEBUG("SumoConnectionLibSUMO::setVehicleRoute " << vehID);
    // if (x.empty()) return;
    // //veh needs to be on the inital edge
    // std::string edge;
    // int64_t t;
    // getEdgeID(vehID, edge, t);
    // if (edge != x[0]) {
    //     setEdgeID(vehID, x[0]);
    // }
    // unique_lock<mutex> lock(tracimutex);

    // try {
    //     parent->traci->vehicle.setRoute(vehID, x);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("SocketException: setVehicleRoute:  " << e.what());
    //     KERROR("SocketException: " + vehID);
    // }

    // lock.unlock();
    // return;
}

void SumoConnectionLibSUMO::Vehicle::setSpeed(std::string vehID, double s) {
    // KDEBUG("SumoConnectionLibSUMO::setVehicleSpeed " << vehID << " " << s);
    // unique_lock<mutex> lock(tracimutex);

    // try {
    //     parent->traci->vehicle.setSpeed(vehID, s);
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("SocketException: setVehicleSpeed:  " << e.what());
    //     KERROR("SocketException: " + vehID);
    // }

    // lock.unlock();
    // return;
}


std::string SumoConnectionLibSUMO::getNewSaveStatePath(){
    return Config::getInstance()->getOutputDir()+"/state_"+std::to_string(getSumoTime())+"_"+std::to_string(saveStateCounter++)+".sbx";
}

void SumoConnectionLibSUMO::exportSim(int64_t time){
    // unique_lock<mutex> lock(tracimutex);

    // //do we want it now?
    // if(time < 0){
    //     const std::string path = getNewSaveStatePath();
    //     try {
    //         traci->simulation.saveState(path); 
    //         KDEBUG("saved snapshot to " << path);
        
    //     } catch (libsumo::TraCIException &e) {
    //         KERROR("TraCIException: exportSim: " << path << ", " << e.what());
    //     }

    //     lock.unlock();
    //     return;
    // } else {
    //     //use the best fit
    //     KDEBUG("SumoConnectionLibSUMO::exportSim @ " << time << ", let's have a look at existing snapshots... ");
    //     for (const auto & entry : std::filesystem::directory_iterator(Config::getInstance()->getOutputDir()))
    //         std::cout << entry.path() << std::endl;
    // }

}

std::string SumoConnectionLibSUMO::getBoundaries(){
    // unique_lock<mutex> lock(tracimutex);
    std::string value = "";
    // try {
    //     for (auto vec : traci->simulation.getNetBoundary() )
    //         value += vec.getString() +" ";
    // } catch (libsumo::TraCIException &e) {
    //     KERROR("TraCIException: getBoundaries: " << e.what());
    // }

    // lock.unlock();
    return value;

}

std::string SumoConnectionLibSUMO::getOffset() {  
    return "getOffset is currently not supported by SUMO";
}

std::string SumoConnectionLibSUMO::getRoadMap() { 
    return "getRoadMap is currently not supported by SUMO, please have a look at the .sce";
}