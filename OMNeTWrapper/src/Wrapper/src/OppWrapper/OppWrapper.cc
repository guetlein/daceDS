/*
 * KafkadaceDS::Connection.cpp
 *
 *  Created on: Mar 16, 2020
 *      Author: guetlein
 */


#include "OppWrapper.h"


#define CONFIG_OPP_PRODUCER_SUFFIX "OPP_PRODUCER_SUFFIX"
#define CONFIG_OPP_CONSUMER_SUFFIX "OPP_CONSUMER_SUFFIX"
#define CONFIG_OPP_TOPIC_SEND "OPP_TOPIC_SEND"
#define CONFIG_OPP_TOPIC_RECEIVE "OPP_TOPIC_RECEIVE"

std::shared_ptr<daceDS::OppWrapper> daceDS::OppWrapper::inst = 0;

void daceDS::OppWrapper::createInstance(std::string sce, std::string sim){
    inst = std::make_shared<daceDS::OppWrapper>(sce,sim);
}

std::shared_ptr<daceDS::OppWrapper> daceDS::OppWrapper::getInstance(){
    return inst;
}

void daceDS::OppWrapper::init(){
    KINFO("subscribing");

    if(!Config::getInstance()->readConfig(CONFIG_PROPERTIES)){
        KERROR(GetCurrentWorkingDir() << CONFIG_PROPERTIES <<"! Exiting...");
        exit(1);
    }
    std::string host = Config::getInstance()->get(CONFIG_KAFKABROKER);
    ////////////////////////
    //init Decocder, NEEDED for SENDING
    AvroHelper::getInstance()->initSchema(SCHEMA_NAME_SYNCMSG, SCHEMA_DEF_SYNCMSG);
    AvroHelper::getInstance()->initSchema(SCHEMA_NAME_RADIOMSG11P, SCHEMA_DEF_RADIOMSG11P);

    //////////////////////////////////////
    //init Producer
    std::string producerID = scenarioID+"_"+simulatorID+"_"+Config::getInstance()->get(CONFIG_OPP_PRODUCER_SUFFIX);
    producer = std::make_shared<KafkaProducer>();
    //which topic will we publish to?
    std::vector<std::string> ptopics = {};
    producer->init(host, ptopics, producerID);


    interaction = std::make_shared<OppInteractionImpl>(daceDS::OppWrapper::getInstance());
    interactionHandler = std::make_shared<InteractionHandlerCommunication11p>(interaction, producer);
    auto interactionConsumerID = scenarioID+"_"+simulatorID+"_int_"+Config::getInstance()->get(CONFIG_OPP_CONSUMER_SUFFIX);
    interactionConsumer = std::make_shared<KafkaConsumer>(interactionHandler, true);
    string t1 = "interaction.simulation."+scenarioID+".communication.80211p.request";
    std::vector<std::string> itopics;
    itopics.push_back(t1);
    interactionConsumer->subscribe(host, itopics, interactionConsumerID);
    KINFO (interactionConsumerID  << " listening to="<<t1); 
    interactionConsumer->setTimeSync(Scheduler::getInstance()->getTimeSync());
    Scheduler::getInstance()->registerInteractionHandler(interactionHandler);



    provision = std::make_shared<OppProvisionImpl>(daceDS::OppWrapper::getInstance());
    provisionHandler = std::make_shared<ProvisionHandlerCommunication11p>(provision);
    auto provisionConsumerID = scenarioID+"_"+simulatorID+"_"+Config::getInstance()->get(CONFIG_OPP_CONSUMER_SUFFIX);
    provisionConsumer = std::make_shared<KafkaConsumer>(provisionHandler, true);
    // string t1 = Config::getInstance()->getInteractionTopic(DOMAIN_COMMUNICATION, LAYER_11P, simulatorID, Config::getInstance()->get(CONFIG_OPP_TOPIC_SEND));
    // string t1 = "interaction.simulation."+scenarioID+".communication.11p."+simulatorID+".msg.send";
    // listen to all instances in the micro layer
    // string t2 = "^" + Config::getInstance()->getProvisionTopic(DOMAIN_TRAFFIC, LAYER_MICRO, "([^.]+)", "vehicle"); //match all instances
    // string t2 = "^provision.simulation."+scenarioID+".traffic.micro.([^.]+).vehicle"; //match all instances
    // escape all dots
    // t2 = strReplace(t2, ".", "\\.");
    // string t2 = "provision.simulation."+scenarioID+".traffic.micro.vehicle"; //todo: compare performance of integrated projectors with initial architecture
    string t2 = "^provision.simulation."+scenarioID+".communication.80211p.logicalNetwork.([^.]+).received"; 
    std::vector<std::string> topics;
    topics.push_back(t2);
    provisionConsumer->subscribe(host, topics, provisionConsumerID);
    KINFO (provisionConsumerID  << " listening to="<<t2);
    provisionConsumer->setTimeSync(Scheduler::getInstance()->getTimeSync());
    Scheduler::getInstance()->registerProvisionHandler(provisionHandler);
}

