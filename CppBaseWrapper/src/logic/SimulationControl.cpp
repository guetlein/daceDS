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

#include "SimulationControl.h"
using namespace daceDS;

void SimulationControl::addObserver(datamodel::Observer o) {
    KDEBUG(" Added Observer task=" << o.Task << " Element=" << o.Element << " Filter=" << o.Filter << " Period=" << o.Period << " Trigger=" << o.Trigger);
    observers.push_back(std::make_shared<Observer>(producer, o));
}

void SimulationControl::runObservers() {
    for (auto ob : observers) {
        ob->observe();
    }
}
