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
#include "SumoObserver.h"

using namespace daceDS;

SumoObserver::SumoObserver(std::shared_ptr<Producer> p, datamodel::Observer o, std::shared_ptr<SUMO_API_MODE> s) : Observer(p, o) {
    sumoConnection = s;
    topic = Config::getInstance()->getProvisionTopic(observer.element);


    //we want to create the topic beforehand, so that various consumer implementation using regex subscriptions don't get confused
    p->createTopic(topic);

    //we cannot query filter items at this stage, therefore we create topics on suspicion
    if (isPersistentSubject() && observer.filter.size() > 0) {
        for (std::string entityID : split(observer.filter, daceDS::Constants::CHAR_SPLIT_TOKEN)) {
            if (entityID == daceDS::Constants::STR_FILTER_ALL_TOKEN) continue;
            getPersistentTopic(entityID);
        }
    }
}

bool SumoObserver::observe(int64_t time) {
    KDEBUG("observe called @ " << time);
    if (time % observer.period != 0) {
        KDEBUG("not now! skip (period=" << observer.period << ")");
        return;
    }
    observeTime = time;
    bool succ = true;
    KDEBUG("observer.element=" << observer.element);
    KDEBUG("observer.type=" << observer.type);
    //each scope has is own implementation

    std::string scope = observer.element.substr(0, observer.element.find("."));
    KDEBUG("scope=" << scope);

    if (scope == "vehicle") {
        succ = observeVehicle();
    } else if (scope == "edge") {
        succ = observeEdge();
        if(!succ){
            std::cout << "observeedge() returned false" << std::endl;
        }
    } else if (scope == "detector") {
        succ = observeDetector();
        // } else if(observer.Subject == "generic"){
        // 	observeGeneric();
    } else {
        KERROR("scope '" << scope << "' NOT KNOWN");
    }

    KDEBUG("observe done");
    return succ;
}

/*************************************
 * Different observer implementations
 * ***********************************/

bool SumoObserver::observeVehicle() {
    bool succ = true;
    //get recent entity names and apply filters
    std::vector<string> subjects;
    int64_t time;
    sumoConnection->vehicle->getList(subjects, time);
    KDEBUG("got " << subjects.size() << " subjects");
    filterEntities(subjects);
    KDEBUG("got " << subjects.size() << " filtered subjects");

    //gather data
    for (std::string vehID : subjects) {
        KDEBUG("observeVehicle for " << vehID);
        //full struct
        if (observer.element == "vehicle") {
            datamodel::Micro val;
            sumoConnection->vehicle->getVehicle(vehID, val, time);
            succ = send<datamodel::Micro>(vehID, val);
        } else if (observer.element == "vehicle.acceleration") {
            double val;
            sumoConnection->vehicle->getAcceleration(vehID, val, time);
            succ = send<double>(vehID, val);
        } else if (observer.element == "vehicle.angle") {
            double val;
            sumoConnection->vehicle->getAcceleration(vehID, val, time);
            succ = send<double>(vehID, val);
        } else if (observer.element == "vehicle.edge") {
            std::string val;
            sumoConnection->vehicle->getEdgeID(vehID, val, time);
            succ = send<std::string>(vehID, val);
        } else if (observer.element == "vehicle.lane") {
            int val;
            sumoConnection->vehicle->getLaneIndex(vehID, val, time);
            succ = send<int>(vehID, val);
        } else if (observer.element == "vehicle.position") {
            Vec3D val;
            sumoConnection->vehicle->getPosition(vehID, val, time);
            succ = send<Vec3D>(vehID, val);
        } else if (observer.element == "vehicle.positionEdge") {
            double val;
            sumoConnection->vehicle->getLanePosition(vehID, val, time);
            succ = send<double>(vehID, val);
        } else if (observer.element == "vehicle.slope") {
            double val;
            sumoConnection->vehicle->getSlope(vehID, val, time);
            succ = send<double>(vehID, val);
        } else if (observer.element == "vehicle.speed") {
            double val;
            sumoConnection->vehicle->getSpeed(vehID, val, time);
            succ = send<double>(vehID, val);
        }
    }
    return succ;
}

