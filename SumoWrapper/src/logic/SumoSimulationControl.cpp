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

#include "SumoSimulationControl.h"
using namespace daceDS;

void SumoSimulationControl::init(std::shared_ptr<Producer> p, std::shared_ptr<SumoWrapper> w, std::shared_ptr<ProvisionImpl> prov, int port, std::shared_ptr<ProvisionHandler> ph) {
    SimulationControl::init(p, w, ph);
    #ifdef USING_TRACI
        KDEBUG("creating a sumoConnection instance connected to port " << port);
        sumoConnection = std::make_shared<SumoConnectionTraCI>(port);
    #else 
        KDEBUG("creating a sumoConnection instance connected to internal libsumo instance");
        sumoConnection = std::make_shared<SumoConnectionLibSUMO>();
    #endif

    provision = prov;
}

//gets called when a new description / sce is received, typically on startip or when borders change
void SumoSimulationControl::notifySceChange() {
    sceChanged = true;
}

void SumoSimulationControl::processDescription() {
    KDEBUG("Checking if resp for whole map");
    for (std::string r : provision->getSim()->responsibilities) {
        if (r == Config::getInstance()->get("coSimRespForAllTag")) {
            sumoConnection->setResponsibleForWholeMap(true);
            KDEBUG("Responsible for whole map! ");
        }
    }

    KDEBUG("Checking for resp based on nodes");
    if (!sumoConnection->getResponsibleForWholeMap() && provision->getSim()->responsibilities.size() > 0) {
        sumoConnection->setResponsibleForWholeMap(false);
        std::string absoluteRoadMapPath = Config::getInstance()->getResourceDir()+"/"+provision->getFileByType(STR_TYPE_ROADMAP);
        std::cout << "looking for roadmap at " <<  absoluteRoadMapPath << std::endl;
        
        std::vector<std::string> responsibleLinks = PrepareRun::getResponsibleLinks(absoluteRoadMapPath, provision->getSim()->responsibilities);
        std::vector<std::string> outgoingLinks = PrepareRun::getOutgoingLinks(absoluteRoadMapPath, provision->getSim()->responsibilities);

        sumoConnection->setRespLinks(responsibleLinks);
        sumoConnection->setOutgoingLinks(outgoingLinks);

        // KDEBUG("Responsibilities: ");
        // for (std::string ss : sumoConnection->getRespLinks()) {
        //     KDEBUG(ss << ", ");
        // }
    } else {
        sumoConnection->setResponsibleForWholeMap(false);
        KDEBUG("ATTENTION! Haven't got a single resonsibility :-(");
    }
    sceChanged = false;

}

void SumoSimulationControl::close() {

    KINFO("SumoSimulationControl::close() was triggered");
    sumoConnection->closeSumo();
    done = true;
}


//#define DSTRAFFIC_MEASURE_DBG 1

