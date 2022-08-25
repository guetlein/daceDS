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

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

#include "datamodel/Scenario.hh"
#include "datamodel/BB.hh"
#include "util/Defines.h"
#include "util/log.h"
#include "util/Config.h"

namespace daceDS {

void createDirs();
std::string GetCurrentWorkingDir(void);
std::vector<std::string> split(const std::string& s, char delimiter);
std::string concat(std::vector<std::string> v, char delimiter);
bool isValidAttributeStr(std::string att);
const std::string getCurrentDate();
void printSce(datamodel::Scenario& s, datamodel::BB& t);

std::string strReplace(std::string s, std::string from, std::string to);
std::string escapeSpecialChars(std::string s);
std::string decodeSpecialChars(std::string s);
}  // namespace daceDS
