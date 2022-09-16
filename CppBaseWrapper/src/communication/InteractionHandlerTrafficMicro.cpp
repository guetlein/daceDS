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
#include "InteractionHandlerTrafficMicro.h"
using namespace daceDS;

InteractionHandlerTrafficMicro::InteractionHandlerTrafficMicro(std::shared_ptr<Interaction> i, std::shared_ptr<Producer> p) {
    api = i;
    producer = p;
}

bool InteractionHandlerTrafficMicro::handle(ConsumedMessage* msg) {

    InteractionHandler::handle(msg);

    KDBGCB("Got msg on topic " << msg->topic);

    // when to process a message that lies between to simulator timesteps?
    // //if timestamp is in future, then put in buffer
    // if(msg.timestamp > timeSync.getLBTSinMs()){
    //     buffer.push_back(msg);
    // }
    // //if timestamp is in past, there is some serious problem. should be avoided by timesync
    // if(msg.timestamp < timeSync.getLBTSinMs()){
    //     KERROR("Received message from the past");
    //     exit(1);
    // }

    //decode metadata for interaction channel
    TopicMetadata meta = AvroHelper::getInstance()->getTopicMetadata(msg->topic);
    //InteractionTopicMetadata imeta = AvroHelper::getInstance()->getInteractionTopicMetadata(meta);
    InteractionTopicMetadata2 imeta2 = AvroHelper::getInstance()->getInteractionTopicMetadata2(meta);

    bool succ = false;
    //this will be infered from layer description
    // switch (imeta.scope) {
    //     case Scope::VEHICLE:
    //         parseScopeVehicle(msg, imeta);

    //         break;

    //     case Scope::EDGE:
    //         break;

    //     default:
    //         parseScopeWorld(msg, imeta);
    // }

    if(imeta2.hierarchy.size() == 0){
        parseScopeWorld(msg, imeta2);
    } else {

    
        KDBGCB("h0: " << imeta2.hierarchy[0]);

        if (imeta2.hierarchy[0] == "vehicle"){
            parseScopeVehicle(msg, imeta2);
        } else if (imeta2.hierarchy[0] == "edge"){

        } else { //then h0 must be compund or attr of world scope
            parseScopeWorld(msg, imeta2);
        }
    }
    return true;
}

// bool InteractionHandler::handleBuffer(int64_t simTime) {
//     for(ConsumedMessage& msg : buffer){

//     }
// }

/*
void InteractionHandler::parseScopeBase(ConsumedMessage& msg, InteractionTopicMetadata& imeta) {
    std::string msgkey = msg->key;
    switch (imeta.attribute) {
        case BASE::ExportSim:
            switch (imeta.method) {
                case Method::GET:
                    api->vehicle->getAcceleration(vehicleID);
                    break;
                case Method::SET:
                    double x;
                    AvroHelper::getInstance()->decodeBaseType(*msg, x);
                    api->Interaction::vehicle->setAcceleration(vehicleID, x);
                    break;
            }
            break;
}
*/

// void InteractionHandler::parseScopeVehicle(ConsumedMessage& msg, InteractionTopicMetadata& imeta) {
//     std::string vehicleID = msg->key;
//     KDEBUG("msg->key is " << msg->key);

//     std::vector<char> encodedOutput;
//     int64_t observeTime;

//     switch (imeta.attribute) {
//         case Attribute::ACCELERATION: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     double val;
//                     api->vehicle->getAcceleration(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//                 case Method::SET: {
//                     double x;
//                     AvroHelper::getInstance()->decodeBaseType(*msg, x);
//                     api->Interaction::vehicle->setAcceleration(vehicleID, x);
//                 } break;
//             }
//         } break;

//         case Attribute::ANGLE: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     double val;
//                     api->Interaction::vehicle->getAngle(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//                 case Method::SET: {
//                     double x;
//                     AvroHelper::getInstance()->decodeBaseType(*msg, x);
//                     api->Interaction::vehicle->setAngle(vehicleID, x);
//                 } break;
//             }
//         } break;

//         case Attribute::EDGE: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     std::string val;
//                     api->Interaction::vehicle->getEdge(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//                 case Method::SET:  // useful???
//                 {
//                     std::string edge;
//                     AvroHelper::getInstance()->decodeBaseType(*msg, edge);
//                     api->Interaction::vehicle->setEdge(vehicleID, edge);
//                 } break;
//             }
//         } break;

//         case Attribute::LANE: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     int val;
//                     api->Interaction::vehicle->getLane(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//                 case Method::SET: {
//                     int x;
//                     AvroHelper::getInstance()->decodeBaseType(*msg, x);
//                     api->Interaction::vehicle->setLane(vehicleID, x);
//                 } break;
//             }
//         } break;