void SumoSimulationControl::run() {

    #ifdef USING_TRACI
        KDEBUG("Connecting to simulator ...");
        if(!sumoConnection->connect()){

            KDEBUG("Connection failed!");
            std::dynamic_pointer_cast<SumoWrapper>(wrapper)->statusMsg("failed (to connect to SUMO instance)");

            KDEBUG("Killing process ...");
            wrapper->killWrapper();
        }
    #endif

    KDEBUG("Making status post ...");
    std::dynamic_pointer_cast<SumoWrapper>(wrapper)->statusMsg("initialized");
        
    KDEBUG("Processing sce ...");
    processDescription();
    KDEBUG("Done with sce ...");

    if (sumoConnection->getResponsibleForWholeMap()) {
        KDEBUG("resp. for whole map");
    } else {
        KDEBUG("resp. for " << sumoConnection->getRespLinks().size() << "links");
    }

    for(auto const& layerParam : provision->getSim()->parameters){
        if(layerParam.first == "ghosting") {
            if(layerParam.second == "false"){
                sumoConnection->setGhostingDisabled();
            }
        }
    }

    int64_t stepLengthMS = provision->getSim()->stepLength;
    int64_t simEndMS = provision->getScenario()->simulationEnd * 1000;
    int64_t simStartMS = provision->getScenario()->simulationStart * 1000;



    //joining
    wrapper->timeSync->joinTiming(simStartMS);
    
    KDEBUG("Starting simulation from " << simStartMS << " until " << simEndMS);
    std::dynamic_pointer_cast<SumoWrapper>(wrapper)->statusMsg("simulating");


    //first step
    wrapper->timeSync->timeAdvance(stepLengthMS);

    int64_t simulatorTimeMs = 0;
    int64_t lbts = stepLengthMS;

    #ifdef DSTRAFFIC_MEASURE_DBG
        std::ofstream csvFile(Config::getInstance()->getLogDir()+"measurements_"+provision->getScenario()->scenarioID+"_"+provision->getSim()->instanceID+".csv");
        auto scenarioStartTime = std::chrono::high_resolution_clock::now();
    #endif



    while ( simulatorTimeMs < simEndMS && !done) {

        //1. make sim step
        #ifdef DSTRAFFIC_MEASURE_DBG
            auto startSim = std::chrono::high_resolution_clock::now();
        #endif

        //todo: find out biggest step for increasing sync step
        sumoConnection->proceedSim(1);
        simulatorTimeMs+=1*stepLengthMS;
        

        // simulatorTimeMs+=1000;
        // sumoConnection->setTime(simulatorTimeMs);

        #ifdef DSTRAFFIC_MEASURE_DBG
            auto endSim = std::chrono::high_resolution_clock::now();
            int64_t s = std::chrono::duration_cast<std::chrono::microseconds>(endSim-startSim).count();


            auto startOutgoing = std::chrono::high_resolution_clock::now();
        #endif


        //2. send out ndms
        processOutgoing(simulatorTimeMs);


        //3. sync
        // we may be done with step simulatorTimeMs, therefore we are requesting to go to simulatorTimeMs+stepLengthMS
        #ifdef DSTRAFFIC_MEASURE_DBG

            auto endOutgoing = std::chrono::high_resolution_clock::now();
            int64_t outg = std::chrono::duration_cast<std::chrono::microseconds>(endOutgoing-startOutgoing).count();

            auto startSync = std::chrono::high_resolution_clock::now();
        #endif

        KDEBUG("requesting time adavance to " << simulatorTimeMs + stepLengthMS);
        // wrapper->timeSync->timeAdvance(stepLengthMS);
        wrapper->timeSync->timeAdvance(1000);
        lbts = wrapper->timeSync->getLBTSInMS();
        KDEBUG("got time adavance grant & and all msgs");

        #ifdef DSTRAFFIC_MEASURE_DBG
            auto endSync = std::chrono::high_resolution_clock::now();
            int64_t t = std::chrono::duration_cast<std::chrono::microseconds>(endSync-startSync).count() ;

        #endif



        #ifdef DSTRAFFIC_MEASURE_DBG
            auto startProcess = std::chrono::high_resolution_clock::now();
        #endif
        //at this point, we have received all messages with a timestamp < lbts
        //we can now process these messages, while the simulator is still at simulatorTimeMs
        processInteractions(lbts);
        processProvisions(lbts);  

        #ifdef DSTRAFFIC_MEASURE_DBG
            auto endProcess = std::chrono::high_resolution_clock::now();
            int64_t p = std::chrono::duration_cast<std::chrono::microseconds>(endProcess-startProcess).count() ;
        #endif



        //4. observe changes in the sim still at simulatorTimeMs
        #ifdef DSTRAFFIC_MEASURE_DBG
            auto startObs = std::chrono::high_resolution_clock::now();
        #endif
        // std::cout << "observe\n";
        runObservers(simulatorTimeMs);       
        
        #ifdef DSTRAFFIC_MEASURE_DBG
            auto endObs = std::chrono::high_resolution_clock::now();
            int64_t o = std::chrono::duration_cast<std::chrono::microseconds>(endObs-startObs).count();
            // std::cout << o << "us for observers\n";
        #endif

 
        

        #ifdef DSTRAFFIC_MEASURE_DBG
            totalT+=t;
            totalS+=s;
            totalOutg+=outg;
            totalP+=p;
            totalO+=o;

            csvFile << t << "," <<outg << "," << s << "," << p << "," << o << "\n";


            //writeVehicleData(simulatorTimeMs);
  
        #endif
    }

    #ifdef DSTRAFFIC_MEASURE_DBG

        auto scenarioEndTime = std::chrono::high_resolution_clock::now();
        int64_t totalScenarioTime = std::chrono::duration_cast<std::chrono::microseconds>(scenarioEndTime-scenarioStartTime).count();
        csvFile.close();


        std::ofstream csvFile2(Config::getInstance()->getLogDir()+"measurements_"+provision->getScenario()->scenarioID+"_"+provision->getSim()->instanceID+"_summary.csv");
        csvFile2 << "sumScenarioDuraiton, sumSyncDuration, sumProcessOutDuration,  sumSumoDuration, sumProcessingDuration, sumObserverDuration, sentOutVehicles, adoptedVehicles, failedAdoptions\n";
        csvFile2 << totalScenarioTime << "," << totalT << "," << totalOutg << ","<< totalS << "," << totalP << "," << totalO << "," << sentVehicles << "," << adoptedVehicles << "," << failedAdoptions << "\n";
        csvFile2.close();


        std::ofstream csvFile3(Config::getInstance()->getLogDir()+"measurements_"+provision->getScenario()->scenarioID+"_"+provision->getSim()->instanceID+"_vehs.txt");
        csvFile3 << "adopted:\n";
        for(auto v : adoptedVehicleIDs){
            csvFile3 << v << "\n";
        }
        csvFile3 << "failed adoption:\n";
        for(auto v : failedAdoptionIDs){
            csvFile3 << v << "\n";
        }
        csvFile3 << "sent out:\n";
        for(auto v : sentVehicleIDs){
            csvFile3 << v << "\n";
        }
        csvFile3.close();


        std::cout << "Simulation loop is over\n-------------------\n";
        std::cout << "total time for sce ="<<totalScenarioTime<<"us\n";
        std::cout << "total time for sync="<<totalT<<"us\n";
        std::cout << "total time for sumo="<<totalS<<"us\n";
        std::cout << "total time for proc="<<totalP<<"us\n";
        std::cout << "total time for obse="<<totalO<<"us\n-------------\n";
    #endif

    KINFO("main loop finished");
}

