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
#include "vts_audiosink.h"
#include "vts_videosink.h"
#include "vts_avbuffer.h"

using namespace std;

typedef struct audioDecoderMetrics
{
    int32_t id;
    int32_t decodedFrames;
    int32_t errorFrames;
    int32_t errorFrames_cb;
    int32_t droppedFrames;
} audioDecoderMetrics_t;

typedef struct audioSinkMetrics
{
    int32_t id;
    int32_t renderLatency;
} audioSinkMetrics_t;

typedef struct videoDecoderMetrics
{
    int32_t id;
    int32_t decodedFrames;
    int32_t errorFrames;
    int32_t errorFrames_cb;
    int32_t droppedFrames;
} videoDecoderMetrics_t;

typedef struct videoSinkMetrics
{
    int32_t id;
    int32_t receivedFrames;
    int32_t presentedFrames;
    int32_t framesDroppedLate;
    int32_t framesDroppedFRC;
    int32_t framesRepeatedFRC;
    int32_t framesRepeatedMiss;
    int32_t underflow;
} videoSinkMetrics_t;

static vtsL4AVTestConfiguration_t gTestConfiguration;
static vector<audioDecoderMetrics_t> gBeforeAudioDecoderMetrics, gAfterAudioDecoderMetrics;
static audioDecoderMetrics_t gDeltaAudioDecoderMetrics;
static vector<audioSinkMetrics_t> gBeforeAudioSinkMetrics, gAfterAudioSinkMetrics;
static audioSinkMetrics_t gDeltaAudioSinkMetrics;
static vector<videoDecoderMetrics_t> gBeforeVideoDecoderMetrics, gAfterVideoDecoderMetrics;
static videoDecoderMetrics_t gDeltaVideoDecoderMetrics;
static vector<videoSinkMetrics_t> gBeforeVideoSinkMetrics, gAfterVideoSinkMetrics;
static videoSinkMetrics_t gDeltaVideoSinkMetrics;
static HeapMetrics gBeforeHeapMetrics, gAfterHeapMetrics;

