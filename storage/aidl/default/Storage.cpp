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

#include "Storage.h"

#include <android-base/chrono_utils.h>
#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <fstab/fstab.h>
#include <fs_mgr.h>

using ::ndk::ScopedAStatus;
using ::aidl::android::hardware::health::storage::IStorage;
using ::aidl::android::hardware::health::storage::IGarbageCollectCallback;
using ::aidl::android::hardware::health::storage::Result;

namespace aidl {
namespace android {
namespace hardware {
namespace health {
namespace storage {

static std::string getGarbageCollectPath() {
    struct fstab* fstab = fs_mgr_read_fstab_default();
    if (!fstab) return "";

    for (int i = 0; i < fstab->num_entries; i++) {
        const struct fstab_rec* entry = &fstab->recs[i];
        if (entry->sysfs_path && strlen(entry->sysfs_path) > 0) {
            std::string path = entry->sysfs_path;
            path += "/manual_gc";
            fs_mgr_free_fstab(fstab);
            return path;
        }
    }
    fs_mgr_free_fstab(fstab);
    return "";
}

ScopedAStatus Storage::garbageCollect(int64_t timeoutSeconds,
                                      const std::shared_ptr<IGarbageCollectCallback>& callback) {
    Result result = Result::SUCCESS;
    std::string path = getGarbageCollectPath();

    if (path.empty()) {
        LOG(WARNING) << "Cannot find Dev GC path";
        result = Result::UNKNOWN_ERROR;
    } else {
        android::base::Timer timer;
        LOG(INFO) << "Start Dev GC on " << path;
        while (true) {
            std::string require;
            if (!android::base::ReadFileToString(path, &require)) {
                PLOG(WARNING) << "Reading manual_gc failed in " << path;
                result = Result::IO_ERROR;
                break;
            }
            require = android::base::Trim(require);
            if (require == "" || require == "off" || require == "disabled") {
                LOG(DEBUG) << "No more to do Dev GC";
                break;
            }
            LOG(DEBUG) << "Trigger Dev GC on " << path;
            if (!android::base::WriteStringToFile("1", path)) {
                PLOG(WARNING) << "Start Dev GC failed on " << path;
                result = Result::IO_ERROR;
                break;
            }
            if (timer.duration() >= std::chrono::seconds(timeoutSeconds)) {
                LOG(WARNING) << "Dev GC timeout";
                break;
            }
            sleep(2);
        }
        LOG(INFO) << "Stop Dev GC on " << path;
        if (!android::base::WriteStringToFile("0", path)) {
            PLOG(WARNING) << "Stop Dev GC failed on " << path;
            result = Result::IO_ERROR;
        }
    }

    if (callback != nullptr) {
        auto ret = callback->onFinish(result);
        if (!ret.isOk()) {
            LOG(WARNING) << "Cannot return result to callback: " << ret.getMessage();
        }
    }
    return ScopedAStatus::ok();
}

ScopedAStatus Storage::debug(int fd) {
    if (fd < 0) {
        return ScopedAStatus::ok();
    }

    std::stringstream output;
    std::string path = getGarbageCollectPath();
    if (path.empty()) {
        output << "Cannot find Dev GC path";
    } else {
        std::string require;
        if (android::base::ReadFileToString(path, &require)) {
            output << path << ":" << require << std::endl;
        }
        if (android::base::WriteStringToFile("0", path)) {
            output << "stop success" << std::endl;
        }
    }

    if (!android::base::WriteStringToFd(output.str(), fd)) {
        PLOG(WARNING) << "debug: cannot write to fd";
    }
    fsync(fd);
    return ScopedAStatus::ok();
}

}  // namespace storage
}  // namespace health
}  // namespace hardware
}  // namespace android
}  // namespace aidl