//         case Attribute::POSITION: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     Vec3D val;
//                     api->Interaction::vehicle->getPosition(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeVec3D(val);
//                 } break;
//                 case Method::SET: {
//                     Vec3D pos;
//                     AvroHelper::getInstance()->decodeVec3D(msg, pos);
//                     api->Interaction::vehicle->setPosition(vehicleID, pos);
//                 } break;
//             }
//         } break;

//         case Attribute::POSITIONEDGE: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     double val;
//                     api->Interaction::vehicle->getPositionEdge(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//                 case Method::SET: {
//                     double x;
//                     AvroHelper::getInstance()->decodeBaseType(*msg, x);
//                     api->Interaction::vehicle->setPositionEdge(vehicleID, x);
//                 } break;
//             }
//         } break;

//         case Attribute::ROUTE: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     std::vector<std::string> val;
//                     api->Interaction::vehicle->getRoute(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//                 case Method::SET: {
//                     std::vector<std::string> route;
//                     AvroHelper::getInstance()->decodeBaseType(*msg, route);
//                     api->Interaction::vehicle->setRoute(vehicleID, route);
//                 } break;
//             }
//         } break;

//         case Attribute::SPEED: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     double val;
//                     api->Interaction::vehicle->getSpeed(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;

//                 case Method::SET: {
//                     double x;
//                     AvroHelper::getInstance()->decodeBaseType(*msg, x);
//                     api->Interaction::vehicle->setSpeed(vehicleID, x);
//                 } break;
//             }
//         } break;

//         case Attribute::SLOPE: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     double val;
//                     api->Interaction::vehicle->getSlope(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//             }
//         } break;

//         case Attribute::TYPE: {
//             switch (imeta.method) {
//                 case Method::GET: {
//                     std::string val;
//                     api->Interaction::vehicle->getType(vehicleID, val, observeTime);
//                     encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
//                 } break;
//             }
//         } break;

//         default:
//             KERROR("imeta.attribute is not known");
//     }

//     //was there something to return? //todo: err && setter err
//     if (imeta.method == Method::GET) {
//         std::string retTopic = msg->topic;
//         retTopic = retTopic.substr(0, retTopic.size() - 4);
//         retTopic += ".ret";  //todo: do this in getTopicMetdata data and handle special cases
//         producer->publish(retTopic, msg->key, encodedOutput, observeTime);
//     }
// }


