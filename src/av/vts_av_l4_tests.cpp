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
#include <cstring>

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
static vtsL4AVStreamConfiguration_t gStreamConfiguration;
static vector<audioDecoderMetrics_t> gAudioDecoderMetrics;
static audioDecoderMetrics_t gActualAudioDecoderMetrics;
static vector<audioSinkMetrics_t> gAudioSinkMetrics;
static vector<videoDecoderMetrics_t> gVideoDecoderMetrics;
static videoDecoderMetrics_t gActualVideoDecoderMetrics;
static vector<videoSinkMetrics_t> gVideoSinkMetrics;
static videoSinkMetrics_t gActualVideoSinkMetrics;
HeapMetrics gBeforeHeapMetrics, gAfterHeapMetrics;
HeapMetrics gBeforeSecureHeapMetrics, gAfterSecureHeapMetrics;

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
                                     vector<audioDecoderMetrics_t> &audioDecoderMetrics,
                                     audioDecoderMetrics_t &actualMetrics)
    {
        for (size_t i = 0; i < audioDecoderMetrics.size(); ++i)
        {
            if(audioDecoderMetrics[i].id != id)
                continue;

            if(audioDecoderMetrics[i].decodedFrames != -1 &&
               audioDecoderMetrics[i].decodedFrames != actualMetrics.decodedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(audioDecoderMetrics[i].errorFrames != -1 &&
               audioDecoderMetrics[i].errorFrames != actualMetrics.errorFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(audioDecoderMetrics[i].droppedFrames != -1 &&
               audioDecoderMetrics[i].droppedFrames != 0)
            {
                UT_ASSERT_TRUE(false);
            }
            break;
        }
    }

    void validateVideoDecoderMetrics(int32_t id,
                                     vector<videoDecoderMetrics_t> &videoDecoderMetrics,
                                     videoDecoderMetrics_t &actualMetrics)
    {
        for (size_t i = 0; i < videoDecoderMetrics.size(); ++i)
        {
            if(videoDecoderMetrics[i].id != id)
                continue;

            if(videoDecoderMetrics[i].decodedFrames != -1 &&
               videoDecoderMetrics[i].decodedFrames != actualMetrics.decodedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoDecoderMetrics[i].errorFrames != -1 &&
               videoDecoderMetrics[i].errorFrames != actualMetrics.errorFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoDecoderMetrics[i].droppedFrames != -1 &&
               videoDecoderMetrics[i].droppedFrames != 0)
            {
                UT_ASSERT_TRUE(false);
            }
            break;
        }
    }

    void validateVideoSinkMetrics(int32_t id,
                                  vector<videoSinkMetrics_t> &videoSinkMetrics,
                                  videoSinkMetrics_t &actualMetrics)
    {
        for (size_t i = 0; i < videoSinkMetrics.size(); ++i)
        {
            if(videoSinkMetrics[i].id != id)
                continue;

            //TODO: check how to validate this metric
            /*if(videoSinkMetrics[i].presentedFrames != -1 &&
               videoSinkMetrics[i].presentedFrames != actualMetrics.presentedFrames)
            {
                UT_ASSERT_TRUE(false);
            }*/
            if(videoSinkMetrics[i].receivedFrames != -1 &&
               videoSinkMetrics[i].receivedFrames != actualMetrics.receivedFrames)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoSinkMetrics[i].framesDroppedLate != -1 &&
               videoSinkMetrics[i].framesDroppedLate != 0)
            {
                UT_ASSERT_TRUE(false);
            }
            if(videoSinkMetrics[i].framesRepeatedMiss != -1 &&
               videoSinkMetrics[i].framesRepeatedMiss != 0)
            {
                UT_ASSERT_TRUE(false);
            }
            break;
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
	bool isAudioTrackSwitch = (strcmp(testName, "AudioTrackSwitch") == 0);
	bool isVideoTrackSwitch = (strcmp(testName, "VideoTrackSwitch") == 0);
        // TODO: Currently all platforms support only one id (0).
        // Update this at runtime when multi-instance support is available.
        int32_t id = 0;
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
            /* Get the metrics before start of play */
            AVBUFFER_GETMETRICS(gAVBufferManager, false, gBeforeHeapMetrics);
            AVBUFFER_GETMETRICS(gAVBufferManager, true, gBeforeSecureHeapMetrics);

	    if (isAudioTrackSwitch)
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
               // Start playback
               UT_LOG_INFO("[Test] Play stream");
               mPlayer->play(gStreamConfiguration.input);
               // Let track 1 play for 30 seconds
               this_thread::sleep_for(chrono::seconds(30));
               // Switch track 1 -> track 2
               UT_LOG_INFO("[Test] Switch audio track: 1 -> 2");
               bool switchResult = mPlayer->switchAudioTrack(1);
               UT_ASSERT_TRUE(switchResult);
               // Let track 2 play for 30 seconds
               this_thread::sleep_for(chrono::seconds(30));
               // Switch track 2 -> track 1
               UT_LOG_INFO("[Test] Switch audio track: 2 -> 1");
               switchResult = mPlayer->switchAudioTrack(0);
               UT_ASSERT_TRUE(switchResult);
               // Let track 1 play again for 15 seconds
               this_thread::sleep_for(chrono::seconds(30));
               mPlayer->stop();
               mPlayer->close();
               mPlayer.reset();
            }

	    else if (isVideoTrackSwitch)
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
               // Start playback
               UT_LOG_INFO("[Test] Play stream");
               mPlayer->play(gStreamConfiguration.input);
               // Let track 1 play for 30 seconds
               this_thread::sleep_for(chrono::seconds(30));
               // Switch track 1 -> track 2
               UT_LOG_INFO("[Test] Switch video track: 1 -> 2");
               bool switchResult = mPlayer->switchVideoTrack(1);
               UT_ASSERT_TRUE(switchResult);
               // Let track 2 play for 30 seconds
               this_thread::sleep_for(chrono::seconds(30));
               // Switch track 2 -> track 1
               UT_LOG_INFO("[Test] Switch video track: 2 -> 1");
               switchResult = mPlayer->switchVideoTrack(0);
               UT_ASSERT_TRUE(switchResult);
               // Let track 1 play again for 15 seconds
               this_thread::sleep_for(chrono::seconds(30));
               mPlayer->stop();
               mPlayer->close();
               mPlayer.reset();
            }

	    else
	    {
                playWithTrickModes(gStreamConfiguration.input, gTestConfiguration.trickModes);
	    }
            this_thread::sleep_for(chrono::milliseconds(gTestConfiguration.playDealy));

            /* Get the metrics after start of play */
            getAudioDecoderMetrics(gAudioDecoderMetrics);
            getVideoDecoderMetrics(gVideoDecoderMetrics);
            getVideoSinkMetrics(gVideoSinkMetrics);
            AVBUFFER_GETMETRICS(gAVBufferManager, false, gAfterHeapMetrics);
            AVBUFFER_GETMETRICS(gAVBufferManager, true, gAfterSecureHeapMetrics);

            /* Validate the metrics */

            validateAudioDecoderMetrics(id,
                                        gAudioDecoderMetrics, gActualAudioDecoderMetrics);
            validateVideoDecoderMetrics(id,
                                        gVideoDecoderMetrics, gActualVideoDecoderMetrics);
            validateVideoSinkMetrics(id,
                                     gVideoSinkMetrics, gActualVideoSinkMetrics);

            /* check if the decoder freed all the buffer pools */
            UT_ASSERT_TRUE(gBeforeHeapMetrics.secure == gAfterHeapMetrics.secure);
            UT_ASSERT_TRUE(gBeforeHeapMetrics.bytesUsed == gAfterHeapMetrics.bytesUsed);
            UT_ASSERT_TRUE(gBeforeHeapMetrics.bytesTotal == gAfterHeapMetrics.bytesTotal);

            UT_ASSERT_TRUE(gBeforeSecureHeapMetrics.secure == gAfterSecureHeapMetrics.secure);
            UT_ASSERT_TRUE(gBeforeSecureHeapMetrics.bytesUsed == gAfterSecureHeapMetrics.bytesUsed);
            UT_ASSERT_TRUE(gBeforeSecureHeapMetrics.bytesTotal == gAfterSecureHeapMetrics.bytesTotal);

            requireQualityCheck = gTestConfiguration.userChoice;
            if(requireQualityCheck)
            {
		bool response;
		if (isAudioTrackSwitch)
		{
			response = validateTest(
					"\nWas the audio track switching smooth? (Y/N)?");
		}
		else
		{
			response = validateTest(
                                        "\nIs the stream playback good (Y/N)?");
		}
                UT_ASSERT_TRUE(response);
            }
        }
    }

