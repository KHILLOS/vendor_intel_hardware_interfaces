/* SPDX-License-Identifier: Apache-2.0 */
/*
 * Copyright (C) 2024 The KhillOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "HealthImpl.h"

#include <android-base/logging.h>
#include <health/utils.h>
#include <health2impl/Health.h>
#include <libhealthd/battery_notifypkt.h>
#include <libhealthloop.intel/healthloop.h>
#include <libbatterymonitor/batterymonitor.h>

using ::ndk::ScopedAStatus;
using ::aidl::android::hardware::health::HealthInfo;
using ::aidl::android::hardware::health::HealthConfig;
using ::aidl::android::hardware::health::BatteryCapacityLevel;
using ::aidl::android::hardware::health::BatteryChargingPolicy;
using ::aidl::android::hardware::health::BatteryChargingState;
using ::aidl::android::hardware::health::BatteryHealth;
using ::aidl::android::hardware::health::BatteryHealthData;
using ::aidl::android::hardware::health::BatteryStatus;
using ::aidl::android::hardware::health::DiskStats;
using ::aidl::android::hardware::health::StorageInfo;
using ::aidl::android::hardware::health::HingeInfo;
using ::aidl::android::hardware::health::IHealth;

namespace aidl {
namespace android {
namespace hardware {
namespace health {

HealthImpl::HealthImpl() {
    config_ = std::make_unique<healthd_config>();
    InitHealthdConfig(config_.get());
    healthd_board_init(config_.get());
}

ScopedAStatus HealthImpl::getHealthInfo(HealthInfo* _aidl_return) {
    struct BatteryProperties props;
    healthd_board_battery_update(&props);
    
    // Convert from healthd BatteryProperties to AIDL HealthInfo
    _aidl_return->chargerAcOnline = props.chargerAcOnline;
    _aidl_return->chargerUsbOnline = props.chargerUsbOnline;
    _aidl_return->chargerWirelessOnline = props.chargerWirelessOnline;
    _aidl_return->chargerDockOnline = props.chargerDockOnline;
    _aidl_return->maxChargingCurrentMicroamps = props.maxChargingCurrentMicroamps;
    _aidl_return->maxChargingVoltageMicrovolts = props.maxChargingVoltageMicrovolts;
    
    _aidl_return->batteryStatus = static_cast<BatteryStatus>(props.batteryStatus);
    _aidl_return->batteryHealth = static_cast<BatteryHealth>(props.batteryHealth);
    _aidl_return->batteryPresent = props.batteryPresent;
    _aidl_return->batteryLevel = props.batteryLevel;
    _aidl_return->batteryVoltageMillivolts = props.batteryVoltageMillivolts;
    _aidl_return->batteryTemperatureTenthsCelsius = props.batteryTemperatureTenthsCelsius;
    _aidl_return->batteryCurrentMicroamps = props.batteryCurrentMicroamps;
    _aidl_return->batteryCycleCount = props.batteryCycleCount;
    _aidl_return->batteryFullChargeUah = props.batteryFullChargeUah;
    _aidl_return->batteryChargeCounterUah = props.batteryChargeCounterUah;
    _aidl_return->batteryTechnology = props.batteryTechnology;
    _aidl_return->batteryCurrentAverageMicroamps = props.batteryCurrentAverageMicroamps;
    
    // Intel-specific: battery mediation
    if (get_battery_mediation_present() == false) {
        _aidl_return->batteryCapacityLevel = BatteryCapacityLevel::UNKNOWN;
        _aidl_return->batteryChargeTimeToFullNowSeconds = 0;
        _aidl_return->batteryFullChargeDesignCapacityUah = 0;
    }
    
    _aidl_return->chargingState = BatteryChargingState::UNKNOWN;
    _aidl_return->chargingPolicy = BatteryChargingPolicy::UNKNOWN;
    
    // Storage info
    std::vector<StorageInfo> storageInfo;
    get_storage_info(storageInfo);
    _aidl_return->storageInfos = storageInfo;
    
    // Disk stats
    std::vector<DiskStats> diskStats;
    get_disk_stats(diskStats);
    _aidl_return->diskStats = diskStats;
    
    return ScopedAStatus::ok();
}

ScopedAStatus HealthImpl::getHealthConfig(HealthConfig* _aidl_return) {
    _aidl_return->batteryLowCapacityPercent = 15;  // Default 15%
    return ScopedAStatus::ok();
}

ScopedAStatus HealthImpl::shouldKeepScreenOn(bool* _aidl_return) {
    *_aidl_return = false;
    return ScopedAStatus::ok();
}

}  // namespace health
}  // namespace hardware
}  // namespace android
}  // namespace aidl
