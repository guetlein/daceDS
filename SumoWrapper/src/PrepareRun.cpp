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
#include "PrepareRun.h"
// #include <libsumo/Simulation.h>
using namespace daceDS;

#define S3 "http://foobar/"

//todo:various attacks possible, not meant for use on public systems
std::string PrepareRun::createSumoConf(std::shared_ptr<datamodel::Scenario> sce, std::shared_ptr<datamodel::BB> sim, std::string roadMapPath) {
    KDEBUG("creating sumo conf ");

    //dir was created with first receveived file
    KDEBUG("dir=" << Config::getInstance()->getResourceDir());

    //copy default config and replace placeholders
    std::string path = Config::getInstance()->getResourceDir() + "/sumocfg.sumocfg";
    std::string defaultConfPath = GetCurrentWorkingDir() + "/" + Config::getInstance()->get("defaultConf");
    if(!std::filesystem::exists(defaultConfPath)){
        KERROR("cannot read from default file from " << path);
        exit(1);
    }
    
    KDEBUG("Loading defaultConf from " << defaultConfPath);
    std::ifstream src(defaultConfPath, std::ios::binary);
    std::ostringstream buffer;
    buffer << src.rdbuf();
    std::string defaultConfString = buffer.str();

    std::vector<std::string> additionalFileVec;
    //first additionals;
    //todo: overwrite global input/map ?
    //instance's resources
    KDEBUG("looking for local additional input");
    for (std::pair<std::string, std::string> res : sim->resources) {
        KDEBUG("checking for type of input");
        if (res.second != "Additional") {
            continue;
        }

        std::string fid = res.first;
        //in case this is a reference we will strip the id to the filename at this point
        if(fid.find(":") != std::string::npos){
            fid = fid.substr(fid.find_last_of("/\\") + 1);
        }

        additionalFileVec.push_back(fid);
        KDEBUG("adding " << fid << " as local additional input");
    }

    //sumo cares about order, route file needs come after vtype definitions
    KDEBUG("looking for local traffic input");
    for (std::pair<std::string, std::string> res : sim->resources) {
        if (res.second != "Traffic") {
            continue;
        }

        std::string fid = res.first;
        //in case this is a reference we will strip the id to the filename at this point
        if(fid.find(":") != std::string::npos){
            fid = fid.substr(fid.find_last_of("/\\") + 1);
            KDEBUG("stripped absolute file ref to relative id");
        } else {
            KDEBUG("id is already a relative id");
        }

        //only if path not already present
        if (std::find(additionalFileVec.begin(), additionalFileVec.end(), fid) == additionalFileVec.end()) {
            additionalFileVec.push_back(fid);
            KDEBUG("adding " << fid << "  traffic input");

        } else {
            KDEBUG("ignoring resource " << fid << " cause already present");
        }
    }


    KDEBUG("collected " << additionalFileVec.size() << " files for the additional field: ");

    //bring all together
    bool firstFile = true;
    std::string additionalFiles = "";
    for (auto s : additionalFileVec) {
        if (!firstFile) {
            additionalFiles += ",";
        }
        firstFile = false;
        additionalFiles += s;
    }
    KDEBUG(additionalFiles);

    KDEBUG("replacing the {ADD} section of config file with that string");
    defaultConfString.replace(defaultConfString.find("{ADD}"), std::string("{ADD}").size(), additionalFiles);
    KDEBUG("done replacing");

    //results
    std::string output = "";
    KDEBUG("looking for desired results");
    for (std::pair<std::string, std::string> res : sim->results) {
        auto outputType = res.first;
        auto outputPath = res.second;

        if (!validOutput(outputType)) {
            KDEBUG("Ignoring requested result: " << outputType);
            continue;
        }

        KDEBUG("outputType=" << outputType);
        KDEBUG("outputPath=" << outputPath);
        auto outputLine = "<" + outputType + " value =\"" + Config::getInstance()->getOutputDir() + "/" + outputPath + "\"/>\n";
        output += outputLine;
        KDEBUG("outputPath=" << outputLine);
    }

    defaultConfString.replace(defaultConfString.find("{OUTPUT}"), std::string("{OUTPUT}").size(), output);

    //relatives paths to the sumoconfig location are alright
    defaultConfString.replace(defaultConfString.find("{NET}"), std::string("{NET}").size(), roadMapPath);
    defaultConfString.replace(defaultConfString.find("{START}"), std::string("{START}").size(), std::to_string(sce->simulationStart));
    defaultConfString.replace(defaultConfString.find("{END}"), std::string("{END}").size(), std::to_string(sce->simulationEnd));
    defaultConfString.replace(defaultConfString.find("{SL}"), std::string("{SL}").size(), std::to_string(sim->stepLength / 1000.0));

    //defaultConfString.replace(defaultConfString.find("{CUSTOM}"), std::string("{CUSTOM}").size(), sim->CustomParams); //todo: currently used as execution parameters

    std::ofstream dst(path);
    dst << defaultConfString;
    dst.flush();
    dst.close();

    KDEBUG("saved to " << path);

    //copy gui
    return path;
}

