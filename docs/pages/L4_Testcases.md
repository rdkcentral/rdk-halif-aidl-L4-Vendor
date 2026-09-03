# L4 Testcases

## Table of Contents

- [Overview](#overview)
- [AV Playback with Secure/non-secure](#10-av-playback-with-secure-and-non-secure)
  - [Normal playback](#11-normal-playback)
    - [Baseline AV Playback HD](#baseline-av-playback-hd)
    - [HDR Metadata Propagation](#hdr-metadata-propagation)
    - [Pixel Format Coverage](#pixel-format-coverage)
    - [Interlaced Signaling](#interlaced-signaling)
    - [Low Bitrate](#low-bitrate)
    - [Adaptive Bitrate (ABR) streaming](#adaptive-bitrate-abr-streaming)
  - [Pause / Resume, Trick Modes and Seek](#12-pause-resume-trick-modes-and-seek)
  - [AV Synchronization](#13-av-synchronization)
  - [Robustness & Error Handling](#14-robustness-and-error-handling)
  - [Performance & Longevity](#15-performance-and-longevity)
  - [Audio Codec Switching](#16-Audio-Codec-Switching)
  - [Video Codec Switching](#17-Video-Codec-Switching)
- [Audio only Playback with Secure/non-secure](#20-audio-only-playback-with-secure-and-non-secure)
  - [Normal playback](#21-normal-playback)
  - [Pause / Resume, Trick Modes and Seek](#22-pause-resume-trick-modes-and-seek)
  - [Robustness Error Handling Performance](#23-robustness-error-handling-and-performance)
- [Video only Playback with Secure/non-secure](#30-video-only-playback-with-secure-and-non-secure)
  - [Normal playback](#31-normal-playback)
  - [Pause / Resume, Trick Modes and Seek](#32-pause-resume-trick-modes-and-seek)
  - [Robustness & Error Handling](#33-robustness-and-error-handling)
- [Validation Methods](#validation-methods)

## Overview

This document defines a comprehensive media validation test plan for L4, designed to be executed in multiple phases.

The RDK Media Stack Architecture provides the foundation for these tests: [L4 VTS Architecture](https://github.com/rdkcentral/rdk-halif-aidl-L4-Vendor/wiki)

**Current Scope (Phase 1 & Phase 2):** Foundational test coverage for [video](https://github.com/rdkcentral/rdk-halif-aidl/blob/develop/videodecoder/current/com/rdk/hal/videodecoder/Codec.aidl) & [audio](https://github.com/rdkcentral/rdk-halif-aidl/blob/develop/audiodecoder/current/com/rdk/hal/audiodecoder/Codec.aidl) codecs and advanced features (HDR10, HLG, Dolby Vision).

**Future Phases:** Will expand to include:

- Video plane mapping with source types, plane geometry (X, Y, WIDTH, HEIGHT), z-order management, alpha blending, and multi-plane scenarios (main + PIP video swapping)
- Multi-input audio mixing (concurrent stream processing), volume control (setVolume, getVolume), mute state management, volume ramping (setVolumeRamp), Audio Passthrough, Audio output port enumeration and configuration.
- Native graphics window handle integration with EGL display for compositor testing

This document will be updated as new phases are initiated.

The [Validation Methods](#validation-methods) outlined are common to all test cases, ensuring consistent quality and performance verification standards across the test suite.

This test plan is structured around primary use cases to ensure comprehensive validation of the media pipeline:

- **Normal Playback:** Verifies that various audio and video streams are decoded and rendered correctly under standard conditions. This includes baseline codec validation([Acodec](https://github.com/rdkcentral/rdk-halif-aidl/blob/develop/audiodecoder/current/com/rdk/hal/audiodecoder/Codec.aidl), [VCodec](https://github.com/rdkcentral/rdk-halif-aidl/blob/develop/videodecoder/current/com/rdk/hal/videodecoder/Codec.aidl)) , HDR metadata propagation (HDR10, HLG), pixel format handling (YUV420p, YUV422p, YUV444p), interlaced signaling, low bitrate scenarios, and adaptive bitrate (ABR) streaming with dynamic resolution/bitrate switching.

- **Pause / Resume, Trick Modes & Seeking:** Ensures that user-initiated controls (fast-forward, rewind, pause, resume, and seeking) are handled smoothly and accurately without introducing visual artifacts or audio glitches.

- **AV Synchronization:** Validates audio-video synchronization accuracy and measures drift over extended playback periods to ensure lip-sync is maintained within acceptable tolerances.

- **Robustness, Error Handling, and Performance:** Tests decoder resilience when processing malformed or incomplete streams, and validates system stability during extended playback sessions.

## 1.0 AV Playback with Secure and Non-secure

### 1.1 Normal playback

#### Baseline AV Playback HD

**Purpose:**
Validate complete decoder lifecycle including open/start/decode/queue operations, callback handling, stop/flush/EOS sequences, and basic audio-video synchronization.

**List of Streams:**

| TC ID                 |   Codec   | Stream |  Wrapper Requirements Exercised |
| ----------------------| --------- |--------|---------------------------------|
| VTS_L4_H264_AC3_1001  | H264 AC3  | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_8M_ac3_48k_6ch_512k_a1875_v3600_60sec.mp4 | Decoder lifecycle, buffer flow, EOS, VideoSink AV Sync |
| VTS_L4_H265_EAC3_1001 | H265 EAC3 | vts_h265_1080i_sdr_60.0f_BT709_yuv420p_8M_eac3_48k_6ch_512k_a1875_v3599_60sec.mp4|Same as above + codec selection |
| VTS_L4_MPEG2_AAC_1001 | MPEG2 AAC | vts_mpeg2_1080p_sdr_60.0f_BT709_yuv420p_20M_aac_48k_6ch_256k_a2814_v3600_60sec.mp4| Legacy codec path, interop correctness |
| VTS_L4_VP9_AAC_1001   | VP9 AAC   | vts_vp9_1080p_sdr_60.0f_BT709_yuv420p_15M_aac_48k_2ch_512k_a2814_v3600_60sec.mp4 | Non-MPEG codec negotiation |
| VTS_L4_AV1_AAC_1001   | AV1 AAC   | vts_av1_1080p_sdr_30.0f_BT709_yuv420p_10M_aac_48k_2ch_256k_a2814_v1800_60sec.mp4 | New codec path, capability reporting |

---

#### HDR Metadata Propagation

**Purpose:**
Validate proper propagation of DynamicRange and ColorPrimaries metadata via FrameMetadata callbacks. This test focuses on metadata accuracy, not subjective picture quality.

**List of Streams:**

| TC ID                |  HDR Mode   |   Stream   |  Wrapper Requirements Exercised    |
| -------------------- | ------------|------------|------------------------------------|
| VTS_L4_HDR10_2001    | HDR10       | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_60M_eac3_48k_6ch_512k_a1875_v3600_60sec.mp4 | HDR enum mapping, BT.2020 metadata, UHD timing  |
| VTS_L4_HDR10P_2001   | HDR10+      | vts_h265_2160p_hdr10plus_60.0f_BT2020_yuv420p_60M_eac3_48k_6ch_512k_a1875_v3600_60sec.mp4| HDR10+ dynamic metadata propagation|
| VTS_L4_HLG_2001      | HLG         | vts_h265_2160p_hlg_60.0f_BT2020_yuv420p_60M_ac3_48k_6ch_512k_a1875_v3600_60sec.mp4 | Alternate HDR mode propagation |
| VTS_L4_SDR_2001      | SDR         | vts_vp9_2160p_sdr_60.0f_BT2020_yuv420p_20M_opus_48k_6ch_512k_a3001_v3600_61sec.webm | 4K UHD SDR decode with YUV420p, VP9 WebM container handling |
| VTS_L4_DVISION_2001  | Dolby Vision| vts_h265_2160p_dolby_vision_60.0f_BT2020_yuv420p_20M_eac3_48k_6ch_512k_a3001_v3600_61sec.mp4 | Dolby Vision metadata propagation, dual-layer enhancement handling, BT.2020 UHD timing |

---

#### Pixel Format Coverage

**Purpose:**
Validate correct handling of various pixel formats to detect issues in format negotiation, buffer layout, and sink compatibility.

**List of Streams:**

| TC ID              | Pixel Format | Stream | Wrapper Requirements Exercised |
| -------------------|--------------|--------|--------------------------------|
| VTS_L4_YUV422_3001 | YUV422       | vts_h265_2160p_hdr10_60.0f_BT2020_yuv422p_25M_a0_v3600_60sec.mp4 | Non-420 buffer handling, surface negotiation |
| VTS_L4_YUV444_3001 | YUV444       | vts_av1_1080p_sdr_60.0f_BT709_yuv444p_12M_aac_48k_2ch_256k_a2814_v3600_60sec.mp4 | Rare format path, buffer size/layout correctness |

---

#### Interlaced Signaling

**Purpose:**
Validate correct scan type signaling and decoder state stability when handling interlaced content. This test focuses on metadata accuracy, not deinterlacing quality.

**List of Streams:**

| TC ID              | Scan Type   | Stream | Wrapper Requirements Exercised |
| -------------------|------------ |--------|--------------------------------|
| VTS_L4_1080i_4001  |1080i MPEG-2 | vts_mpeg2_1080i_sdr_60.0f_BT709_yuv420p_20M_aac_48k_6ch_256k_a2814_v3600_60sec.mp4 | Interlaced metadata, timing |
| VTS_L4_1080i_4002  | 1080i H.264 | vts_h264_1080i_sdr_29.97f_BT709_yuv420p_5M_ac3_48k_6ch_256k_a1875_v1797_60sec.mp4 | Interlaced handling in modern codec |

---

#### Low Bitrate

**Purpose:**
Validate decoder behavior under constrained data rates, including buffer queueing, underflow tolerance, and proper EOS handling.

**List of Streams:**

| TC ID                 | Codec     | Stream | Wrapper Requirements Exercised   |
| ----------------------|-----------|--------| ---------------------------------|
| VTS_L4_H264_AAC_5001  | H.264 AAC | vts_h264_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_512k_a2814_v1440_60sec.mp4 | Timing, underflow resilience |
| VTS_L4_H265_AC3_5001  | H.265 AC3 | vts_h265_1080p_sdr_24.0f_BT709_yuv420p_2.5M_ac3_48k_2ch_256k_a1875_v1440_60sec.mp4 | Same, HEVC path |
| VTS_L4_MPEG2_AAC_5001 | MPEG-2 AAC| vts_mpeg2_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_128k_a2814_v1440_60sec.mp4 | Legacy low-bitrate behavior |
| VTS_L4_VP9_AAC_5001   | VP9 AAC   | vts_vp9_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_512k_a2814_v1440_60sec.mp4 | VP9 timing behavior |
| VTS_L4_AV1_AAC_5001   | AV1 AAC   | vts_av1_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_128k_a2814_v1440_60sec.mp4 | AV1 low bitrate |

---

#### Adaptive Bitrate (ABR) Streaming

**Purpose:**
Validate seamless bitrate and resolution switching(Dynamic Resolution Switching) during playback, simulating adaptive streaming scenarios where streams transition between different quality levels (e.g., 480p, 720p, 1080p,2k,4k). Test streams include both low-to-high bitrate transitions (simulating improving network conditions) and high-to-low bitrate transitions (simulating degrading network conditions) to ensure robust adaptation in varying network environments.

**List of Streams:**

| TC ID                 | Codec     | Stream | Wrapper Requirements Exercised   |
| ----------------------|-----------|--------| ---------------------------------|
| VTS_L4_H264_AAC_6001  | H.264 AAC | vts_h264_abr_multi_bitrate_sdr_BT709_yuv420p_aac_48k_2ch_256k_300sec.mp4 | Dynamic resolution/bitrate switching, buffer management |
| VTS_L4_H265_EAC3_6001 | H.265 EAC3| vts_h265_abr_multi_bitrate_hdr10_BT2020_yuv420p_eac3_48k_6ch_512k_300sec.mp4 | ABR with HDR metadata transitions |
| VTS_L4_VP9_OPUS_6001  | VP9 OPUS  | vts_vp9_abr_multi_bitrate_sdr_BT709_yuv420p_opus_48k_2ch_256k_300sec.webm | WebM container ABR handling |
| VTS_L4_H264_AAC_6002  | H.264 AAC | vts_h264_abr_multi_bitrate_2k_BT709_yuv420p_aac_48k_2ch_256k_300sec.mp4 | Dynamic resolution/bitrate switching till 2k |
| VTS_L4_H265_EAC3_6002 | H.265 EAC3| vts_h265_abr_multi_bitrate_2k_BT709_yuv420p_eac3_48k_6ch_512k_300sec.mp4 | ABR till 2k  |

---

### 1.2 Pause Resume Trick Modes and Seek

**Purpose:**
Validate pause, resume, fast-forward, rewind, and seek operations with minimal frame drops and accurate FrameMetadata reporting throughout all operations.

**List of Streams:**

| TC ID                 | Codec     | Stream | Wrapper Requirements Exercised |
| ----------------------|-----------|--------|--------------------------------|
| VTS_L4_H264_AAC_7001  | H.264 AAC | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_20M_aac_48k_6ch_512k_a28126_v36000_600sec.mp4| Decoder state management during pause/resume, seek accuracy |
| VTS_L4_H264_AC3_7001  | H.264 AC3 | vts_h264_1080i_sdr_29.97f_BT709_yuv420p_5M_ac3_48k_6ch_256k_a1875_v1797_60sec.mp4 | Interlaced handling with trick mode operations |
| VTS_L4_H265_EAC3_7001 | H.265 EAC3| vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_18M_eac3_48k_6ch_512k_a18750_v36000_600sec.mp4 | HDR metadata persistence during trick modes |
| VTS_L4_VP9_OPUS_7001  | VP9 OPUS  | vts_vp9_2160p_sdr_60.0f_BT2020_yuv420p_18M_opus_48k_8ch_256k_a30001_v36000_601sec.webm | WebM container seek and trick mode support |
| VTS_L4_AV1_AC3_7001   | AV1 AC3   | vts_av1_2160p_hlg_60.0f_BT2020_yuv420p_20M_ac3_48k_6ch_512k_a18750_v36000_600sec.mp4 | AV1 codec with HLG during trick modes |

---

### 1.3 AV Synchronization

**Purpose:**
Validate audio-video synchronization accuracy and measure drift over extended playback periods to ensure lip-sync is maintained within acceptable tolerances.

**List of Streams:**

| TC ID               | Codec       | Stream | Wrapper Requirements Exercised |
| --------------------|-------------|--------|--------------------------------|
| VTS_L4_AVSYNC_8001  | H.264 AC3   | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_ac3_48k_6ch_256k_a28126_v36000_600sec.mp4 | AV sync callback accuracy, timestamp propagation |
| VTS_L4_AVSYNC_8002  | H.265 EAC3  | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_25M_eac3_48k_6ch_512k_a18750_v36000_600sec.mp4 | 4K HDR sync accuracy |
| VTS_L4_AVSYNC_8003  | VP9 OPUS    | vts_vp9_1080p_sdr_30.0f_BT709_yuv420p_10M_opus_48k_2ch_256k_a30001_v18000_600sec.webm | WebM container sync handling |

### 1.4 Robustness and Error Handling

**Purpose:**
Validate the player's resilience when processing corrupted streams. This ensures that an error in one track (e.g., Video) does not crash the application or stop the other track (e.g., Audio) from playing. Verify that errors are properly reported without causing system crashes or hangs.

**List of Streams:**

**Video Stream Corruption (with Valid Audio):**

| TC ID | Error Condition | Stream | Expected Behavior |
| :--- | :--- | :--- | :--- |
| VTS_L4_H264_ERR_9001 | H264 Bitflip slice | vts_h264_1080p_slice_corruption_with_valid_aac_60sec.mp4 | Video artifacts visible; Audio plays uninterrupted. |
| VTS_L4_H264_ERR_9002 | H264 NAL header | vts_h264_1080p_nal_header_corruption_with_valid_aac_60sec.mp4 | Error callback; Video recovers; Audio continuous |
| VTS_L4_H265_ERR_9001 | HEVC Slice segment | vts_h265_1080p_slice_corruption_with_valid_ac3_60sec.mp4 | HEVC artifacts; Audio plays uninterrupted. |
| VTS_L4_VP9_ERR_9001  | VP9 Frame header | vts_vp9_1080p_header_corruption_with_valid_opus_60sec.webm | Video frames dropped; Audio stays in sync. |
| VTS_L4_AV1_ERR_9001  | AV1 OBU corruption | vts_av1_1080p_obu_corruption_with_valid_aac_60sec.mp4 | OBU error handled; Audio plays uninterrupted. |

**Audio Stream Corruption (with Valid Video):**

| TC ID | Error Condition | Stream | Expected Behavior |
| :--- | :--- | :--- | :--- |
| VTS_L4_AAC_ERR_9001 | AAC ADTS header | vts_h264_1080p_valid_video_with_aac_adts_corruption_60sec.mp4 | Audio glitches/silence; Video plays smooth. |
| VTS_L4_AC3_ERR_9001 | AC3 Sync frame | vts_h264_1080p_valid_video_with_ac3_sync_corruption_60sec.mp4 | Audio resyncs; Video plays smooth. |
| VTS_L4_EAC3_ERR_9002 | EAC3 Header | vts_h265_1080p_valid_video_with_eac3_header_corruption_60sec.mp4 | Audio error; Video plays smooth. |

---

### 1.5 Performance and Longevity

**Purpose:**
Validate decoder stability, memory management, and performance consistency(No A/V drift) during extended playback sessions. Ensure no resource leaks or performance degradation over time.

**List of Streams:**

| TC ID | Codec | Stream | Duration | Expected Result |
| :--- | :--- | :--- | :--- | :--- |
| VTS_L4_H264_AAC_1101 | H.264 AAC | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_aac_48k_2ch_256k_a168750_v648000_10800sec.mp4 | 3 hours | No A/V drift , No frame drops, no memory leaks |
| VTS_L4_H265_AC3_1102 | H.265 AC3 | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_25M_ac3_48k_6ch_512k_a112500_v648000_10800sec.mp4 | 3 hours | No A/V drift, No frame drops, stable HDR metadata |
| VTS_L4_VP9_OPUS_1103 | VP9 OPUS | vts_vp9_1080p_sdr_60.0f_BT709_yuv420p_15M_opus_48k_2ch_256k_a180000_v648000_10800sec.webm | 3 hours | No A/V drift, No frame drops, no resource leaks |
| VTS_L4_AV1_AAC_1104  | AV1 AAC  | vts_av1_2160p_sdr_30.0f_BT709_yuv420p_20M_aac_48k_2ch_256k_a168750_v648000_10800sec.mp4 | 3 hours | No A/V drift, No frame drops, consistent performance |

### 1.6 Audio Codec Switching

**Purpose:**
Validate the switching of multiple audio codecs on the fly, without pausing the stream. 

**List of Streams:**

| TC ID | Codec | Stream | Duration | Expected Result |
| :--- | :--- | :--- | :--- | :--- |
| VTS_L4_H265_EAC3_AAC_8001 | H265 EAC3 AAC | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_15M_eac3_aac_48k_6ch_256k_a9375_v18000_300sec.mp4 | 300 seconds | Seamless audio codec switch |
| VTS_L4_H265_AC3_8002 | H265 AC3 | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_15M_ac3_48k_2ch_256k_a5625_v10800_180sec.mp4 | 180 seconds | Seamless audio track switch |

### 1.7 Video Codec Switching

**Purpose:**
Validate the switching of multiple video codecs on the fly, without pausing the stream. 

**List of Streams:**

| TC ID | Codec | Stream | Duration | Expected Result |
| :--- | :--- | :--- | :--- | :--- |
| VTS_L4_H265_EAC3_AAC_8001 | H265 EAC3 AAC | vts_multi_codec_1080p_hdr10_60.0f_BT2020_yuv420p_48k_6ch_512k_a3750_v7200_120sec.mp4 | 120 seconds | Seamless video codec switch |



---

## 2.0 Audio only Playback with Secure and Non-secure

### 2.1 Normal Playback

**Purpose:**
Validate audio-only decode pipeline including buffer queueing, flushing, EOS handling, and decoder state transitions across various audio codecs and sample rates.

**List of Streams:**

| TC ID              | Audio Codec | Stream  | Wrapper Requirements Exercised |
| -------------------| ------------|---------|--------------------------------|
| VTS_L4_AAC_1201    |  AAC        | vts_aac_11.025k_2ch_64k_a647_v0_60sec.mp4 | Basic decoded audio path |
| VTS_L4_AC3_1201    | AC-3        | vts_ac3_32k_2ch_128k_a1250_v0_60sec.mp4 | Passthrough control plane |
| VTS_L4_EAC3_1201   | E-AC-3      | vts_eac3_32k_3ch_128k_a1250_v0_60sec.mp4 | Advanced passthrough |
| VTS_L4_EAC3_ATMOS_1201| EAC3 with Atmos| vts_eac3_atmos_32k_2ch_128k_a1250_v0_60sec.mp4 | Advanced passthrough |
| VTS_L4_PCM_1201    | PCM         | vts_pcm_44.1k_2ch_a2584_v0_60sec.mp4 | PCM queueing + sink behavior |
| VTS_L4_OPUS_1201   | Opus        | vts_opus_12k_2ch_128k_a3001_v0_61sec.webm | WebM audio handling |
| VTS_L4_VORBIS_1201 | Vorbis      | vts_vorbis_44.1k_2ch_128k_a2586_v0_61sec.webm | Legacy WebM audio |
| VTS_L4_FLAC_1201   | FLAC        | vts_flac_44.1k_2ch_a575_v0_60sec.mp4 | Lossless decode path |
| VTS_L4_MP3_1201    | MP3         | vts_mp3_16k_2ch_128k_a1669_v0_60sec.mp4 | Legacy audio decode |

---

### 2.2 Pause Resume Trick Modes and Seek

**Purpose:**
Validate audio-only pause, resume, seek, and trick mode operations ensuring smooth transitions without audio pops, clicks, or buffer underruns.

**List of Streams:**

| TC ID           | Codec | Stream | Wrapper Requirements Exercised |
| --------------- |-------|--------|--------------------------------|
| VTS_L4_AAC_1301 | AAC   | vts_aac_48k_6ch_512k_a28126_v36000_600sec.mp4| Audio decoder state management, seek accuracy, pause/resume without artifacts |
| VTS_L4_AC3_1301 | AC3   | vts_ac3_48k_6ch_512k_a28126_v36000_600sec.mp4| Passthrough mode trick operations, buffer continuity |
| VTS_L4_EAC3_1301| EAC3  | vts_eac3_48k_6ch_512k_a28126_v36000_600sec.mp4| Advanced audio codec seek/pause handling |

---

### 2.3 Robustness, Error Handling and Performance

**Purpose:**
Validate audio decoder resilience when processing corrupted or malformed audio streams. Ensure graceful error handling without crashes or audio pipeline stalls.

**List of Streams:**

**AAC Error Streams:**

| TC ID           | Error Condition            | Stream | Expected Behavior |
| --------------- | -------------------------- | -------| ----------------- |
| VTS_L4_AAC_1401 | ADTS header corruption     | vts_aac_48k_6ch_adts_header_corruption_512k_a2814_v0_61sec.mp4 | Error reported, attempts recovery at next valid frame |
| VTS_L4_AAC_1402 | Bitrate field corruption   | vts_aac_48k_6ch_bitrate_field_corruption_256k_a2814_v0_61sec.mp4 | Graceful handling, playback continues |
| VTS_L4_AAC_1403 | Channel mode corruption    | vts_aac_48k_6ch_channel_mode_corruption_256k_a2814_v0_59sec.mp4 | Channel config error handled, no crash |
| VTS_L4_AAC_1404 | Dynamic range corruption   | vts_aac_48k_6ch_dynamic_range_corruption_512k_a2814_v0_51sec.mp4 | Metadata error, audio playback continues |

**AC3 Error Streams:**

| TC ID           | Error Condition            | Stream | Expected Behavior |
| --------------- | -------------------------- | -------| ----------------- |
| VTS_L4_AC3_1401 | Sync frame corruption      | vts_ac3_48k_6ch_sync_frame_corruption_512k_a2814_v0_60sec.mp4 | Sync loss detected, resynchronization attempted |
| VTS_L4_AC3_1402 | BSI corruption             | vts_ac3_48k_6ch_bsi_corruption_512k_a2814_v0_60sec.mp4 | BSI error handled, decoder continues |

**E-AC3 Error Streams:**

| TC ID           | Error Condition            | Stream | Expected Behavior |
| --------------- | -------------------------- | -------| ----------------- |
| VTS_L4_EAC3_1401| Frame header corruption    | vts_eac3_48k_8ch_frame_header_corruption_768k_a3001_v0_60sec.mp4 | Header error reported, playback recovery |

---

## 3.0 Video only Playback with Secure and Non-secure

### 3.1 Normal Playback

**Purpose:**
Validate video-only decode pipeline across various codecs, resolutions, and HDR modes without audio track dependencies.

**List of Streams:**

| TC ID            | Video Codec | Stream  | Wrapper Requirements Exercised |
| -----------------| ------------|---------|--------------------------------|
| VTS_L4_H264_1501 | H.264       | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_a0_v3600_60sec.mp4| Video-only decoder pipeline, buffer flow without audio |
| VTS_L4_HEVC_1501 | H.265       | vts_h265_2160p_hdr10_24.0f_BT2020_yuv420p_25M_a0_v1440_60sec.mp4| 4K HDR video-only decode, metadata propagation |
| VTS_L4_HEVC_1502 | H.265       | vts_h265_1080p_sdr_60.0f_BT709_yuv420p_5M_a0_v3600_60sec.mp4| H.265 video-only low bitrate handling |
| VTS_L4_VP9_1501  | VP9         | vts_vp9_2160p_sdr_60.0f_BT709_yuv420p_15M_a0_v3600_60sec.webm | WebM video-only decode, 4K processing |
| VTS_L4_AV1_1501  | AV1         | vts_av1_2160p_hdr10_60.0f_BT2020_yuv420p_15M_a0_v3600_60sec.mp4| AV1 4K HDR video-only with YUV420p format |

---

### 3.2 Pause Resume Trick Modes and Seek

**Purpose:**
Validate video-only pause, resume, seek, and trick mode operations with accurate frame navigation and proper decoder state management without audio synchronization dependencies.

**List of Streams:**

| TC ID            | Video Codec | Stream  | Wrapper Requirements Exercised |
| -----------------| ------------|---------|--------------------------------|
| VTS_L4_H264_1601 | H.264       | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_a0_v3600_60sec.mp4| Video decoder seek accuracy, trick mode state management |
| VTS_L4_HEVC_1601 | H.265       | vts_h265_2160p_hdr10_24.0f_BT2020_yuv420p_25M_a0_v1440_60sec.mp4| 4K HDR video seek/pause handling, metadata persistence |
| VTS_L4_HEVC_1602 | H.265       | vts_h265_1080p_sdr_60.0f_BT709_yuv420p_5M_a0_v3600_60sec.mp4| H.265 trick mode operations, keyframe navigation |
| VTS_L4_VP9_1601  | VP9         | vts_vp9_2160p_sdr_60.0f_BT709_yuv420p_15M_a0_v3600_60sec.webm | VP9 video-only seek/pause in WebM container |
| VTS_L4_AV1_1601  | AV1         | vts_av1_2160p_hdr10_60.0f_BT2020_yuv420p_15M_a0_v3600_60sec.mp4| AV1 4K trick mode handling with HDR |

---

### 3.3 Robustness and Error Handling

**Purpose:**
Validate video decoder resilience when processing corrupted video-only streams. Ensure graceful error handling, proper error reporting, and recovery without system crashes or hangs.

**List of Streams:**

**H.264 Error Streams:**

| TC ID                 | Error Condition              | Stream | Expected Behavior |
| ----------------------| ---------------------------- | -------| ----------------- |
| VTS_L4_H264_ERR_1701  | NAL header corruption        | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_nal_header_corruption_10M_a0_v1278_60sec.mp4 | Error callback triggered, decoder recovers |
| VTS_L4_H264_ERR_1702  | Partial frame truncation     | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_partial_frame_truncation_10M_a0_v1362_57sec.mp4 | Graceful handling, continues decoding |
| VTS_L4_H264_ERR_1703  | PTS/DTS corruption           | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_pts_dts_corruption_10M_a0_v1437_60sec.mp4 | Timestamp error handling, playback continues |
| VTS_L4_H264_ERR_1704  | SPS/PPS corruption           | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_sps_pps_corruption_10M_a0_v1108_60sec.mp4 | Parameter set error, decoder attempts recovery |
| VTS_L4_H264_ERR_1705  | Bitflip slice corruption     | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_bitflip_slice_10M_a0_v1438_60sec.mp4 | Slice error detected, continues with next frame |

**H.265 Error Streams:**

| TC ID                 | Error Condition              | Stream | Expected Behavior |
| ----------------------| ---------------------------- | -------| ----------------- |
| VTS_L4_H265_ERR_1700  | HEVC NAL header corruption   | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_hevc_nal_header_corruption_25M_a0_v1042_59sec.mp4 | NAL error handling, decoder recovers |
| VTS_L4_H265_ERR_1702  | Partial frame truncation     | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_partial_frame_truncation_25M_a0_v1732_58sec.mp4 | Graceful handling, continues decoding |
| VTS_L4_H265_ERR_1703  | PTS/DTS corruption           | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_pts_dts_corruption_25M_a0_v1945_66sec.mp4 | Timestamp error handling, playback continues |
| VTS_L4_H265_ERR_1704  | Tile entry point corruption  | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_hevc_tile_entry_point_corruption_25M_a0_v1752_60sec.mp4 | Tile parsing error, decoder recovers |
| VTS_L4_H265_ERR_1705  | GOP truncation/duplication   | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_gop_truncation_duplication_25M_a0_v1936_65sec.mp4 | GOP structure error handled |

**MPEG-2 Error Streams:**

| TC ID                 | Error Condition              | Stream | Expected Behavior |
| ----------------------| ---------------------------- | -------| ----------------- |
| VTS_L4_MPEG2_ERR_1701 | Sequence header corruption   | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_mpeg2_sequence_header_corruption_15M_a0_v1799_60sec.mp4 | Header error handling, decoder recovers |
| VTS_L4_MPEG2_ERR_1702 | GOP header corruption        | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_mpeg2_gop_header_corruption_15M_a0_v1799_60sec.mp4 | GOP error detected, continues decoding |
| VTS_L4_MPEG2_ERR_1703 | Partial frame truncation     | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_partial_frame_truncation_15M_a0_v1696_60sec.mp4 | Graceful handling, playback continues |
| VTS_L4_MPEG2_ERR_1704 | PTS/DTS corruption           | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_pts_dts_corruption_15M_a0_v1798_60sec.ts | Timestamp error handling |

**VP9 Error Streams:**

| TC ID                 | Error Condition              | Stream | Expected Behavior |
| ----------------------| ---------------------------- | -------| ----------------- |
| VTS_L4_VP9_ERR_1701   | Superframe index corruption  | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_superframe_index_corruption_18M_a0_v1800_60sec.webm | Superframe error, decoder recovers |
| VTS_L4_VP9_ERR_1702   | Probability table desync     | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_prob_table_desync_18M_a0_v1800_60sec.webm | Probability context error handled |
| VTS_L4_VP9_ERR_1703   | Resolution change stress     | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_resolution_change_stress_18M_a0_v1800_60sec.webm | Dynamic resolution handling |
| VTS_L4_VP9_ERR_1704   | Loop filter corruption       | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_loop_filter_max_18M_a0_v1800_60sec.webm | Filter parameter error handled |

**AV1 Error Streams:**

| TC ID                 | Error Condition              | Stream | Expected Behavior |
| ----------------------| ---------------------------- | -------| ----------------- |
| VTS_L4_AV1_ERR_1701   | OBU header fuzz              | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_obu_header_fuzz_15M_a0_v1800_60sec.mp4 | OBU parsing error, decoder recovers |
| VTS_L4_AV1_ERR_1702   | Tile group size mismatch     | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_tile_group_size_mismatch_15M_a0_v1800_60sec.mp4 | Tile error handling |
| VTS_L4_AV1_ERR_1703   | CDF update corruption        | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_cdf_update_corruption_15M_a0_v1800_60sec.mp4 | Symbol decoder error handled |
| VTS_L4_AV1_ERR_1704   | Film grain param overflow    | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_film_grain_param_overflow_15M_a0_v1800_60sec.mp4 | Parameter validation, continues decoding |

---

## Validation Methods

The following validation methods are applied consistently across all test cases to ensure comprehensive quality and performance verification.

### 1. Metrics Validation

When playback metrics are available, verify key performance indicators against expected values defined in the stream configuration file:

- **Frame Counts:** Validate decoded, received, presented, repeated, error, underflow, and dropped frame counts.
- **Synchronization:** Measure A/V sync accuracy and No drift over time.
- **Performance:** Monitor CPU/memory usage, decoder throughput, and buffer utilization.

### 2. Event & Metadata Validation

Register listeners for critical playback events to validate stream integrity and proper callback handling:

**Events:**

- `onFirstFrameRendered` - Confirms first frame presentation
- `onDecodeError` - Reports decoding errors with error codes
- `onEndOfStream` - Signals proper EOS handling
- `onAudioUnderflow` / `onVideoUnderflow` - Detects buffer starvation
- `onAudioResumed` / `onVideoResumed` - Confirms recovery from underflow

**Metadata Validation:**

- Verify FrameMetadata fields (resolution, frame rate, color space, HDR mode, pixel format)
- Cross-check metadata values against stream configuration file
- Validate metadata consistency throughout playback

### 3. Playback Quality Validation

Validate subjective playback quality to ensure content is free from visual or audible artifacts:

- **Visual Quality:** No blocking, tearing, color banding, or synchronization issues
- **Audio Quality:** No pops, clicks, distortion, or sync drift
- **Automation:** Quality validation will be automated using **Raft** framework

### 4. Robustness Validation

For error handling tests:

- Verify no system crashes or hangs when processing corrupted streams
- Confirm error callbacks are triggered with appropriate error codes
- Validate graceful recovery and continuation of playback where possible

---

**Note:**
Input streams utilize various container formats (.mp4, .ts, .webm). Future test phases may include adaptive bitrate (ABR) formats such as HLS and MPEG-DASH.
