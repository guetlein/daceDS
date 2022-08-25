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
#include <sys/types.h>
#include <sys/wait.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cwchar>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>
#include <map>
#include <regex>
#include <string>

#include "communication/AvroHelper.h"
#include "communication/kafka/KafkaProducer.h"
#include "datamodel/ResourceFile.hh"
#include "datamodel/Scenario.hh"
#include "datamodel/BB.hh"
#include "util/Config.h"
#include "util/Utils.h"
#include "util/Defines.h"

#ifndef USING_TRACI
    #include "api/SumoConnectionLibSUMO.h"
#endif

#include <pugixml.hpp>

namespace daceDS {
class PrepareRun {
   public:
    static std::string createSumoConf(std::shared_ptr<datamodel::Scenario> sce, std::shared_ptr<datamodel::BB> sim, std::string roadMapPath);
    static int startSUMO(std::string sceID, std::shared_ptr<datamodel::BB> sim, int port, std::string config);
    static int startLibSUMO(std::string sceID, std::shared_ptr<datamodel::BB> sim, std::string config);
    static bool validOutput(std::string item);
    static void sendResults(std::string sceID, std::shared_ptr<datamodel::BB> sim, std::shared_ptr<KafkaProducer> producer);
    static bool checkFailure(int pid);
    static bool checkStillRunning(int pid);

    static std::vector<std::string> getResponsibleLinks(std::string mapPath, std::vector<std::string> responsibleNodes);
    static std::vector<std::string> getOutgoingLinks(std::string mapPath, std::vector<std::string> responsibleNodes);
    static std::vector<std::string> getIncomingLinks(std::string mapPath, std::vector<std::string> responsibleNodes);
};
}  // namespace daceDS
