# L4 Testcases

## Table of Contents

- [Overview](#overview)
- [AV Playback with Secure/non-secure](#10-av-playback-with-secure-and-non-secure)
  - [Normal playback](#11-normal-playback)
  - [Pause / Resume, Trick Modes and Seek](#12-pause-resume-trick-modes-and-seek)
  - [Robustness & Error Handling ](#13-robustness-and-error-handling)
  - [Performance & Longevity](#14-performance-and-longevity)
- [Audio only Playback with Secure/non-secure](#20-audio-only-playback-with-secure-and-non-secure)
  - [Normal playback](#21-normal-playback)
  - [Pause / Resume, Trick Modes and Seek](#22-pause-resume-trick-modes-and-seek)
  - [Robustness Error Handling Performance](#23-robustness-error-handling-and-performance)
- [Video only Playback with Secure/non-secure](#30-video-only-playback-with-secure-and-non-secure)
  - [Normal playback](#31-normal-playback)
  - [Pause / Resume, Trick Modes and Seek](#32-pause-resume-trick-modes-and-seek)
  - [Robustness & Error Handling ](#33-robustness-and-error-handling)
  - [Performance & Longevity](#34-performance-and-longevity)
- [Validations methods](#validations-methods)

## Overview

This document defines the test plan for a comprehensive media validation suite for L4, to be executed in multiple phases.

The RDK Media Stack Architecture provides the foundation for these tests [L4 VTS Arc](https://github.com/rdkcentral/rdk-halif-aidl-L4-Vendor/wiki)

Current Scope (Phase 1): Foundational test coverage for H.264 video and AAC audio.

Future Phases: Will expand to include HEVC,VP9, AV1, advanced features (HDR, Dolby Vision), and more.

The [Validation Methods](#validations-methods) outlined are common for all test cases, ensuring a consistent standard of quality and performance verification.
<br>This document will be updated as new phases are initiated.

This test plan is structured around three primary use cases to ensure comprehensive validation of the media pipeline:

- **Normal Playback:** Verifies that various audio and video streams can be decoded and rendered correctly under standard conditions. This includes a wide range of resolutions, frame rates, codecs, and stream structures.

- **Pause / Resume Trick Modes & Seeking:** Ensures that user-initiated actions like fast-forward, rewind, pause, resume, and seeking are handled smoothly, accurately, and without visual or audible corruption.

- **Robustness, Error Handling, and Performance:** Tests the resilience of the decoder when presented with malformed or incomplete streams and validates system stability during long-duration playback sessions.

*Note: Full test case list and streams used for tests will be available at [Testcases](https://github.com/rdkcentral/rdk-halif-aidl-unified-vts/tree/main/host/tests/TestCases).*

*Note: Stream-to-Stream ID mapping is available in [Stream Catalog](Stream_Catalog.md).*

## 1.0 AV Playback with Secure and Non-secure

### 1.1 Normal playback

**Objective:**
<br> Validate video and audio playback with varying parameters:

- **Video:** Resolution, Dynamic Range, Color Space, Bit Depth, Frame Rate, Pixel Format, Bitrate.
- **Audio:** Sample Rate, Number of Channels, Bitrate.
<br>EX:

| TC ID | Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| ---------------------- | --------- | -------- | --------------------------------- | --------------------------------- |
| VTS_L4_H264_AC3_1001 | H264 AC3 | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_8M_ac3_48k_6ch_512k_a1875_v3600_60sec.mp4 | VTS_SID_038 | Decoder lifecycle, buffer flow, EOS, VideoSink AV Sync |
| VTS_L4_H265_EAC3_1001 | H265 EAC3 | vts_h265_1080i_sdr_60.0f_BT709_yuv420p_8M_eac3_48k_6ch_512k_a1875_v3599_60sec.mp4 | VTS_SID_045 | Same as above + codec selection |
| VTS_L4_MPEG2_AAC_1001 | MPEG2 AAC | vts_mpeg2_1080p_sdr_60.0f_BT709_yuv420p_20M_aac_48k_6ch_256k_a2814_v3600_60sec.mp4 | VTS_SID_072 | Legacy codec path, interop correctness |
| VTS_L4_VP9_AAC_1001 | VP9 AAC | vts_vp9_1080p_sdr_60.0f_BT709_yuv420p_15M_aac_48k_2ch_512k_a2814_v3600_60sec.mp4 | VTS_SID_083 | Non-MPEG codec negotiation |
| VTS_L4_AV1_AAC_1001 | AV1 AAC | vts_av1_1080p_sdr_30.0f_BT709_yuv420p_10M_aac_48k_2ch_256k_a2814_v1800_60sec.mp4 | VTS_SID_016 | New codec path, capability reporting |

---

#### HDR Metadata Propagation

**Purpose:**
Validate proper propagation of DynamicRange and ColorPrimaries metadata via FrameMetadata callbacks. This test focuses on metadata accuracy, not subjective picture quality.

**List of Streams:**

| TC ID | HDR Mode | Stream | Stream ID | Wrapper Requirements Exercised |
| -------------------- | ------------ | ------------ | ------------------------------------ | ------------------------------------ |
| VTS_L4_HDR10_2001 | HDR10 | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_60M_eac3_48k_6ch_512k_a1875_v3600_60sec.mp4 | VTS_SID_060 | HDR enum mapping, BT.2020 metadata, UHD timing |
| VTS_L4_HDR10P_2001 | HDR10+ | vts_h265_2160p_hdr10plus_60.0f_BT2020_yuv420p_60M_eac3_48k_6ch_512k_a1875_v3600_60sec.mp4 | VTS_SID_062 | HDR10+ dynamic metadata propagation |
| VTS_L4_HLG_2001 | HLG | vts_h265_2160p_hlg_60.0f_BT2020_yuv420p_60M_ac3_48k_6ch_512k_a1875_v3600_60sec.mp4 | VTS_SID_063 | Alternate HDR mode propagation |
| VTS_L4_SDR_2001 | SDR | vts_vp9_2160p_sdr_60.0f_BT2020_yuv420p_20M_opus_48k_6ch_512k_a3001_v3600_61sec.webm | VTS_SID_086 | 4K UHD SDR decode with YUV420p, VP9 WebM container handling |
| VTS_L4_DVISION_2001 | Dolby Vision | vts_h265_2160p_dolby_vision_60.0f_BT2020_yuv420p_20M_eac3_48k_6ch_512k_a3001_v3600_61sec.mp4 | VTS_SID_055 | Dolby Vision metadata propagation, dual-layer enhancement handling, BT.2020 UHD timing |

---

#### Pixel Format Coverage

**Purpose:**
Validate correct handling of various pixel formats to detect issues in format negotiation, buffer layout, and sink compatibility.

**List of Streams:**

| TC ID | Pixel Format | Stream | Stream ID | Wrapper Requirements Exercised |
| ------------------- | -------------- | -------- | -------------------------------- | -------------------------------- |
| VTS_L4_YUV422_3001 | YUV422 | vts_h265_2160p_hdr10_60.0f_BT2020_yuv422p_25M_a0_v3600_60sec.mp4 | VTS_SID_061 | Non-420 buffer handling, surface negotiation |
| VTS_L4_YUV444_3001 | YUV444 | vts_av1_1080p_sdr_60.0f_BT709_yuv444p_12M_aac_48k_2ch_256k_a2814_v3600_60sec.mp4 | VTS_SID_017 | Rare format path, buffer size/layout correctness |

---

#### Interlaced Signaling

**Purpose:**
Validate correct scan type signaling and decoder state stability when handling interlaced content. This test focuses on metadata accuracy, not deinterlacing quality.

**List of Streams:**

| TC ID | Scan Type | Stream | Stream ID | Wrapper Requirements Exercised |
| ------------------- | ------------ | -------- | -------------------------------- | -------------------------------- |
| VTS_L4_1080i_4001 | 1080i MPEG-2 | vts_mpeg2_1080i_sdr_60.0f_BT709_yuv420p_20M_aac_48k_6ch_256k_a2814_v3600_60sec.mp4 | VTS_SID_071 | Interlaced metadata, timing |
| VTS_L4_1080i_4002 | 1080i H.264 | vts_h264_1080i_sdr_29.97f_BT709_yuv420p_5M_ac3_48k_6ch_256k_a1875_v1797_60sec.mp4 | VTS_SID_032 | Interlaced handling in modern codec |

---

#### Low Bitrate

**Purpose:**
Validate decoder behavior under constrained data rates, including buffer queueing, underflow tolerance, and proper EOS handling.

**List of Streams:**

| TC ID | Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| ---------------------- | ----------- | -------- | --------------------------------- | --------------------------------- |
| VTS_L4_H264_AAC_5001 | H.264 AAC | vts_h264_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_512k_a2814_v1440_60sec.mp4 | VTS_SID_042 | Timing, underflow resilience |
| VTS_L4_H265_AC3_5001 | H.265 AC3 | vts_h265_1080p_sdr_24.0f_BT709_yuv420p_2.5M_ac3_48k_2ch_256k_a1875_v1440_60sec.mp4 | VTS_SID_051 | Same, HEVC path |
| VTS_L4_MPEG2_AAC_5001 | MPEG-2 AAC | vts_mpeg2_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_128k_a2814_v1440_60sec.mp4 | VTS_SID_073 | Legacy low-bitrate behavior |
| VTS_L4_VP9_AAC_5001 | VP9 AAC | vts_vp9_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_512k_a2814_v1440_60sec.mp4 | VTS_SID_088 | VP9 timing behavior |
| VTS_L4_AV1_AAC_5001 | AV1 AAC | vts_av1_480p_sdr_24.0f_BT709_yuv420p_2.5M_aac_48k_2ch_128k_a2814_v1440_60sec.mp4 | VTS_SID_021 | AV1 low bitrate |

---

#### Adaptive Bitrate (ABR) Streaming

**Purpose:**
Validate seamless bitrate and resolution switching(Dynamic Resolution Switching) during playback, simulating adaptive streaming scenarios where streams transition between different quality levels (e.g., 480p, 720p, 1080p,2k,4k). Test streams include both low-to-high bitrate transitions (simulating improving network conditions) and high-to-low bitrate transitions (simulating degrading network conditions) to ensure robust adaptation in varying network environments.

**List of Streams:**

| TC ID | Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| ---------------------- | ----------- | -------- | --------------------------------- | --------------------------------- |
| VTS_L4_H264_AAC_6001 | H.264 AAC | vts_h264_abr_multi_bitrate_sdr_BT709_yuv420p_aac_48k_2ch_256k_300sec.mp4 | VTS_SID_044 | Dynamic resolution/bitrate switching, buffer management |
| VTS_L4_H265_EAC3_6001 | H.265 EAC3 | vts_h265_abr_multi_bitrate_hdr10_BT2020_yuv420p_eac3_48k_6ch_512k_300sec.mp4 | VTS_SID_065 | ABR with HDR metadata transitions |
| VTS_L4_VP9_OPUS_6001 | VP9 OPUS | vts_vp9_abr_multi_bitrate_sdr_BT709_yuv420p_opus_48k_2ch_256k_300sec.webm | VTS_SID_089 | WebM container ABR handling |
| VTS_L4_H264_AAC_6002 | H.264 AAC | vts_h264_abr_multi_bitrate_2k_BT709_yuv420p_aac_48k_2ch_256k_300sec.mp4 | VTS_SID_043 | Dynamic resolution/bitrate switching till 2k |
| VTS_L4_H265_EAC3_6002 | H.265 EAC3 | vts_h265_abr_multi_bitrate_2k_BT709_yuv420p_eac3_48k_6ch_512k_300sec.mp4 | VTS_SID_064 | ABR till 2k |

---

### 1.2 Pause Resume Trick Modes and Seek

**Objective**:<br> Validate trick modes and seeking with minimal frame drops and accurate FrameMetadata reporting.

| TC ID | Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| ---------------------- | ----------- | -------- | -------------------------------- | -------------------------------- |
| VTS_L4_H264_AAC_7001 | H.264 AAC | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_20M_aac_48k_6ch_512k_a28126_v36000_600sec.mp4 | VTS_SID_037 | Decoder state management during pause/resume, seek accuracy |
| VTS_L4_H264_AC3_7001 | H.264 AC3 | vts_h264_1080i_sdr_29.97f_BT709_yuv420p_5M_ac3_48k_6ch_256k_a1875_v1797_60sec.mp4 | VTS_SID_032 | Interlaced handling with trick mode operations |
| VTS_L4_H265_EAC3_7001 | H.265 EAC3 | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_18M_eac3_48k_6ch_512k_a18750_v36000_600sec.mp4 | VTS_SID_057 | HDR metadata persistence during trick modes |
| VTS_L4_VP9_OPUS_7001 | VP9 OPUS | vts_vp9_2160p_sdr_60.0f_BT2020_yuv420p_18M_opus_48k_8ch_256k_a30001_v36000_601sec.webm | VTS_SID_085 | WebM container seek and trick mode support |
| VTS_L4_AV1_AC3_7001 | AV1 AC3 | vts_av1_2160p_hlg_60.0f_BT2020_yuv420p_20M_ac3_48k_6ch_512k_a18750_v36000_600sec.mp4 | VTS_SID_019 | AV1 codec with HLG during trick modes |

---

### 1.3 Robustness and Error Handling

| TC ID | Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| -------------------- | ------------- | -------- | -------------------------------- | -------------------------------- |
| VTS_L4_AVSYNC_8001 | H.264 AC3 | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_ac3_48k_6ch_256k_a28126_v36000_600sec.mp4 | VTS_SID_036 | AV sync callback accuracy, timestamp propagation |
| VTS_L4_AVSYNC_8002 | H.265 EAC3 | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_25M_eac3_48k_6ch_512k_a18750_v36000_600sec.mp4 | VTS_SID_059 | 4K HDR sync accuracy |
| VTS_L4_AVSYNC_8003 | VP9 OPUS | vts_vp9_1080p_sdr_30.0f_BT709_yuv420p_10M_opus_48k_2ch_256k_a30001_v18000_600sec.webm | VTS_SID_082 | WebM container sync handling |

### 1.4 Robustness and Error Handling

**Purpose:**
Validate the player's resilience when processing corrupted streams. This ensures that an error in one track (e.g., Video) does not crash the application or stop the other track (e.g., Audio) from playing. Verify that errors are properly reported without causing system crashes or hangs.

**List of Streams:**

**Video Stream Corruption (with Valid Audio):**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| :--- | :--- | :--- | :--- | :--- |
| VTS_L4_H264_ERR_9001 | H264 Bitflip slice | vts_h264_1080p_slice_corruption_with_valid_aac_60sec.mp4 | VTS_SID_039 | Video artifacts visible; Audio plays uninterrupted. |
| VTS_L4_H264_ERR_9002 | H264 NAL header | vts_h264_1080p_nal_header_corruption_with_valid_aac_60sec.mp4 | VTS_SID_033 | Error callback; Video recovers; Audio continuous |
| VTS_L4_H265_ERR_9001 | HEVC Slice segment | vts_h265_1080p_slice_corruption_with_valid_ac3_60sec.mp4 | VTS_SID_053 | HEVC artifacts; Audio plays uninterrupted. |
| VTS_L4_VP9_ERR_9001 | VP9 Frame header | vts_vp9_1080p_header_corruption_with_valid_opus_60sec.webm | VTS_SID_077 | Video frames dropped; Audio stays in sync. |
| VTS_L4_AV1_ERR_9001 | AV1 OBU corruption | vts_av1_1080p_obu_corruption_with_valid_aac_60sec.mp4 | VTS_SID_015 | OBU error handled; Audio plays uninterrupted. |

**Audio Stream Corruption (with Valid Video):**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| :--- | :--- | :--- | :--- | :--- |
| VTS_L4_AAC_ERR_9001 | AAC ADTS header | vts_h264_1080p_valid_video_with_aac_adts_corruption_60sec.mp4 | VTS_SID_040 | Audio glitches/silence; Video plays smooth. |
| VTS_L4_AC3_ERR_9001 | AC3 Sync frame | vts_h264_1080p_valid_video_with_ac3_sync_corruption_60sec.mp4 | VTS_SID_041 | Audio resyncs; Video plays smooth. |
| VTS_L4_EAC3_ERR_9002 | EAC3 Header | vts_h265_1080p_valid_video_with_eac3_header_corruption_60sec.mp4 | VTS_SID_054 | Audio error; Video plays smooth. |

---

### 1.4 Performance and Longevity

**Objective**: <br> Validate long-duration playback and performance.

| TC ID | Codec | Stream | Stream ID | Duration | Expected Result |
| :--- | :--- | :--- | :--- | :--- | :--- |
| VTS_L4_H264_AAC_1101 | H.264 AAC | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_aac_48k_2ch_256k_a168750_v648000_10800sec.mp4 | VTS_SID_035 | 3 hours | No A/V drift , No frame drops, no memory leaks |
| VTS_L4_H265_AC3_1102 | H.265 AC3 | vts_h265_2160p_hdr10_60.0f_BT2020_yuv420p_25M_ac3_48k_6ch_512k_a112500_v648000_10800sec.mp4 | VTS_SID_058 | 3 hours | No A/V drift, No frame drops, stable HDR metadata |
| VTS_L4_VP9_OPUS_1103 | VP9 OPUS | vts_vp9_1080p_sdr_60.0f_BT709_yuv420p_15M_opus_48k_2ch_256k_a180000_v648000_10800sec.webm | VTS_SID_084 | 3 hours | No A/V drift, No frame drops, no resource leaks |
| VTS_L4_AV1_AAC_1104 | AV1 AAC | vts_av1_2160p_sdr_30.0f_BT709_yuv420p_20M_aac_48k_2ch_256k_a168750_v648000_10800sec.mp4 | VTS_SID_020 | 3 hours | No A/V drift, No frame drops, consistent performance |

---

## 2.0 Audio only Playback with Secure and Non-secure

### 2.1 Normal Playback

**Objective**: <br> Validate only audio playback across codecs, sample rates, and channel configurations.

| **TC ID**        | **Audio Codec**     | **Sample Rate (Hz)** |  **Channels** | **Expected Result**              |
| -----------------| ------------------- | -------------------- | ------------  | -------------------------------- |
| VTS_L4_AAC-101   | AAC-LC              | 44.1k                | Stereo        | Smooth playback, no glitches     |

| TC ID | Audio Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| ------------------- | ------------ | --------- | -------------------------------- | -------------------------------- |
| VTS_L4_AAC_1201 | AAC | vts_aac_11.025k_2ch_64k_a647_v0_60sec.mp4 | VTS_SID_001 | Basic decoded audio path |
| VTS_L4_AC3_1201 | AC-3 | vts_ac3_32k_2ch_128k_a1250_v0_60sec.mp4 | VTS_SID_007 | Passthrough control plane |
| VTS_L4_EAC3_1201 | E-AC-3 | vts_eac3_32k_3ch_128k_a1250_v0_60sec.mp4 | VTS_SID_022 | Advanced passthrough |
| VTS_L4_EAC3_ATMOS_1201 | EAC3 with Atmos | vts_eac3_atmos_32k_2ch_128k_a1250_v0_60sec.mp4 | VTS_SID_025 | Advanced passthrough |
| VTS_L4_PCM_1201 | PCM | vts_pcm_44.1k_2ch_a2584_v0_60sec.mp4 | VTS_SID_075 | PCM queueing + sink behavior |
| VTS_L4_OPUS_1201 | Opus | vts_opus_12k_2ch_128k_a3001_v0_61sec.webm | VTS_SID_074 | WebM audio handling |
| VTS_L4_VORBIS_1201 | Vorbis | vts_vorbis_44.1k_2ch_128k_a2586_v0_61sec.webm | VTS_SID_076 | Legacy WebM audio |
| VTS_L4_FLAC_1201 | FLAC | vts_flac_44.1k_2ch_a575_v0_60sec.mp4 | VTS_SID_026 | Lossless decode path |
| VTS_L4_MP3_1201 | MP3 | vts_mp3_16k_2ch_128k_a1669_v0_60sec.mp4 | VTS_SID_066 | Legacy audio decode |

---

### 2.2 Pause Resume Trick Modes and Seek

**Objective**:<br> Validate audio trick modes and pause/resume behavior.

**List of Streams:**

| TC ID | Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| --------------- | ------- | -------- | -------------------------------- | -------------------------------- |
| VTS_L4_AAC_1301 | AAC | vts_aac_48k_6ch_512k_a28126_v36000_600sec.mp4 | VTS_SID_002 | Audio decoder state management, seek accuracy, pause/resume without artifacts |
| VTS_L4_AC3_1301 | AC3 | vts_ac3_48k_6ch_512k_a28126_v36000_600sec.mp4 | VTS_SID_008 | Passthrough mode trick operations, buffer continuity |
| VTS_L4_EAC3_1301 | EAC3 | vts_eac3_48k_6ch_512k_a28126_v36000_600sec.mp4 | VTS_SID_023 | Advanced audio codec seek/pause handling |

---

### 2.3 Robustness, Error Handling and Performance

**Objective**: <br>Validate edge cases for audio playback.

**AAC Error Streams:**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| --------------- | -------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_AAC_1401 | ADTS header corruption | vts_aac_48k_6ch_adts_header_corruption_512k_a2814_v0_61sec.mp4 | VTS_SID_003 | Error reported, attempts recovery at next valid frame |
| VTS_L4_AAC_1402 | Bitrate field corruption | vts_aac_48k_6ch_bitrate_field_corruption_256k_a2814_v0_61sec.mp4 | VTS_SID_004 | Graceful handling, playback continues |
| VTS_L4_AAC_1403 | Channel mode corruption | vts_aac_48k_6ch_channel_mode_corruption_256k_a2814_v0_59sec.mp4 | VTS_SID_005 | Channel config error handled, no crash |
| VTS_L4_AAC_1404 | Dynamic range corruption | vts_aac_48k_6ch_dynamic_range_corruption_512k_a2814_v0_51sec.mp4 | VTS_SID_006 | Metadata error, audio playback continues |

**AC3 Error Streams:**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| --------------- | -------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_AC3_1401 | Sync frame corruption | vts_ac3_48k_6ch_sync_frame_corruption_512k_a2814_v0_60sec.mp4 | VTS_SID_010 | Sync loss detected, resynchronization attempted |
| VTS_L4_AC3_1402 | BSI corruption | vts_ac3_48k_6ch_bsi_corruption_512k_a2814_v0_60sec.mp4 | VTS_SID_009 | BSI error handled, decoder continues |

**E-AC3 Error Streams:**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| --------------- | -------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_EAC3_1401 | Frame header corruption | vts_eac3_48k_8ch_frame_header_corruption_768k_a3001_v0_60sec.mp4 | VTS_SID_024 | Header error reported, playback recovery |

---

## 3.0 Video only Playback with Secure and Non-secure

### 3.1 Normal Playback

**Objective**:
Validate video-only playback across codecs, resolutions, frame rates, and dynamic ranges (no audio pipeline active).

| TC ID | Video Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| ----------------- | ------------ | --------- | -------------------------------- | -------------------------------- |
| VTS_L4_H264_1501 | H.264 | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_a0_v3600_60sec.mp4 | VTS_SID_034 | Video-only decoder pipeline, buffer flow without audio |
| VTS_L4_HEVC_1501 | H.265 | vts_h265_2160p_hdr10_24.0f_BT2020_yuv420p_25M_a0_v1440_60sec.mp4 | VTS_SID_056 | 4K HDR video-only decode, metadata propagation |
| VTS_L4_HEVC_1502 | H.265 | vts_h265_1080p_sdr_60.0f_BT709_yuv420p_5M_a0_v3600_60sec.mp4 | VTS_SID_052 | H.265 video-only low bitrate handling |
| VTS_L4_VP9_1501 | VP9 | vts_vp9_2160p_sdr_60.0f_BT709_yuv420p_15M_a0_v3600_60sec.webm | VTS_SID_087 | WebM video-only decode, 4K processing |
| VTS_L4_AV1_1501 | AV1 | vts_av1_2160p_hdr10_60.0f_BT2020_yuv420p_15M_a0_v3600_60sec.mp4 | VTS_SID_018 | AV1 4K HDR video-only with YUV420p format |

---

### 3.2 Pause Resume Trick Modes and Seek

**Objective**:
Validate smooth handling of video-only trick modes without audio.

**List of Streams:**

| TC ID | Video Codec | Stream | Stream ID | Wrapper Requirements Exercised |
| ----------------- | ------------ | --------- | -------------------------------- | -------------------------------- |
| VTS_L4_H264_1601 | H.264 | vts_h264_1080p_sdr_60.0f_BT709_yuv420p_12M_a0_v3600_60sec.mp4 | VTS_SID_034 | Video decoder seek accuracy, trick mode state management |
| VTS_L4_HEVC_1601 | H.265 | vts_h265_2160p_hdr10_24.0f_BT2020_yuv420p_25M_a0_v1440_60sec.mp4 | VTS_SID_056 | 4K HDR video seek/pause handling, metadata persistence |
| VTS_L4_HEVC_1602 | H.265 | vts_h265_1080p_sdr_60.0f_BT709_yuv420p_5M_a0_v3600_60sec.mp4 | VTS_SID_052 | H.265 trick mode operations, keyframe navigation |
| VTS_L4_VP9_1601 | VP9 | vts_vp9_2160p_sdr_60.0f_BT709_yuv420p_15M_a0_v3600_60sec.webm | VTS_SID_087 | VP9 video-only seek/pause in WebM container |
| VTS_L4_AV1_1601 | AV1 | vts_av1_2160p_hdr10_60.0f_BT2020_yuv420p_15M_a0_v3600_60sec.mp4 | VTS_SID_018 | AV1 4K trick mode handling with HDR |

---

### 3.3 Robustness and Error Handling

**Objective**:
Validate resilience of video-only playback under error or malformed conditions.

| **TC ID**       | **Error Condition**             | **Expected Result** |
| ----------------| --------------------------------| ------------------- |
| VTS_L4_H264-VID301 | Missing audio track            | Video plays smoothly without crash |
| VTS_L4_H264-VID302 | Corrupt I-frame                | Brief artifact, recovers at next I-frame |
| VTS_L4_H264-VID303 | Truncated video stream         | Clean EOS, no freeze |
| VTS_L4_H264-VID304 | Corrupt video metadata (PTS)   | Graceful handling, no crash |

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| ---------------------- | ---------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_H264_ERR_1701 | NAL header corruption | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_nal_header_corruption_10M_a0_v1278_60sec.mp4 | VTS_SID_028 | Error callback triggered, decoder recovers |
| VTS_L4_H264_ERR_1702 | Partial frame truncation | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_partial_frame_truncation_10M_a0_v1362_57sec.mp4 | VTS_SID_029 | Graceful handling, continues decoding |
| VTS_L4_H264_ERR_1703 | PTS/DTS corruption | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_pts_dts_corruption_10M_a0_v1437_60sec.mp4 | VTS_SID_030 | Timestamp error handling, playback continues |
| VTS_L4_H264_ERR_1704 | SPS/PPS corruption | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_sps_pps_corruption_10M_a0_v1108_60sec.mp4 | VTS_SID_031 | Parameter set error, decoder attempts recovery |
| VTS_L4_H264_ERR_1705 | Bitflip slice corruption | vts_h264_1080i_sdr_24.0f_BT709_yuv420p_bitflip_slice_10M_a0_v1438_60sec.mp4 | VTS_SID_027 | Slice error detected, continues with next frame |

**H.265 Error Streams:**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| ---------------------- | ---------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_H265_ERR_1700 | HEVC NAL header corruption | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_hevc_nal_header_corruption_25M_a0_v1042_59sec.mp4 | VTS_SID_047 | NAL error handling, decoder recovers |
| VTS_L4_H265_ERR_1702 | Partial frame truncation | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_partial_frame_truncation_25M_a0_v1732_58sec.mp4 | VTS_SID_049 | Graceful handling, continues decoding |
| VTS_L4_H265_ERR_1703 | PTS/DTS corruption | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_pts_dts_corruption_25M_a0_v1945_66sec.mp4 | VTS_SID_050 | Timestamp error handling, playback continues |
| VTS_L4_H265_ERR_1704 | Tile entry point corruption | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_hevc_tile_entry_point_corruption_25M_a0_v1752_60sec.mp4 | VTS_SID_048 | Tile parsing error, decoder recovers |
| VTS_L4_H265_ERR_1705 | GOP truncation/duplication | vts_h265_1080p_hdr10_30.0f_BT2020_yuv422p_gop_truncation_duplication_25M_a0_v1936_65sec.mp4 | VTS_SID_046 | GOP structure error handled |

**MPEG-2 Error Streams:**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| ---------------------- | ---------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_MPEG2_ERR_1701 | Sequence header corruption | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_mpeg2_sequence_header_corruption_15M_a0_v1799_60sec.mp4 | VTS_SID_068 | Header error handling, decoder recovers |
| VTS_L4_MPEG2_ERR_1702 | GOP header corruption | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_mpeg2_gop_header_corruption_15M_a0_v1799_60sec.mp4 | VTS_SID_067 | GOP error detected, continues decoding |
| VTS_L4_MPEG2_ERR_1703 | Partial frame truncation | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_partial_frame_truncation_15M_a0_v1696_60sec.mp4 | VTS_SID_069 | Graceful handling, playback continues |
| VTS_L4_MPEG2_ERR_1704 | PTS/DTS corruption | vts_mpeg2_1080i_sdr_30.0f_BT709_yuv420p_pts_dts_corruption_15M_a0_v1798_60sec.ts | VTS_SID_070 | Timestamp error handling |

**VP9 Error Streams:**

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| ---------------------- | ---------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_VP9_ERR_1701 | Superframe index corruption | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_superframe_index_corruption_18M_a0_v1800_60sec.webm | VTS_SID_081 | Superframe error, decoder recovers |
| VTS_L4_VP9_ERR_1702 | Probability table desync | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_prob_table_desync_18M_a0_v1800_60sec.webm | VTS_SID_079 | Probability context error handled |
| VTS_L4_VP9_ERR_1703 | Resolution change stress | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_resolution_change_stress_18M_a0_v1800_60sec.webm | VTS_SID_080 | Dynamic resolution handling |
| VTS_L4_VP9_ERR_1704 | Loop filter corruption | vts_vp9_1080p_sdr_30.0f_BT2020_yuv420p_vp9_loop_filter_max_18M_a0_v1800_60sec.webm | VTS_SID_078 | Filter parameter error handled |

**Objective**:
Validate long-duration stability of video-only playback.

| TC ID | Error Condition | Stream | Stream ID | Expected Behavior |
| ---------------------- | ---------------------------- | ------- | ----------------- | ----------------- |
| VTS_L4_AV1_ERR_1701 | OBU header fuzz | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_obu_header_fuzz_15M_a0_v1800_60sec.mp4 | VTS_SID_013 | OBU parsing error, decoder recovers |
| VTS_L4_AV1_ERR_1702 | Tile group size mismatch | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_tile_group_size_mismatch_15M_a0_v1800_60sec.mp4 | VTS_SID_014 | Tile error handling |
| VTS_L4_AV1_ERR_1703 | CDF update corruption | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_cdf_update_corruption_15M_a0_v1800_60sec.mp4 | VTS_SID_011 | Symbol decoder error handled |
| VTS_L4_AV1_ERR_1704 | Film grain param overflow | vts_av1_1080p_hdr10_30.0f_BT2020_yuv420p_av1_film_grain_param_overflow_15M_a0_v1800_60sec.mp4 | VTS_SID_012 | Parameter validation, continues decoding |

---

## Validations methods

### 1. Metrics Validation

If playback metrics are available, Verify key performance indicators against the stream configuration file:

- **Frame Counts:** decoded, received, presented, repeated, error, underflow, and dropped.

- **Synchronization:** A/V sync accuracy and drift over time.

### 2. Event & Metadata Validation

Register listeners for key playback events to validate stream integrity

- `onFirstFrameRendered`, `onDecodeError`, `onEndOfStream`, `onAudioUnderflow`, `onVideoUnderflow`, `onAudioResumed` `onVideoResumed`,

Cross-check these with the expected values in the stream configuration file.

### 3. Playback Quality Validation

Validate subjective playback quality (e.g., free of visible or audible artifacts).
This step will be automated using **Raft**.

**Note**:
Input streams may use different container formats (.mp4, .ts, .webm). If required, adaptive bitrate (ABR) formats such as HLS and MPEG-DASH will also be tested.
required, adaptive bitrate (ABR) formats such as HLS and MPEG-DASH will also be tested.
