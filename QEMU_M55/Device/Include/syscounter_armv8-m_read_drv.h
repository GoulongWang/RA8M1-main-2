/*
 * Copyright (c) 2019-2021 Arm Limited
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
 * \file syscounter_armv8-m_read_drv.h
 *
 * \brief Driver for Armv8-M System Counter Read, covering CNTReadBase Frame
 *        Features of driver:
 *          1. Read counter value
 */

#ifndef __SYSCOUNTER_ARMV8_M_READ_DRV_H__
#define __SYSCOUNTER_ARMV8_M_READ_DRV_H__

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SYSCOUNTER_ARMV8_M_READ_REGISTER_BIT_WIDTH          32u
    /*!< Armv8-M System Counter Read registers bit width */

/**
 *  \brief Armv8-M System Counter device configuration structure
 */
struct syscounter_armv8_m_read_dev_cfg_t {
    const uint32_t base;
};

/**
 * \brief Armv8-M System Counter device structure
 */
struct syscounter_armv8_m_read_dev_t {
    const struct syscounter_armv8_m_read_dev_cfg_t* const cfg;
        /*!< Armv8-M System Counter configuration structure */
};

/**
 * \brief Read counter value
 *
 * \param[in] dev Counter device struct \ref syscounter_armv8_m_read_dev_t
 *
 * \return 64 bit counter value
 *
 * \note This function doesn't check if dev is NULL.
 */
uint64_t syscounter_armv8_m_read_get_counter_value(
        struct syscounter_armv8_m_read_dev_t* dev);

#ifdef __cplusplus
}
#endif
#endif /* __SYSCOUNTER_ARMV8_M_READ_DRV_H__ */