//todo: this is not so nice, just for demo purposes
void InteractionHandlerTrafficMicro::parseScopeVehicle(ConsumedMessage* msg, InteractionTopicMetadata2& imeta) {
    std::string vehicleID = msg->key;
    KDBGCB("msg->key is " << msg->key);

    std::vector<char> encodedOutput;
    int64_t observeTime;

    //lvl 0 action
    if(imeta.hierarchy.size() == 1){

    } else {
        std::string lvl1 = imeta.hierarchy[1];
        if(lvl1 == "acceleration") {
            switch (imeta.method) {
                case Method::GET: {
                    double val;
                    api->vehicle->getAcceleration(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
                case Method::SET: {
                    double x;
                    AvroHelper::getInstance()->decodeBaseType(*msg, x);
                    api->Interaction::vehicle->setAcceleration(vehicleID, x);
                } break;
            }
        } 
        else if(lvl1 == "angle") {
            switch (imeta.method) {
                case Method::GET: {
                    double val;
                    api->Interaction::vehicle->getAngle(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
                case Method::SET: {
                    double x;
                    AvroHelper::getInstance()->decodeBaseType(*msg, x);
                    api->Interaction::vehicle->setAngle(vehicleID, x);
                } break;
            }
        }
        else if(lvl1 == "edge") {
            switch (imeta.method) {
                case Method::GET: {
                    std::string val;
                    api->Interaction::vehicle->getEdge(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
                case Method::SET:  // useful???
                {
                    std::string edge;
                    AvroHelper::getInstance()->decodeBaseType(*msg, edge);
                    api->Interaction::vehicle->setEdge(vehicleID, edge);
                } break;
            }
        } 
        else if(lvl1 == "lane") {
            switch (imeta.method) {
                case Method::GET: {
                    int val;
                    api->Interaction::vehicle->getLane(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
                case Method::SET: {
                    int x;
                    AvroHelper::getInstance()->decodeBaseType(*msg, x);
                    api->Interaction::vehicle->setLane(vehicleID, x);
                } break;
            }
        } 
        else if(lvl1 == "position") {
            switch (imeta.method) {
                case Method::GET: {
                    Vec3D val;
                    api->Interaction::vehicle->getPosition(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeVec3D(val);
                } break;
                case Method::SET: {
                    Vec3D pos;
                    AvroHelper::getInstance()->decodeVec3D(*msg, pos);
                    api->Interaction::vehicle->setPosition(vehicleID, pos);
                } break;
            }
        } 
        else if(lvl1 == "positionEdge") {
            switch (imeta.method) {
                case Method::GET: {
                    double val;
                    api->Interaction::vehicle->getPositionEdge(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
                case Method::SET: {
                    double x;
                    AvroHelper::getInstance()->decodeBaseType(*msg, x);
                    api->Interaction::vehicle->setPositionEdge(vehicleID, x);
                } break;
            }
        } 
        else if(lvl1 == "route") {
            switch (imeta.method) {
                case Method::GET: {
                    std::vector<std::string> val;
                    api->Interaction::vehicle->getRoute(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
                case Method::SET: {
                    std::vector<std::string> route;
                    AvroHelper::getInstance()->decodeBaseType(*msg, route);
                    api->Interaction::vehicle->setRoute(vehicleID, route);
                } break;
            }
        } 
        else if(lvl1 == "speed") {
            switch (imeta.method) {
                case Method::GET: {
                    double val;
                    api->Interaction::vehicle->getSpeed(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;

                case Method::SET: {
                    double x;
                    AvroHelper::getInstance()->decodeBaseType(*msg, x);
                    api->Interaction::vehicle->setSpeed(vehicleID, x);
                } break;
            }
        } 
        else if(lvl1 == "slope") {
            switch (imeta.method) {
                case Method::GET: {
                    double val;
                    api->Interaction::vehicle->getSlope(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
            }
        } 
        else if(lvl1 == "type") {
            switch (imeta.method) {
                case Method::GET: {
                    std::string val;
                    api->Interaction::vehicle->getType(vehicleID, val, observeTime);
                    encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
                } break;
            }
        } 
        else {
            KERROR("lvl1 '" << lvl1 << "' is not known");
        }
    }

    //was there something to return? //todo: err && setter err
    if (imeta.method == Method::GET) {
        std::string retTopic = msg->topic;
        retTopic = retTopic.substr(0, retTopic.size() - 4);
        retTopic += ".ret";  //todo: do this in getTopicMetdata data and handle special cases
        producer->publish(retTopic, msg->key, encodedOutput, observeTime);
    }
}


void InteractionHandlerTrafficMicro::parseScopeWorld(ConsumedMessage* msg, InteractionTopicMetadata2& imeta) {
    std::string vehicleID = msg->key;
    KDBGCB("parseScopeWorld msg->key is " << msg->key);
    for(auto s : imeta.hierarchy){
        KDEBUG(s);
    }

    std::vector<char> encodedOutput;
    int64_t observeTime;

    //lvl 0 actions
    if(imeta.hierarchy.size()==0 && imeta.action == "exportSim") {
        KDBGCB("action='"+imeta.action+"'");
        api->exportSim(-1);
        KDBGCB("action done!");
    } 
    //lvl 0 primitive
    else if(imeta.hierarchy.size()==1){
        if(imeta.hierarchy[0] == "simTime"){
            if(imeta.method==Method::GET){
                int val = api->getSimTime();
                encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
            } else {
                KDBGCB("no method '" << (int)imeta.method << "' for " << imeta.hierarchy[0]);
            }
        }
        else if(imeta.hierarchy[0] == "offset"){
            if(imeta.method==Method::GET){
                std::string val = api->getOffset();
                encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
            } else {
                KDBGCB("no method '" <<  (int)imeta.method << "' for " << imeta.hierarchy[0]);
            }
        }
        else if(imeta.hierarchy[0] == "boundaries"){
            if(imeta.method==Method::GET){
                std::string val = api->getBoundaries();
                encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
            } else {
                KDBGCB("no method '" <<  (int)imeta.method << "' for " << imeta.hierarchy[0]);
            }
        }
        else if(imeta.hierarchy[0] == "roadMap"){
            if(imeta.method==Method::GET){
                std::string val = api->getRoadMap();
                encodedOutput = AvroHelper::getInstance()->encodeBaseType(val);
            } else {
                KDBGCB("no method '" <<  (int)imeta.method << "' for " << imeta.hierarchy[0]);
            }
        }

    }

    //was there something to return? //todo: err && setter err
    if (imeta.method == Method::GET) {
        std::string retTopic = msg->topic;
        retTopic = retTopic.substr(0, retTopic.size() - 4);
        retTopic += ".ret";  //todo: do this in getTopicMetdata data and handle special cases
        producer->publish(retTopic, msg->key, encodedOutput, observeTime);
    }
}