bool SumoObserver::observeEdge() {
    bool succ = true;
    KDEBUG("observeEdge");
    //get recent entity names and apply filters
    //we might also do this only once - edges are persistent
    std::vector<string> subjects = sumoConnection->edge->getIDList();
    // std::cout << observeTime << ": observing "<< subjects.size() << "edges"<<std::endl;
    filterEntities(subjects);
    filterInternalEntities(subjects);

    //gather data
    for (std::string edgeID : subjects) {
        if (observer.element == "edge") {
            auto me = sumoConnection->edge->getEdge(edgeID);
            // std::cout << edgeID << " has " << me.vehicles.size() << " vehicles"<<std::endl;
            succ = send<datamodel::MicroEdge>(edgeID, me);
            if(!succ){
                std::cout <<"send<datamodel::MicroEdge> returned as false" <<std::endl;
            }
        } else if (observer.element == "edge.emission") {
            succ = send<std::string>(edgeID, sumoConnection->edge->getEmissions(edgeID));
        } else if (observer.element == "edge.flow") {
            succ = send<double>(edgeID, sumoConnection->edge->getFlow(edgeID));
        } else if (observer.element == "edge.occupancy") {
            succ = send<double>(edgeID, sumoConnection->edge->getOccupancy(edgeID));
        } else if (observer.element == "edge.speed") {
            succ = send<double>(edgeID, sumoConnection->edge->getSpeed(edgeID));
        }
        else if (observer.element == "edge.vehicles") {
            std::vector<datamodel::Micro> vehicles = sumoConnection->edge->getVehicles(edgeID);
            if(observer.trigger == "not_empty" && vehicles.size()==0){
                continue;
            }
            // succ = send<std::vector<datamodel::Micro>>(edgeID, vehicles);

            //dont use lists
            for(datamodel::Micro vehicle : vehicles){
                succ = succ && send<datamodel::Micro>(edgeID, vehicle);
            }


        } else if (observer.element == "edge.vehicleIDs") {
            std::vector<std::string> ids = sumoConnection->edge->getVehicleIDs(edgeID);
            if(observer.trigger == "not_empty" && ids.size()==0){
                continue;
            }
            succ = send<std::vector<std::string>>(edgeID, ids);
        }
    }
    return succ;
}

bool SumoObserver::observeDetector() {
    bool succ = true;
    std::vector<std::string> e1 = sumoConnection->detector->e1->getIDs();
    filterEntities(e1);
    KDEBUG("applied filter on e1, size of e1 is" << e1.size());
    for (std::string detID : e1) {
        KDEBUG("e1 id=" << detID);
        KDEBUG("Element " << observer.element);
        if (observer.element == "detector") {
            succ = send<datamodel::MicroDetector>(detID, sumoConnection->detector->e1->getDetector(detID));
        } else if (observer.element == "detector.lastStepVehicleNumber") {
            int val = sumoConnection->detector->e1->getLastStepVehicleNumber(detID);
            succ = send<int>(detID, val);
        } else if (observer.element == "detector.lastStepMeanSpeed") {
            double val = sumoConnection->detector->e1->getLastStepMeanSpeed(detID);
            succ = send<double>(detID, val);
        } else if (observer.element == "detector.lastStepVehicleIDs") {
            KDEBUG("evaluated ids");
            std::vector<std::string> val = sumoConnection->detector->e1->getLastStepVehicleIDs(detID);
            if(observer.trigger == "not_empty" && val.size()==0){
                continue;
            }
            KDEBUG("call send for ids");
            succ = send<std::vector<std::string>>(detID, val);
        } else if (observer.element == "detector.lastStepOccupancy") {
            double val = sumoConnection->detector->e1->getLastStepOccupancy(detID);
            succ = send<double>(detID, val);
        } else if (observer.element == "detector.lastStepMeanLength") {
            double val = sumoConnection->detector->e1->getLastStepMeanLength(detID);
            succ = send<double>(detID, val);
        } else if (observer.element == "detector.timeSinceDetection") {
            double val = sumoConnection->detector->e1->getTimeSinceDetection(detID);
            succ = send<double>(detID, val);
        } else {
            KDEBUG("Element " << observer.element << " is not known!");
        }
    }

    std::vector<std::string> e3 = sumoConnection->detector->e3->getIDs();
    filterEntities(e3);
    KDEBUG("applied filter on e3, size is" << e1.size());
    for (std::string detID : e3) {
        KDEBUG("e3 id=" << detID);
        KDEBUG("Element " << observer.element);
        if (observer.element == "") {
            succ = send<datamodel::MicroDetector>(detID, sumoConnection->detector->e3->getDetector(detID));
        } else if (observer.element == "detector.lastStepVehicleNumber") {
            double val = sumoConnection->detector->e3->getLastStepVehicleNumber(detID);
            succ = send<int>(detID, val);
        } else if (observer.element == "detector.lastStepMeanSpeed") {
            double val = sumoConnection->detector->e3->getLastStepMeanSpeed(detID);
            succ = send<double>(detID, val);
        } else if (observer.element == "detector.lastStepVehicleIDs") {
            std::vector<std::string> val = sumoConnection->detector->e3->getLastStepVehicleIDs(detID);
            if(observer.trigger == "not_empty" && val.size()==0){
                continue;
            }
            succ = send<std::vector<std::string>>(detID, val);

        } else if (observer.element == "detector.lastStepHaltingNumber") {
            int val = sumoConnection->detector->e3->getLastStepHaltingNumber(detID);
            succ = send<int>(detID, val);
        }
    }
    return succ;
}

