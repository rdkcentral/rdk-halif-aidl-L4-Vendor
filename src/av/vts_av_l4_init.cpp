/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2016 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**
 * @addtogroup HPK Hardware Porting Kit
 * @{
 *
 */

/**
 * @addtogroup AV_MODULE AV pipeline Module
 * @{
 *
 */

/**
 * @defgroup AV_MODULE_HALTEST AV Module HAL Tests
 * @{
 *
 */

/**
 * @defgroup AV_MODULE_HALTEST_L4 AV Module HAL Tests L4 File
 * @{
 *
 * /

/**
 * @defgroup AV_MODULE_HALTEST_L4_INIT Initializes L4 test av module test
 * @{
 * @parblock
 *
 * ### Initializes L4 test av module test
 *
 * Initializes L4 test av module test
 *
 * **Pre-Conditions:** @n
 * **Dependencies:** None@n
 *
 * @endparblock
 *
 */

/**
 * @file vts_av_l4_init.c
 *
 */

#include "vts_av_l4_init.h"
#include "vts_av_l4_parse_testconfig.h"

using namespace std;

UT_status_t VTS_AV_init()
{


    string playerName = vts_av_parse_get_palayer("AVModule");

    VTSPlayer::globalInit(playerName);
    return UT_STATUS_OK;
}

void VTS_AV_destroy()
{
    string playerName = vts_av_parse_get_palayer("AVModule");
    VTSPlayer::globalDeInit(playerName);
}

/** @} */ // End of AV_MODULE_HALTEST_L4_INIT
/** @} */ // End of AV_MODULE_HALTEST_L4
/** @} */ // End of AV_MODULE_HALTEST
/** @} */ // End of AV_MODULE
/** @} */ // End of HPK