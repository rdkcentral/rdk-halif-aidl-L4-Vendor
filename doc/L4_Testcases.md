# L4 Testcases

This document outlines the test plan for a comprehensive media validation suite for L4, which will be executed in multiple phases.

Current Scope (Phase 1): Foundational test coverage for H.264 video and AAC audio.

Future Phases: Will expand to include HEVC, AV1, advanced features (HDR, Dolby Vision), and more.

The validation methods outlined are common for all test cases, ensuring a consistent standard of quality and performance verification. This document will be updated as new phases are initiated.

**Phase 1 Cover with H264 & AAC**:

## Normal playback

**Objective**: Validate video & audio playback across codecs

| **TC ID** | **Resolution**  | **Frame Rate** | **Bitrate (Mbps)** |  **Expected Result**|
| --------- | --------------- | -------------- | ------------------ |  ------------------ |
| VTS_L4_H264_AAC-101  | 1920x1080 (FHD) | 60p        | 12               | Smooth playback, no AV drift,No frame drops, correct metadata,Decoder should fail gracefully (if unsupported)|
| VTS_L4_H264_AAC-102  | 1920x1080 (FHD) | 30p        |  8               | Same as above       |
| VTS_L4_H264_AAC-103  | 1280x720 (HD)   | 25p        | 5                | Same as above       |
| VTS_L4_H264_AAC-104  | 640x360 (SD)    | 30p        | 2                | Same as above       |
| VTS_L4_H264_AAC-105  | 3840x2160 (4K)  | 60p        | 18               | Same as above       |
| VTS_L4_H264_AAC-106  | 7680x4320 (8K)  | 30p        | 25               | Same as above       |

## Frame Rate Coverage

**Objective**: Validate additional frame rates at 1080p to ensure comprehensive coverage.

| **TC ID** | **Resolution** | **Frame Rate** | **Expected Result**        |
| --------- | -------------- | -------------- | -------------------------- |
| VTS_L4_H264_AAC-201  | 1080p      | 23.976p        | Correct playback, no drift |
| VTS_L4_H264_AAC-202  | 1080p      | 24p            | Same as above              |
| VTS_L4_H264_AAC-203  | 1080p      | 25p            | Same as above              |
| VTS_L4_H264_AAC-204  | 1080p      | 29.97p         | Same as above              |
| VTS_L4_H264_AAC-205  | 1080p      | 30p            | Same as above              |
| VTS_L4_H264_AAC-206  | 1080p      | 50p            | Same as above              |
| VTS_L4_H264_AAC-207  | 1080p      | 59.94p         | Same as above              |
| VTS_L4_H264_AAC-208  | 1080p      | 60p            | Same as above              |

## GOP & Reference Frames

**Objective**: Validate decoding of different GOP structures and single frames.

| **TC ID** | **GOP Structure** | **Expected Result**             |
| --------- | ----------------- | ------------------------------- |
| VTS_L4_H264_AAC-301  | Single I-frame    | Immediate output                |
| VTS_L4_H264_AAC-302  | Closed GOP        | Stable decode                   |
| VTS_L4_H264_AAC-303  | Open GOP          | Correct playback, no frame loss |
| VTS_L4_H264_AAC-304  | Min GOP (I + 1 P) | Works without crash             |
| VTS_L4_H264_AAC-305  | Max GOP length    | Handles without drift           |

## Resolution & Aspect Ratio Switching

**Objective**:Validate seamless transitions between resolutions and aspect ratios.

| **TC ID** | **Test**                            | **Expected Result**             |
| --------- | ----------------------------------- | ------------------------------- |
| VTS_L4_H264_AAC-401  | Switch 720p → 1080p →4k  (channel ZAP) | Seamless, no corruption         |
| VTS_L4_H264_AAC-402  | Switch aspect ratio 16:9 → 4:3      | FrameMetadata updated correctly |

## Trick and seek Modes

**Objective**: Validate trick modes and seeking with minimal frame drops and accurate FrameMetadata.

| **TC ID** | **Action**                  | **Expected Result**                 |
| --------- | --------------------------- | ----------------------------------- |
| VTS_L4_H264_AAC-501  | Play 10-min @ 2x speed      | no AV drift      |
| VTS_L4_H264_AAC-502  | Play 10-min @ 8x speed      | No microblocks  |
| VTS_L4_H264_AAC-503  | Backward 2x                 | Smooth backward playback            |
| VTS_L4_H264_AAC-504  | Pause → Resume              | No sync jump                  |
| VTS_L4_H264_AAC-505  | Pause → Trick mode → Resume | Seamless, no corruption             |
| VTS_L4_H264_AAC-506  | Pause → seek → resume       | Seamless, no corruption             |

