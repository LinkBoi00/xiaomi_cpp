// This file is autogenerated by hidl-gen. Do not edit manually.
// Source: android.hardware.audio@7.0
// Location: hardware/interfaces/audio/7.0/

#ifndef HIDL_GENERATED_ANDROID_HARDWARE_AUDIO_V7_0_EXPORTED_CONSTANTS_H_
#define HIDL_GENERATED_ANDROID_HARDWARE_AUDIO_V7_0_EXPORTED_CONSTANTS_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_SKIP_DEVIVE_DEFAULT = 0,
    AUDIO_SKIP_DEVIVE_PROXY = 1,
    AUDIO_SKIP_DEVIVE_REMOTE_SUBMIX = 2,
    AUDIO_SKIP_DEVIVE_A2DP = 3,
} audio_skip_device_t;

typedef enum {
    AUDIO_MICROPHONE_CHANNEL_MAPPING_UNUSED = 0u,
    AUDIO_MICROPHONE_CHANNEL_MAPPING_DIRECT = 1u,
    AUDIO_MICROPHONE_CHANNEL_MAPPING_PROCESSED = 2u,
} audio_microphone_channel_mapping_t;

typedef enum {
    AUDIO_MICROPHONE_LOCATION_UNKNOWN = 0u,
    AUDIO_MICROPHONE_LOCATION_MAINBODY = 1u,
    AUDIO_MICROPHONE_LOCATION_MAINBODY_MOVABLE = 2u,
    AUDIO_MICROPHONE_LOCATION_PERIPHERAL = 3u,
} audio_microphone_location_t;

typedef enum {
    AUDIO_MICROPHONE_DIRECTIONALITY_UNKNOWN = 0u,
    AUDIO_MICROPHONE_DIRECTIONALITY_OMNI = 1u,
    AUDIO_MICROPHONE_DIRECTIONALITY_BI_DIRECTIONAL = 2u,
    AUDIO_MICROPHONE_DIRECTIONALITY_CARDIOID = 3u,
    AUDIO_MICROPHONE_DIRECTIONALITY_HYPER_CARDIOID = 4u,
    AUDIO_MICROPHONE_DIRECTIONALITY_SUPER_CARDIOID = 5u,
} audio_microphone_directionality_t;

typedef enum {
    MIC_DIRECTION_UNSPECIFIED = 0,
    MIC_DIRECTION_FRONT = 1,
    MIC_DIRECTION_BACK = 2,
    MIC_DIRECTION_EXTERNAL = 3,
} audio_microphone_direction_t;

typedef enum {
    AUDIO_DUAL_MONO_MODE_OFF = 0,
    AUDIO_DUAL_MONO_MODE_LR = 1,
    AUDIO_DUAL_MONO_MODE_LL = 2,
    AUDIO_DUAL_MONO_MODE_RR = 3,
} audio_dual_mono_mode_t;

typedef enum {
    AUDIO_TIMESTRETCH_STRETCH_DEFAULT = 0,
    AUDIO_TIMESTRETCH_STRETCH_VOICE = 1,
} audio_timestretch_stretch_mode_t;

enum {
    HAL_AUDIO_TIMESTRETCH_FALLBACK_MUTE = 1,
    HAL_AUDIO_TIMESTRETCH_FALLBACK_FAIL = 2,
};

#ifdef __cplusplus
}
#endif

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_AUDIO_V7_0_EXPORTED_CONSTANTS_H_