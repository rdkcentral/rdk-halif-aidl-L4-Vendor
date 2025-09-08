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
 * @defgroup AV_MODULE_HALTEST_L4_PARSE_TESTCONFIG Parse L4 av module test configuration
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
#include <filesystem>
#include <string>
#include "vts_av_l4_parse_testconfig.h"

using namespace std;

std::string vts_av_parse_get_palayer(void* pTestInstance, const char *module)
{
    ut_kvp_status_t status;
    ut_kvp_instance_t* kvpTestInstance = (ut_kvp_instance_t*)pTestInstance;
    char playerName[UT_KVP_MAX_ELEMENT_SIZE] = {0};
    std::string playerString;

    status = ut_kvp_getStringField(kvpTestInstance, TEST_PLAYER, playerName, UT_KVP_MAX_ELEMENT_SIZE);

    playerString = playerName;

    return playerString;
}

UT_status_t vts_av_parse_testconfig(void* pTestInstance, const char *module, const char *testGroup,
                                    const char *testName, vtsL4AVTestConfiguration_t& testConfiguration)
{
    ut_kvp_status_t status;
    ut_kvp_instance_t* kvpTestInstance = (ut_kvp_instance_t*)pTestInstance;
    char streamInputPath[UT_KVP_MAX_ELEMENT_SIZE] = {0};
    int32_t streamCount = 0;
    std::filesystem::path fullPath;

    const std::filesystem::path currentPath = std::filesystem::current_path();

    const string testList = string(module) + "." + string(testGroup) + "." + string(testName);
    const string streamList = testList + "." + string("streams");
    const string controlParams = testList + "." + string("controls");

    status = ut_kvp_getStringField(kvpTestInstance, STREAM_INPUT_PATH, streamInputPath, UT_KVP_MAX_ELEMENT_SIZE);

    /* Parse control parameters */
    string playDelay = controlParams + "." + string("delay_between_play");
    string trickModes = controlParams + "." + string("trick_modes");
    testConfiguration.playDealy = ut_kvp_getUInt32Field(kvpTestInstance, playDelay.c_str());

    /* Parse stream list */
    streamCount = ut_kvp_getListCount(kvpTestInstance, streamList.c_str());
    for(int32_t i = 0; i < streamCount; i++)
    {
        char resultKVP[UT_KVP_MAX_ELEMENT_SIZE] = {0};
        string streamName = streamList + "." + to_string(i);
        status = ut_kvp_getStringField(kvpTestInstance, streamName.c_str(), resultKVP, UT_KVP_MAX_ELEMENT_SIZE);
        fullPath = currentPath;
        fullPath.append(streamInputPath);
        fullPath.append(resultKVP);
        testConfiguration.testStreams.push_back(fullPath.string());
    }

    int32_t trickModeCount = ut_kvp_getListCount(kvpTestInstance, trickModes.c_str());
    for(int32_t i = 0; i < trickModeCount; i++)
    {
        vtsL4TrickMode_t trickMode;
        char action[UT_KVP_MAX_ELEMENT_SIZE] = {0};
        string actionKey = trickModes + "." + to_string(i) + "." + string("action");
        string durationKey = trickModes + "." + to_string(i) + "." + string("duration");
        string speedKey = trickModes + "." + to_string(i) + "." + string("speed");
        string positionKeyFwd = trickModes + "." + to_string(i) + "." + string("position_fwd");
        string positionKeyBwd = trickModes + "." + to_string(i) + "." + string("position_bwd");
        status = ut_kvp_getStringField(kvpTestInstance, actionKey.c_str(), action, UT_KVP_MAX_ELEMENT_SIZE);
        if (strcmp(action, "play") == 0)
        {
            trickMode.action = VTS_L4_PLAYER_PLAY;
        }
        else if (strcmp(action, "pause") == 0)
        {
            trickMode.action = VTS_L4_PLAYER_PAUSE;
        }
        else if (strcmp(action, "stop") == 0)
        {
            trickMode.action = VTS_L4_PLAYER_STOP;
        }
        else if (strcmp(action, "seek") == 0)
        {
            trickMode.action = VTS_L4_PLAYER_SEEK;
        }
        else if (strcmp(action, "trick_mode") == 0)
        {
            trickMode.action = VTS_L4_PLAYER_TRICK_MODE;
        }
        else
        {
            UT_LOG_ERROR("Unknown trick mode action: %s", action);
            return UT_STATUS_FAILURE;
        }

        trickMode.duration = 0;
        trickMode.speed = 1.0;
        trickMode.position = 0;
        if (ut_kvp_fieldPresent(kvpTestInstance, durationKey.c_str()))
        {
            uint32_t duration = ut_kvp_getUInt32Field(kvpTestInstance, durationKey.c_str());
            if(duration == 2147483647)
            {
                trickMode.duration = -1; // Use max int32_t value to indicate full length play
            }
            else
            {
                trickMode.duration = duration;
            }
        }
        if (ut_kvp_fieldPresent(kvpTestInstance, speedKey.c_str()))
        {
            trickMode.speed = static_cast<float>(ut_kvp_getFloatField(kvpTestInstance, speedKey.c_str()));
        }
        if (ut_kvp_fieldPresent(kvpTestInstance, positionKeyFwd.c_str()))
        {
            uint32_t position = ut_kvp_getUInt32Field(kvpTestInstance, positionKeyFwd.c_str());
            trickMode.position = position;
        }
        if (ut_kvp_fieldPresent(kvpTestInstance, positionKeyBwd.c_str()))
        {
            int32_t position = static_cast<int32_t>(ut_kvp_getUInt32Field(kvpTestInstance, positionKeyBwd.c_str()));
            trickMode.position = -position;
        }

        testConfiguration.trickModes.push_back(trickMode);
    }

    return UT_STATUS_OK;
}

/** @} */ // End of AV_MODULE_HALTEST_L4_PARSE_TESTCONFIG
/** @} */ // End of AV_MODULE_HALTEST_L4
/** @} */ // End of AV_MODULE_HALTEST
/** @} */ // End of AV_MODULE
/** @} */ // End of HPK