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
#pragma once

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
// #include <utils/traci/TraCIAPI.h>
#include <libsumo/Simulation.h>

#include <chrono>
#include <cstdio>
#include <cwchar>
#include <exception>
#include <fstream>
#include <iostream>
#include <locale>
#include <memory>
#include <regex>
#include <string>

#include "PrepareRun.h"
#include "api/InteractionImpl.h"
#include "api/OrchestrationImpl.h"
#include "api/ProvisionImpl.h"
#include "api/Traffic/Micro/ProvisionHandlerTrafficMicro.h"
#ifdef USING_TRACI
    #include "api/SumoConnectionTraCI.h"
#else
    #include "api/SumoConnectionLibSUMO.h"
#endif
#include "communication/InteractionHandler.h"
#include "communication/OrchestrationHandler.h"
#include "communication/ProvisionHandler.h"
#include "communication/kafka/KafkaConsumer.h"
#include "communication/kafka/KafkaProducer.h"
#include "logic/SumoSimulationControl.h"
#include "main/SimulationWrapper.h"
#include "util/Defines.h"
#include "util/log.h"



namespace daceDS {

class SumoSimulationControl;
class ProvisionImpl;

class SumoWrapper : public SimulationWrapper {
   protected:
    std::shared_ptr<ProvisionImpl> provision;
    std::shared_ptr<SumoSimulationControl> ctrl;
    std::shared_ptr<KafkaProducer> producer;
    std::shared_ptr<KafkaProducer> statusProducer;
    int sumoPID = 0;
    
   public:
    SumoWrapper(std::string s, std::string t) : SimulationWrapper(s, t){};
    ~SumoWrapper(){};

    std::shared_ptr<ProvisionImpl> getProvision() { return provision; };

    void runWrapper();
    void waitForScenario();
    void killWrapper();
    void terminateWrapper();

    void statusMsg(std::string msg);

};

}  // namespace daceDS
