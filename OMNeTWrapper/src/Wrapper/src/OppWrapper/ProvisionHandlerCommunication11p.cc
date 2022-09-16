

#include "ProvisionHandlerCommunication11p.h"

#include<cstddef>
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<string>

using namespace daceDS;

bool ProvisionHandlerCommunication11p::handle(ConsumedMessage* msg){
 std::string topic = msg->topic;
    KDEBUG ( "got message in topic=" << topic );

    if (ProvisionHandler::handle(msg)){
        return true;
    }

    // if(topic.find("interaction")!=std::string::npos){
    //     KINFO ("decoding radiomsg");
    //     daceDS::datamodel::RadioMsg radiomsg = AvroHelper::getInstance()->decodeRadioMsg("payload", msg->payload, msg->len);
    //     KINFO ("decoded radiomsg!");
    //     cb->handleTopicSendRadioMsg(msg->timestamp, radiomsg);

    // } 
    
    
    if(topic.find("micro.vehicle")!=std::string::npos){
        daceDS::datamodel::Micro micro = AvroHelper::getInstance()->decodeMicro("payload", msg->payload, msg->len);
        KDEBUG ("decoded micro info of " << micro.vehicleID);
        // (std::dynamic_pointer_cast<OppProvisionImpl> (api))->handleTopicMicro(msg->timestamp, msg->st, micro);
        KDEBUG ("called oppprovisionimpl");
    } else {
        KDEBUG ("dont know topic " << topic);
        return false;
    }
    return true;
}