int PrepareRun::startLibSUMO(std::string sceID, std::shared_ptr<datamodel::BB> sim, std::string config) {   
    #ifndef USING_TRACI
        libsumo::Simulation::load({"-c", config, "-v"});
    #endif
    return 0;
}

int PrepareRun::startSUMO(std::string sceID, std::shared_ptr<datamodel::BB> sim, int port, std::string config) {   
    KDEBUG("startSUMO() forking");
    int pid = fork();
    KDEBUG("startSUMO() forked");
    if (pid == 0) {
        std::cout << "startSUMO() in fork" <<std::endl;
        std::cout <<"starting sumo process listening on port " << port <<std::endl;

        std::vector<std::string> pam{Config::getInstance()->get("exe"),
                                     "-c", config,
                                     "--remote-port", std::to_string(port),
                                     "--verbose",
                                     "-l", Config::getInstance()->getLogDir() + Config::getInstance()->get("simulatorLogFile"),
                                     "--error-log", Config::getInstance()->getLogDir() + Config::getInstance()->get("simulatorErrorLogFile"),
                                     "--quit-on-end"};

        std::cout <<"exec cmd:" << concat(pam, ' ') <<std::endl;

        //todo: incorporate cpams
        // std::vector<std::string> cpam = split(sim->CustomParams, ' ');
        // pam.insert(pam.end(), cpam.begin(), cpam.end());

        char* arr[pam.size() + 1];
        for (int c = 0; c < pam.size(); c++) {
            arr[c] = pam[c].c_str();
        }
        arr[pam.size()] = NULL;

        // KDEBUG("args array:");
        // for (char* c : arr) {
        //     KDEBUG(c);
        // }
        // KDEBUG("in line:" << concat(pam, ' '));

        int ret = execv(Config::getInstance()->get("exe").c_str(), arr);// on success, never returns
        KERROR("StartSumo(): "<< ret);
        std::cout <<"startsumo error:" << ret <<std::endl;
        exit(ret);
    } 

    return pid;
}
bool PrepareRun::checkFailure(int pid){
    int status;
    pid_t return_pid = waitpid(pid, &status, WNOHANG);
    if (return_pid == -1) {
        /* error */
        return true;
    } else if (return_pid == 0) {
        /* child is still running */
        return false;
    } else if (return_pid == pid) {
        /* child is finished. exit status in status */
        if(status == 0){
            return false;
        }
        KERROR("SUMO execute process failed with: " << status);
        return true;
    }
}
bool PrepareRun::checkStillRunning(int pid){
    int status;
    pid_t return_pid = waitpid(pid, &status, WNOHANG);
    if (return_pid == -1) {
        /* error */
        return false;
    } else if (return_pid == 0) {
        /* child is still running */
        return true;
    } else if (return_pid == pid) {
        /* child is finished. exit status in status */
        return false;
    }
}

bool PrepareRun::validOutput(std::string item) {
    std::vector<std::string> validItems = {"fcd-output",
                                           "full-output",
                                           "statistic-output",
                                           "person-summary-output",
                                           "summary",
                                           //    "collision-output",
                                           "stop-output",
                                           "vehroute-output",
                                           "tripinfo-output",
                                           "queue-output",
                                           "lanechange-output",
                                           "amitran-output",
                                           "vtk-output",
                                           "emission-output",
                                           "netstate-dump"};
    return std::find(validItems.begin(), validItems.end(), item) != validItems.end();
}

