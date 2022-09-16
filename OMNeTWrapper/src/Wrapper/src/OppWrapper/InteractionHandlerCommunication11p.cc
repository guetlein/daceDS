

#include "InteractionHandlerCommunication11p.h"

#include<cstddef>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>

using namespace daceDS;

bool InteractionHandlerCommunication11p::handle(ConsumedMessage* msg){
 std::string topic = msg->topic;
    KINFO ( "got message in topic=" << topic );

    // if (InteractionHandler::handle(msg)){
    //     return true;
    // }

    if(topic.find("interaction")!=std::string::npos){
        // KINFO ("decoding radiomsg");
        // daceDS::datamodel::RadioMsg radiomsg = AvroHelper::getInstance()->decodeRadioMsg("payload", msg->payload, msg->len);
        // KINFO ("decoded radiomsg!");
        // cb->handleTopicSendRadioMsg(msg->timestamp, radiomsg);
        
        daceDS::datamodel::InteractionMsg intmsg = AvroHelper::getInstance()->decodeInteractionMsg("payload", msg->payload, msg->len); 
        KINFO ("decoded InteractionMsg!");
        if (intmsg.MethodID == "node.position.set"){
            (std::static_pointer_cast<OppInteractionImpl> (api))->handleInteractionPositionUpdate(msg->timestamp, msg->st, intmsg);
        } else {
            KINFO("method '"<<intmsg.MethodID<<"' is not known");
        }
        // cb->handleTopicSendRadioMsg(msg->timestamp, radiomsg);
    } 
    
    
    // if(topic.find("micro.vehicle")!=std::string::npos){
    //     daceDS::datamodel::Micro micro = AvroHelper::getInstance()->decodeMicro("payload", msg->payload, msg->len);
    //     KDEBUG ("decoded micro info of " << micro.vehicleID);
    //     (std::dynamic_pointer_cast<OppInteractionImpl> (api))->handleTopicMicro(msg->timestamp, msg->st, micro);
    //     KDEBUG ("called oppprovisionimpl");
    // } else {
    //     KDEBUG ("dont know topic " << topic);
    //     return false;
    // }
    return true;
}


