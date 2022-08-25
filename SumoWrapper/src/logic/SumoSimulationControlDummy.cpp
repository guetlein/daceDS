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
#include "logic/SumoSimulationControlDummy.h"

using namespace daceDS;

void SumoSimulationControlDummy::init(SumoWrapperTraCI* t, SumoWrapper* w) {
    traci = t;
    wrapper = w;
}

void SumoSimulationControlDummy::run() {
    int64_t fakeClock = wrapper->scenarioDescription.SimulationStart;
    for (int64_t i = wrapper->scenarioDescription.SimulationStart; fakeClock < wrapper->scenarioDescription.SimulationEnd + 1;) {
        try {
            traci->setTime(fakeClock);

            //post action phase
            KDEBUG("runObservers: ");
            runObservers();
            KDEBUG("runObservers DONE ");

        } catch (const std::exception& e) {
            KDEBUG("runFederateLoop: Exception at border processing");
            KDEBUG(e.what());
        }
    }
}

void SumoSimulationControlDummy::addObserver(datamodel::Observer o) {
    observers.push_back(o);
}

void SumoSimulationControlDummy::runObservers() {
    //custom observers
    for (datamodel::Observer ob : observers) {
        KDEBUG(" Task=" << ob.Task << " Attribute=" << ob.Attribute << " Filter=" << ob.Filter << " Subject=" << ob.Subject << " Period=" << ob.Period << " Trigger=" << ob.Trigger);

        if (ob.Task != "publish") continue;

        traci->observe(ob);
    }

    //border observers
    for (datamodel::Observer ob : borderObservers) {
        KDEBUG("Running border observer for " << ob.Subject);
        traci->observe(ob);
    }
}