void SumoSimulationControl::processOutgoing(int64_t simulatorTimeMs) {
    try {

        #ifdef USING_TRACI
            std::vector<datamodel::Micro> vehiclesOnEdge = sumoConnection->getVehiclesNewOnBorderEdge2();
        #else 
            std::vector<datamodel::Micro> vehiclesOnEdge = sumoConnection->getAndReleaseLeavingVehicles();
        #endif


        //clean ghosts of cars that don't remain in border zone
        //to be able to transfer them later again
        //todo: only after some steps? whats the best tradeoff for performaance? currently every 60 secs
        if (simulatorTimeMs%60000==0){
            KDEBUG("Cleaning ghosts");
            sumoConnection->cleanGhosts(); 
        }
        #ifdef USING_TRACI
             for (datamodel::Micro veh : vehiclesOnEdge) {
                KDEBUG("looking at " << veh.vehicleID);
                if (sumoConnection->isVehicleGhost(veh.vehicleID)) {
                    KDEBUG("is a ghost --> skip");
                    continue;
                }

                sumoConnection->release(veh.vehicleID, veh.edge);
                std::string topic = Config::getInstance()->getProvisionTopic("edge."+escapeSpecialChars(veh.edge)+".vehicles");

                std::vector<char> encoded = AvroHelper::getInstance()->encodeMicro(veh);

                KDEBUG("transfering " << veh.vehicleID << " on " << topic);
                producer->publish(topic, veh.vehicleID, encoded, simulatorTimeMs, false);
            }
        #else
            // transfer leavers
            for (datamodel::Micro veh : vehiclesOnEdge) {
                std::string topic = Config::getInstance()->getProvisionTopic("edge."+escapeSpecialChars(veh.edge)+".vehicles");
                std::vector<char> encoded = AvroHelper::getInstance()->encodeMicro(veh);
                producer->publish(topic, veh.vehicleID, encoded, simulatorTimeMs, false);
                DSTRAFFIC_CTRL_DBG(simulatorTimeMs<<": sent out veh with id='"<< veh.vehicleID<<"' and pos="<<veh.position.x<<"|"<<veh.position.y << " on topic " << topic);
                sentVehicles++;
                sentVehicleIDs.push_back(veh.vehicleID + " at " + std::to_string(simulatorTimeMs));
            }

        #endif
        

    } catch (const std::exception& e) {
        KERROR("runFederateLoop: Exception at border processing");
        KERROR(e.what());
    }

    //system defined observer: provide updates for external ghosts (i.e. publish micro while veh is on incoming link)
    #ifndef USING_TRACI
        if(sumoConnection->getGhostingEnabled()){
            provideGhostUpdates(simulatorTimeMs);
        }
    #endif

}