## Audio Sample Rate with Video

| **TC ID**              | **Video Format** | **Audio Codec** | **Audio Sample Rate (Hz)** | **Expected Result**                                                               |
| ---------------------- | ---------------- | --------------- | -------------------------- | --------------------------------------------------------------------------------- |
| VTS_L4_H264_AAC-601 | 1080p30 H.264    | AAC-LC          | 22.05k                     | Correct audio pitch and speed, A/V sync maintained throughout.                    |
| VTS_L4_H264_AAC-602 | 1080p30 H.264    | AAC-LC          | 44.1k                      | Same as above.                                                                    |
| VTS_L4_H264_AAC-603 | 1080p30 H.264    | AAC-LC          | 48k                        | Same as above.                                                                    |
| VTS_L4_H264_AAC-604 | 1080p30 H.264    | AAC-LC          | 96k                        | Same as above, provided the rate is supported by hardware.                        |
| VTS_L4_H264_AAC-605 | 1080p30 H.264    | AAC-LC          | 192k                       | Correct playback if platform supports high-res audio, otherwise graceful failure. |

## Audio Channel Configurations with Video

| **TC ID**              | **Video Format** | **Audio Codec** | **Audio Channels**          | **Expected Result**                                                         |
| ---------------------- | ---------------- | --------------- | --------------------------- | --------------------------------------------------------------------------- |
| VTS_L4_H264_AAC-701 | 1080p30 H.264    | AAC-LC          | Mono (1.0)                  | Correct audio output from center, A/V sync maintained.                      |
| VTS_L4_H264_AAC-702 | 1080p30 H.264    | AAC-LC          | Stereo (2.0)                | Correct L/R channel separation, A/V sync maintained.                        |
| VTS_L4_H264_AAC-703 | 1080p30 H.264    | AAC-LC          | 5.1 Surround                | Correct channel mapping verified with audio test file, A/V sync maintained. |
| VTS_L4_H264_AAC-704 | 1080p30 H.264    | AAC-LC          | 7.1 Surround                | Correct channel mapping for all 8 channels, A/V sync maintained.            |
| VTS_L4_H264_AAC-705 | 4K60 H.264       | AAC-LC          | 5.1 → 2.0 switch mid-stream | Seamless downmix, no drift, metadata updated correctly.                     |
| VTS_L4_H264_AAC-706 | 4K60 H.264       | AAC-LC          | 2.0 → 5.1 switch mid-stream | Seamless switch to multichannel, no corruption, sync maintained.            |

## Robustness & Error Handling (Todo)

**Objective**: Validate decoder behavior under error conditions.

| TC ID | Error Condition | Expected Result |
| :--- | :--- | :--- |
| VTS_L4_H264_ERR-801 | Stream with corrupted I-frame | Brief glitch/blockiness, then recovers at next I-frame. No crash. |
| VTS_L4_H264_ERR-802 | Stream with missing P/B-frames | Decoder should conceal the error or drop frames, but not crash. |
| VTS_L4_H264_ERR-803 | Truncated/incomplete stream | Playback stops cleanly at the end. No crash or freeze. |
| VTS_L4_H264_ERR-804 | Invalid Presentation Timestamps (PTS) | Graceful handling; may see A/V sync issues but should not crash. |
| VTS_L4_H264_AAC-805  | Seek near EOS (last GOP)    | Lands on last I-frame; clean EOS; no extra frames after EOS  |
| VTS_L4_H264_AAC-806  | Seek to middle GOP          |  Seamless, no corruption  |

## Performance & Longevity (Todo)

**Objective**: Validate long-duration playback and performance.

| TC ID | Test | Expected Result |
| :--- | :--- | :--- |
|VTS_L4_H264_AAC-901| 2-hour playback (1080p, 60p, 12 Mbps) | No drift, <0.1% frame drops, stable playback|

## Audio only Playback Test Cases

### Normal Playback

**Objective**: Validate audio playback across codecs, sample rates, and channel configurations.

