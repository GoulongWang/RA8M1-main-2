/*
 * Copyright (c) 2019-2023 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file device_definition.c
 * \brief This file defines exports the structures based on the peripheral
 * definitions from device_cfg.h.
 * This file is meant to be used as a helper for baremetal
 * applications and/or as an example of how to configure the generic
 * driver structures.
 */

#include "device_definition.h"
#include "platform_base_address.h"

/* UART CMSDK driver structures */
#ifdef UART0_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_S = {.base = UART0_BASE_S,
                                                                  .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_S = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART0_CMSDK_DEV_S = {&(UART0_CMSDK_DEV_CFG_S), &(UART0_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART0_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART0_CMSDK_DEV_CFG_NS = {.base = UART0_BASE_NS,
                                                                   .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART0_CMSDK_DEV_DATA_NS = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART0_CMSDK_DEV_NS = {&(UART0_CMSDK_DEV_CFG_NS), &(UART0_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART1_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART1_CMSDK_DEV_CFG_S = {.base = UART1_BASE_S,
                                                                  .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART1_CMSDK_DEV_DATA_S = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART1_CMSDK_DEV_S = {&(UART1_CMSDK_DEV_CFG_S), &(UART1_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART1_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART1_CMSDK_DEV_CFG_NS = {.base = UART1_BASE_NS,
                                                                   .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART1_CMSDK_DEV_DATA_NS = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART1_CMSDK_DEV_NS = {&(UART1_CMSDK_DEV_CFG_NS), &(UART1_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART2_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART2_CMSDK_DEV_CFG_S = {.base = UART2_BASE_S,
                                                                  .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART2_CMSDK_DEV_DATA_S = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART2_CMSDK_DEV_S = {&(UART2_CMSDK_DEV_CFG_S), &(UART2_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART2_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART2_CMSDK_DEV_CFG_NS = {.base = UART2_BASE_NS,
                                                                   .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART2_CMSDK_DEV_DATA_NS = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART2_CMSDK_DEV_NS = {&(UART2_CMSDK_DEV_CFG_NS), &(UART2_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART3_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART3_CMSDK_DEV_CFG_S = {.base = UART3_BASE_S,
                                                                  .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART3_CMSDK_DEV_DATA_S = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART3_CMSDK_DEV_S = {&(UART3_CMSDK_DEV_CFG_S), &(UART3_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART3_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART3_CMSDK_DEV_CFG_NS = {.base = UART3_BASE_NS,
                                                                   .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART3_CMSDK_DEV_DATA_NS = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART3_CMSDK_DEV_NS = {&(UART3_CMSDK_DEV_CFG_NS), &(UART3_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART4_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART4_CMSDK_DEV_CFG_S = {.base = UART4_BASE_S,
                                                                  .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART4_CMSDK_DEV_DATA_S = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART4_CMSDK_DEV_S = {&(UART4_CMSDK_DEV_CFG_S), &(UART4_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART4_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART4_CMSDK_DEV_CFG_NS = {.base = UART4_BASE_NS,
                                                                   .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART4_CMSDK_DEV_DATA_NS = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART4_CMSDK_DEV_NS = {&(UART4_CMSDK_DEV_CFG_NS), &(UART4_CMSDK_DEV_DATA_NS)};
#endif

#ifdef UART5_CMSDK_S
static const struct uart_cmsdk_dev_cfg_t UART5_CMSDK_DEV_CFG_S = {.base = UART5_BASE_S,
                                                                  .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART5_CMSDK_DEV_DATA_S = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART5_CMSDK_DEV_S = {&(UART5_CMSDK_DEV_CFG_S), &(UART5_CMSDK_DEV_DATA_S)};
#endif
#ifdef UART5_CMSDK_NS
static const struct uart_cmsdk_dev_cfg_t UART5_CMSDK_DEV_CFG_NS = {.base = UART5_BASE_NS,
                                                                   .default_baudrate = DEFAULT_UART_BAUDRATE};
static struct uart_cmsdk_dev_data_t UART5_CMSDK_DEV_DATA_NS = {.state = 0, .system_clk = 0, .baudrate = 0};
struct uart_cmsdk_dev_t UART5_CMSDK_DEV_NS = {&(UART5_CMSDK_DEV_CFG_NS), &(UART5_CMSDK_DEV_DATA_NS)};
#endif

/* SSE-300 PPC driver structures */
#ifdef PPC_SSE300_MAIN0_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN0_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_MAIN0};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN0_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN0_DEV_S = {&PPC_SSE300_MAIN0_CFG_S, &PPC_SSE300_MAIN0_DATA_S};
#endif

#ifdef PPC_SSE300_MAIN0_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN0_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                              .ppc_name = PPC_SSE300_MAIN0};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN0_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN0_DEV_NS = {&PPC_SSE300_MAIN0_CFG_NS, &PPC_SSE300_MAIN0_DATA_NS};
#endif

#ifdef PPC_SSE300_MAIN_EXP0_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP0_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_MAIN_EXP0};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP0_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP0_DEV_S = {&PPC_SSE300_MAIN_EXP0_CFG_S, &PPC_SSE300_MAIN_EXP0_DATA_S};
#endif

#ifdef PPC_SSE300_MAIN_EXP0_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP0_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                  .ppc_name = PPC_SSE300_MAIN_EXP0};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP0_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP0_DEV_NS = {&PPC_SSE300_MAIN_EXP0_CFG_NS, &PPC_SSE300_MAIN_EXP0_DATA_NS};
#endif

#ifdef PPC_SSE300_MAIN_EXP1_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP1_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_MAIN_EXP1};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP1_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP1_DEV_S = {&PPC_SSE300_MAIN_EXP1_CFG_S, &PPC_SSE300_MAIN_EXP1_DATA_S};
#endif

#ifdef PPC_SSE300_MAIN_EXP1_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP1_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                  .ppc_name = PPC_SSE300_MAIN_EXP1};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP1_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP1_DEV_NS = {&PPC_SSE300_MAIN_EXP1_CFG_NS, &PPC_SSE300_MAIN_EXP1_DATA_NS};
#endif

#ifdef PPC_SSE300_MAIN_EXP2_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP2_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_MAIN_EXP2};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP2_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP2_DEV_S = {&PPC_SSE300_MAIN_EXP2_CFG_S, &PPC_SSE300_MAIN_EXP2_DATA_S};
#endif

#ifdef PPC_SSE300_MAIN_EXP2_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP2_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                  .ppc_name = PPC_SSE300_MAIN_EXP2};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP2_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP2_DEV_NS = {&PPC_SSE300_MAIN_EXP2_CFG_NS, &PPC_SSE300_MAIN_EXP2_DATA_NS};
#endif

#ifdef PPC_SSE300_MAIN_EXP3_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP3_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_MAIN_EXP3};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP3_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP3_DEV_S = {&PPC_SSE300_MAIN_EXP3_CFG_S, &PPC_SSE300_MAIN_EXP3_DATA_S};
#endif

#ifdef PPC_SSE300_MAIN_EXP3_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_MAIN_EXP3_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                  .ppc_name = PPC_SSE300_MAIN_EXP3};
static struct ppc_sse300_dev_data_t PPC_SSE300_MAIN_EXP3_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_MAIN_EXP3_DEV_NS = {&PPC_SSE300_MAIN_EXP3_CFG_NS, &PPC_SSE300_MAIN_EXP3_DATA_NS};
#endif

#ifdef PPC_SSE300_PERIPH0_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH0_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_PERIPH0};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH0_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH0_DEV_S = {&PPC_SSE300_PERIPH0_CFG_S, &PPC_SSE300_PERIPH0_DATA_S};
#endif

#ifdef PPC_SSE300_PERIPH0_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH0_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                .ppc_name = PPC_SSE300_PERIPH0};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH0_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH0_DEV_NS = {&PPC_SSE300_PERIPH0_CFG_NS, &PPC_SSE300_PERIPH0_DATA_NS};
#endif

#ifdef PPC_SSE300_PERIPH1_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH1_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_PERIPH1};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH1_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH1_DEV_S = {&PPC_SSE300_PERIPH1_CFG_S, &PPC_SSE300_PERIPH1_DATA_S};
#endif

#ifdef PPC_SSE300_PERIPH1_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH1_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                .ppc_name = PPC_SSE300_PERIPH1};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH1_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH1_DEV_NS = {&PPC_SSE300_PERIPH1_CFG_NS, &PPC_SSE300_PERIPH1_DATA_NS};
#endif

#ifdef PPC_SSE300_PERIPH_EXP0_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP0_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_PERIPH_EXP0};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP0_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP0_DEV_S = {&PPC_SSE300_PERIPH_EXP0_CFG_S, &PPC_SSE300_PERIPH_EXP0_DATA_S};
#endif

#ifdef PPC_SSE300_PERIPH_EXP0_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP0_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                    .ppc_name = PPC_SSE300_PERIPH_EXP0};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP0_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP0_DEV_NS = {&PPC_SSE300_PERIPH_EXP0_CFG_NS,
                                                         &PPC_SSE300_PERIPH_EXP0_DATA_NS};
#endif

#ifdef PPC_SSE300_PERIPH_EXP1_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP1_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_PERIPH_EXP1};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP1_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP1_DEV_S = {&PPC_SSE300_PERIPH_EXP1_CFG_S, &PPC_SSE300_PERIPH_EXP1_DATA_S};
#endif

#ifdef PPC_SSE300_PERIPH_EXP1_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP1_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                    .ppc_name = PPC_SSE300_PERIPH_EXP1};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP1_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP1_DEV_NS = {&PPC_SSE300_PERIPH_EXP1_CFG_NS,
                                                         &PPC_SSE300_PERIPH_EXP1_DATA_NS};
#endif

#ifdef PPC_SSE300_PERIPH_EXP2_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP2_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_PERIPH_EXP2};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP2_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP2_DEV_S = {&PPC_SSE300_PERIPH_EXP2_CFG_S, &PPC_SSE300_PERIPH_EXP2_DATA_S};
#endif

#ifdef PPC_SSE300_PERIPH_EXP2_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP2_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                    .ppc_name = PPC_SSE300_PERIPH_EXP2};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP2_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP2_DEV_NS = {&PPC_SSE300_PERIPH_EXP2_CFG_NS,
                                                         &PPC_SSE300_PERIPH_EXP2_DATA_NS};
#endif

#ifdef PPC_SSE300_PERIPH_EXP3_S
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP3_CFG_S = {
    .sacfg_base = SSE300_SACFG_BASE_S, .nsacfg_base = SSE300_NSACFG_BASE_NS, .ppc_name = PPC_SSE300_PERIPH_EXP3};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP3_DATA_S = {
    .sacfg_ns_ppc = 0, .sacfg_sp_ppc = 0, .nsacfg_nsp_ppc = 0, .int_bit_mask = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP3_DEV_S = {&PPC_SSE300_PERIPH_EXP3_CFG_S, &PPC_SSE300_PERIPH_EXP3_DATA_S};
#endif

#ifdef PPC_SSE300_PERIPH_EXP3_NS
static struct ppc_sse300_dev_cfg_t PPC_SSE300_PERIPH_EXP3_CFG_NS = {.nsacfg_base = SSE300_NSACFG_BASE_NS,
                                                                    .ppc_name = PPC_SSE300_PERIPH_EXP3};
static struct ppc_sse300_dev_data_t PPC_SSE300_PERIPH_EXP3_DATA_NS = {.nsacfg_nsp_ppc = 0, .is_initialized = false};
struct ppc_sse300_dev_t PPC_SSE300_PERIPH_EXP3_DEV_NS = {&PPC_SSE300_PERIPH_EXP3_CFG_NS,
                                                         &PPC_SSE300_PERIPH_EXP3_DATA_NS};
#endif

/* System counters */
#ifdef SYSCOUNTER_CNTRL_ARMV8_M_S

#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT > SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT > SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT > SYSCOUNTER_ARMV8_M_SCALE_VAL_INT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT is invalid.
#endif
#if SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT > SYSCOUNTER_ARMV8_M_SCALE_VAL_FRACT_MAX
#error SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT is invalid.
#endif

static const struct syscounter_armv8_m_cntrl_dev_cfg_t SYSCOUNTER_CNTRL_ARMV8_M_DEV_CFG_S = {
    .base = SYSCNTR_CNTRL_BASE_S,
    .scale0.integer = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_INT,
    .scale0.fixed_point_fraction = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE0_FRACT,
    .scale1.integer = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_INT,
    .scale1.fixed_point_fraction = SYSCOUNTER_ARMV8_M_DEFAULT_SCALE1_FRACT};
static struct syscounter_armv8_m_cntrl_dev_data_t SYSCOUNTER_CNTRL_ARMV8_M_DEV_DATA_S = {.is_initialized = false};
struct syscounter_armv8_m_cntrl_dev_t SYSCOUNTER_CNTRL_ARMV8_M_DEV_S = {&(SYSCOUNTER_CNTRL_ARMV8_M_DEV_CFG_S),
                                                                        &(SYSCOUNTER_CNTRL_ARMV8_M_DEV_DATA_S)};
#endif

#ifdef SYSCOUNTER_READ_ARMV8_M_S
static const struct syscounter_armv8_m_read_dev_cfg_t SYSCOUNTER_READ_ARMV8_M_DEV_CFG_S = {
    .base = SYSCNTR_READ_BASE_S,
};
struct syscounter_armv8_m_read_dev_t SYSCOUNTER_READ_ARMV8_M_DEV_S = {
    &(SYSCOUNTER_READ_ARMV8_M_DEV_CFG_S),
};
#endif
#ifdef SYSCOUNTER_READ_ARMV8_M_NS
static const struct syscounter_armv8_m_read_dev_cfg_t SYSCOUNTER_READ_ARMV8_M_DEV_CFG_NS = {
    .base = SYSCNTR_READ_BASE_NS,
};
struct syscounter_armv8_m_read_dev_t SYSCOUNTER_READ_ARMV8_M_DEV_NS = {
    &(SYSCOUNTER_READ_ARMV8_M_DEV_CFG_NS),
};
#endif

/* System timers */
#ifdef SYSTIMER0_ARMV8_M_S
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER0_ARMV8_M_DEV_CFG_S = {
    .base = SYSTIMER0_ARMV8_M_BASE_S, .default_freq_hz = SYSTIMER0_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER0_ARMV8_M_DEV_DATA_S = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_S = {&(SYSTIMER0_ARMV8_M_DEV_CFG_S),
                                                         &(SYSTIMER0_ARMV8_M_DEV_DATA_S)};
#endif

#ifdef SYSTIMER0_ARMV8_M_NS
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER0_ARMV8_M_DEV_CFG_NS = {
    .base = SYSTIMER0_ARMV8_M_BASE_NS, .default_freq_hz = SYSTIMER0_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER0_ARMV8_M_DEV_DATA_NS = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER0_ARMV8_M_DEV_NS = {&(SYSTIMER0_ARMV8_M_DEV_CFG_NS),
                                                          &(SYSTIMER0_ARMV8_M_DEV_DATA_NS)};
#endif

#ifdef SYSTIMER1_ARMV8_M_S
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER1_ARMV8_M_DEV_CFG_S = {
    .base = SYSTIMER1_ARMV8_M_BASE_S, .default_freq_hz = SYSTIMER1_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER1_ARMV8_M_DEV_DATA_S = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_S = {&(SYSTIMER1_ARMV8_M_DEV_CFG_S),
                                                         &(SYSTIMER1_ARMV8_M_DEV_DATA_S)};
#endif

#ifdef SYSTIMER1_ARMV8_M_NS
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER1_ARMV8_M_DEV_CFG_NS = {
    .base = SYSTIMER1_ARMV8_M_BASE_NS, .default_freq_hz = SYSTIMER1_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER1_ARMV8_M_DEV_DATA_NS = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER1_ARMV8_M_DEV_NS = {&(SYSTIMER1_ARMV8_M_DEV_CFG_NS),
                                                          &(SYSTIMER1_ARMV8_M_DEV_DATA_NS)};
#endif

#ifdef SYSTIMER2_ARMV8_M_S
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER2_ARMV8_M_DEV_CFG_S = {
    .base = SYSTIMER2_ARMV8_M_BASE_S, .default_freq_hz = SYSTIMER2_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER2_ARMV8_M_DEV_DATA_S = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER2_ARMV8_M_DEV_S = {&(SYSTIMER2_ARMV8_M_DEV_CFG_S),
                                                         &(SYSTIMER2_ARMV8_M_DEV_DATA_S)};
#endif

#ifdef SYSTIMER2_ARMV8_M_NS
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER2_ARMV8_M_DEV_CFG_NS = {
    .base = SYSTIMER2_ARMV8_M_BASE_NS, .default_freq_hz = SYSTIMER2_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER2_ARMV8_M_DEV_DATA_NS = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER2_ARMV8_M_DEV_NS = {&(SYSTIMER2_ARMV8_M_DEV_CFG_NS),
                                                          &(SYSTIMER2_ARMV8_M_DEV_DATA_NS)};
#endif

#ifdef SYSTIMER3_ARMV8_M_S
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER3_ARMV8_M_DEV_CFG_S = {
    .base = SYSTIMER3_ARMV8_M_BASE_S, .default_freq_hz = SYSTIMER3_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER3_ARMV8_M_DEV_DATA_S = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER3_ARMV8_M_DEV_S = {&(SYSTIMER3_ARMV8_M_DEV_CFG_S),
                                                         &(SYSTIMER3_ARMV8_M_DEV_DATA_S)};
#endif

#ifdef SYSTIMER3_ARMV8_M_NS
static const struct systimer_armv8_m_dev_cfg_t SYSTIMER3_ARMV8_M_DEV_CFG_NS = {
    .base = SYSTIMER3_ARMV8_M_BASE_NS, .default_freq_hz = SYSTIMER3_ARMV8M_DEFAULT_FREQ_HZ};
static struct systimer_armv8_m_dev_data_t SYSTIMER3_ARMV8_M_DEV_DATA_NS = {.is_initialized = false};
struct systimer_armv8_m_dev_t SYSTIMER3_ARMV8_M_DEV_NS = {&(SYSTIMER3_ARMV8_M_DEV_CFG_NS),
                                                          &(SYSTIMER3_ARMV8_M_DEV_DATA_NS)};
#endif

/* System Watchdogs */
#ifdef SYSWDOG_ARMV8_M_S
static const struct syswdog_armv8_m_dev_cfg_t SYSWDOG_ARMV8_M_DEV_CFG_S = {.base = SYSWDOG_ARMV8_M_CNTRL_BASE_S};
struct syswdog_armv8_m_dev_t SYSWDOG_ARMV8_M_DEV_S = {&(SYSWDOG_ARMV8_M_DEV_CFG_S)};
#endif

#ifdef SYSWDOG_ARMV8_M_NS
static const struct syswdog_armv8_m_dev_cfg_t SYSWDOG_ARMV8_M_DEV_CFG_NS = {.base = SYSWDOG_ARMV8_M_CNTRL_BASE_NS};
struct syswdog_armv8_m_dev_t SYSWDOG_ARMV8_M_DEV_NS = {&(SYSWDOG_ARMV8_M_DEV_CFG_NS)};
#endif

/* ARM MPC SSE 300 driver structures */
/* Ranges controlled by this SRAM_MPC */
#ifdef MPC_SRAM_S
static const struct mpc_sie_memory_range_t MPC_SRAM_RANGE_S = {
    .base = MPC_SRAM_RANGE_BASE_S, .limit = MPC_SRAM_RANGE_LIMIT_S, .range_offset = 0, .attr = MPC_SIE_SEC_ATTR_SECURE};

static const struct mpc_sie_memory_range_t MPC_SRAM_RANGE_NS = {.base = MPC_SRAM_RANGE_BASE_NS,
                                                                .limit = MPC_SRAM_RANGE_LIMIT_NS,
                                                                .range_offset = 0,
                                                                .attr = MPC_SIE_SEC_ATTR_NONSECURE};

#define MPC_SRAM_RANGE_LIST_LEN 2u
static const struct mpc_sie_memory_range_t *MPC_SRAM_RANGE_LIST[MPC_SRAM_RANGE_LIST_LEN] = {&MPC_SRAM_RANGE_S,
                                                                                            &MPC_SRAM_RANGE_NS};

static struct mpc_sie_dev_cfg_t MPC_SRAM_DEV_CFG_S = {
    .base = MPC_SRAM_BASE_S, .range_list = MPC_SRAM_RANGE_LIST, .nbr_of_ranges = MPC_SRAM_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_SRAM_DEV_DATA_S = {.is_initialized = false};
struct mpc_sie_dev_t MPC_SRAM_DEV_S = {&(MPC_SRAM_DEV_CFG_S), &(MPC_SRAM_DEV_DATA_S)};
#endif

#ifdef MPC_QSPI_S
static const struct mpc_sie_memory_range_t MPC_QSPI_RANGE_S = {
    .base = MPC_QSPI_RANGE_BASE_S, .limit = MPC_QSPI_RANGE_LIMIT_S, .range_offset = 0, .attr = MPC_SIE_SEC_ATTR_SECURE};

static const struct mpc_sie_memory_range_t MPC_QSPI_RANGE_NS = {.base = MPC_QSPI_RANGE_BASE_NS,
                                                                .limit = MPC_QSPI_RANGE_LIMIT_NS,
                                                                .range_offset = 0,
                                                                .attr = MPC_SIE_SEC_ATTR_NONSECURE};

#define MPC_QSPI_RANGE_LIST_LEN 2u
static const struct mpc_sie_memory_range_t *MPC_QSPI_RANGE_LIST[MPC_QSPI_RANGE_LIST_LEN] = {&MPC_QSPI_RANGE_S,
                                                                                            &MPC_QSPI_RANGE_NS};

static struct mpc_sie_dev_cfg_t MPC_QSPI_DEV_CFG_S = {
    .base = MPC_QSPI_BASE_S, .range_list = MPC_QSPI_RANGE_LIST, .nbr_of_ranges = MPC_QSPI_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_QSPI_DEV_DATA_S = {.is_initialized = false};
struct mpc_sie_dev_t MPC_QSPI_DEV_S = {&(MPC_QSPI_DEV_CFG_S), &(MPC_QSPI_DEV_DATA_S)};
#endif

#ifdef MPC_DDR4_S
static const struct mpc_sie_memory_range_t MPC_DDR4_RANGE_S = {
    .base = MPC_DDR4_RANGE_BASE_S, .limit = MPC_DDR4_RANGE_LIMIT_S, .range_offset = 0, .attr = MPC_SIE_SEC_ATTR_SECURE};

static const struct mpc_sie_memory_range_t MPC_DDR4_RANGE_NS = {.base = MPC_DDR4_RANGE_BASE_NS,
                                                                .limit = MPC_DDR4_RANGE_LIMIT_NS,
                                                                .range_offset = 0,
                                                                .attr = MPC_SIE_SEC_ATTR_NONSECURE};

#define MPC_DDR4_RANGE_LIST_LEN 2u
static const struct mpc_sie_memory_range_t *MPC_DDR4_RANGE_LIST[MPC_DDR4_RANGE_LIST_LEN] = {&MPC_DDR4_RANGE_S,
                                                                                            &MPC_DDR4_RANGE_NS};

static struct mpc_sie_dev_cfg_t MPC_DDR4_DEV_CFG_S = {
    .base = MPC_DDR4_BASE_S, .range_list = MPC_DDR4_RANGE_LIST, .nbr_of_ranges = MPC_DDR4_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_DDR4_DEV_DATA_S = {.is_initialized = false};
struct mpc_sie_dev_t MPC_DDR4_DEV_S = {&(MPC_DDR4_DEV_CFG_S), &(MPC_DDR4_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM0_S
static const struct mpc_sie_memory_range_t MPC_ISRAM0_RANGE_S = {.base = MPC_ISRAM0_RANGE_BASE_S,
                                                                 .limit = MPC_ISRAM0_RANGE_LIMIT_S,
                                                                 .range_offset = 0,
                                                                 .attr = MPC_SIE_SEC_ATTR_SECURE};

static const struct mpc_sie_memory_range_t MPC_ISRAM0_RANGE_NS = {.base = MPC_ISRAM0_RANGE_BASE_NS,
                                                                  .limit = MPC_ISRAM0_RANGE_LIMIT_NS,
                                                                  .range_offset = 0,
                                                                  .attr = MPC_SIE_SEC_ATTR_NONSECURE};

#define MPC_ISRAM0_RANGE_LIST_LEN 2u
static const struct mpc_sie_memory_range_t *MPC_ISRAM0_RANGE_LIST[MPC_ISRAM0_RANGE_LIST_LEN] = {&MPC_ISRAM0_RANGE_S,
                                                                                                &MPC_ISRAM0_RANGE_NS};

static struct mpc_sie_dev_cfg_t MPC_ISRAM0_DEV_CFG_S = {
    .base = MPC_ISRAM0_BASE_S, .range_list = MPC_ISRAM0_RANGE_LIST, .nbr_of_ranges = MPC_ISRAM0_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_ISRAM0_DEV_DATA_S = {.is_initialized = false};
struct mpc_sie_dev_t MPC_ISRAM0_DEV_S = {&(MPC_ISRAM0_DEV_CFG_S), &(MPC_ISRAM0_DEV_DATA_S)};
#endif

#ifdef MPC_ISRAM1_S
static const struct mpc_sie_memory_range_t MPC_ISRAM1_RANGE_S = {.base = MPC_ISRAM1_RANGE_BASE_S,
                                                                 .limit = MPC_ISRAM1_RANGE_LIMIT_S,
                                                                 .range_offset = 0,
                                                                 .attr = MPC_SIE_SEC_ATTR_SECURE};

static const struct mpc_sie_memory_range_t MPC_ISRAM1_RANGE_NS = {.base = MPC_ISRAM1_RANGE_BASE_NS,
                                                                  .limit = MPC_ISRAM1_RANGE_LIMIT_NS,
                                                                  .range_offset = 0,
                                                                  .attr = MPC_SIE_SEC_ATTR_NONSECURE};

#define MPC_ISRAM1_RANGE_LIST_LEN 2u
static const struct mpc_sie_memory_range_t *MPC_ISRAM1_RANGE_LIST[MPC_ISRAM1_RANGE_LIST_LEN] = {&MPC_ISRAM1_RANGE_S,
                                                                                                &MPC_ISRAM1_RANGE_NS};

static struct mpc_sie_dev_cfg_t MPC_ISRAM1_DEV_CFG_S = {
    .base = MPC_ISRAM1_BASE_S, .range_list = MPC_ISRAM1_RANGE_LIST, .nbr_of_ranges = MPC_ISRAM1_RANGE_LIST_LEN};
static struct mpc_sie_dev_data_t MPC_ISRAM1_DEV_DATA_S = {.is_initialized = false};
struct mpc_sie_dev_t MPC_ISRAM1_DEV_S = {&(MPC_ISRAM1_DEV_CFG_S), &(MPC_ISRAM1_DEV_DATA_S)};
#endif

#ifdef MPS3_IO_S
static struct arm_mps3_io_dev_cfg_t MPS3_IO_DEV_CFG_S = {.base = FPGA_IO_BASE_S};
struct arm_mps3_io_dev_t MPS3_IO_DEV_S = {.cfg = &(MPS3_IO_DEV_CFG_S)};
#endif

#ifdef MPS3_IO_NS
static struct arm_mps3_io_dev_cfg_t MPS3_IO_DEV_CFG_NS = {.base = FPGA_IO_BASE_NS};
struct arm_mps3_io_dev_t MPS3_IO_DEV_NS = {.cfg = &(MPS3_IO_DEV_CFG_NS)};
#endif

#ifdef SMSC9220_ETH_S
static struct smsc9220_eth_dev_cfg_t SMSC9220_ETH_DEV_CFG_S = {.base = ETHERNET_BASE_S};
static struct smsc9220_eth_dev_data_t SMSC9220_ETH_DEV_DATA_S = {
    .state = 0,
    .wait_ms = 0,
    .ongoing_packet_length = 0,
    .ongoing_packet_length_sent = 0,
    .current_rx_size_words = 0,
};
struct smsc9220_eth_dev_t SMSC9220_ETH_DEV_S = {
    .cfg = &(SMSC9220_ETH_DEV_CFG_S),
    .data = &(SMSC9220_ETH_DEV_DATA_S),
};
#endif

#ifdef SMSC9220_ETH_NS
static struct smsc9220_eth_dev_cfg_t SMSC9220_ETH_DEV_CFG_NS = {.base = ETHERNET_BASE_NS};
static struct smsc9220_eth_dev_data_t SMSC9220_ETH_DEV_DATA_NS = {
    .state = 0,
    .wait_ms = 0,
    .ongoing_packet_length = 0,
    .ongoing_packet_length_sent = 0,
    .current_rx_size_words = 0,
};
struct smsc9220_eth_dev_t SMSC9220_ETH_DEV_NS = {
    .cfg = &(SMSC9220_ETH_DEV_CFG_NS),
    .data = &(SMSC9220_ETH_DEV_DATA_NS),
};
#endif

/* CMSDK GPIO driver structures */
#ifdef GPIO0_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO0_CMSDK_DEV_CFG_S = {.base = GPIO0_CMSDK_BASE_S};
struct gpio_cmsdk_dev_t GPIO0_CMSDK_DEV_S = {&(GPIO0_CMSDK_DEV_CFG_S)};
#endif

#ifdef GPIO0_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO0_CMSDK_DEV_CFG_NS = {.base = GPIO0_CMSDK_BASE_NS};
struct gpio_cmsdk_dev_t GPIO0_CMSDK_DEV_NS = {&(GPIO0_CMSDK_DEV_CFG_NS)};
#endif

#ifdef GPIO1_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO1_CMSDK_DEV_CFG_S = {.base = GPIO1_CMSDK_BASE_S};
struct gpio_cmsdk_dev_t GPIO1_CMSDK_DEV_S = {&(GPIO1_CMSDK_DEV_CFG_S)};
#endif

#ifdef GPIO1_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO1_CMSDK_DEV_CFG_NS = {.base = GPIO1_CMSDK_BASE_NS};
struct gpio_cmsdk_dev_t GPIO1_CMSDK_DEV_NS = {&(GPIO1_CMSDK_DEV_CFG_NS)};
#endif

#ifdef GPIO2_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO2_CMSDK_DEV_CFG_S = {.base = GPIO2_CMSDK_BASE_S};
struct gpio_cmsdk_dev_t GPIO2_CMSDK_DEV_S = {&(GPIO2_CMSDK_DEV_CFG_S)};
#endif

#ifdef GPIO2_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO2_CMSDK_DEV_CFG_NS = {.base = GPIO2_CMSDK_BASE_NS};
struct gpio_cmsdk_dev_t GPIO2_CMSDK_DEV_NS = {&(GPIO2_CMSDK_DEV_CFG_NS)};
#endif

#ifdef GPIO3_CMSDK_S
static const struct gpio_cmsdk_dev_cfg_t GPIO3_CMSDK_DEV_CFG_S = {.base = GPIO3_CMSDK_BASE_S};
struct gpio_cmsdk_dev_t GPIO3_CMSDK_DEV_S = {&(GPIO3_CMSDK_DEV_CFG_S)};
#endif

#ifdef GPIO3_CMSDK_NS
static const struct gpio_cmsdk_dev_cfg_t GPIO3_CMSDK_DEV_CFG_NS = {.base = GPIO3_CMSDK_BASE_NS};
struct gpio_cmsdk_dev_t GPIO3_CMSDK_DEV_NS = {&(GPIO3_CMSDK_DEV_CFG_NS)};
#endif

/* PL022 SPI driver structures */
#ifdef SPI0_PL022_S
static const struct spi_pl022_dev_cfg_t SPI0_PL022_DEV_CFG_S = {
    .base = FPGA_SPI_ADC_BASE_S,
    .default_ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                         .word_size = 8,
                         .bit_rate = 100000}};
static struct spi_pl022_dev_data_t SPI0_PL022_DEV_DATA_S = {.state = 0,
                                                            .sys_clk = 0,
                                                            .ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                                                                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                                                                         .word_size = 8,
                                                                         .bit_rate = 100000}};
struct spi_pl022_dev_t SPI0_PL022_DEV_S = {.cfg = &(SPI0_PL022_DEV_CFG_S), .data = &(SPI0_PL022_DEV_DATA_S)};
#endif

#ifdef SPI0_PL022_NS
static const struct spi_pl022_dev_cfg_t SPI0_PL022_DEV_CFG_NS = {
    .base = FPGA_SPI_ADC_BASE_NS,
    .default_ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                         .word_size = 8,
                         .bit_rate = 100000}};
static struct spi_pl022_dev_data_t SPI0_PL022_DEV_DATA_NS = {.state = 0,
                                                             .sys_clk = 0,
                                                             .ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                                                                          .frame_format = SPI_PL022_CFG_FRF_MOT,
                                                                          .word_size = 8,
                                                                          .bit_rate = 100000}};
struct spi_pl022_dev_t SPI0_PL022_DEV_NS = {.cfg = &(SPI0_PL022_DEV_CFG_NS), .data = &(SPI0_PL022_DEV_DATA_NS)};
#endif

#ifdef SPI1_PL022_S
static const struct spi_pl022_dev_cfg_t SPI1_PL022_DEV_CFG_S = {
    .base = FPGA_SPI_SHIELD0_BASE_S,
    .default_ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                         .word_size = 8,
                         .bit_rate = 100000}};
static struct spi_pl022_dev_data_t SPI1_PL022_DEV_DATA_S = {.state = 0,
                                                            .sys_clk = 0,
                                                            .ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                                                                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                                                                         .word_size = 8,
                                                                         .bit_rate = 100000}};
struct spi_pl022_dev_t SPI1_PL022_DEV_S = {.cfg = &(SPI1_PL022_DEV_CFG_S), .data = &(SPI1_PL022_DEV_DATA_S)};
#endif

#ifdef SPI1_PL022_NS
static const struct spi_pl022_dev_cfg_t SPI1_PL022_DEV_CFG_NS = {
    .base = FPGA_SPI_SHIELD0_BASE_NS,
    .default_ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                         .word_size = 8,
                         .bit_rate = 100000}};
static struct spi_pl022_dev_data_t SPI1_PL022_DEV_DATA_NS = {.state = 0,
                                                             .sys_clk = 0,
                                                             .ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                                                                          .frame_format = SPI_PL022_CFG_FRF_MOT,
                                                                          .word_size = 8,
                                                                          .bit_rate = 100000}};
struct spi_pl022_dev_t SPI1_PL022_DEV_NS = {.cfg = &(SPI1_PL022_DEV_CFG_NS), .data = &(SPI1_PL022_DEV_DATA_NS)};
#endif

#ifdef SPI2_PL022_S
static const struct spi_pl022_dev_cfg_t SPI2_PL022_DEV_CFG_S = {
    .base = FPGA_SPI_SHIELD1_BASE_S,
    .default_ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                         .word_size = 8,
                         .bit_rate = 100000}};
static struct spi_pl022_dev_data_t SPI2_PL022_DEV_DATA_S = {.state = 0,
                                                            .sys_clk = 0,
                                                            .ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                                                                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                                                                         .word_size = 8,
                                                                         .bit_rate = 100000}};
struct spi_pl022_dev_t SPI2_PL022_DEV_S = {.cfg = &(SPI2_PL022_DEV_CFG_S), .data = &(SPI2_PL022_DEV_DATA_S)};
#endif

#ifdef SPI2_PL022_NS
static const struct spi_pl022_dev_cfg_t SPI2_PL022_DEV_CFG_NS = {
    .base = FPGA_SPI_SHIELD1_BASE_NS,
    .default_ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                         .frame_format = SPI_PL022_CFG_FRF_MOT,
                         .word_size = 8,
                         .bit_rate = 100000}};
static struct spi_pl022_dev_data_t SPI2_PL022_DEV_DATA_NS = {.state = 0,
                                                             .sys_clk = 0,
                                                             .ctrl_cfg = {.spi_mode = SPI_PL022_MASTER_SELECT,
                                                                          .frame_format = SPI_PL022_CFG_FRF_MOT,
                                                                          .word_size = 8,
                                                                          .bit_rate = 100000}};
struct spi_pl022_dev_t SPI2_PL022_DEV_NS = {.cfg = &(SPI2_PL022_DEV_CFG_NS), .data = &(SPI2_PL022_DEV_DATA_NS)};
#endif

/* I2C_SBCon driver structures */
#ifdef I2C0_SBCON_S
static struct i2c_sbcon_dev_cfg_t I2C0_SBCON_DEV_CFG_S = {
    .base = FPGA_SBCon_I2C_AUDIO_BASE_S, .default_freq_hz = 100000, .sleep_us = &wait_us};
static struct i2c_sbcon_dev_data_t I2C0_SBCON_DEV_DATA_S = {.freq_us = 0, .sys_clk = 0, .state = 0};
struct i2c_sbcon_dev_t I2C0_SBCON_DEV_S = {.cfg = &(I2C0_SBCON_DEV_CFG_S), .data = &(I2C0_SBCON_DEV_DATA_S)};
#endif

#ifdef I2C0_SBCON_NS
static struct i2c_sbcon_dev_cfg_t I2C0_SBCON_DEV_CFG_NS = {
    .base = FPGA_SBCon_I2C_AUDIO_BASE_NS, .default_freq_hz = 100000, .sleep_us = &wait_us};
static struct i2c_sbcon_dev_data_t I2C0_SBCON_DEV_DATA_NS = {.freq_us = 0, .sys_clk = 0, .state = 0};
struct i2c_sbcon_dev_t I2C0_SBCON_DEV_NS = {.cfg = &(I2C0_SBCON_DEV_CFG_NS), .data = &(I2C0_SBCON_DEV_DATA_NS)};
#endif

#ifdef I2C1_SBCON_S
static struct i2c_sbcon_dev_cfg_t I2C1_SBCON_DEV_CFG_S = {
    .base = FPGA_SBCon_I2C_SHIELD0_BASE_S, .default_freq_hz = 100000, .sleep_us = &wait_us};
static struct i2c_sbcon_dev_data_t I2C1_SBCON_DEV_DATA_S = {.freq_us = 0, .sys_clk = 0, .state = 0};
struct i2c_sbcon_dev_t I2C1_SBCON_DEV_S = {.cfg = &(I2C1_SBCON_DEV_CFG_S), .data = &(I2C1_SBCON_DEV_DATA_S)};
#endif

#ifdef I2C1_SBCON_NS
static struct i2c_sbcon_dev_cfg_t I2C1_SBCON_DEV_CFG_NS = {
    .base = FPGA_SBCon_I2C_SHIELD0_BASE_NS, .default_freq_hz = 100000, .sleep_us = &wait_us};
static struct i2c_sbcon_dev_data_t I2C1_SBCON_DEV_DATA_NS = {.freq_us = 0, .sys_clk = 0, .state = 0};
struct i2c_sbcon_dev_t I2C1_SBCON_DEV_NS = {.cfg = &(I2C1_SBCON_DEV_CFG_NS), .data = &(I2C1_SBCON_DEV_DATA_NS)};
#endif

#ifdef I2C2_SBCON_S
static struct i2c_sbcon_dev_cfg_t I2C2_SBCON_DEV_CFG_S = {
    .base = FPGA_SBCon_I2C_SHIELD1_BASE_S, .default_freq_hz = 100000, .sleep_us = &wait_us};
static struct i2c_sbcon_dev_data_t I2C2_SBCON_DEV_DATA_S = {.freq_us = 0, .sys_clk = 0, .state = 0};
struct i2c_sbcon_dev_t I2C2_SBCON_DEV_S = {.cfg = &(I2C2_SBCON_DEV_CFG_S), .data = &(I2C2_SBCON_DEV_DATA_S)};
#endif

#ifdef I2C2_SBCON_NS
static struct i2c_sbcon_dev_cfg_t I2C2_SBCON_DEV_CFG_NS = {
    .base = FPGA_SBCon_I2C_SHIELD1_BASE_NS, .default_freq_hz = 100000, .sleep_us = &wait_us};
static struct i2c_sbcon_dev_data_t I2C2_SBCON_DEV_DATA_NS = {.freq_us = 0, .sys_clk = 0, .state = 0};
struct i2c_sbcon_dev_t I2C2_SBCON_DEV_NS = {.cfg = &(I2C2_SBCON_DEV_CFG_NS), .data = &(I2C2_SBCON_DEV_DATA_NS)};
#endif

/* I2S driver structures */
#ifdef MPS3_I2S_S
static const struct audio_i2s_mps3_dev_cfg_t MPS3_I2S_DEV_CFG_S = {.base = FPGA_I2S_BASE_S};
struct audio_i2s_mps3_dev_t MPS3_I2S_DEV_S = {
    &(MPS3_I2S_DEV_CFG_S),
};
#endif

#ifdef MPS3_I2S_NS
static const struct audio_i2s_mps3_dev_cfg_t MPS3_I2S_DEV_CFG_NS = {.base = FPGA_I2S_BASE_NS};
struct audio_i2s_mps3_dev_t MPS3_I2S_DEV_NS = {
    &(MPS3_I2S_DEV_CFG_NS),
};
#endif

/* TGU driver structures */
#ifdef TGU_ARMV8_M_ITCM_S
static const struct tgu_armv8_m_dev_cfg_t TGU_ARMV8_M_ITCM_DEV_CFG_S = {.base = ITGU_CTRL_BASE};
static struct tgu_armv8_m_dev_data_t TGU_ARMV8_M_ITCM_DEV_DATA_S = {
    .range_list = 0, .nbr_of_ranges = 0, .is_initialized = false};
struct tgu_armv8_m_dev_t TGU_ARMV8_M_ITCM_DEV_S = {
    &(TGU_ARMV8_M_ITCM_DEV_CFG_S),
    &(TGU_ARMV8_M_ITCM_DEV_DATA_S),
};
#endif

#ifdef TGU_ARMV8_M_DTCM_S
static const struct tgu_armv8_m_dev_cfg_t TGU_ARMV8_M_DTCM_DEV_CFG_S = {.base = DTGU_CTRL_BASE};
static struct tgu_armv8_m_dev_data_t TGU_ARMV8_M_DTCM_DEV_DATA_S = {
    .range_list = 0,
    .nbr_of_ranges = 0,
    .is_initialized = false,
};
struct tgu_armv8_m_dev_t TGU_ARMV8_M_DTCM_DEV_S = {
    &(TGU_ARMV8_M_DTCM_DEV_CFG_S),
    &(TGU_ARMV8_M_DTCM_DEV_DATA_S),
};
#endif

/* Color LCD driver structures */
#ifdef MPS3_CLCD_S
static const struct clcd_mps3_dev_cfg_t MPS3_CLCD_DEV_CFG_S = {.base = CLCD_Config_Reg_BASE_S};
struct clcd_mps3_dev_t MPS3_CLCD_DEV_S = {
    &(MPS3_CLCD_DEV_CFG_S),
};
#endif

#ifdef MPS3_CLCD_NS
static const struct clcd_mps3_dev_cfg_t MPS3_CLCD_DEV_CFG_NS = {.base = CLCD_Config_Reg_BASE_NS};
struct clcd_mps3_dev_t MPS3_CLCD_DEV_NS = {
    &(MPS3_CLCD_DEV_CFG_NS),
};
#endif

/* RTC driver structures */
#ifdef RTC_PL031_S
static const struct rtc_pl031_dev_cfg_t RTC_PL031_DEV_CFG_S = {.base = RTC_BASE_S};
struct rtc_pl031_dev_t RTC_PL031_DEV_S = {.cfg = &(RTC_PL031_DEV_CFG_S)};
#endif

#ifdef RTC_PL031_NS
static const struct rtc_pl031_dev_cfg_t RTC_PL031_DEV_CFG_NS = {.base = RTC_BASE_NS};
struct rtc_pl031_dev_t RTC_PL031_DEV_NS = {.cfg = &(RTC_PL031_DEV_CFG_NS)};
#endif
