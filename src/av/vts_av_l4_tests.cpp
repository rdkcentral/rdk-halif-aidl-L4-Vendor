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
 * @parblock
 *
 * ### L4 Test Cases for AV Modules HAL :
 *
 * This module includes Level 4 functional tests (success and failure scenarios).
 * This is to ensure that the API meets the operational requirements of the module across all vendors.
 *
 * **Pre-Conditions:** @n
 * **Dependencies:** None@n
 *
 * @endparblock
 *
 */

/**
 * @file vts_av_l4_tests.c
 *
 */

#include <thread>
#include <chrono>

#include <ut.h>
#include <ut_log.h>
#include <ut_kvp_profile.h>
#include <ut_control_plane.h>

#include "vts_player.h"
#include "vts_gstreamer_player.h"
#include "vts_av_l4_init.h"
#include "vts_helper_functions.h"
#include "vts_av_l4_parse_testconfig.h"



using namespace std;

static vtsL4AVTestConfiguration_t gTestConfiguration;
static vtsL4AVStreamConfiguration_t gStreamConfiguration;

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L4_AVMODULE, UT_TESTS_L4)

class VTS_L4_AVMODULE : public UTCore
{
    public:
        VTS_L4_AVMODULE() : UTCore()
        {
            mPlayer = nullptr;
        }

        ~VTS_L4_AVMODULE() noexcept override = default;

    public:
        std::unique_ptr<VTSPlayer> mPlayer;
        const char *mModuleName = "AVModule";
        const char *mTestGroup = "L4";

    void SetUp() override
    {
        try {
#ifndef ENABLE_L4_ONLY
            // Binder threadpool startup is sufficient for these L4 playback tests.
            android::ProcessState::self()->startThreadPool();
#endif

        } catch (const std::exception& ex) {
            UT_LOG_ERROR("Exception in SetUp(): %s", ex.what());
            UT_ASSERT_TRUE(false);
        } catch (...) {
            UT_LOG_ERROR("Unknown exception in SetUp()");
            UT_ASSERT_TRUE(false);
        }
    }

    void playWithTrickModes(std::string streamPath, std::vector<vtsL4TrickMode_t> trickModes)
    {
        string playerName = vts_av_parse_get_palayer("AVModule");

        mPlayer = VTSPlayer::create(playerName);
        if (!mPlayer)
        {
            UT_ASSERT_TRUE(false);
            UT_LOG_ERROR("Failed to create the player");
            return;
        }

        if (!mPlayer->init())
        {
            UT_ASSERT_TRUE(false);
            UT_LOG_ERROR("Player Init failed");
            return;
        }

        for(auto& trickMode: trickModes)
        {
            switch(trickMode.action)
            {
                case VTS_L4_PLAYER_PLAY:
                {
                    UT_LOG_INFO("Playing State: PLAY");
                    mPlayer->play(streamPath);
                    if (trickMode.duration > 0)
                    {
                        this_thread::sleep_for(chrono::seconds(trickMode.duration));
                    }
                    else
                    {
                        mPlayer->loop();
                    }
                    break;
                }
                case VTS_L4_PLAYER_PAUSE:
                {
                    UT_LOG_INFO("Playing State: PAUSE");
                    mPlayer->pause();
                    if (trickMode.duration > 0)
                    {
                        this_thread::sleep_for(chrono::seconds(trickMode.duration));
                    }
                    break;
                }
                case VTS_L4_PLAYER_STOP:
                {
                    UT_LOG_INFO("Playing State: STOP");
                    mPlayer->stop();
                    break;
                }
                case VTS_L4_PLAYER_SEEK:
                {
                    if(trickMode.position < 0)
                    {
                        UT_LOG_INFO("Playing JUMP Backward: %ld ms", trickMode.position);
                    }
                    else
                    {
                        UT_LOG_INFO("Playing JUMP Forward: %ld ms", trickMode.position);
                    }
                    mPlayer->seek(trickMode.position);
                    if (trickMode.duration > 0)
                    {
                        this_thread::sleep_for(chrono::seconds(trickMode.duration));
                    }
                    break;
                }
                case VTS_L4_PLAYER_TRICK_MODE:
                {
                    UT_LOG_INFO("Playing At Speed: %fx", trickMode.speed);
                    mPlayer->trickMode(trickMode.speed);
                    if (trickMode.duration > 0)
                    {
                        this_thread::sleep_for(chrono::seconds(trickMode.duration));
                    }
                    break;
                }
                default:
                    UT_LOG_ERROR("Unknown trick mode action: %d", trickMode.action);
                    UT_ASSERT_TRUE(false);
            }
        }

        mPlayer->stop();
        mPlayer->close();
        mPlayer.reset();
    }

