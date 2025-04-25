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
 * \file syscounter_armv8-m_read_drv.c
 *
 * \brief Driver for Armv8-M System Counter Read, covering CNTReadBase Frame
 *
 *        Details in \file syscounter_armv8-m_cntrl_drv.h
 */

#include "syscounter_armv8-m_read_drv.h"
#include "syscounter_armv8-m_read_reg_map.h"

/** Getter bit manipulation macro */
#define GET_BIT(WORD, BIT_INDEX) (bool)(((WORD) & (1u << (BIT_INDEX))))

/** Getter bit-field manipulation macro */
#define GET_BIT_FIELD(WORD, BIT_MASK, BIT_OFFSET) \
            ((WORD & BIT_MASK) >> BIT_OFFSET)

/** Clear-and-Set bit-field manipulation macro */
#define ASSIGN_BIT_FIELD(WORD, BIT_MASK, BIT_OFFSET, VALUE) \
            (WORD = ((WORD & ~(BIT_MASK)) | ((VALUE << BIT_OFFSET) & BIT_MASK)))

/** Bit mask for given width bit-field manipulation macro */
#define BITMASK(width) ((1u<<(width))-1)

uint64_t syscounter_armv8_m_read_get_counter_value(
        struct syscounter_armv8_m_read_dev_t* dev)
{
    struct cnt_read_base_reg_map_t* p_cnt =
            (struct cnt_read_base_reg_map_t*)dev->cfg->base;
    uint32_t high = 0;
    uint32_t low = 0;
    uint32_t high_prev = 0;
    uint64_t value = 0;

    /* Make sure the 64-bit read will be atomic to avoid overflow between
     * the low and high registers read
     */
    high = p_cnt->cntcv_high;
    do {
        high_prev = high;
        low = p_cnt->cntcv_low;
        high = p_cnt->cntcv_high;
    }while(high != high_prev);

    value = low |
            (((uint64_t)high) << SYSCOUNTER_ARMV8_M_READ_REGISTER_BIT_WIDTH);
    return value;
}