void SumoSimulationControl::processInteractions(int64_t t) {
    
    //todo: add pointer to interaction handler
    
    // interaction->processBuffer(t,0);
}

void SumoSimulationControl::processProvisions(int64_t t) {

    KDEBUG("calling processBuffer for time="<<t);
    phdl->processBuffer(t,0);
    KDEBUG("processBuffer is done");

    // 
    //todo: now we should be able to remove some overhead (i.e., incomingVehilceList on provsionHandler)
    try {
        //			//we may have received an interaction to early
        //vector<Vehicle>::iterator it=toAdopt.begin();

        auto lst = provision->vehicle->getAndClearIncomingVehicles();
        KDEBUG("size of incoming vehicle list is " << lst.size());

        if(sumoConnection->getGhostingEnabled()){
            sumoConnection->removeAllGhostsFromSim();
        }

        for (datamodel::Micro m : lst) {

            if(sumoConnection->getGhostingEnabled()){

                //adoption or ghost?
                //we must avoid to add vehicles that are on edges we subscribed only for ghost feedback
                //var 1: check if edge is outgoing --> evaluate performance impact
                if(!sumoConnection->isRespLink(m.edge)){

                    // in addition, we need to have the vid in our ghost list in order to update
                    if(sumoConnection->isVehicleGhost(m.vehicleID)){
                        sumoConnection->updateGhost(m);
                    }

                    continue;
                }
            }

            //adoption
            KDEBUG("adopting "<<m.vehicleID);

            //filter visited edges from route
            vector<std::string> effectiveRoute;
            bool alreadyVisited = true;
            for (std::string link : m.route) {
                if (link == m.edge) {
                    alreadyVisited = false;
                }
                if (alreadyVisited) {
                    continue;
                }
                effectiveRoute.push_back(link);
            }

            m.route = effectiveRoute;

            DSTRAFFIC_TRANSFER_DBG("speed: " << m.speed);
            DSTRAFFIC_TRANSFER_DBG("posEdge: " << m.positionEdge);
            DSTRAFFIC_TRANSFER_DBG("lane: " << m.lane);

            if (sumoConnection->adopt(-1, m)) {
                adoptedVehicles++;
                adoptedVehicleIDs.push_back(m.vehicleID+ " at " + std::to_string(t));
            } else {
                KDEBUG("Failed to add vehicle " << m.vehicleID);
                failedAdoptions++;
                failedAdoptionIDs.push_back(m.vehicleID+ " at " + std::to_string(t));
            }
        }

    } catch (const std::exception& e) {
        KERROR("runFederateLoop: Exception at adoptions ");
        KERROR(e.what());
    }
}

