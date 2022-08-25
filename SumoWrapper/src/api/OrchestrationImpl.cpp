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
#include "OrchestrationImpl.h"
using namespace daceDS;

std::vector<std::string> Orchestration::methods = {
    "ctrl"/*,
    "status"*/};

void OrchestrationImpl::ctrl(OrchestrationTopicMetadata& ometa, datamodel::CtrlMsg& ctrl) {
    if (ctrl.Command == "terminate") {
        cout << "I should terminate " << endl;
        cout << "I should terminate " << endl;
        cout << "I should terminate " << endl;
        cout << "I should terminate " << endl;
        wrapper->endWrapper();
    } else if (ctrl.Command == "die" || ctrl.Command == "kill") {
        cout << "should die abruptly " << endl;
        cout << "should die abruptly " << endl;
        cout << "should die abruptly " << endl;
        cout << "should die abruptly " << endl;
        wrapper->killWrapper();
        exit(1);
    } else {
        cout << "handleOrchestrationCtrl request=" << ctrl.Command << endl;
        cout << "handleOrchestrationCtrl request=" << ctrl.Command << endl;
        cout << "handleOrchestrationCtrl request=" << ctrl.Command << endl;
        cout << "handleOrchestrationCtrl request=" << ctrl.Command << endl;
    }
}

void OrchestrationImpl::status(OrchestrationTopicMetadata& ometa, datamodel::StatusMsg& state) {
    //cout << "Command::STATUS action=" << state.Action << endl;
}