/*************************************
 * Different sender implementations
 * ***********************************/

template <typename T>
bool SumoObserver::send(std::string key, T value) {
    bool succ = true;
    
    #ifdef VIM
    KDEBUG("sending sth with key=" << key << " on " << topic);
    if (observer.type == daceDS::Constants::STR_SERTYPE_JSON) {
        succ = sendAsJSON(topic, key, value);
    } else if (observer.type == daceDS::Constants::STR_SERTYPE_AVRO) {
        succ = sendAsAvro(topic, key, value);
    } else {
        KERROR("Type '" << observer.type << "' NOT KNOWN");
    }
    #endif

    //if persistent, we use specific topicics    
    #ifndef VIM
        std::string ptopic = topic;
         KDEBUG("isPersistentSubject()=" << isPersistentSubject());
        if (isPersistentSubject()) {
            ptopic = getPersistentTopic(key);
            KDEBUG("ptopic=" << ptopic);
        }
        KDEBUG("nv sending sth with key=" << key << " on " << ptopic);
        if (observer.type == daceDS::Constants::STR_SERTYPE_JSON) {
            succ = sendAsJSON(ptopic, key, value);
            
            if(!succ){
                std::cout <<"sendAsJSON returned as false" <<std::endl;
            }
        } else if (observer.type == daceDS::Constants::STR_SERTYPE_AVRO) {
            succ = sendAsAvro(ptopic, key, value);
        } else {
            KERROR("Type '" << observer.type << "' NOT KNOWN");
        }
    
    #endif

    return succ;
}
std::string SumoObserver::getPersistentTopic(std::string key) {
    std::string scope = observer.element.substr(0, observer.element.find("."));
    if (persistentTopics.count(key) == 0) {
        persistentTopics[key] = Config::getInstance()->getPersistentTopic(scope, key, observer.element);
        producer->createTopic(persistentTopics[key]);
    }
    return persistentTopics[key];
}

