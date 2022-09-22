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

#include <exception>
#include <memory>
#include <string>
#include <map>
#include <vector>

#include "util/Config.h"
#include "util/Defines.h"


namespace daceDS {
class Interaction {
   public:
    Interaction(){};
    virtual ~Interaction(){};

    static std::vector<std::string> methods;
    static std::vector<std::string> getMethods() {
        return methods;
    }

};
}  // namespace daceDS
