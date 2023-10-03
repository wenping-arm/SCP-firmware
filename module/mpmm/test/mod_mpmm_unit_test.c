/*
 * Arm SCP/MCP Software
 * Copyright (c) 2023, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "scp_unity.h"
#include "unity.h"

#include <Mockfwk_core.h>
#include <Mockfwk_id.h>
#include <Mockfwk_module.h>
#include <Mockfwk_notification.h>
#include <config_mpmm.h>

#include <internal/Mockfwk_core_internal.h>

#include <mod_power_domain.h>
#include <mod_scmi_perf.h>

#include <fwk_element.h>
#include <fwk_macros.h>
#include <fwk_module_idx.h>
#include <fwk_notification.h>

#include UNIT_TEST_SRC

int fake_plugin_set_limits(struct plugin_limits_req *data)
{
    return FWK_SUCCESS;
}

struct perf_plugins_handler_api handler_api = {
    .plugin_set_limits = fake_plugin_set_limits,
};

uint32_t adj_max_limit;
struct mod_mpmm_domain_ctx dev_ctx_table[1];
struct mod_mpmm_core_ctx core_ctx_table;
struct perf_plugins_perf_update perf_update = {
    .domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0),
    .adj_max_limit = &adj_max_limit,
};
struct perf_plugins_perf_report fake_perf_report = { 0 };

void setUp(void)
{
    struct mod_mpmm_domain_ctx *domain_ctx;

    mpmm_ctx.mpmm_domain_count = 1;
    mpmm_ctx.domain_ctx = domain_ctx = &dev_ctx_table[0];
    mpmm_ctx.perf_plugins_handler_api = &handler_api;

    domain_ctx->domain_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);
    domain_ctx->num_cores = 1;
    domain_ctx->num_cores_online = 0;
    domain_ctx->perf_limit = 1;
    domain_ctx->wait_for_perf_transition = true;
    domain_ctx->core_ctx[0] = core_ctx_table;
    domain_ctx->domain_config = &fake_domain_conf[FAKE_MOD_MPMM_DOM_DEFAULT];
}

void tearDown(void)
{
}

void utest_mpmm_start_func_mod_id_success(void)
{
    int status;

    fwk_module_is_valid_module_id_ExpectAndReturn(fwk_module_id_mpmm, true);

    status = mpmm_start(fwk_module_id_mpmm);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_start_func_notif_subscribe_success(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_module_is_valid_module_id_ExpectAndReturn(elem_id, false);
    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_notification_subscribe_IgnoreAndReturn(FWK_SUCCESS);

    status = mpmm_start(elem_id);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

int fwk_notif_sub_cback_fail_subscribe_pre_transition(
    fwk_id_t notification_id,
    fwk_id_t source_id,
    fwk_id_t target_id,
    int cmock_num_calls)
{
    if (cmock_num_calls == 0) {
        return FWK_E_STATE;
    }

    return FWK_SUCCESS;
}

int fwk_notif_sub_cback_fail_subscribe_post_transition(
    fwk_id_t notification_id,
    fwk_id_t source_id,
    fwk_id_t target_id,
    int cmock_num_calls)
{
    if (cmock_num_calls == 2) {
        return FWK_E_STATE;
    }

    return FWK_SUCCESS;
}

void utest_mpmm_start_func_notif_subscribe_pre_transition_fail(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_module_is_valid_module_id_ExpectAndReturn(elem_id, false);
    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_notification_subscribe_Stub(
        fwk_notif_sub_cback_fail_subscribe_pre_transition);

    status = mpmm_start(elem_id);
    TEST_ASSERT_EQUAL(status, FWK_E_STATE);
}

void utest_mpmm_start_func_notif_subscribe_post_transition_fail(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_module_is_valid_module_id_ExpectAndReturn(elem_id, false);
    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_notification_subscribe_Stub(
        fwk_notif_sub_cback_fail_subscribe_post_transition);

    status = mpmm_start(elem_id);
    TEST_ASSERT_EQUAL(status, FWK_E_STATE);
}

void utest_mpmm_init_func_success(void)
{
    int status;

    status = mpmm_init(fwk_module_id_mpmm, FAKE_MOD_MPMM_ELEM_1, NULL);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_init_func_element_count_fail(void)
{
    int status;

    status = mpmm_init(fwk_module_id_mpmm, FAKE_MOD_MPMM_ELEM_0, NULL);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mpmm_element_init_func_success(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_build_sub_element_id_IgnoreAndReturn(elem_id);

    status = mpmm_element_init(
        elem_id,
        FAKE_MOD_MPMM_ELEM_1,
        &fake_domain_conf[FAKE_MOD_MPMM_DOM_DEFAULT]);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_element_init_func_element_count_0_fail(void)
{
    int status;

    status = mpmm_element_init(fwk_module_id_mpmm, FAKE_MOD_MPMM_ELEM_0, NULL);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mpmm_element_init_func_element_count_max_fail(void)
{
    int status;

    status = mpmm_element_init(
        fwk_module_id_mpmm, (MPMM_MAX_NUM_CORES_IN_DOMAIN + 1), NULL);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mpmm_element_init_func_max_threshold_count_fail(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);

    status = mpmm_element_init(
        elem_id,
        FAKE_MOD_MPMM_ELEM_1,
        &fake_domain_conf[FAKE_MOD_MPMM_DOM_MAX_THRESHOLD_COUNT]);
    TEST_ASSERT_EQUAL(status, FWK_E_SUPPORT);
}

void utest_mpmm_element_init_func_threshold_count_not_equal(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_build_sub_element_id_IgnoreAndReturn(elem_id);

    status = mpmm_element_init(
        elem_id,
        FAKE_MOD_MPMM_ELEM_1,
        &fake_domain_conf[FAKE_MOD_MPMM_DOM_NUM_THRESHOLD_NOT_EQUAL]);
    TEST_ASSERT_EQUAL(status, FWK_E_DEVICE);
}

void utest_mpmm_bind_func_round_success(void)
{
    int status;
    unsigned int round = 0;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    status = mpmm_bind(elem_id, round);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_bind_func_module_id_not_valid(void)
{
    int status;
    unsigned int round = 1;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_module_is_valid_module_id_ExpectAndReturn(elem_id, false);

    status = mpmm_bind(elem_id, round);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_bind_func_module_bind_success(void)
{
    int status;
    unsigned int round = 1;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_module_is_valid_module_id_ExpectAndReturn(elem_id, true);
    fwk_module_bind_ExpectAnyArgsAndReturn(FWK_SUCCESS);

    status = mpmm_bind(elem_id, round);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_process_bind_request_func_success(void)
{
    int status;
    const void *api = NULL;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);

    status = mpmm_process_bind_request(elem_id, elem_id, elem_id, &api);
    TEST_ASSERT_NOT_EQUAL(api, NULL);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_process_bind_request_func_id_not_equal(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_is_equal_ExpectAnyArgsAndReturn(false);

    status = mpmm_process_bind_request(elem_id, elem_id, elem_id, NULL);
    TEST_ASSERT_EQUAL(status, FWK_E_ACCESS);
}

void utest_mpmm_process_notification_func_new_level_success(void)
{
    int status;
    struct mod_pd_power_state_pre_transition_notification_params *params;
    struct fwk_event resp_event = { 0 };
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);
    struct fwk_event event = {
        .target_id = elem_id,
    };

    dev_ctx_table[0].current_perf_level =
        fake_pct_table[FAKE_PCT_TALBE_DEFAULT].threshold_perf->perf_limit + 1;
    params = (struct mod_pd_power_state_pre_transition_notification_params *)
                 event.params;
    params->target_state = MOD_PD_STATE_ON;

    fwk_module_is_valid_element_id_ExpectAndReturn(elem_id, true);
    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_IgnoreAndReturn(true);

    status = mpmm_process_notification(&event, &resp_event);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].core_ctx[0].pd_blocked, true);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].wait_for_perf_transition, true);
    TEST_ASSERT_EQUAL(resp_event.is_delayed_response, true);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].core_ctx[0].cookie, event.cookie);
}

void utest_mpmm_process_notification_func_transition_success(void)
{
    int status;
    struct fwk_event event = { 0 };
    struct fwk_event resp_event = { 0 };
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    event.target_id = elem_id;

    fwk_module_is_valid_element_id_ExpectAndReturn(elem_id, true);
    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(false);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);

    status = mpmm_process_notification(&event, &resp_event);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].core_ctx[0].online, false);
}

void utest_mpmm_process_notification_func_core_idx_larger(void)
{
    int status;
    struct fwk_event event = { 0 };
    struct fwk_event resp_event = { 0 };
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    event.target_id = elem_id;

    fwk_module_is_valid_element_id_ExpectAndReturn(elem_id, true);
    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(false);

    status = mpmm_process_notification(&event, &resp_event);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mpmm_process_notification_func_perf_limit(void)
{
    int status;
    struct mod_pd_power_state_pre_transition_notification_params *params;
    struct fwk_event resp_event = { 0 };
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);
    struct fwk_event event = {
        .target_id = elem_id,
    };

    dev_ctx_table[0].current_perf_level =
        fake_pct_table[FAKE_PCT_TALBE_DEFAULT].threshold_perf->perf_limit;

    params = (struct mod_pd_power_state_pre_transition_notification_params *)
                 event.params;
    params->target_state = MOD_PD_STATE_ON;

    fwk_module_is_valid_element_id_ExpectAndReturn(elem_id, true);
    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_IgnoreAndReturn(true);

    status = mpmm_process_notification(&event, &resp_event);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].core_ctx[0].pd_blocked, false);
}

void utest_mpmm_update_func_success(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    mpmm_ctx.domain_ctx[0].num_cores_online = 1;

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);

    status = mpmm_update(&perf_update);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    TEST_ASSERT_EQUAL(
        *perf_update.adj_max_limit, mpmm_ctx.domain_ctx[0].perf_limit);
}

void utest_mpmm_update_func_domain_idx(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(false);

    status = mpmm_update(&perf_update);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mpmm_update_func_num_cores_online(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);

    status = mpmm_update(&perf_update);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_update_func_perf_transition(void)
{
    int status;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    mpmm_ctx.domain_ctx[0].num_cores_online = 1;
    mpmm_ctx.domain_ctx[0].current_perf_level =
        mpmm_ctx.domain_ctx[0].perf_limit + 1;
    mpmm_ctx.domain_ctx[0].wait_for_perf_transition = false;

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, 0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);

    status = mpmm_update(&perf_update);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].wait_for_perf_transition, true);
}

void utest_mpmm_report_func_success(void)
{
    int status;

    fwk_id_get_element_idx_ExpectAnyArgsAndReturn(0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);

    status = mpmm_report(&fake_perf_report);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
    TEST_ASSERT_EQUAL(
        mpmm_ctx.domain_ctx[0].current_perf_level, fake_perf_report.level);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].wait_for_perf_transition, false);
}

void utest_mpmm_report_func_domain_idx(void)
{
    int status;

    fwk_id_get_element_idx_ExpectAnyArgsAndReturn(0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(false);

    status = mpmm_report(&fake_perf_report);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
}

void utest_mpmm_report_func_num_perf_transition(void)
{
    int status;

    fwk_id_get_element_idx_ExpectAnyArgsAndReturn(0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);

    mpmm_ctx.domain_ctx[0].wait_for_perf_transition = false;

    status = mpmm_report(&fake_perf_report);
    TEST_ASSERT_EQUAL(status, FWK_SUCCESS);
}

void utest_mpmm_report_func_delayed_resp(void)
{
    int status;

    fwk_id_get_element_idx_ExpectAnyArgsAndReturn(0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);
    fwk_get_delayed_response_ExpectAnyArgsAndReturn(FWK_E_PARAM);

    mpmm_ctx.domain_ctx[0].core_ctx[0].pd_blocked = true;

    status = mpmm_report(&fake_perf_report);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].core_ctx[0].pd_blocked, false);
}

void utest_mpmm_report_func_put_event(void)
{
    int status;

    fwk_id_get_element_idx_ExpectAnyArgsAndReturn(0);
    fwk_id_is_equal_ExpectAnyArgsAndReturn(true);
    fwk_get_delayed_response_ExpectAnyArgsAndReturn(FWK_SUCCESS);
    __fwk_put_event_ExpectAnyArgsAndReturn(FWK_E_PARAM);

    mpmm_ctx.domain_ctx[0].core_ctx[0].pd_blocked = true;

    status = mpmm_report(&fake_perf_report);
    TEST_ASSERT_EQUAL(status, FWK_E_PARAM);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx[0].core_ctx[0].pd_blocked, false);
}

void utest_mpmm_core_evaluate_threshold_func_success(void)
{
    uint32_t cached_counters;
    uint32_t delta;
    struct mpmm_reg mpmm = { .MPMMCR = MPMM_MPMMCR_EN_MASK };
    struct amu_reg amu_aux;

    core_ctx_table.mpmm = &mpmm;
    core_ctx_table.amu_aux = &amu_aux;
    core_ctx_table.cached_counters = &cached_counters;
    core_ctx_table.delta = &delta;
    core_ctx_table.threshold = 0xFFFFFFFF;

    mpmm_core_evaluate_threshold(&dev_ctx_table[0], &core_ctx_table);
    TEST_ASSERT_NOT_EQUAL(core_ctx_table.threshold, 0xFFFFFFFF);
}

void utest_mpmm_core_evaluate_threshold_func_counter_not_enabled(void)
{
    struct mpmm_reg mpmm;

    core_ctx_table.mpmm = &mpmm;

    mpmm_core_evaluate_threshold(&dev_ctx_table[0], &core_ctx_table);
    TEST_ASSERT_EQUAL(
        core_ctx_table.threshold,
        dev_ctx_table[0].domain_config->num_threshold_counters);
}

void utest_mpmm_domain_set_thresholds_func_success(void)
{
    struct mpmm_reg mpmm = { 0 };

    dev_ctx_table[0].core_ctx[0].mpmm = &mpmm;
    dev_ctx_table[0].core_ctx[0].online = 1;

    mpmm_domain_set_thresholds(&dev_ctx_table[0]);
    TEST_ASSERT_EQUAL(
        dev_ctx_table[0].core_ctx[0].mpmm->MPMMCR, (1 << MPMM_MPMMCR_GEAR_POS));
}

void utest_get_domain_ctx_func_null(void)
{
    struct mod_mpmm_domain_ctx *domain_ctx;
    fwk_id_t elem_id = FWK_ID_ELEMENT(FWK_MODULE_IDX_MPMM, 0);

    fwk_id_get_element_idx_ExpectAndReturn(elem_id, mpmm_ctx.mpmm_domain_count);

    domain_ctx = get_domain_ctx(elem_id);
    TEST_ASSERT_EQUAL(domain_ctx, NULL);
}

void utest_mpmm_core_threshold_policy_func_highest_gear(void)
{
    uint32_t gear;
    uint32_t delta = 0;

    core_ctx_table.delta = &delta;

    gear = mpmm_core_threshold_policy(&dev_ctx_table[0], &core_ctx_table);
    TEST_ASSERT_EQUAL(
        gear, (dev_ctx_table[0].domain_config->num_threshold_counters - 1));
}

void utest_find_perf_limit_from_pct_func_default_limit(void)
{
    uint32_t perf_limit;
    uint32_t threshold_map =
        (fake_pct_table[FAKE_PCT_TABLE_NO_THRESHOLD_BITMAP]
             .threshold_perf->threshold_bitmap +
         1);

    perf_limit = find_perf_limit_from_pct(
        &fake_pct_table[FAKE_PCT_TABLE_NO_THRESHOLD_BITMAP], threshold_map);
    TEST_ASSERT_EQUAL(
        perf_limit,
        fake_pct_table[FAKE_PCT_TABLE_NO_THRESHOLD_BITMAP].default_perf_limit);
}

void utest_mpmm_evaluate_perf_limit_func_no_entry(void)
{
    uint32_t perf_limit;

    mpmm_ctx.domain_ctx[0].num_cores_online = 2;

    perf_limit = mpmm_evaluate_perf_limit(mpmm_ctx.domain_ctx);
    TEST_ASSERT_EQUAL(
        perf_limit,
        fake_pct_table[FAKE_PCT_TALBE_DEFAULT].threshold_perf->perf_limit);
}

void utest_mpmm_monitor_and_control_func_no_cores_online(void)
{
    uint32_t prev_perf_limit = mpmm_ctx.domain_ctx->perf_limit;

    mpmm_ctx.domain_ctx[0].num_cores_online = 0;

    mpmm_monitor_and_control(mpmm_ctx.domain_ctx);
    TEST_ASSERT_EQUAL(mpmm_ctx.domain_ctx->perf_limit, prev_perf_limit);
}

int mod_mpmm_test_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(utest_mpmm_start_func_mod_id_success);
    RUN_TEST(utest_mpmm_start_func_notif_subscribe_success);
    RUN_TEST(utest_mpmm_start_func_notif_subscribe_pre_transition_fail);
    RUN_TEST(utest_mpmm_start_func_notif_subscribe_post_transition_fail);

    RUN_TEST(utest_mpmm_init_func_success);
    RUN_TEST(utest_mpmm_init_func_element_count_fail);

    RUN_TEST(utest_mpmm_element_init_func_success);
    RUN_TEST(utest_mpmm_element_init_func_element_count_0_fail);
    RUN_TEST(utest_mpmm_element_init_func_element_count_max_fail);
    RUN_TEST(utest_mpmm_element_init_func_max_threshold_count_fail);
    RUN_TEST(utest_mpmm_element_init_func_threshold_count_not_equal);

    RUN_TEST(utest_mpmm_bind_func_round_success);
    RUN_TEST(utest_mpmm_bind_func_module_id_not_valid);
    RUN_TEST(utest_mpmm_bind_func_module_bind_success);

    RUN_TEST(utest_mpmm_process_bind_request_func_success);
    RUN_TEST(utest_mpmm_process_bind_request_func_id_not_equal);

    RUN_TEST(utest_mpmm_process_notification_func_new_level_success);
    RUN_TEST(utest_mpmm_process_notification_func_transition_success);
    RUN_TEST(utest_mpmm_process_notification_func_core_idx_larger);
    RUN_TEST(utest_mpmm_process_notification_func_perf_limit);

    RUN_TEST(utest_mpmm_update_func_success);
    RUN_TEST(utest_mpmm_update_func_domain_idx);
    RUN_TEST(utest_mpmm_update_func_num_cores_online);
    RUN_TEST(utest_mpmm_update_func_perf_transition);

    RUN_TEST(utest_mpmm_report_func_success);
    RUN_TEST(utest_mpmm_report_func_domain_idx);
    RUN_TEST(utest_mpmm_report_func_num_perf_transition);
    RUN_TEST(utest_mpmm_report_func_delayed_resp);
    RUN_TEST(utest_mpmm_report_func_put_event);

    RUN_TEST(utest_mpmm_core_evaluate_threshold_func_success);
    RUN_TEST(utest_mpmm_core_evaluate_threshold_func_counter_not_enabled);

    RUN_TEST(utest_mpmm_domain_set_thresholds_func_success);

    RUN_TEST(utest_get_domain_ctx_func_null);

    RUN_TEST(utest_mpmm_core_threshold_policy_func_highest_gear);

    RUN_TEST(utest_find_perf_limit_from_pct_func_default_limit);

    RUN_TEST(utest_mpmm_evaluate_perf_limit_func_no_entry);

    RUN_TEST(utest_mpmm_monitor_and_control_func_no_cores_online);

    return UNITY_END();
}

#if !defined(TEST_ON_TARGET)
int main(void)
{
    return mod_mpmm_test_main();
}
#endif
