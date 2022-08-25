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
#include "InteractionImpl.h"
using namespace daceDS;

std::vector<std::string> InteractionImpl::methods = {
    "vehicle.vehicleID.get",
    "vehicle.acceleration.get",
    "vehicle.angle.get",
    "vehicle.road.get",
    "vehicle.lane.get",
    "vehicle.position.get",
    "vehicle.positionEdge.get",
    "vehicle.route.get",
    "vehicle.speed.get",
    "vehicle.type.get",
    "vehicle.acceleration.set",
    "vehicle.angle.set",
    "vehicle.road.set",
    "vehicle.lane.set",
    "vehicle.position.set",
    "vehicle.positionEdge.set",
    "vehicle.route.set",
    "vehicle.speed.set",
    "exportSim",
    "simTime.get",
    "boundaries.get",
    "offset.get",
    "roadMap.get"};

void InteractionImpl::Vehicle::getList(std::vector<std::string>& value, int64_t & time) {
    sumoConnection->vehicle->getList(value, time);
}

void InteractionImpl::Vehicle::getAcceleration(std::string vehID, double& value, int64_t & time) {
    sumoConnection->vehicle->getAcceleration(vehID, value, time);
}

void InteractionImpl::Vehicle::getAngle(std::string vehID, double& value, int64_t & time) {
    sumoConnection->vehicle->getAngle(vehID, value, time);
}

void InteractionImpl::Vehicle::getEdge(std::string vehID, std::string& value, int64_t & time) {
    sumoConnection->vehicle->getEdgeID(vehID, value, time);
}

void InteractionImpl::Vehicle::getLane(std::string vehID, int& value, int64_t & time) {
    sumoConnection->vehicle->getLaneIndex(vehID, value, time);
}

void InteractionImpl::Vehicle::getPosition(std::string vehID, Vec3D& value, int64_t & time) {
    sumoConnection->vehicle->getPosition(vehID, value, time);
}

void InteractionImpl::Vehicle::getPositionEdge(std::string vehID, double& value, int64_t & time) {
    sumoConnection->vehicle->getLanePosition(vehID, value, time);
}

void InteractionImpl::Vehicle::getRoute(std::string vehID, std::vector<std::string>& value, int64_t & time) {
    sumoConnection->vehicle->getRoute(vehID, value, time);
}

void InteractionImpl::Vehicle::getSlope(std::string vehID, double& value, int64_t & time) {
    sumoConnection->vehicle->getSlope(vehID, value, time);
}

void InteractionImpl::Vehicle::getSpeed(std::string vehID, double& value, int64_t & time) {
    sumoConnection->vehicle->getSpeed(vehID, value, time);
}

void InteractionImpl::Vehicle::getType(std::string vehID, std::string& value, int64_t & time) {
    sumoConnection->vehicle->getVehicleClass(vehID, value, time);
}

void InteractionImpl::Vehicle::getVehicle(std::string vehID, datamodel::Micro& value, int64_t & time) {
    sumoConnection->vehicle->getVehicle(vehID, value, time);
}

//regarding roads
std::vector<std::string> InteractionImpl::Edge::getEdgeList() {
    return sumoConnection->edge->getIDList();
}

std::string InteractionImpl::Edge::getEmissions(std::string edgeID) {
    return sumoConnection->edge->getEmissions(edgeID);
}
double InteractionImpl::Edge::getFlow(std::string edgeID) {
    return sumoConnection->edge->getFlow(edgeID);
}
double InteractionImpl::Edge::getOccupancy(std::string edgeID) {
    return sumoConnection->edge->getOccupancy(edgeID);
}
double InteractionImpl::Edge::getSpeed(std::string edgeID) {
    return sumoConnection->edge->getSpeed(edgeID);
}
std::vector<std::string> InteractionImpl::Edge::getVehicleIDs(std::string edgeID) {
    return sumoConnection->edge->getVehicleIDs(edgeID);
}

void InteractionImpl::Edge::setAllowedTypes(std::string edgeID, int laneID, std::vector<std::string> types){
    sumoConnection->edge->setAllowedTypes(edgeID, laneID, types);
}

std::vector<std::string> InteractionImpl::Edge::getAllowedTypes(std::string edgeID, int laneID){
    return sumoConnection->edge->getAllowedTypes(edgeID, laneID);
}

int InteractionImpl::Edge::getLaneNumber(std::string edgeID){
    return sumoConnection->edge->getLaneNumber(edgeID);
}

double InteractionImpl::Edge::getLaneGradient(std::string edgeID, int laneID){
    return sumoConnection->edge->getLaneGradient(edgeID, laneID);
}

double InteractionImpl::Edge::getLaneLength(std::string edgeID, int laneID){
    return sumoConnection->edge->getLaneLength(edgeID, laneID);
}

double InteractionImpl::Edge::getLaneWidth(std::string edgeID, int laneID){
    return sumoConnection->edge->getLaneWidth(edgeID, laneID);
}



/*
##### SETTERS #####
*/
void InteractionImpl::Vehicle::setAcceleration(std::string vehID, double s) {
    sumoConnection->vehicle->setAcceleration(vehID, s);
}
void InteractionImpl::Vehicle::setAngle(std::string vehID, double angle) {
    sumoConnection->vehicle->setAngle(vehID, angle);
    return;
}

void InteractionImpl::Vehicle::setEdge(std::string vehID, std::string edge) {
    sumoConnection->vehicle->setEdgeID(vehID, edge);
}

void InteractionImpl::Vehicle::setLane(std::string vehID, int lane) {
    sumoConnection->vehicle->setLaneIndex(vehID, lane);
}

void InteractionImpl::Vehicle::setPosition(std::string vehID, Vec3D pos) {
    sumoConnection->vehicle->setPosition(vehID, pos);
}

void InteractionImpl::Vehicle::setPositionEdge(std::string vehID, double x) {
    sumoConnection->vehicle->setLanePosition(vehID, x);
}

void InteractionImpl::Vehicle::setRoute(std::string vehID, std::vector<std::string> x) {
    sumoConnection->vehicle->setRoute(vehID, x);
}

void InteractionImpl::Vehicle::setSpeed(std::string vehID, double s) {
    sumoConnection->vehicle->setSpeed(vehID, s);
}


//world
int64_t InteractionImpl::getSimTime() { 
    return sumoConnection->getTime();
}
std::string InteractionImpl::getBoundaries() { 
    return sumoConnection->getBoundaries();
}
std::string InteractionImpl::getOffset() {  
    return sumoConnection->getOffset();
};

std::string InteractionImpl::getRoadMap() { 
    return sumoConnection->getRoadMap();
};

void InteractionImpl::exportSim(int64_t time){
     sumoConnection->exportSim(time);
}