/*
    void startOTFCodecSwitchTest(const char* moduleName, const char * testGroup, const char *testName)
    {
        UT_status_t status;
        bool requireQualityCheck = false;
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
            UT_LOG_INFO("Testing Stream for OTF Codec Switching: %s", gStreamConfiguration.input.c_str());
            
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

            // Start playing (starts with AAC by default)
            UT_LOG_INFO("[Test] Play stream");
            mPlayer->play(gStreamConfiguration.input);

            // Let the stream play for 5 seconds (AAC format)
            this_thread::sleep_for(chrono::seconds(5));

            // Switch to EAC3 on the fly
            UT_LOG_INFO("[Test] Switch audio codec on the fly: AAC -> EAC3");
            bool switchResult = mPlayer->switchAudioCodec(false); // false = EAC3
            UT_ASSERT_TRUE(switchResult);

            // Let the stream play for 5 seconds (EAC3 format)
            this_thread::sleep_for(chrono::seconds(5));

            // Switch back to AAC on the fly
            UT_LOG_INFO("[Test] Switch audio codec on the fly: EAC3 -> AAC");
            switchResult = mPlayer->switchAudioCodec(true); // true = AAC
            UT_ASSERT_TRUE(switchResult);

            // Let the stream play for 5 seconds (AAC format again)
            this_thread::sleep_for(chrono::seconds(5));

            // Stop playback
            mPlayer->stop();
            mPlayer->close();
            mPlayer.reset();

            this_thread::sleep_for(chrono::milliseconds(gTestConfiguration.playDealy));

            requireQualityCheck = gTestConfiguration.userChoice;
            if(requireQualityCheck)
            {
                bool response = validateTest("\nWas the OTF audio codec switching smooth (Y/N)?");
                UT_ASSERT_TRUE(response);
            }
        }
    } 
*/

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


