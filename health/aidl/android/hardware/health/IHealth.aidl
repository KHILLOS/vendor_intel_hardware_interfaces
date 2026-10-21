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

package android.hardware.health;

import android.hardware.health.HealthInfo;
import android.hardware.health.HealthConfig;

/**
 * Intel-specific Health AIDL interface.
 * Extends the standard android.hardware.health.IHealth with Intel-specific features.
 */
@VintfStability
interface IHealth {
    /**
     * Retrieves the health information.
     *
     * @return HealthInfo containing battery and storage health data.
     */
    HealthInfo getHealthInfo();

    /**
     * Retrieves the health configuration.
     *
     * @return HealthConfig containing health configuration data.
     */
    HealthConfig getHealthConfig();

    /**
     * Checks if screen should be kept on during charging.
     *
     * @return true if screen should be kept on, false otherwise.
     */
    boolean shouldKeepScreenOn();
}