void daceDS::OppWrapper::handlePositionUpdate(int64_t t , int64_t st, std::string vehicleID, double x, double y, double z){

    int64_t lts = st / 1000;
    
    KINFO("daceDS::OppWrapper::handlePositionUpdate: st="<<st << ", lts="<<lts << " v="<<vehicleID<<+": "<<x<<"|"<<y);

    daceDS::datamodel::Micro micro;
    micro.vehicleID = vehicleID;
    micro.position.x = x;
    micro.position.y = y;
    micro.position.z = z;
    unreadSubscribtions[lts].push_back(micro);
}

void daceDS::OppWrapper::handleTopicSendRadioMsg(daceDS::datamodel::RadioMsg msg){
    KDEBUG("unreadsendMsgRequests.push_back(msg) from sender=" << msg.sender);
    unreadsendMsgRequests.push_back(msg);
}

void daceDS::OppWrapper::notifyMsgReceived(daceDS::datamodel::RadioMsg msg){
    KDEBUG("notifiyMsgReceived");
    vector<char> out = AvroHelper::getInstance()->encodeRadioMsg(msg);
    KDEBUG("encoded");
//    string topic = Config::getInstance()->getInteractionTopic(DOMAIN_COMMUNICATION, LAYER_11P, simulatorID, Config::getInstance()->get(CONFIG_OPP_TOPIC_RECEIVE));
    // string topic = "interaction.simulation."+scenarioID+".communication.11p."+simulatorID+".msg.receive";
    string topic = "provision.simulation."+scenarioID+".communication.11p.msg.received";
    KDEBUG("sending on "+topic);
    // producer->publish(topic, out);

    KDEBUG("notifiyMsgReceived sent out on topic=" << topic);
}

void daceDS::OppWrapper::notifyMsgSent(daceDS::datamodel::RadioMsg msg){
    KDEBUG("notifiyMsgSent");
    vector<char> out = AvroHelper::getInstance()->encodeRadioMsg(msg);
    KDEBUG("encoded");
//    string topic = Config::getInstance()->getInteractionTopic(DOMAIN_COMMUNICATION, LAYER_11P, simulatorID, Config::getInstance()->get(CONFIG_OPP_TOPIC_RECEIVE));
    // string topic = "interaction.simulation."+scenarioID+".communication.11p."+simulatorID+".msg.receive";
    string topic = "provision.simulation."+scenarioID+".communication.11p.msg.sent";
    KDEBUG("sending on "+topic);
    // producer->publish(topic, out);
    KDEBUG("notifiyMsgSent sent out on topic=" << topic);
}

void daceDS::OppWrapper::setNetbounds(veins::TraCICoord netbounds1, veins::TraCICoord netbounds2, int margin)
{
    coordinateTransformation.reset(new veins::TraCICoordinateTransformation(netbounds1, netbounds2, margin));
}

veins::Coord daceDS::OppWrapper::traci2omnet(veins::TraCICoord coord) const
{
    ASSERT(coordinateTransformation.get());
    return coordinateTransformation->traci2omnet(coord);
}

std::list<veins::Coord> daceDS::OppWrapper::traci2omnet(const std::list<veins::TraCICoord>& list) const
{
    ASSERT(coordinateTransformation.get());
    return coordinateTransformation->traci2omnet(list);
}

veins::TraCICoord daceDS::OppWrapper::omnet2traci(veins::Coord coord) const
{
    ASSERT(coordinateTransformation.get());
    return coordinateTransformation->omnet2traci(coord);
}

std::list<veins::TraCICoord> daceDS::OppWrapper::omnet2traci(const std::list<veins::Coord>& list) const
{
    ASSERT(coordinateTransformation.get());
    return coordinateTransformation->omnet2traci(list);
}

veins::Heading daceDS::OppWrapper::traci2omnetHeading(double heading) const
{
    ASSERT(coordinateTransformation.get());
    return coordinateTransformation->traci2omnetHeading(heading);
}

double daceDS::OppWrapper::omnet2traciHeading(veins::Heading heading) const
{
    ASSERT(coordinateTransformation.get());
    return coordinateTransformation->omnet2traciHeading(heading);
}
