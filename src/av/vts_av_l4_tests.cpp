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

#include "vts_audiodecoder.h"
#include "vts_videodecoder.h"

using namespace std;

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L4_AVMODULE, UT_TESTS_L4)

static sp<IAVBuffer> vts_avbuffer_getService()
{
    return
        android::interface_cast<IAVBuffer>(
            android::defaultServiceManager()->getService(
                android::String16(IAVBuffer::serviceName().c_str())
            )
        );
}

class VTS_L4_AVMODULE : public UTCore
{
public:
    VTS_L4_AVMODULE() : UTCore()
    {
        mPlayer = nullptr;
    }

    ~VTS_L4_AVMODULE() override = default;

    public:
        std::unique_ptr<VTSPlayer> mPlayer;
        const char *mModuleName = "AVModule";
        const char *mTestGroup = "L4";
        vector<tuple<IAudioDecoder::Id, sp<IAudioDecoder>, sp<AudioDecoderEventListener>>> mAudioDecoderList;
        vector<tuple<IVideoDecoder::Id, sp<IVideoDecoder>, sp<VideoDecoderEventListener>>> mVideoDecoderList;

    void SetUp() override
    {

        try {
            vector<IAudioDecoder::Id> audioDecoderIds;
            vector<IVideoDecoder::Id> videoDecoderIds;
            //TODO: check if this required
            android::ProcessState::self()->startThreadPool();

            /* Get the Audio decoder IDs */
            AUDIODECODER_MANAGER_GETAUDIODECODERIDS(audioDecoderIds);

            for(const auto& _decoderId: audioDecoderIds)
            {
                sp<IAudioDecoder> audioDecoder;

                sp<AudioDecoderEventListener> eventListener = new AudioDecoderEventListener();

                eventListener->setDecoderID(_decoderId.value);

                /* Get Audio decoder handle */
                AUDIODECODER_MANAGER_GETAUDIODECODER(_decoderId, audioDecoder);

                /* Register events */
                AUDIODECODER_REGISTER_EVENT_SUCCESS(audioDecoder, eventListener);

                mAudioDecoderList.push_back({_decoderId, audioDecoder, eventListener});
            }

            /* Get the Video decoder IDs */
            VIDEODECODER_MANAGER_GETVIDEODECODERIDS(videoDecoderIds);

            for(const auto& _decoderId: videoDecoderIds)
            {
                sp<IVideoDecoder> videoDecoder;

                sp<VideoDecoderEventListener> eventListener = new VideoDecoderEventListener();

                eventListener->setDecoderID(_decoderId.value);

                /* Get Video decoder handle */
                VIDEODECODER_MANAGER_GETVIDEODECODER(_decoderId, videoDecoder);

                /* Register events */
                VIDEODECODER_REGISTER_EVENT_SUCCESS(videoDecoder, eventListener);

                mVideoDecoderList.push_back({_decoderId, videoDecoder, eventListener});
            }

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
        string playerName = vts_av_parse_get_palayer(gAVTestConfiguration, "AVModule");

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

    void startTest(void* pTestInstance, const char* moduleName, const char * testGroup, const char *testName, bool requireQualityCheck = false)
    {
        vtsL4AVTestConfiguration_t testConfiguration;

        vts_av_parse_testconfig(pTestInstance, moduleName, testGroup, testName, testConfiguration);

        for(auto& stream: testConfiguration.testStreams)
        {
            UT_LOG_INFO("Testing Stream: %s", stream.c_str());
            playWithTrickModes(stream, testConfiguration.trickModes);
            this_thread::sleep_for(chrono::milliseconds(testConfiguration.playDealy));

            if(requireQualityCheck)
            {
                bool response = validateTest("Is the stream playback good (Y/N)?");
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
 * @brief Test the ability to play audio only file
 *
 * This test verifies the playback of simple audiostream.
 *
 * **Test Group ID:** 04@n
 * **Test Case ID:** 001@n
 *
 * **Test Procedure:**
 * Refer to UT specification documentation [audioDecoder_High-Level_TestSpecification.md](../../docs/pages/audioDecoder_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L4_AVMODULE, audioOnlyPlay)
{
    const char *testName = "audioOnlyPlay";
    bool requireQualityCheck = true;

    startTest(gAVTestConfiguration, mModuleName, mTestGroup, testName, requireQualityCheck);
}

/**
 * @brief Test the ability to play audio only file
 *
 * This test verifies the playback of simple audiostream.
 *
 * **Test Group ID:** 04@n
 * **Test Case ID:** 001@n
 *
 * **Test Procedure:**
 * Refer to UT specification documentation [audioDecoder_High-Level_TestSpecification.md](../../docs/pages/audioDecoder_High-Level_TestSpecification.md)
 */
UT_ADD_TEST(VTS_L4_AVMODULE, audioOnlyTrickPlay)
{
    const char *testName = "audioOnlyTrickPlay";

    startTest(gAVTestConfiguration, mModuleName, mTestGroup, testName);
}

/** @} */ // End of AV_MODULE_HALTEST_L4
/** @} */ // End of AV_MODULE_HALTEST
/** @} */ // End of AV_MODULE
/** @} */ // End of HPK