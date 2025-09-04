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

## 1.0 AV Playback with Secure and Non-secure

### 1.1 Normal playback

**Objective:**
<br> Validate video and audio playback with varying parameters:

- **Video:** Resolution, Dynamic Range, Color Space, Bit Depth, Frame Rate, Pixel Format, Bitrate.
- **Audio:** Sample Rate, Number of Channels, Bitrate.
<br>EX:

| **TC ID**        | **Video Codec**|**Audio Codec**| **Resolution**  | **Dynamic Range**| **ColorSpace**|**Depth**|**Frame Rate** |**Pixel Format**| **Bitrate (Mbps)** |  **Expected Result**|
| -----------------| ---------------|---------------|---------------- | ---------------- | --------------| --------| --------------| ----------------|------------------- |  ------------------ |
| VTS_L4_H264_AAC1001  |H264            |AAC            |1920x1080 (FHD)  | SDR              | BT.709        |8bit    |60p            | YUV420          |12                  | Smooth playback, No AV drift, no frame drops, correct metadata,If unsupported, the decoder should fail gracefully without crashes.|
| VTS_L4_H264_AC32002  |H264            |DOLBY_AC3    |1920x1080 (FHD)  | SDR              | BT.709        |10bit    |60p            | YUV420          |12                  | Smooth playback, No AV drift, no frame drops, correct metadata,If unsupported, the decoder should fail gracefully without crashes.|
| VTS_L4_H265_AC31001  |H265            |DOLBY_AC3    |3840x2160      | HDR              | BT.2020        |10bit    |60p            | YUV420          |18                  | Smooth playback, No AV drift, no frame drops, correct metadata,If unsupported, the decoder should fail gracefully without crashes.|

*Note: The full list of test case IDs will be available in the test configuration.*

| #|Test Dimension | Comments |
|--|---------------|----------|
| 1 | Video Codec | Test every codec which the platform capabilities report. <br> Test unsupported codecs to check for errors. <br > [list of Video Codec](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.7.1/videodecoder/current/com/rdk/hal/videodecoder/Codec.aidl)|
| 2 | Audio Codec | Test every codec which the platform capabilities report. <br> Test unsupported codecs to check for errors. <br > [list of Audio Codec](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.7.1/audiodecoder/current/com/rdk/hal/audiodecoder/Codec.aidl)|
| 3 | Resolution | Test all Viper resolutions: <br> - 3840x2160 <br> - 2560x1440 <br> - 1920x1080 <br> - 1280x720 <br> - 960x540 <br> - 640x360 <br> - 320x240 <br> YouTube portrait mode videos from phones. <br> YouTube adds: <br> - 426x240 <br> - 7680x4320 (8K) |
| 4 | Dynamic range | - SDR <br> - HLG <br> - HDR10 <br> - HDR10+ <br> - Dolby Vision <br>[list of Dynamic range](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.7.1/videodecoder/current/com/rdk/hal/videodecoder/DynamicRange.aidl)|
| 5 | Color space | - BT.601 <br> - BT.709 <br> - BT.2020 |
| 6 | Frame rate | Frame Rates: 23.976, 24, 25, 29.97, 30, 50, 59.94, 60 |
| 7 | Frame types | Progressive and interlaced frame types. |
| 8 | Pixel format | - YCbCr4:4:4 <br> - YCbCr4:2:2 <br> - YCbCr4:2:0 <br>[list of Pixel format](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.7.1/videodecoder/current/com/rdk/hal/videodecoder/PixelFormat.aidl) |
| 9 | Bit rates | Viper bitrates (UK): 18000, 12200, 5800, 4250, 1850, 1200, 500 <br> Check the maximum bit rates supported by codec/profile/level |
| 10 | Reference frames | Single reference frames are passed for decode and are expected to be output immediately. |
| 11 | GOP structure | Open GOPs <br> Closed GOPs <br> Min and max M and N for each supported codec/profile/level |
| 12 | Active Format Description | Test all AFD codes appear in FrameMetadata. <br> Complete list of AFD codes |
| 13 | Resolution switching | Moving between different resolution encodings, without breaking GOP sequences. |
| 14 | Aspect ratio switching | Picture aspect ratios must be reported correctly in FrameMetadata. <br> Pixel aspect ratios must be reported correctly in FrameMetadata. |

### 1.2 Pause Resume Trick Modes and Seek

**Objective**:<br> Validate trick modes and seeking with minimal frame drops and accurate FrameMetadata reporting.

| **TC ID** | **Action**                  | **Expected Result**                 |
| --------- | --------------------------- | ----------------------------------- |
| VTS_L4_H264_AAC-201  | Play 10-min @ 2x speed      | no AV drift      |
| VTS_L4_H264_AAC-202  | Play 10-min @ 8x speed      | No microblocks  |
| VTS_L4_H264_AAC-203  | Backward 2x                 | Smooth backward playback            |
| VTS_L4_H264_AAC-204  | Pause → Resume              | No sync jump                  |
| VTS_L4_H264_AAC-205  | Pause → Trick mode → Resume | Seamless, no corruption             |
| VTS_L4_H264_AAC-206  | Pause → seek → resume       | Seamless, no corruption             |

### 1.3 Robustness and Error Handling

**Objective**: <br> Validate decoder behavior under error conditions.