| **TC ID** | **Codec**           | **Sample Rate (Hz)** |  **Channels** | **Expected Result**              |
| --------- | ------------------- | -------------------- | ------------ | -------------------------------- |
| VTS_L4_AAC-101   | AAC-LC              | 44.1k            | Stereo       | Smooth playback, no glitches     |
| VTS_L4_AAC-102   | AAC-LC              | 48k              | 5.1 Surround | Correct channel mapping|
| VTS_L4_AAC-103   | Dolby Digital       | 48k              | 5.1 Surround | Correct channel mapping|
| VTS_L4_AAC-104   | PCM                 | 48k              | Stereo       | Bit-perfect playback |
| VTS_L4_AAC-105   | PCM                 | 96k              | Stereo       | Correct rendering, no distortion |

### Sample Rate Coverage

**Objective**: Validate PCM playback across a wide range of sample rates.

| **TC ID** | **Codec** | **Sample Rate (Hz)** | **Expected Result**                           |
| --------- | --------- | -------------------- | --------------------------------------------- |
| VTS_L4_PCM-201   | PCM       | 8k                   | Plays without distortion                      |
| VTS_L4_PCM-202   | PCM       | 16k                  | Plays cleanly                                 |
| VTS_L4_PCM-203   | PCM       | 22.05k               | No glitches                                   |
| VTS_L4_PCM-204   | PCM       | 32k                  | No glitches                                   |
| VTS_L4_PCM-205   | PCM       | 44.1k                | Reference quality                             |
| VTS_L4_PCM-206   | PCM       | 48k                  | Standard playback                             |
| VTS_L4_PCM-207   | PCM       | 96k                  | High-res audio supported                      |
| VTS_L4_PCM-208   | PCM       | 192k                 | High-res audio supported (if platform allows) |

### Channel Configurations

**Objective**: Validate audio playback across different channel setups.

| **TC ID** | **Channels**             | **Expected Result**                     |
| --------- | ------------------------ | --------------------------------------- |
| VTS_L4_AAC-301   | Mono (1.0)               | Plays without error                     |
| VTS_L4_AAC-302   | Stereo (2.0)             | Left/Right balanced                     |
| VTS_L4_AAC-303   | 2.1 (Stereo + Subwoofer) | Subwoofer receives correct channel      |
| VTS_L4_AAC-304   | 5.1 Surround             | Proper mapping (FL, FR, C, LFE, RL, RR) |
| VTS_L4_AAC-305   | 7.1 Surround             | Proper mapping for 8-channel setup      |

### Pause / Resume & Trick Modes

**Objective**: Validate audio trick modes and pause/resume behavior.

| **TC ID** | **Action**                            | **Expected Result**                      |
| --------- | ------------------------------------- | ---------------------------------------- |
| VTS_L4_AAC-401   | Pause → Resume                        | No audible pops or glitches              |
| VTS_L4_AAC-402   | Play 10-min stream @ 2x               | Finishes in \~5 min, audio stays in sync |
| VTS_L4_AAC-403   | Play 10-min stream @ 8x               | Finishes in \~1.25 min, no crashes       |
| VTS_L4_AAC-404   | Play → Pause → Fast Forward → Resume  | Smooth transitions, no corruption        |
| VTS_L4_AAC-405   | Loop playback (repeat track 10 times) | No drift over long duration              |

### Robustness Error Handling Performance(Todo)

**Objective**: Validate edge cases for audio playback.

| **TC ID** | **Test**                                       | **Expected Result**               |
| --------- | ---------------------------------------------- | --------------------------------- |
| VTS_L4_AAC-501   | Silence-only audio (PCM 48k, 16-bit)           | Decoder outputs silence, no noise |
| VTS_L4_AAC-502   | Gapless playback (two AAC tracks back-to-back) | Seamless, no click/pop            |
| VTS_L4_AAC-503   | Low-bitrate MP3 (32 kbps)                      | Still decodes without error       |
| VTS_L4_AAC-504   | High-bitrate MP3 (320 kbps)                    | Proper playback                   |
| VTS_L4_AAC-506   | Long-duration test (2 hrs AAC)                 | No drift, stable playback         |

**Validations:**

1. If metrics are available, check the number of decoded, error, and dropped frames, and verify these against the stream configuration file.
2. register the onFrameOutput event and check the frame metadata like eos,discontinuity and verify with stream configuration file.
3. Validate with user choice is playback fine without artifacts, this will automated with raft.

**Note**:
Input streams can different container format (.mp4, .ts, .webm) and if required will add ABR formats like HLS and MPEG-DASH.