void SumoSimulationControl::addObserver(datamodel::Observer o) {
    KDEBUG(" Added Observer task=" << o.Task << " Element=" << o.Element << " Filter=" << o.Filter << " Period=" << o.Period << " Trigger=" << o.Trigger);
    observers.push_back(std::make_shared<SumoObserver>(producer, o, sumoConnection));
}

void SumoSimulationControl::runObservers(int64_t simulatorTimeMs) {
    //user defined observers
    KDEBUG("Number of Observers: " << observers.size());
    for (auto ob : observers) {
        bool succ = dynamic_pointer_cast<SumoObserver>(ob)->observe(simulatorTimeMs);
        if(!succ){
            KERROR("publishing failed for observer="<<ob->observer.task<< " " <<ob->observer.element<< " " <<ob->observer.filter<< " " <<ob->observer.type);
        }
    }
}    

void SumoSimulationControl::provideGhostUpdates(int64_t simulatorTimeMs) {
    #ifndef USING_TRACI
        auto freshlyAdoptedVehicles = sumoConnection->getAndReleaseFreshlyAdoptedVehicles();
        freshlyAdoptedVehicleIDs.clear();
        for (datamodel::Micro veh : freshlyAdoptedVehicles) {
            std::string topic = Config::getInstance()->getProvisionTopic("edge."+escapeSpecialChars(veh.edge)+".vehicles");
            std::vector<char> encoded = AvroHelper::getInstance()->encodeMicro(veh);
            producer->publish(topic, "ghost_"+veh.vehicleID, encoded, simulatorTimeMs, false);
            freshlyAdoptedVehicleIDs.push_back(veh.vehicleID);
            DSTRAFFIC_CTRL_DBG ("simulatorTimeMs="<<simulatorTimeMs<<": providing ghost update for '"<< veh.vehicleID << "' with pos="<<veh.position.x<<"|"<<veh.position.y );
        }
        //fixme: this lead to errors because the providing instance consumed their own messages and continously tried to adopt them again
        // now i keep track of these ids and ignore them when proc messages. not very efficient...
    #endif
}


void SumoSimulationControl::writeVehicleData(int64_t simulatorTimeMs){

       //'''''''''''''''''''''
        // TEST
        //''''''''''''''''''
            try{
                std::ofstream fcd(Config::getInstance()->getLogDir()+"fcd_"+provision->getScenario()->scenarioID+"_"+provision->getSim()->instanceID+".csv", std::ios_base::app);


                for(auto vid : libsumo::Vehicle::getIDList()){
                    datamodel::Micro mic;
                    int64_t time;
                    sumoConnection->vehicle->getVehicle(vid, mic, time);
                    // std::cout << "simulatorTimeMs="<<simulatorTimeMs<<": position of veh after observers: "<<mic.position.x<<"|"<<mic.position.y<<std::endl;
                    // std::cout << "simulatorTimeMs="<<simulatorTimeMs<<": edge of veh after observers: "<<mic.edge<<std::endl;
                    // std::cout << "simulatorTimeMs="<<simulatorTimeMs<<": speed of veh after observers: "<<mic.speed<<std::endl;

                    string line = to_string(simulatorTimeMs)+","+mic.vehicleID+","+to_string(mic.position.x)+","+to_string(mic.position.y)+","+to_string(mic.speed)+"\n";


                    fcd << line;
                }
                fcd.close();

            } catch ( std::exception& e) {}
        //'''''''''''''''''''''
        // TEST
        //''''''''''''''''''
}