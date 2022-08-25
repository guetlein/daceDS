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
#include "Utils.h"
namespace daceDS {

void createDirs() {
    std::string dir = Config::getInstance()->getResourceDir();
    KDEBUG("getResourceDir=" << dir);
    if (std::filesystem::create_directories(dir)) {
        KDEBUG("Created resourceDir!");
    } else {
        KERROR("cannot create folder error: " << strerror(errno));
        exit(1);
    }

    dir = Config::getInstance()->getOutputDir();
    KDEBUG("getOutputDir=" << dir);
    if (std::filesystem::create_directories(dir)) {
        KDEBUG("Created OutputDir!");
    } else {
        KERROR("cannot create folder error: " << strerror(errno));
        exit(1);
    }

    dir = Config::getInstance()->getLogDir();
    KDEBUG("getLogDir=" << dir);
    if (std::filesystem::exists(dir)) {
        KDEBUG("logDir already existing!");
    } else {
        if (std::filesystem::create_directories(dir)) {
            KDEBUG("Created logDir!");
        } else {
            KERROR("cannot create folder error: " << strerror(errno));
            exit(1);
        }
    }
}

std::string GetCurrentWorkingDir(void) {
    char buff[FILENAME_MAX];
    getcwd(buff, FILENAME_MAX);
    std::string current_working_dir(buff);
    return current_working_dir;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string concat(std::vector<std::string> v, char delimiter) {
    std::stringstream ss;
    for (std::string e : v) {
        ss << e << delimiter;
    }
    return ss.str();
}

bool isValidAttributeStr(std::string att) {
    return std::find(validAttributes.begin(), validAttributes.end(), att) !=
           validAttributes.end();
}

const std::string getCurrentDate() {
    char cBuffer[100];
    time_t zaman;
    struct tm* ltime;
    static struct timeval _t;
    static struct timezone tz;

    time(&zaman);
    ltime = (struct tm*)localtime(&zaman);
    gettimeofday(&_t, &tz);

    strftime(cBuffer, 40, "%d.%m.%y %H:%M:%S", ltime);
    sprintf(cBuffer, "%s.%d", cBuffer, (int)_t.tv_usec);
    return std::string(cBuffer);
}

static void printSce(datamodel::Scenario& s, datamodel::BB& t) {
    std::cout << ":: BB Config ::" << std::endl;
    std::cout << "ScenarioID: " << s.scenarioID << std::endl;
    std::cout << "Start     : " << s.simulationStart << std::endl;
    std::cout << "End       : " << s.simulationEnd << std::endl;
    std::cout << "SimID     : " << t.instanceID << std::endl;
    std::cout << "StepLength: " << t.stepLength << std::endl;
    std::cout << "Layer     : " << t.layer << std::endl;
}

std::string strReplace(std::string s, std::string from, std::string to) {
    if (!from.empty())
        for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
            s.replace(pos, from.size(), to);
    return s;
}

std::string escapeSpecialChars(std::string s) {
    //check for magic string and escape ("---"" -> "---2d---2d---2d")
    size_t pos = 0;
    while ((pos = s.find(daceDS::Constants::STR_MAGIC, pos)) != std::string::npos) {
        s.replace(pos, daceDS::Constants::STR_MAGIC.length(), daceDS::Constants::STR_MAGIC_REPLACE);
        pos += daceDS::Constants::STR_MAGIC_REPLACE.length();
    }

    //replace everything besides ['-',a-z,0-9] with "---HEX(c)"
    std::stringstream ss;
    for (int i = 0; i < s.length(); i++) {
        char c = s.at(i);
        if (!(isalpha(c) || isdigit(c) || c == '-')) {
            ss << daceDS::Constants::STR_MAGIC << std::setfill('0') << std::setw(2) << std::right << std::hex << (int)c << std::dec;
        } else {
            ss << c;
        }
    }
    return ss.str();
}

std::string decodeSpecialChars(std::string s) {
    //check for magic string and extract special char)
    size_t pos = 0;
    while ((pos = s.find(daceDS::Constants::STR_MAGIC, pos)) != std::string::npos) {
        //remove magic string
        s.replace(pos, daceDS::Constants::STR_MAGIC.length(), "");
        //convert hex to ascii
        std::string hexRepresentation = s.substr(pos, 2);
        char c = (char)stoi(hexRepresentation);
        s.replace(pos, 2, std::to_string(c));
        pos += 1;
    }
}
}
