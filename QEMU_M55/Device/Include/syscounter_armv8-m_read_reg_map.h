/*
 * Copyright (c) 2021 ARM Limited
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
 * \file syscounter_armv8-m_read_reg_map.h
 * \brief Register map for Armv8-M System Counter Read,
 *        covering CNTReadBase Frame
 */

#ifndef __SYSCOUNTER_ARMV8_M_READ_REG_MAP_H__
#define __SYSCOUNTER_ARMV8_M_READ_REG_MAP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief CNTReadBase Register map structure
 */
struct cnt_read_base_reg_map_t {
    volatile const uint32_t cntcv_low;
        /*!< Offset: 0x000 (R/W) Counter Count Value [31:0] Register */
    volatile const uint32_t cntcv_high;
        /*!< Offset: 0x004 (R/W) Counter Count Value [63:32] Register */
    volatile const uint32_t reserved0[1010];
        /*!< Offset: 0x004-0xFCC Reserved (RAZWI) */
    volatile const uint32_t cntpidr4;
        /*!< Offset: 0xFD0 (RO) Peripheral ID Register */
    volatile const uint32_t reserved1[3];
        /*!< Offset: 0xFD4-0xFDC Reserved (RAZWI) */
    volatile const uint32_t cntpidr0;
        /*!< Offset: 0xFE0 (RO) Peripheral ID Register */
    volatile const uint32_t cntpidr1;
        /*!< Offset: 0xFE4 (RO) Peripheral ID Register */
    volatile const uint32_t cntpidr2;
        /*!< Offset: 0xFE8 (RO) Peripheral ID Register */
    volatile const uint32_t cntpidr3;
        /*!< Offset: 0xFEC (RO) Peripheral ID Register */
    volatile const uint32_t cntcidr0;
        /*!< Offset: 0xFF0 (RO) Component ID Register */
    volatile const uint32_t cntcidr1;
        /*!< Offset: 0xFF4 (RO) Component ID Register */
    volatile const uint32_t cntcidr2;
        /*!< Offset: 0xFF8 (RO) Component ID Register */
    volatile const uint32_t cntcidr3;
        /*!< Offset: 0xFFC (RO) Component ID Register */
};

#ifdef __cplusplus
}
#endif

#endif /* __SYSCOUNTER_ARMV8_M_READ_REG_MAP_H__ */
