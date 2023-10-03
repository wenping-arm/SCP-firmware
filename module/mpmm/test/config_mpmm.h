/*
 * Arm SCP/MCP Software
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * config_template.h Template file
 */
#include <mpmm.h>

#include <mod_mpmm.h>

#include <fwk_macros.h>
#include <fwk_module_idx.h>

#define NUM_OF_MPMM_GEARS  1
#define DEFAULT_PERF_LIMIT 0xFFFFFFFF

enum fake_mpmm_element_idx {
    FAKE_MOD_MPMM_ELEM_0,
    FAKE_MOD_MPMM_ELEM_1,
    FAKE_MOD_MPMM_ELEM_COUNT,
};

enum fake_pct_table_idx {
    FAKE_PCT_TALBE_DEFAULT,
    FAKE_PCT_TABLE_NO_THRESHOLD_BITMAP,
    FAKE_PCT_TABLE_COUNT,
};

enum fake_mpmm_domain_idx {
    FAKE_MOD_MPMM_DOM_DEFAULT,
    FAKE_MOD_MPMM_DOM_MAX_THRESHOLD_COUNT,
    FAKE_MOD_MPMM_DOM_NUM_THRESHOLD_NOT_EQUAL,
    FAKE_MOD_MPMM_DOM_NO_PCT_ENTRY,
    FAKE_MOD_MPMM_DOM_COUNT,
};

static struct mpmm_reg fake_mpmm_reg = {
    .PPMCR = (NUM_OF_MPMM_GEARS << MPMM_PPMCR_NUM_GEARS_POS),
};

static const struct mod_mpmm_core_config fake_core_config = {
    .pd_id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_POWER_DOMAIN, 0),
    .mpmm_reg_base = (uintptr_t)&fake_mpmm_reg,
    .core_starts_online = true,
};

static struct mod_mpmm_pct_table fake_pct_table[FAKE_PCT_TABLE_COUNT] = {
    [FAKE_PCT_TALBE_DEFAULT] = {
        .cores_online = 1,
        .default_perf_limit = DEFAULT_PERF_LIMIT,
        .num_perf_limits = 2,
        .threshold_perf = { {
                              .threshold_bitmap = 0,
                              .perf_limit = 0,
                            },
                            {
                              .threshold_bitmap = 0,
                              .perf_limit = 0,
                            },
                          }
    },
    [FAKE_PCT_TABLE_NO_THRESHOLD_BITMAP] = {
        .cores_online = 1,
        .default_perf_limit = 1,
        .num_perf_limits = 1,
        .threshold_perf = { {
                              .threshold_bitmap = 0,
                              .perf_limit = 0,
                            },
                          }
    },
};

static struct mod_mpmm_domain_config fake_domain_conf[FAKE_MOD_MPMM_DOM_COUNT] = {
    [FAKE_MOD_MPMM_DOM_DEFAULT] = { .core_config = &fake_core_config,
                                    .pct = fake_pct_table,
                                    .pct_size = FWK_ARRAY_SIZE(fake_pct_table),
                                    .btc = 1,
                                    .num_threshold_counters = 1,
                                    .perf_id = FWK_ID_ELEMENT_INIT(
                                        FWK_MODULE_IDX_DVFS,
                                        0) },
    [FAKE_MOD_MPMM_DOM_MAX_THRESHOLD_COUNT] = { .core_config =
                                                    &fake_core_config,
                                                .pct = fake_pct_table,
                                                .pct_size = FWK_ARRAY_SIZE(
                                                    fake_pct_table),
                                                .btc = 0,
                                                .num_threshold_counters =
                                                    (MPMM_MAX_THRESHOLD_COUNT +
                                                     1),
                                                .perf_id = FWK_ID_ELEMENT_INIT(
                                                    FWK_MODULE_IDX_DVFS,
                                                    0) },
    [FAKE_MOD_MPMM_DOM_NUM_THRESHOLD_NOT_EQUAL] = { .core_config =
                                                        &fake_core_config,
                                                    .pct = fake_pct_table,
                                                    .pct_size = FWK_ARRAY_SIZE(
                                                        fake_pct_table),
                                                    .btc = 0,
                                                    .num_threshold_counters =
                                                        (NUM_OF_MPMM_GEARS + 1),
                                                    .perf_id =
                                                        FWK_ID_ELEMENT_INIT(
                                                            FWK_MODULE_IDX_DVFS,
                                                            0) },
    [FAKE_MOD_MPMM_DOM_NO_PCT_ENTRY] = { .core_config = &fake_core_config,
                                         .pct = NULL,
                                         .pct_size = 0,
                                         .btc = 0,
                                         .num_threshold_counters =
                                             (NUM_OF_MPMM_GEARS + 1),
                                         .perf_id = FWK_ID_ELEMENT_INIT(
                                             FWK_MODULE_IDX_DVFS,
                                             0) },
};
