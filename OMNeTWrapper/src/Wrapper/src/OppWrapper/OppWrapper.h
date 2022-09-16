/*
 * KafkaConnection.cpp
 *
 *  Created on: Mar 16, 2020
 *      Author: guetlein
 */

#ifndef KafkaConnection_H
#define KafkaConnection_H

#include <iostream>
#include <string>

#include "ProvisionHandlerCommunication11p.h"
#include "InteractionHandlerCommunication11p.h"
#include "OppProvisionImpl.h"
#include "OppInteractionImpl.h"
#include "Scheduler.h"
#include "datamodel/Micro.hh"
#include "datamodel/RadioMsg.hh"
#include "communication/AvroHelper.h"
#include "util/Defines.h"
#include "util/Utils.h"
#include "communication/kafka/KafkaProducer.h"
#include "communication/kafka/KafkaConsumer.h"
#include "veins/modules/mobility/traci/TraCICoord.h"
#include "veins/modules/mobility/traci/TraCICoordinateTransformation.h"
#include "veins/modules/messages/BaseFrame1609_4_m.h"
#include "veins/modules/messages/DemoSafetyMessage_m.h"


namespace daceDS {


class KafkaProducer;
class KafkaConsumer;
class ProvisionHandlerCommunication11p;
class InteractionHandlerCommunication11p;
class OppInteractionImpl;

class OppWrapper : public SimulationWrapper{

public:
    static std::shared_ptr<daceDS::OppWrapper> inst;
    static std::shared_ptr<daceDS::OppWrapper> getInstance();
    static void createInstance(std::string sce, std::string sim);

     std::map<long, std::vector<daceDS::datamodel::Micro>> unreadSubscribtions;
//    std::vector<daceDS::datamodel::Micro> unreadSubscribtions;
    std::vector<daceDS::datamodel::RadioMsg> unreadsendMsgRequests;

    std::shared_ptr<KafkaProducer> producer;
    std::shared_ptr<OppProvisionImpl> provision;
    std::shared_ptr<KafkaConsumer> provisionConsumer;
    std::shared_ptr<ProvisionHandlerCommunication11p> provisionHandler;

    std::shared_ptr<OppInteractionImpl> interaction;
    std::shared_ptr<KafkaConsumer> interactionConsumer;
    std::shared_ptr<InteractionHandlerCommunication11p> interactionHandler;

    OppWrapper(std::string sc, std::string sim) : SimulationWrapper(sc,sim){
        std::cout << "daceDS::OppWrapper()" << std::endl;
    }
    ~OppWrapper(){};

    void init();
    std::string getTopic(std::string channel, std::string topic);
//    void join();
//    void timeAdvance(long time);
    void handlePositionUpdate(int64_t t , int64_t st, std::string vehicleID, double x, double y, double z);
//    void handleTopicSyncMsg(std::string key, c::SyncMsg msg);
    void handleTopicSendRadioMsg(daceDS::datamodel::RadioMsg msg);

    void notifyMsgReceived(daceDS::datamodel::RadioMsg msg);
    void notifyMsgSent(daceDS::datamodel::RadioMsg msg);

    //todo
    void close(){ KINFO("todo: close()");};

    std::unique_ptr<veins::TraCICoordinateTransformation> coordinateTransformation;
    void setNetbounds(veins::TraCICoord netbounds1, veins::TraCICoord netbounds2, int margin);
    veins::Coord traci2omnet(veins::TraCICoord coord) const;
    std::list<veins::Coord> traci2omnet(const std::list<veins::TraCICoord>& list) const;
    veins::TraCICoord omnet2traci(veins::Coord coord) const;
    std::list<veins::TraCICoord> omnet2traci(const std::list<veins::Coord>& list) const;
    veins::Heading traci2omnetHeading(double heading) const;
    double omnet2traciHeading(veins::Heading heading) const;

    std::map<int, std::string> mac2name;

    string getNameForMac(int mac){
         if (mac2name.count(mac)){
             return mac2name[mac];
         }

         std::cout <<"don'T know mac="<<mac<<std::endl;


         return "unknown";
     };
    void setNameForMac(int mac, string name){
        mac2name[mac] = name;
        //  std::cout <<mac <<"~"<<name <<std::endl;
    };








    };
}

#endif
