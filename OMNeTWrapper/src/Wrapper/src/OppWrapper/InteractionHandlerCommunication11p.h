#pragma once

#include "OppWrapper.h"
#include "api/Provision.h"
#include "communication/InteractionHandler.h"
#include "communication/AvroHelper.h"

/*
Each consumer uses a single messageHandler implementation to process incoming messages.
Used to implement handlers for orchestration, interaction, and provision channel.
*/

namespace daceDS {
class OppWrapper;

class InteractionHandlerCommunication11p : public InteractionHandler {
   public:
    InteractionHandlerCommunication11p(std::shared_ptr<Interaction> i, std::shared_ptr<Producer> p) : InteractionHandler(i, p) {};
    virtual ~InteractionHandlerCommunication11p(){};

    bool handle(ConsumedMessage* msg);
};
}