bool SumoObserver::isPersistentSubject() {
    std::string scope = observer.element.substr(0, observer.element.find("."));
     KDEBUG("scope=" << scope);
    return (scope == "edge" || scope == "detector");
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, std::string value) {
    std::string json = "{\"" + observer.element + "\": \"" + value + "\"}";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, std::vector<std::string> value) {
    std::string json = "{\"" + observer.element + "\": [";
    bool first = true;
    for (auto elem : value) {
        if (first) {
            first = false;
        } else {
            json += ", ";
        }
        json += "\"" + elem + "\"";
    }
    json += "]}";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, std::vector<double> value) {
    std::string json = "{\"" + observer.element + "\": [";
    bool first = true;
    for (auto elem : value) {
        if (first) {
            first = false;
        } else {
            json += ", ";
        }
        json += elem;
    }
    json += "]}";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, std::vector<datamodel::Micro> value) {
    std::string json = "{\"" + observer.element + "\": [";
    bool first = true;
    for (auto micro : value) {
        if (first) {
            first = false;
        } else {
            json += ", ";
        }

        string elem =
            "{"
            "\"acceleration\" : " +
            std::to_string(micro.acceleration) +
            ", "
            "\"angle\" : " +
            std::to_string(micro.angle) +
            ", "
            "\"edge\" :  \"" +
            micro.edge +
            "\", "
            "\"lane\" : " +
            std::to_string(micro.lane) +
            ", "
            "\"position\" : {\"x\":" +
            std::to_string(micro.position.x) + std::string(", \"y\" :") + std::to_string(micro.position.y) + std::string(", \"z\" : ") + std::to_string(micro.position.z) +
            "}, "
            "\"positionEdge\" : " +
            std::to_string(micro.positionEdge) +
            ", "
            "\"route\" : \"" +
            concat(micro.route, ',') +
            "\", "
            "\"slope\" : " +
            std::to_string(micro.slope) +
            ", "
            "\"speed\" : " +
            std::to_string(micro.speed) +
            ", "
            "\"vehicleID\" : \"" +
            micro.vehicleID +
            "\", "
            "\"type\" : \"" +
            micro.type + "\" } ";

        json += elem;
    }
    json += "]}";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, double value) {
    std::string json = "{\"" + observer.element + "\": " + std::to_string(value) + "}";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, int value) {
    std::string json = "{\"" + observer.element + "\": " + std::to_string(value) + "}";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, Vec3D value) {
    std::string json = "{\"" + observer.element + "\": {\"x\":" + std::to_string(value.x) + std::string(", \"y\" :") + std::to_string(value.y) + std::string(", \"z\" : ") + std::to_string(value.z) + "}}";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, datamodel::Micro micro) {
    string json =
        "{"
        "\"acceleration\" : " +
        std::to_string(micro.acceleration) +
        ", "
        "\"angle\" : " +
        std::to_string(micro.angle) +
        ", "
        "\"edge\" :  \"" +
        micro.edge +
        "\", "
        "\"lane\" : " +
        std::to_string(micro.lane) +
        ", "
        "\"position\" : {\"x\":" +
        std::to_string(micro.position.x) + std::string(", \"y\" :") + std::to_string(micro.position.y) + std::string(", \"z\" : ") + std::to_string(micro.position.z) +
        "}, "
        "\"positionEdge\" : " +
        std::to_string(micro.positionEdge) +
        ", "
        "\"route\" : \"" +
        concat(micro.route, ',') +
        "\", "
        "\"slope\" : " +
        std::to_string(micro.slope) +
        ", "
        "\"speed\" : " +
        std::to_string(micro.speed) +
        ", "
        "\"vehicleID\" : \"" +
        micro.vehicleID +
        "\", "
        "\"type\" : \"" +
        micro.type + "\" } ";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, datamodel::MicroEdge value) {
    string json =
        "{"
        "\"edgeID\" : \"" +
        value.edgeID +
        "\", "
        "\"emission\" :  \"" +
        value.emission +
        "\", "
        "\"flow\" : " +
        std::to_string(value.flow) +
        ", "
        "\"occupancy\" : " +
        std::to_string(value.occupancy) +
        ", "
        "\"speed\" : " +
        std::to_string(value.speed) +
        ", "
        "\"vehicleIDs\" : [";

    bool first = true;
    for (auto id : value.vehicleIDs) {
        if (first) {
            first = false;
        } else {
            json += ", ";
        }
        json += "\""+id+"\"";
    }
    json += "], \"vehicles\" : [";

    first = true;
    for (auto micro : value.vehicles) {
        if (first) {
            first = false;
        } else {
            json += ", ";
        }

        string elem =
            "{"
            "\"acceleration\" : " +
            std::to_string(micro.acceleration) +
            ", "
            "\"angle\" : " +
            std::to_string(micro.angle) +
            ", "
            "\"edge\" :  \"" +
            micro.edge +
            "\", "
            "\"lane\" : " +
            std::to_string(micro.lane) +
            ", "
            "\"position\" : {\"x\":" +
            std::to_string(micro.position.x) + std::string(", \"y\" :") + std::to_string(micro.position.y) + std::string(", \"z\" : ") + std::to_string(micro.position.z) +
            "}, "
            "\"positionEdge\" : " +
            std::to_string(micro.positionEdge) +
            ", "
            "\"route\" : \"" +
            concat(micro.route, ',') +
            "\", "
            "\"slope\" : " +
            std::to_string(micro.slope) +
            ", "
            "\"speed\" : " +
            std::to_string(micro.speed) +
            ", "
            "\"vehicleID\" : \"" +
            micro.vehicleID +
            "\", "
            "\"type\" : \"" +
            micro.type + "\" } ";

        json += elem;
    }
    json += "] } ";

    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsJSON(std::string topic, std::string key, datamodel::MicroDetector value) {
    std::string json = "{\"detectorID\": \"" + value.detectorID +
                       "\", "
                       "\"values\" : [";
    bool first = true;
    for (auto v : value.values) {
        if (first) {
            first = false;
        } else {
            json += ", ";
        }
        json += "{\"" + v.first + "\" : ";
        size_t type = v.second.idx();
        switch (v.second.idx()) {
            case 0:
                json += v.second.get_bool() ? "true" : "false";
                break;
            case 1:
                json += std::to_string(v.second.get_double());
                break;
            case 2:
                json += std::to_string(v.second.get_int());
                break;
            case 3:
                json += "\"" + v.second.get_string() + "\"";
                break;
        }
        json += "}";
    }
    json += "]";
    return producer->publish(topic, key, json, observeTime, repeatOnError);
}

bool SumoObserver::sendAsAvro(std::string topic, std::string key, std::string value) {
    return producer->publish(topic, key, value, observeTime, repeatOnError); 
}

bool SumoObserver::sendAsAvro(std::string topic, std::string key, std::vector<std::string> value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeBaseType(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);  
}

bool SumoObserver::sendAsAvro(std::string topic, std::string key, std::vector<double> value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeBaseType(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);  
}

// bool SumoObserver::sendAsAvro(std::string topic, std::string key, std::vector<datamodel::Micro> value) {
//     //std::vector<char> encoded = AvroHelper::getInstance()->encodeListMicro(value);
//     std::vector<char> encoded = AvroHelper::getInstance()->encodeMicroList(value);
//     return producer->publish(topic, key, encoded, observeTime, repeatOnError);  
// }

bool SumoObserver::sendAsAvro(std::string topic, std::string key, double value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeBaseType(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);
}

bool SumoObserver::sendAsAvro(std::string topic, std::string key, int value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeBaseType(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);
}

bool SumoObserver::sendAsAvro(std::string topic, std::string key, Vec3D value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeVec3D(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);
}

bool SumoObserver::sendAsAvro(std::string topic, std::string key, datamodel::Micro value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeMicro(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);
}

bool SumoObserver::sendAsAvro(std::string topic, std::string key, datamodel::MicroEdge value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeMicroEdge(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);
}
bool SumoObserver::sendAsAvro(std::string topic, std::string key, datamodel::MicroDetector value) {
    std::vector<char> encoded = AvroHelper::getInstance()->encodeMicroDetector(value);
    return producer->publish(topic, key, encoded, observeTime, repeatOnError);
}

/*************************************
 * Helper methods
 * ***********************************/
void SumoObserver::filterEntities(std::vector<string>& subjects) {
    //apply filter if defined, todo: make it faster
    if (observer.filter != daceDS::Constants::STR_FILTER_ALL_TOKEN) {
        KDEBUG("filterEntities by " << observer.filter);
        std::vector<string> requestedSubjects = split(observer.filter, daceDS::Constants::CHAR_SPLIT_TOKEN);
        std::vector<string> validSubjects;

        //check if requestedSubjects are available
        for (std::string rS : requestedSubjects) {
            if (std::find(subjects.begin(), subjects.end(), rS) != subjects.end()) {
                validSubjects.push_back(rS);
            }
        }
        subjects = validSubjects;
    } else {
        KDEBUG("all are welcome");
    }
}

void SumoObserver::filterInternalEntities(std::vector<string>& subjects) {
    auto it = subjects.begin();
    while (it != subjects.end()) {
        if (it->at(0) == daceDS::Constants::CHAR_IGNORE_TOKEN) {
            it = subjects.erase(it);
        } else {
            ++it;
        }
    }
}
