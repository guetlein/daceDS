#pragma once

#include "OppWrapper.h"
#include "api/Provision.h"
#include "communication/ProvisionHandler.h"
#include "communication/AvroHelper.h"

/*
Each consumer uses a single messageHandler implementation to process incoming messages.
Used to implement handlers for orchestration, interaction, and provision channel.
*/

namespace daceDS {
class OppWrapper;

class ProvisionHandlerCommunication11p : public ProvisionHandler {
   public:
    ProvisionHandlerCommunication11p(std::shared_ptr<Provision> p) : ProvisionHandler(p) {};
    virtual ~ProvisionHandlerCommunication11p(){};

    bool handle(ConsumedMessage* msg);
};
}