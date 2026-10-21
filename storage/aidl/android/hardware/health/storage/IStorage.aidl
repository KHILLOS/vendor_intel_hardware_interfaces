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

package android.hardware.health.storage;

import android.hardware.health.storage.IGarbageCollectCallback;
import android.hardware.health.Result;

/**
 * Intel-specific Storage AIDL interface for garbage collection.
 */
@VintfStability
interface IStorage {
    /**
     * Triggers garbage collection on the device.
     *
     * @param timeoutSeconds maximum time to spend on garbage collection.
     * @param callback callback to be invoked when garbage collection finishes.
     */
    void garbageCollect(in long timeoutSeconds, in IGarbageCollectCallback callback);

    /**
     * Dump debug information to the given file descriptor.
     *
     * @param handle file descriptor to write debug info to.
     */
    void debug(in android.os.FileDescriptor handle);
}

/**
 * Callback for garbage collection completion.
 */
@VintfStability
interface IGarbageCollectCallback {
    /**
     * Called when garbage collection finishes.
     *
     * @param result result of the garbage collection operation.
     */
    void onFinish(in Result result);
}

/**
 * Result codes for storage operations.
 */
@VintfStability
@Backing(type="int")
enum Result {
    SUCCESS = 0,
    UNKNOWN_ERROR = 1,
    IO_ERROR = 2,
}
