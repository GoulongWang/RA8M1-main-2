/*
 * Copyright (c) 2019-2020 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file syswdog_armv8-m_drv.h
 *
 * \brief Driver for Armv8-M System Watchdog
 *
 * This System Watchdog is based on the 64-bit Armv8-M System Counter,
 * generating the physical count for System Watchdog.
 *
 * Main features:
 *   - The Watchdog has a two cycle operation mode. After setting the Watchdog
 *     cycle and enabling the Watchdog, the countdown cycle starts. If the
 *     countdown timer reaches zero, an interrupt is generated on the first
 *     interrupt line, and the cycle is automatically restarted. If the cycle
 *     reaches zero for the second time, a second interrupt is generated.
 *
 *   - For the interrupt mapping, check the subsystem reference manual. The
 *     second interrupt may cause a system reset.
 *
 *   - Operation modes:
 *         Offset mode:
 *         The countdown value is written to the Offset register. The Watchdog
 *         countdown value starts from this.
 *         User has to write the delta value in clock cycles as offset with
 *         set offset function. The Watchdog countdown can be restarted by
 *         writing to the watchdog refresh register (with function
 *         syswdog_armv8_m_refresh_wdog).
 *
 *         Absolute compare value mode:
 *         In this mode, the user has to write the absolute 64 bit counter value
 *         to the compare value registers. When the Watchdog's input timer value
 *         reaches this value, the Watchdog will generate the interrupt.
 *         The driver mainly supports offset mode, but writing the compare value
 *         is an alternative way to control the Watchdog.
 */

#ifndef __SYSWDOG_ARMV8_M_DRV_H__
#define __SYSWDOG_ARMV8_M_DRV_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  \brief Armv8-M System Watchdog device configuration structure
 */
struct syswdog_armv8_m_dev_cfg_t {
    const uint32_t base;
        /*!< Armv8-M System Watchdog device base address */
};

struct syswdog_armv8_m_dev_t {
    const struct syswdog_armv8_m_dev_cfg_t* const cfg;
        /*!< Armv8-M System Watchdog configuration structure */
};

/**
 * \brief Enables Watchdog
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \note This function doesn't check if dev is NULL.
 */
void syswdog_armv8_m_enable_wdog(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Disables Watchdog
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \note This function doesn't check if dev is NULL.
 */
void syswdog_armv8_m_disable_wdog(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Returns the enable status of the Watchdog.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return true if enabled, false otherwise
 *
 * \note This function doesn't check if dev is NULL.
 */
bool syswdog_armv8_m_is_wdog_enabled(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Returns the assert status of the Watchdog Signal interrupt 0.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return true if asserted, false otherwise
 *
 * \note This function doesn't check if dev is NULL.
 */
bool syswdog_armv8_m_read_irq_status_0(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Returns the assert status of the Watchdog Signal interrupt 1.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return true if asserted, false otherwise
 *
 * \note This function doesn't check if dev is NULL.
 */
bool syswdog_armv8_m_read_irq_status_1(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Writes the given value to the Watchdog Offset register
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 * \param[in] value The value to write to the register
 *
 * \note This function doesn't check if dev is NULL.
 * \note Writing the offset causes watchdog refresh.
 */
void syswdog_armv8_m_set_offset(struct syswdog_armv8_m_dev_t* dev,
                                const uint32_t value);

/**
 * \brief Reads the Watchdog Offset register
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Offset register value
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_offset(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Writes the given value to the Watchdog Compare Value register
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \note This function doesn't check if dev is NULL.
 */
void syswdog_armv8_m_set_compare_value(struct syswdog_armv8_m_dev_t* dev,
                                       const uint64_t value);

/**
 * \brief Reads the Watchdog Compare Value register
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Compare Value register value
 *
 * \note This function doesn't check if dev is NULL.
 */
uint64_t syswdog_armv8_m_get_compare_value(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Restarts the Watchdog period. Only used for offset mode.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \note This function doesn't check if dev is NULL.
 */
void syswdog_armv8_m_refresh_wdog(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Control frame Product identifier.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Control frame Product identifier value
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_cntr_product_id(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Control frame Architecture version.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Control frame Architecture version value
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_cntr_architecture_version(
                                struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Control frame Component Revision number.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Control frame Component Revision number
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_cntr_revision_number(
                                struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Control frame Arm JEP106 code.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Control frame Arm JEP106 code
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_cntr_arm_JEP106_code(
                                struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Refresh frame Product identifier.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Refresh frame Product identifier value
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_refr_product_id(struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Refresh frame Architecture version.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Refresh frame Architecture version value
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_refr_architecture_version(
                                struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Refresh frame Component Revision number.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Refresh frame Component Revision number
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_refr_revision_number(
                                struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Reads the Watchdog Refresh frame Arm JEP106 code.
 *
 * \param[in] dev Watchdog device struct \ref syswdog_armv8_m_dev_t
 *
 * \return The Watchdog Refresh frame Arm JEP106 code
 *
 * \note This function doesn't check if dev is NULL.
 */
uint32_t syswdog_armv8_m_get_refr_arm_JEP106_code(
                                struct syswdog_armv8_m_dev_t* dev);

/**
 * \brief Initializes and enables System Watchdog
 *
 *        Init also sets the Watchdog timeout to the given value
 *
 * \param[in] dev    Watchdog device struct \ref syswdog_armv8_m_dev_t
 * \param[in] offset Initial offset value in Watchdog clock cycles
 *
 * \note This function doesn't check if dev is NULL.
 */
void syswdog_armv8_m_init(struct syswdog_armv8_m_dev_t* dev, uint32_t offset);

#ifdef __cplusplus
}
#endif
#endif /* __SYSWDOG_ARMV8_M_DRV_H__ */