/* Test class registration with UT_Core */
UT_ADD_TEST_TO_GROUP(VTS_L4_AVMODULE, UT_TESTS_L4)

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
        vector<tuple<IAudioSink::Id, sp<IAudioSink>, sp<AudioSinkEventListener>>> mAudioSinkList;
        vector<tuple<IVideoDecoder::Id, sp<IVideoDecoder>, sp<VideoDecoderEventListener>>> mVideoDecoderList;
        vector<tuple<IVideoSink::Id, sp<IVideoSink>, sp<VideoSinkEventListener>>> mVideoSinkList;

    void SetUp() override
    {
        try {
            vector<IAudioDecoder::Id> audioDecoderIds;
            vector<IVideoDecoder::Id> videoDecoderIds;
            vector<IAudioSink::Id> audioSinkIds;
            vector<IVideoSink::Id> videoSinkIds;

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

            /* Get the Audio Sink IDs */
            AUDIOSINK_MANAGER_GETAUDIOSINKIDS(audioSinkIds);
            for(const auto& _sinkId: audioSinkIds)
            {
                sp<IAudioSink> audioSink;
                sp<AudioSinkEventListener> eventListener = new AudioSinkEventListener(_sinkId.value);

                /* Get Audio decoder handle */
                AUDIOSINK_MANAGER_GETAUDIOSINK(_sinkId, audioSink);

                /* Register events */
                AUDIOSINK_REGISTER_EVENT_SUCCESS(audioSink, eventListener);

                mAudioSinkList.push_back({_sinkId, audioSink, eventListener});
            }

            /*Get the Video Sink IDs*/
            VIDEOSINK_MANAGER_GETVIDEOSINKIDS(videoSinkIds);
            for(const auto& _sinkId: videoSinkIds)
            {

                sp<IVideoSink> videoSink;
                sp<VideoSinkEventListener> videoSinkEventListener = new VideoSinkEventListener();

                videoSinkEventListener->setSinkID(_sinkId.value);

                /* Get Video Sink handle */
                VIDEOSINK_MANAGER_GETVIDEOSINK(_sinkId, videoSink);

                /* Register events */
                VIDEOSINK_REGISTER_EVENT_SUCCESS(videoSink, videoSinkEventListener);

                mVideoSinkList.push_back({_sinkId, videoSink, videoSinkEventListener});
            }

        } catch (const std::exception& ex) {
            UT_LOG_ERROR("Exception in SetUp(): %s", ex.what());
            UT_ASSERT_TRUE(false);
        } catch (...) {
            UT_LOG_ERROR("Unknown exception in SetUp()");
            UT_ASSERT_TRUE(false);
        }
    }

    void getAudioDecoderMetrics(vector<audioDecoderMetrics_t> &audioDecoderMetrics)
    {
        audioDecoderMetrics.clear();
        for (const auto& [_decoderId, decoderHandle, eventListener] : mAudioDecoderList)
        {
            audioDecoderMetrics_t metrics;

            optional<PropertyValue> propertyValue;
            metrics.decodedFrames = -1;
            metrics.errorFrames = -1;
            metrics.droppedFrames = -1;
            metrics.id = _decoderId.value;

            AUDIODECODER_GET_PROPERTY_SUCCESS(decoderHandle, audiodecoder::Property::METRIC_FRAMES_DECODED, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.decodedFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            AUDIODECODER_GET_PROPERTY_SUCCESS(decoderHandle, audiodecoder::Property::METRIC_DECODE_ERRORS, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.errorFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            AUDIODECODER_GET_PROPERTY_SUCCESS(decoderHandle, audiodecoder::Property::METRIC_FRAMES_DROPPED, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.droppedFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }
            audioDecoderMetrics.push_back(metrics);
        }
    }
    void getAudioSinkMetrics(vector<audioSinkMetrics_t> &audioSinkMetrics)
    {
        audioSinkMetrics.clear();
        for (const auto& [_sinkId, sinkHandle, eventListener] : mAudioSinkList)
        {
            audioSinkMetrics_t metrics;

            optional<PropertyValue> propertyValue;
            metrics.renderLatency = -1;
            metrics.id = _sinkId.value;

            AUDIOSINK_GET_PROPERTY_SUCCESS(sinkHandle, audiosink::Property::RENDER_LATENCY_NS, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.renderLatency = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            audioSinkMetrics.push_back(metrics);
        }
    }
    void getVideoDecoderMetrics(vector<videoDecoderMetrics_t> &videoDecoderMetrics)
    {
        videoDecoderMetrics.clear();
        for (const auto& [_decoderId, decoderHandle, eventListener] : mVideoDecoderList)
        {
            videoDecoderMetrics_t metrics;

            optional<PropertyValue> propertyValue;
            metrics.decodedFrames = -1;
            metrics.errorFrames = -1;
            metrics.droppedFrames = -1;
            metrics.id = _decoderId.value;

            VIDEODECODER_GETPROPERTY(decoderHandle, videodecoder::Property::METRIC_FRAMES_DECODED, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.decodedFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            VIDEODECODER_GETPROPERTY(decoderHandle, videodecoder::Property::METRIC_DECODE_ERRORS, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.errorFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            VIDEODECODER_GETPROPERTY(decoderHandle, videodecoder::Property::METRIC_FRAMES_DROPPED, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.droppedFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }
            videoDecoderMetrics.push_back(metrics);
        }
    }
    void getVideoSinkMetrics(vector<videoSinkMetrics_t> &videoSinkMetrics)
    {
        videoSinkMetrics.clear();
        for (const auto& [_sinkId, sinkHandle, eventListener] : mVideoSinkList)
        {
            videoSinkMetrics_t metrics;

            optional<PropertyValue> propertyValue;

            metrics.receivedFrames = -1;
            metrics.presentedFrames = -1;
            metrics.framesDroppedLate = -1;
            metrics.framesDroppedFRC = -1;
            metrics.framesRepeatedFRC = -1;
            metrics.framesRepeatedMiss = -1;
            metrics.id = _sinkId.value;

            VIDEOSINK_GET_PROPERTY_SUCCESS(sinkHandle, videosink::Property::METRIC_FRAMES_RECEIVED, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.receivedFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            VIDEOSINK_GET_PROPERTY_SUCCESS(sinkHandle, videosink::Property::METRIC_FRAMES_PRESENTED, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.presentedFrames = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            VIDEOSINK_GET_PROPERTY_SUCCESS(sinkHandle, videosink::Property::METRIC_FRAMES_DROPPED_LATE, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.framesDroppedLate = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            VIDEOSINK_GET_PROPERTY_SUCCESS(sinkHandle, videosink::Property::METRIC_FRAMES_DROPPED_FRC, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.framesDroppedFRC = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            VIDEOSINK_GET_PROPERTY_SUCCESS(sinkHandle, videosink::Property::METRIC_FRAMES_REPEATED_FRC, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.framesRepeatedFRC = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }

            VIDEOSINK_GET_PROPERTY_SUCCESS(sinkHandle, videosink::Property::METRIC_FRAMES_REPEATED_MISSING_FRAME, propertyValue);
            if (propertyValue.has_value() && propertyValue->value.has_value())
            {
                metrics.framesRepeatedMiss = propertyValue->value->get<PropertyValue::Value::Tag::intValue>();
            }
            videoSinkMetrics.push_back(metrics);
        }
    }

    void validateAudioDecoderMetrics(int32_t id,
                                     vector<audioDecoderMetrics_t> &audioBeforeDecoderMetrics,
                                     vector<audioDecoderMetrics_t> &audioAfterDecoderMetrics,
                                     audioDecoderMetrics_t &deltaMetrics)
    {
        for (size_t i = 0; i < audioBeforeDecoderMetrics.size(); ++i)
        {
            if(audioBeforeDecoderMetrics[i].id != id)
                continue;

            if(audioBeforeDecoderMetrics[i].decodedFrames != -1 &&
               audioBeforeDecoderMetrics[i].decodedFrames + deltaMetrics.decodedFrames != audioAfterDecoderMetrics[i].decodedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(audioBeforeDecoderMetrics[i].errorFrames != -1 &&
               audioBeforeDecoderMetrics[i].errorFrames + deltaMetrics.errorFrames != audioAfterDecoderMetrics[i].errorFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(audioBeforeDecoderMetrics[i].droppedFrames != -1 &&
               audioBeforeDecoderMetrics[i].droppedFrames + deltaMetrics.droppedFrames != audioAfterDecoderMetrics[i].droppedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            break;
        }
    }

    void validateVideoDecoderMetrics(int32_t id,
                                     vector<videoDecoderMetrics_t> &videoBeforeDecoderMetrics,
                                     vector<videoDecoderMetrics_t> &videoAfterDecoderMetrics,
                                     videoDecoderMetrics_t &deltaMetrics)
    {
        for (size_t i = 0; i < videoBeforeDecoderMetrics.size(); ++i)
        {
            if(videoBeforeDecoderMetrics[i].id != id)
                continue;

            if(videoBeforeDecoderMetrics[i].decodedFrames != -1 &&
               videoBeforeDecoderMetrics[i].decodedFrames + deltaMetrics.decodedFrames != videoAfterDecoderMetrics[i].decodedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoBeforeDecoderMetrics[i].errorFrames != -1 &&
               videoBeforeDecoderMetrics[i].errorFrames + deltaMetrics.errorFrames != videoAfterDecoderMetrics[i].errorFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoBeforeDecoderMetrics[i].droppedFrames != -1 &&
               videoBeforeDecoderMetrics[i].droppedFrames + deltaMetrics.droppedFrames != videoAfterDecoderMetrics[i].droppedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            break;
        }
    }

    void validateVideoSinkMetrics(int32_t id,
                                  vector<videoSinkMetrics_t> &videoBeforeSinkMetrics,
                                  vector<videoSinkMetrics_t> &videoAfterSinkMetrics,
                                  videoSinkMetrics_t &deltaMetrics)
    {
        for (size_t i = 0; i < videoBeforeSinkMetrics.size(); ++i)
        {
            if(videoBeforeSinkMetrics[i].id != id)
                continue;

            if(videoBeforeSinkMetrics[i].presentedFrames != -1 &&
               videoBeforeSinkMetrics[i].presentedFrames + deltaMetrics.presentedFrames != videoAfterSinkMetrics[i].presentedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoBeforeSinkMetrics[i].receivedFrames != -1 &&
               videoBeforeSinkMetrics[i].receivedFrames + deltaMetrics.receivedFrames != videoAfterSinkMetrics[i].receivedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoBeforeSinkMetrics[i].framesDroppedLate != -1 &&
               videoBeforeSinkMetrics[i].framesDroppedLate != videoAfterSinkMetrics[i].framesDroppedLate)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoBeforeSinkMetrics[i].framesRepeatedMiss != -1 &&
               videoBeforeSinkMetrics[i].framesRepeatedMiss != videoAfterSinkMetrics[i].framesRepeatedMiss)
            {
                UT_ASSERT_TRUE(false);
            }
            break;
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
        vts_av_parse_testconfig(pTestInstance, moduleName, testGroup, testName, gTestConfiguration);

        for(auto& stream: gTestConfiguration.testStreams)
        {
            HeapMetrics beforeHeapMetrics, afterHeapMetrics;
            HeapMetrics beforeSecureHeapMetrics, afterSecureHeapMetrics;

            /* Get the metrics before start of play */
            AVBUFFER_GETMETRICS(gAVBufferManager, false, beforeHeapMetrics);
            AVBUFFER_GETMETRICS(gAVBufferManager, true, beforeSecureHeapMetrics);
            getAudioDecoderMetrics(gBeforeAudioDecoderMetrics);
            getVideoDecoderMetrics(gBeforeVideoDecoderMetrics);
            getVideoSinkMetrics(gBeforeVideoSinkMetrics);

            UT_LOG_INFO("Testing Stream: %s", stream.c_str());
            playWithTrickModes(stream, gTestConfiguration.trickModes);
            this_thread::sleep_for(chrono::milliseconds(gTestConfiguration.playDealy));

            /* Get the metrics after start of play */
            getAudioDecoderMetrics(gAfterAudioDecoderMetrics);
            getVideoDecoderMetrics(gAfterVideoDecoderMetrics);
            getVideoSinkMetrics(gAfterVideoSinkMetrics);
            AVBUFFER_GETMETRICS(gAVBufferManager, false, afterHeapMetrics);
            AVBUFFER_GETMETRICS(gAVBufferManager, true, afterSecureHeapMetrics);

            /* Validate the metrics */
            validateAudioDecoderMetrics(AudioDecoderEventListener::getUsedDecoder(), gBeforeAudioDecoderMetrics,
                                        gAfterAudioDecoderMetrics, gDeltaAudioDecoderMetrics);
            validateVideoDecoderMetrics(VideoDecoderEventListener::getUsedDecoder(), gBeforeVideoDecoderMetrics,
                                        gAfterVideoDecoderMetrics, gDeltaVideoDecoderMetrics);
            validateVideoSinkMetrics(VideoSinkEventListener::getUsedDecoder(), gBeforeVideoSinkMetrics,
                                     gAfterVideoSinkMetrics, gDeltaVideoSinkMetrics);

            /* check if the decoder freed all the buffer pools */
            UT_ASSERT_TRUE(beforeHeapMetrics.secure == afterHeapMetrics.secure);
            UT_ASSERT_TRUE(beforeHeapMetrics.bytesUsed == afterHeapMetrics.bytesUsed);
            UT_ASSERT_TRUE(beforeHeapMetrics.bytesTotal == afterHeapMetrics.bytesTotal);

            UT_ASSERT_TRUE(beforeSecureHeapMetrics.secure == afterSecureHeapMetrics.secure);
            UT_ASSERT_TRUE(beforeSecureHeapMetrics.bytesUsed == afterSecureHeapMetrics.bytesUsed);
            UT_ASSERT_TRUE(beforeSecureHeapMetrics.bytesTotal == afterSecureHeapMetrics.bytesTotal);

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