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
#include "SumoWrapper.h"



#ifndef TESTSWITCH
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "usage: SumoWrapper Scenarioid SUMOid" << endl;
        return 0;
    }

    if (!daceDS::Config::getInstance()->readConfig(CONFIG_PROPERTIES)) {
        cout << "Config not found under " << CONFIG_PROPERTIES << "! Exiting..." << endl;
        return 0;
    }

    std::string scenarioID = argv[1];
    std::string simulatorID = argv[2];

    cout << "SumoWrapper started with sceID=" << scenarioID << " and simID=" << simulatorID << endl;

    daceDS::Config::getInstance()->setScenarioID(scenarioID);
    daceDS::Config::getInstance()->setSimulatorID(simulatorID);

    daceDS::SumoWrapper wrapper(scenarioID, simulatorID);

    cout << "running wrapper" << endl;
    wrapper.runWrapper();
    cout << "terminating wrapper" << endl;
    wrapper.terminateWrapper();
}
#endif

void daceDS::SumoWrapper::runWrapper() {
    std::string host = Config::getInstance()->get("kafkaBroker");
    KDEBUG("host=" << host);

    //setup
    producer = std::make_shared<KafkaProducer>(true);
    std::vector<std::string> pubTopics;
    producer->init(host, pubTopics, "provision");
    KDEBUG("producer init");

    statusProducer = std::make_shared<KafkaProducer>(false);
    std::vector<std::string> pubTopics2;
    statusProducer->init(host, pubTopics2, "status");
    KDEBUG("statusProducer init");

    statusMsg("started");

    ctrl.reset(new SumoSimulationControl());
    provision = std::make_shared<ProvisionImpl>(std::shared_ptr<SumoWrapper>(this), ctrl);

    //orchestration
    std::shared_ptr<OrchestrationHandler> ohdl = std::make_shared<OrchestrationHandler>(std::make_shared<OrchestrationImpl>(this));
    orchestrationConsumer.reset(new KafkaConsumer(ohdl, false));
    // int port = 12345 + atoi(scenarioID.substr(scenarioID.size() - 4).c_str()) + atoi(simulatorID.substr(simulatorID.size() - 1).c_str());
    int port = 12345 + (rand() % 10000);
    KDEBUG("Sumo port will be " << port);

    //provision
    std::shared_ptr<ProvisionHandlerTrafficMicro> phdl = std::make_shared<ProvisionHandlerTrafficMicro>(provision);
    
    ctrl->init(producer, std::shared_ptr<SumoWrapper>(this), provision, port, phdl);  //creates SumoConnection

    //interaction
    std::shared_ptr<InteractionImpl> interactionImpl = std::make_shared<InteractionImpl>(ctrl->getSumoConnection());
    std::shared_ptr<InteractionHandler> ihdl = std::make_shared<InteractionHandler>(interactionImpl, producer);
    interactionConsumer.reset(new KafkaConsumer(ihdl, true));



    //init kafka
    //first, create directory: we might receive resources
    createDirs();

    //then we fetch the scenario definition
    KafkaConsumer scenarioConsumer(phdl);   //we need this only once, is deleted afterwards
    std::string scenarioTopic = Config::getInstance()->getProvisionBaseTopic(TOPIC_SCENARIO);
    KDEBUG("Subscribing to " << scenarioTopic);

    std::vector<std::string> scenarioTopics;
    scenarioTopics.push_back(scenarioTopic);
    scenarioConsumer.subscribe(host, scenarioTopics, Constants::STR_SCENARIO_CONSUMER, "");

    statusMsg("(waiting for scenario description...)");
    auto sce = provision->waitForScenario();
    auto sim = provision->getSim();
    scenarioConsumer.stop();


    //this needs to happen after scenario definition was received, otherwise we don't now which resources we need to save
    KafkaConsumer resourceConsumer(phdl);
    std::string resourceTopic = Config::getInstance()->getProvisionBaseTopic("resource");

    std::vector<std::string> resourceTopics;
    resourceTopics.push_back(resourceTopic);
    resourceConsumer.subscribe(host, resourceTopics, Constants::STR_PROVISION_CONSUMER);
    statusMsg("(waiting for resources...)");
    provision->waitForResources();
    resourceConsumer.stop();


    //convert map and generate config files
    std::string roadMapPath = provision->getFileByType(STR_TYPE_ROADMAP);
    std::string sumoconfig = PrepareRun::createSumoConf(sce, sim, roadMapPath);
    std::string absoluteRoadMapPath = Config::getInstance()->getResourceDir()+"/"+roadMapPath;
    std::cout << "looking for roadmap at " <<  absoluteRoadMapPath << std::endl;
    
    std::vector<std::string> responsibleLinks = PrepareRun::getResponsibleLinks(absoluteRoadMapPath, sim->responsibilities);
    std::vector<std::string> incomingLinks = PrepareRun::getIncomingLinks(absoluteRoadMapPath, sim->responsibilities); //is this fasteR?
    std::vector<std::string> outgoingLinks = PrepareRun::getOutgoingLinks(absoluteRoadMapPath, sim->responsibilities);

    //FIXME: what to do for runs, where we may cover all nodes or none?
    //tmp workaround:
    if (incomingLinks.size()==0){ incomingLinks.push_back("42foo");  }
    if (responsibleLinks.size()==0){ responsibleLinks.push_back("42foo"); }

    std::cout << "responsibleLinks: " <<  responsibleLinks.size() << ", incomingLinks: " << incomingLinks.size() << ", outgoingLinks: " << outgoingLinks.size() << std::endl;

    try {
        //fetch methods(=topics) and add scenario related topic part
        //these are independet from layer definition
        std::vector<std::string> orchestrationTopics = Orchestration::getMethods();
        for (int j = 0; j < orchestrationTopics.size(); j++) {
            orchestrationTopics[j] = Config::getInstance()->getOrchestrationTopic(orchestrationTopics[j]);
        }
        orchestrationConsumer->subscribe(host, orchestrationTopics, Constants::STR_ORCHESTRATION_CONSUMER);

        // //these depend on the layer definition
        // std::vector<std::string> interactionTopics = Interaction::getMethods();
        // for (int j = 0; j < interactionTopics.size(); j++) {
        //     interactionTopics[j] = Config::getInstance()->getAPITopic(interactionTopics[j]);
        // }
        //new: we have only one interaction topic
        std::vector<std::string> interactionTopics;
        interactionTopics.push_back(Config::getInstance()->getInteractionTopic("traffic", "micro")+".request");
        interactionConsumer->subscribe(host, interactionTopics, Constants::STR_INTERACTION_CONSUMER);

        //these are independent from the layer
        // std::vector<std::string> provisionTopics = Provision::getMethods();
        // for (int j = 0; j < provisionTopics.size(); j++) {
        //     provisionTopics[j] = Config::getInstance()->getProvisionBaseTopic(provisionTopics[j]);
        // }

        std::vector<std::string> provisionTopics;

        //these depend on the sce (and somehow on the layer, in this case on edges)
        //variant 1: listen to all topics that contain roads in our responsibilty and that are out of micro layer
        for (std::string link : responsibleLinks) {
        // variant 2: listen to all topics that contain incoming roads to our responsibilty region (and that are out of micro layer)
        // for (std::string link : incomingLinks) {
            //														.scenarioid.domain . micro .road.[roadid](.vehicles)
            auto escapedLink = escapeSpecialChars(link);
            // std::string trafficprovisiontopic = "^provision\\.simulation\\." + scenarioID + "\\.traffic\\.micro\\.edge\\." + escapedLink + "\\.vehicles";
            std::string trafficprovisiontopic = "provision.simulation." + scenarioID + ".traffic.micro.edge." + escapedLink + ".vehicles";
            provisionTopics.push_back(trafficprovisiontopic);
        }

       
        //if we want ghosting we also need to listen on outgoing topics 
        //per default it is enabled
        bool ghosting = true;
        for(auto const& layerParam : provision->getSim()->parameters){
            if(layerParam.first == "ghosting") {
                if(layerParam.second == "false"){
                    ghosting = false;
                    std::cout << "Disabling ghosting!" << std::endl;
                }
            }
        }
        if(ghosting){

            std::cout << "Ghosting is enabled!" << std::endl;
            for (std::string link : outgoingLinks) {
                auto escapedLink = escapeSpecialChars(link);
                std::string trafficprovisiontopic = "provision.simulation." + scenarioID + ".traffic.micro.edge." + escapedLink + ".vehicles";
                provisionTopics.push_back(trafficprovisiontopic);
            }
        }

        provisionConsumer.reset(new KafkaConsumer(phdl, true));
        provisionConsumer->subscribe(host, provisionTopics, Constants::STR_PROVISION_CONSUMER);

    } catch (exception& e) {
        KERROR("ERROR WHILE STARTING CONSUMER" << e.what());
        exit(-1);
    }



    //////////////////////
    ////// Observers
    //////////////////////

    // add observer for traffic info on outgoing links 
    bool first = true;
    std::string filter = "";
    for (std::string outgoingLink : outgoingLinks) {
        if (outgoingLink == "" || outgoingLink == "none") {
            continue;
        }
        if (first) {
            first = false;
        } else {
            filter += " ";
        }

        filter += outgoingLink;
    }
    
    // //only if we have at least one entry
    // if(filter.size()>0){
    //     datamodel::Observer o;
    //     o.Task = "publish";
    //     o.Period = sim->stepLength;
    //     o.Element = "edge.vehicles";
    //     o.Filter = filter;
    //     o.Trigger = "not_empty";
    //     o.Type = "avro";
    //     ctrl->addObserver(o);  //todo: getidlist crashes in sumo api
    // }

    ctrl->setOutgoingLinks(outgoingLinks);
    
    // now the others, assuming ordering of observer list is persisted
    KDEBUG("adding " << sim->observers.size() << " observers");
    for (datamodel::Observer o : sim->observers) {
        ctrl->addObserver(o);
        KDEBUG("     added: " << o.Task << " on " << o.Element);
    }

    /////////////////////////////
    //// Timing
    /////////////////////////////

    //join timinig process
    std::string timeTopic = Config::getInstance()->getOrchestrationTopic(TOPIC_SYNCMSG);
    int64_t noParticipants = sce->execution.syncedParticipants;

    timeSync = std::make_shared<TimeSync>(timeTopic, noParticipants);
    //std::shared_ptr<TimeSyncHandler> thdl = std::make_shared<TimeSyncHandler>(timeSync);
    //timeSync->init(std::make_shared<KafkaProducer>(), std::make_shared<KafkaConsumer>(thdl));

    timeSync->init<KafkaConsumer, KafkaProducer>();
    timeSync->prepare();

    provisionConsumer->setTimeSync(timeSync);
    interactionConsumer->setTimeSync(timeSync);

    //////////////////////////
    //// Let's start
    ///////////////////////////
    KINFO("Starting SUMO");
    statusMsg("starting sumo executable...");


    #ifdef USING_TRACI
        sumoPID = PrepareRun::startSUMO(scenarioID, sim, port, sumoconfig);
        usleep(5*1000*1000);
        //check if already failed
        if(PrepareRun::checkFailure(sumoPID)){
            statusMsg("failed (to start sumo)");
            killWrapper();
        }
    #else 
        PrepareRun::startLibSUMO(scenarioID, sim, sumoconfig);
    #endif

    KINFO("Starting SimulationController");
    ctrl->run();
    KINFO("SimulationController done");

    terminateWrapper();
}