| TC ID | Error Condition | Expected Result |
| :--- | :--- | :--- |
| VTS_L4_H264_ERR-301 | Stream with corrupted I-frame | Brief glitch/blockiness, then recovers at next I-frame. No crash. |
| VTS_L4_H264_ERR-302 | Stream with missing P/B-frames | Decoder should conceal the error or drop frames, but not crash. |
| VTS_L4_H264_ERR-303 | Truncated/incomplete stream | Playback stops cleanly at the end. No crash or freeze. |
| VTS_L4_H264_ERR-304 | Invalid Presentation Timestamps (PTS) | Graceful handling; may see A/V sync issues but should not crash. |
| VTS_L4_H264_AAC-305  | Seek near EOS (last GOP)    | Lands on last I-frame; clean EOS; no extra frames after EOS  |
| VTS_L4_H264_AAC-306  | Loss of frames (incomplete GOPs) |  No crash  |
| VTS_L4_H264_AAC-307  | Seek to middle GOP          |  Seamless, no corruption  |
| VTS_L4_H264_AAC-308  |Corrupt metadata encoding    |   No crash  |

### 1.4 Performance and Longevity

**Objective**: <br> Validate long-duration playback and performance.

| TC ID | Test | Expected Result |
| :--- | :--- | :--- |
|VTS_L4_H264_AAC-401| 2-hour playback (1080p, 60p, 12 Mbps) | No drift, <0.1% frame drops, stable playback|
|VTS_L4_H265_AC3-402| 2-hour playback (4k, 60p,HDR, 25 Mbps) | No drift, <0.1% frame drops, stable playback|

## 2.0 Audio only Playback with Secure and Non-secure

### 2.1 Normal Playback

**Objective**: <br> Validate only audio playback across codecs, sample rates, and channel configurations.

| **TC ID**        | **Audio Codec**     | **Sample Rate (Hz)** |  **Channels** | **Expected Result**              |
| -----------------| ------------------- | -------------------- | ------------  | -------------------------------- |
| VTS_L4_AAC-101   | AAC-LC              | 44.1k                | Stereo        | Smooth playback, no glitches     |

| #|Test Dimension | Comments |
|--|---------------|----------|
| 1 | Audio Codec | Test every codec which the platform capabilities report. <br> Test unsupported codecs to check for errors. <br > [list of Audio Codec](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.7.1/audiodecoder/current/com/rdk/hal/audiodecoder/Codec.aidl)|
| 2 | ChannelType | Test every ChannelType which the platform capabilities report. <br> [list of Channel Type](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.7.1/audiodecoder/current/com/rdk/hal/audiodecoder/ChannelType.aidl)|
| 3 | PCMFormat | Test every PCMFormat which the platform capabilities report. <br> [list of PCMFormat](https://github.com/rdkcentral/rdk-halif-aidl/blob/0.7.1/audiodecoder/current/com/rdk/hal/audiodecoder/PCMFormat.aidl)|
| 4 | No of Channels | Test supported Channels  <br> Mono (1.0) <br>Stereo (2.0) <br> 2.1(Stereo + Subwoofer) <br>5.1 Surround <br> 7.1 Surround|

### 2.2 Pause Resume Trick Modes and Seek

**Objective**:<br> Validate audio trick modes and pause/resume behavior.

| **TC ID** | **Action**                            | **Expected Result**                      |
| --------- | ------------------------------------- | ---------------------------------------- |
| VTS_L4_AAC-401   | Pause → Resume                        | No audible pops or glitches              |
| VTS_L4_AAC-402   | Play 10-min stream @ 2x               | Finishes in \~5 min, audio stays in sync |
| VTS_L4_AAC-403   | Play 10-min stream @ 8x               | Finishes in \~1.25 min, no crashes       |
| VTS_L4_AAC-404   | Play → Pause → Fast Forward → Resume  | Smooth transitions, no corruption        |
| VTS_L4_AAC-405   | Loop playback (repeat track 10 times) | No drift over long duration              |

### 2.3 Robustness, Error Handling and Performance

**Objective**: <br>Validate edge cases for audio playback.

| **TC ID** | **Test**                                       | **Expected Result**               |
| --------- | ---------------------------------------------- | --------------------------------- |
| VTS_L4_AAC-501   | Silence-only audio (PCM 48k, 16-bit)           | Decoder outputs silence, no noise |
| VTS_L4_AAC-502   | Gapless playback (two AAC tracks back-to-back) | Seamless, no click/pop            |
| VTS_L4_AAC-503   | Low-bitrate MP3 (32 kbps)                      | Still decodes without error       |
| VTS_L4_AAC-504   | High-bitrate MP3 (320 kbps)                    | Proper playback                   |
| VTS_L4_AAC-506   | Long-duration test (2 hrs AAC)                 | No drift, stable playback         |

## Validations methods

### 1. Metrics Validation

If playback metrics are available, Verify key performance indicators against the stream configuration file:

- **Frame Counts:** decoded, received, presented, repeated, error, underflow, and dropped.

- **Synchronization:** A/V sync accuracy and drift over time.

### 2. Event & Metadata Validation

Register listeners for key playback events to validate stream integrity

- `onFirstFrameRendered:` Confirm the first frame is rendered correctly to measure startup time.

- `onFrameOutput:` Continuously check frame metadata for flags like eos (End of Stream) and discontinuity at expected positions.

Cross-check these with the expected values in the stream configuration file.

### 3. Playback Quality Validation

Validate subjective playback quality (e.g., free of visible or audible artifacts).
This step will be automated using **Raft**.


**Note**:
Input streams may use different container formats (.mp4, .ts, .webm). If required, adaptive bitrate (ABR) formats such as HLS and MPEG-DASH will also be tested.
