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

#include <iostream>
#include <sstream>
#include <string>

#include "Utils.h"

namespace daceDS {

#ifdef KDEBUG3
#define KDEBUG3(msg)                                                                                        \
    do {                                                                                                   \
        std::cout << daceDS::getCurrentDate() << " | KDEBUG3 " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define KDEBUG3(msg) do { } while (false)
#endif

#ifdef KDEBUG
#define KDEBUG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DEBUG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define KDEBUG(msg) do { } while (false) 
#endif

#define KERROR(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [ERROR] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
// #define KERROR(msg) do { } while (false)


#define KINFO(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [INFO] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)



#ifdef KDBGCB
#define KDBGCB(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DBGCB] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define KDBGCB(msg) do { } while (false) 
#endif

#ifdef DS_BASE_DBG
#define DS_BASE_DBG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DS_BASE_DBG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define DS_BASE_DBG(msg) do { } while (false) 
#endif

#ifdef DS_AVRO_DBG
#define DS_AVRO_DBG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DS_AVRO_DBG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define DS_AVRO_DBG(msg) do { } while (false) 
#endif



#ifdef DS_CTRL_DBG
#define DS_CTRL_DBG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DS_CTRL_DBG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define DS_CTRL_DBG(msg) do { } while (false) 
#endif

#ifdef DS_SYNC_DBG
#define DS_SYNC_DBG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DS_SYNC_DBG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define DS_SYNC_DBG(msg) do { } while (false) 
#endif




#ifdef DSTRAFFIC_TRANSFER_DBG
#define DSTRAFFIC_TRANSFER_DBG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DSTRAFFIC_TRANSFER_DBG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define DSTRAFFIC_TRANSFER_DBG(msg) do { } while (false) 
#endif


#ifdef DSTRAFFIC_CTRL_DBG
#define DSTRAFFIC_CTRL_DBG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DSTRAFFIC_CTRL_DBG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define DSTRAFFIC_CTRL_DBG(msg) do { } while (false) 
#endif

#ifdef DSTRAFFIC_MEASURE_DBG
#define DSTRAFFIC_MEASURE_DBG(msg) \
    do {             \
        std::cout << getCurrentDate() << " | [DSTRAFFIC_MEASURE_DBG] " << __PRETTY_FUNCTION__ << " | " << msg << std::endl; \
    } while (false)
#else
#define DSTRAFFIC_MEASURE_DBG(msg) do { } while (false) 
#endif





}