/**
 * @brief Test audio track switching
 *
 * This test verifies the ability to switch between audio tracks
 *
 * **Test Group ID:** 04@n
 * **Test Case ID:** 003@n
 *
 * **Test Procedure:**
 * Refer to UT specification documentation [audioDecoder_High-Level_TestSpecification.md](../../docs/pages/audioDecoder_High-Level_TestSpecification.md)
 */

UT_ADD_TEST(VTS_L4_AVMODULE, AudioTrackSwitch)
{
    const char *testName = "AudioTrackSwitch";

    startTest(mModuleName, mTestGroup, testName);
}

/**
 * @brief Test video track switching
 *
 * This test verifies the ability to switch between video channels
 *
 * **Test Group ID:** 04@n
 * **Test Case ID:** 003@n
 *
 * **Test Procedure:**
 * Refer to UT specification documentation [audioDecoder_High-Level_TestSpecification.md](../../docs/pages/audioDecoder_High-Level_TestSpecification.md)
 */

UT_ADD_TEST(VTS_L4_AVMODULE, VideoTrackSwitch)
{
    const char *testName = "VideoTrackSwitch";

    startTest(mModuleName, mTestGroup, testName);
}

/** @} */ // End of AV_MODULE_HALTEST_L4
/** @} */ // End of AV_MODULE_HALTEST
/** @} */ // End of AV_MODULE
/** @} */ // End of HPK