    void startTest(const char* moduleName, const char * testGroup, const char *testName)
    {
        UT_status_t status;
        bool requireQualityCheck = false;
        // Clear testStreams to avoid accumulating streams from previous runs
        gTestConfiguration.testStreams.clear();
        gTestConfiguration.trickModes.clear();

        status = vts_av_parse_testconfig(moduleName, testGroup, testName, gTestConfiguration);

        if(status != UT_STATUS_OK)
        {
            UT_LOG_INFO("Failed to parse the testconfiguration");
            UT_ASSERT_TRUE(false);
            return;
        }

        for(auto& stream: gTestConfiguration.testStreams)
        {
            status = vts_av_parse_streamconfig(stream, gStreamConfiguration);

            if(status != UT_STATUS_OK)
            {
                UT_LOG_INFO("Failed to parse the streamconfiguration");
                UT_ASSERT_TRUE(false);
                return;
            }
            UT_LOG_INFO("Testing Stream: %s", gStreamConfiguration.input.c_str());
            playWithTrickModes(gStreamConfiguration.input, gTestConfiguration.trickModes);
            this_thread::sleep_for(chrono::milliseconds(gTestConfiguration.playDealy));

            requireQualityCheck = gTestConfiguration.userChoice;
            if(requireQualityCheck)
            {
                bool response = validateTest("\nIs the stream playback good (Y/N)?");
                UT_ASSERT_TRUE(response);
            }
        }
    }

    void readAndDiscardRestOfLine(FILE *in)
    {
        int c;
        while ((c = fgetc(in)) != EOF && c != '\n');
    }

    void readString(char *choice)
    {
        scanf("%s", choice);
        readAndDiscardRestOfLine(stdin);
    }

    bool validateTest(std::string query)
    {
        char response[3];
        std::string compare;
        std::cout << query << endl;
        readString(response);

        compare = response;

        if (compare == "y" || compare == "Y")
        {
            return true;
        }
        return false;
    }

    void TearDown() override
    {
    }
};

/**
 * @brief Test the ability to play media file
 *
 * This test verifies the playback of media file
 *
 * **Test Group ID:** 04@n
 * **Test Case ID:** 001@n
 *
 * **Test Procedure:**
 * Refer to UT specification documentation [audioDecoder_High-Level_TestSpecification.md](../../docs/pages/audioDecoder_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L4_AVMODULE, NormalPlay)
{
    const char *testName = "NormalPlay";

    startTest(mModuleName, mTestGroup, testName);
}

/**
 * @brief Test the ability to play media file with trick modes specified in configuration file
 *
 * This test verifies the playback of media file with trick modes specified in configuration file
 *
 * **Test Group ID:** 04@n
 * **Test Case ID:** 002@n
 *
 * **Test Procedure:**
 * Refer to UT specification documentation [audioDecoder_High-Level_TestSpecification.md](../../docs/pages/audioDecoder_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L4_AVMODULE, playBackWithTrickModes)
{
    const char *testName = "playBackWithTrickModes";

    startTest(mModuleName, mTestGroup, testName);
}

/** @} */ // End of AV_MODULE_HALTEST_L4
/** @} */ // End of AV_MODULE_HALTEST
/** @} */ // End of AV_MODULE
/** @} */ // End of HPK