void PrepareRun::sendResults(std::string sceID, std::shared_ptr<datamodel::BB> sim, std::shared_ptr<KafkaProducer> producer) {
    std::map<std::string, std::string> results = sim->results;
    ReturnMode returnMode = ReturnMode::LISTED;
    //specific return mode command? "returnMode" as key in results dict?
    if (results.count(RESULTS_STR_RETURNMODE) > 0) {
        auto modeStr = results[RESULTS_STR_RETURNMODE];
        if (modeStr == RESULTS_STR_RETURNMODE_NONE) {
            KERROR("return mode is none");
            return;
        } else if (modeStr == RESULTS_STR_RETURNMODE_ALL) {
            returnMode = ReturnMode::ALL;
            KDEBUG("return mode is all");
        } else if (modeStr == RESULTS_STR_RETURNMODE_RESOURCES) {
            returnMode = ReturnMode::RESOURCES;
            KDEBUG("return mode is resources");
        } else if (modeStr == RESULTS_STR_RETURNMODE_LOGS) {
            returnMode = ReturnMode::LOGS;
            KDEBUG("return mode is logs");
        } else if (modeStr == RESULTS_STR_RETURNMODE_LISTED) {
            returnMode = ReturnMode::LISTED;
            KDEBUG("return mode is listed");
        } else {
            KERROR("return mode '" << modeStr << "' is not known, ignoring...");
        }
        results.erase(RESULTS_STR_RETURNMODE);
    }

    //std::string filename = Config::getInstance()->getBaseDir() + "/" + sim->ID + "_results.zip";
    std::string filename = Config::getInstance()->getBaseDir() + "/results.zip";
    std::string resDir = "";
    std::string fileList = "";
    if (returnMode == ReturnMode::ALL) {
        //copy logs to basedir
        std::string cmd = " cp -r " + Config::getInstance()->getLogDir() + " " + Config::getInstance()->getBaseDir();
        system(cmd.c_str());

        //zip everything in basedir
        cmd = " cd " + Config::getInstance()->getBaseDir() + " && zip -r " + filename + " *";
        system(cmd.c_str());

        resDir = Config::getInstance()->getBaseDir();
        fileList = "*";
    } else if (returnMode == ReturnMode::RESOURCES) {
        //zip everything in basedir
        // std::string cmd = " cd " + Config::getInstance()->getBaseDir() + " && zip -r " + filename + " *";
        // system(cmd.c_str());
        resDir = Config::getInstance()->getBaseDir();
        fileList = "*";
    } else if (returnMode == ReturnMode::LOGS) {
        //zip everything in basedir
        // std::string cmd = " cd " + Config::getInstance()->getLogDir() + " && zip -r " + filename + " *";
        // system(cmd.c_str());
        resDir = Config::getInstance()->getLogDir();
        fileList = "*";
    } else if (returnMode == ReturnMode::LISTED) {
        std::string cmd = " cd " + Config::getInstance()->getOutputDir() + " && zip " + filename;

        resDir = Config::getInstance()->getOutputDir();
        //zip all listed resource paths
        for (std::pair<std::string, std::string> ele : results) {
            cmd += " " + ele.second;
            fileList += ele.second + " ";
        }
        system(cmd.c_str());
    }

    std::string cmd2 = "./compressResults.sh " + resDir + " " + filename + " " + fileList;
    int result = system(cmd2.c_str());  //todo: check for failure

    //read zip file
    // may not be there if there was no result
    try {
        std::streampos fileSize;
        std::ifstream file(filename, std::ios::binary);
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<uint8_t> fileData(fileSize);
        file.read((char*)&fileData[0], fileSize);

        //put in avro struct
        datamodel::ResourceFile rf;
        std::string dropPath = std::filesystem::path(filename).filename();
        rf.ID = dropPath;
        rf.Type = "Results";
        //case one: return bytes directly in "rf.File" field
        //rf.File.set_bytes(fileData);

        //case two: upload to DB and return reference
        // std::string storageDestination = "simulation/results/" + sceID + "/" + sim->ID + "/results.zip";
        // std::string cmd = "./uploadFileToDB.sh " + filename + " " + storageDestination;
        // system(cmd.c_str());  //todo: check for failure
        // rf.FileReference.set_string(S3+storageDestination);

        //case three: leave on disk  and return reference
        rf.FileReference.set_string(filename);

        //send out
        std::vector<char>  out = AvroHelper::getInstance()->encodeResourceFile(rf);

        bool repeatOnError = false;
        KINFO("publishing results on " << Config::getInstance()->getProvisionBaseTopic(TOPIC_RESULT));
        producer->publish(Config::getInstance()->getProvisionBaseTopic(TOPIC_RESULT), out, repeatOnError);
        KDEBUG("results were sent bc");
    } catch (std::exception &e) {
        KERROR("results.zip was not found, probably there were no result artifacts");
        KERROR(e.what());
    }
}

    std::vector<std::string> PrepareRun::getResponsibleLinks(std::string mapPath, std::vector<std::string> responsibleNodes){

        std::vector<std::string> responsibleLinks;

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(mapPath.c_str());
      
        if(!result){
            std::cout << "getResponsibleLinks: failed to parse " << mapPath << std::endl;
            return responsibleLinks;
        }


        std::cout <<"responsible edges:\n";

        for (std::string node : responsibleNodes) {

            if(node == "*"){
                responsibleLinks.clear();
                pugi::xpath_node_set edges = doc.select_nodes((std::string(".//edge").c_str()));
                for (pugi::xpath_node_set::const_iterator it = edges.begin(); it != edges.end(); ++it)
                {
                    pugi::xpath_node edge = *it;
                    auto link = edge.node().attribute("id").value();
                    responsibleLinks.push_back(link);
                }
                return responsibleLinks;
            }

            pugi::xpath_node_set edges = doc.select_nodes((std::string(".//edge[@to='"+node+"']").c_str()));
            // std::cout << "edges:\n";
            for (pugi::xpath_node_set::const_iterator it = edges.begin(); it != edges.end(); ++it)
            {
                pugi::xpath_node edge = *it;
                auto link = edge.node().attribute("id").value();
                if(std::find(responsibleLinks.begin(), responsibleLinks.end(), link) != responsibleLinks.end()) {
                    continue;
                }
                std::cout << link << ",";
                responsibleLinks.push_back(link);
            }
        }
        std::cout << "\n";
        return responsibleLinks;
    }

    std::vector<std::string> PrepareRun::getOutgoingLinks(std::string mapPath, std::vector<std::string> responsibleNodes){

        std::vector<std::string> outgoingLinks;

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(mapPath.c_str());

        if(!result){
            std::cout << "getOutgoingLinks: failed to parse " << mapPath << std::endl;
            return outgoingLinks;
        }


        std::cout <<"outgoing edges:\n";
        for (std::string node : responsibleNodes) {
            // std::cout << "looking for outgoing edges from node "+node+"\n";
            pugi::xpath_node_set edges = doc.select_nodes((std::string(".//edge[@from='"+node+"']").c_str()));

            for (pugi::xpath_node_set::const_iterator it = edges.begin(); it != edges.end(); ++it)
            {
                pugi::xpath_node edge = *it;
                // std::cout << edge.node().attribute("id").value()<<",";
                auto to = edge.node().attribute("to").value();
                // std::cout << " goes to " << to << ", ";
                // std::cout << "checking if edge " << to << " is outside: ";

                //check if destination node is NOT in resp set
                if(std::find(responsibleNodes.begin(), responsibleNodes.end(), to) == responsibleNodes.end()) {
                    auto link = edge.node().attribute("id").value();
                    if(std::find(outgoingLinks.begin(), outgoingLinks.end(), link) != outgoingLinks.end()) {
                        continue;
                    }
                    outgoingLinks.push_back(link);
                    std::cout << edge.node().attribute("id").value()<<",";
                } else {
                    //  std::cout << "no " << std::endl;
                }
            }

        }

        std::cout <<"\n";
        return outgoingLinks;
    }
   std::vector<std::string> PrepareRun::getIncomingLinks(std::string mapPath, std::vector<std::string> responsibleNodes){

        std::vector<std::string> incomingLinks;

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_file(mapPath.c_str());

        if(!result){
            std::cout << "getIncomingLinks: failed to parse " << mapPath << std::endl;
            return incomingLinks;
        }

        std::cout <<"incoming edges:\n";

        for (std::string node : responsibleNodes) {
            // std::cout << "looking for incoming edges from node "+node+"\n";
            pugi::xpath_node_set edges = doc.select_nodes((std::string(".//edge[@to='"+node+"']").c_str()));

            for (pugi::xpath_node_set::const_iterator it = edges.begin(); it != edges.end(); ++it)
            {
                pugi::xpath_node edge = *it;
                // std::cout << edge.node().attribute("id").value();
                auto from = edge.node().attribute("from").value();
                // std::cout << " goes to " << to << ", ";
                // std::cout << "checking if edge " << to << " is outside: ";

                //check if source is NOT in resp set
                if(std::find(responsibleNodes.begin(), responsibleNodes.end(), from) == responsibleNodes.end()) {
                    auto link = edge.node().attribute("id").value();
                    if(std::find(incomingLinks.begin(), incomingLinks.end(), link) != incomingLinks.end()) {
                        continue;
                    }
                    incomingLinks.push_back(link);
                    //  std::cout << "yes " << std::endl;
                    std::cout << edge.node().attribute("id").value()<<",";
                } else {
                    //  std::cout << "no " << std::endl;
                }
            }
        }

        std::cout << "\n";
        return incomingLinks;
    }