/* terminate execution in a normal way */
void daceDS::SumoWrapper::terminateWrapper() {
    KERROR("terminateWrapper() is called: Closing SUMO");
    ctrl->close();
   
    usleep(3*1000*1000);

    auto resultsProducer = std::make_shared<KafkaProducer>(true);
    std::vector<std::string> pubTopics;
    resultsProducer->init(Config::getInstance()->get("kafkaBroker"), pubTopics, "results");

    PrepareRun::sendResults(Config::getInstance()->getScenarioID(), provision->getSim(), resultsProducer);
    statusMsg("finished");
    usleep(10*1000*1000);
    
    KINFO("Leaving timing");
    timeSync->leaveTiming();
    KINFO("Stopping consumers");
    provisionConsumer->stop();
    interactionConsumer->stop();
    orchestrationConsumer->stop();

    KINFO("bye bye");
    exit(0);  // important: may be called from other places
}

/* do not interacit with simservice anymore */
void daceDS::SumoWrapper::killWrapper() {
    KERROR("exiting abruptly");
    //cleanUp(); delete folders and files
    ctrl->close();
    provisionConsumer->stop();
    interactionConsumer->stop();
    orchestrationConsumer->stop();
    if(sumoPID>0){
        kill(sumoPID, SIGKILL);
    }
    abort();
}

void daceDS::SumoWrapper::statusMsg(std::string msg){
    statusProducer->publish(Config::getInstance()->getOrchestrationTopic(TOPIC_STATUSMSG), simulatorID+": "+msg);
}