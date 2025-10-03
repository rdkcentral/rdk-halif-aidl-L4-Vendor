/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2025 RDK Management
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
 */

/**
 * @defgroup AV_MODULE_HALTEST_L4_PARSE_TESTCONFIG_H Parse L4 av module test configuration
 * @{
 * @parblock
 *
 * ### Parses L4 Test configuration:
 *
 * Parses L4 Test configuration
 *
 * **Pre-Conditions:** @n
 * **Dependencies:** None@n
 *
 * @endparblock
 *
 */

/**
 * @file vts_av_l4_parse_testconfig.c
 *
 */

#ifndef __VTS_AV_PARSE_TEST_CONFIG_H__
#define __VTS_AV_PARSE_TEST_CONFIG_H__

#include <vector>
#include <string>
#include <ut.h>
#include <ut_log.h>
#include "ut_kvp_profile.h"

#define AVMODULE_STREAM_DOWNLOAD_PATH "AVModule.stream_download_path"
#define AVMODULE_STREAM_INPUT_PATH "AVModule.input_streams_path"
#define AVMODULE_TEST_PLAYER "AVModule.player"
#define AVMODULE_STREAM_INFO "AVStreams"

typedef enum vtsL4PlayerOperations
{
    VTS_L4_PLAYER_PLAY = 0,
    VTS_L4_PLAYER_PAUSE,
    VTS_L4_PLAYER_STOP,
    VTS_L4_PLAYER_SEEK,
    VTS_L4_PLAYER_TRICK_MODE
} vtsL4PlayerOperations_t;

typedef struct vtsL4TrickMode
{
    vtsL4PlayerOperations_t action;
    int32_t duration; // in seconds
    float speed;       // speed for trick mode
    int64_t position; // position to seek to in msec
} vtsL4TrickMode_t;

typedef struct vtsL4AVTestConfiguration
{
    uint32_t playDealy;
    std::vector<vtsL4TrickMode_t> trickModes;
    std::vector<std::string> testStreams;

} vtsL4AVTestConfiguration_t;

typedef struct vtsL4AVStreamConfiguration
{
    std::string input;
    uint32_t duration;
    uint32_t numOfAudioFrames;
    uint32_t numOfVideoFrames;
    uint32_t numOfAudioErrorFrames;
    uint32_t numOfVideoErrorFrames;
} vtsL4AVStreamConfiguration_t;

UT_status_t vts_av_parse_testconfig(const char *module, const char *testGroup, const char *testName, vtsL4AVTestConfiguration_t& testConfiguration);
std::string vts_av_parse_get_palayer(const char *module);
UT_status_t vts_av_parse_streamconfig(std::string stream, vtsL4AVStreamConfiguration_t& streamConfiguration);

#endif //__VTS_AV_PARSE_TEST_CONFIG_H__

/** @} */ // End of AV_MODULE_HALTEST_L4_PARSE_TESTCONFIG_H
/** @} */ // End of AV_MODULE_HALTEST_L4
/** @} */ // End of AV_MODULE_HALTEST
/** @} */ // End of AV_MODULE
/** @} */ // End of HPK
