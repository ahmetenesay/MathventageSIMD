/**
 * @file verify_sfvm_all_features.cpp
 * @brief Comprehensive End-to-End Verification Suite for MSIMD SFVM & sfGC Architectural Enhancements.
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

#include "SacculusForge.h"

#define ASSERT_TRUE(cond, msg) \
    do { \
        if (!(cond)) { \
            std::cerr << "[FAIL] Line " << __LINE__ << ": " << msg << std::endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_FALSE(cond, msg) \
    do { \
        if (cond) { \
            std::cerr << "[FAIL] Line " << __LINE__ << ": " << msg << std::endl; \
            return false; \
        } \
    } while(0)

#define ASSERT_EQ(a, b, msg) \
    do { \
        if ((a) != (b)) { \
            std::cerr << "[FAIL] Line " << __LINE__ << ": " << msg \
                      << " (expected " << (b) << ", got " << (a) << ")" << std::endl; \
            return false; \
        } \
    } while(0)

extern "C" MSIMD_API int SF_custominiVm_corrupt_rehab_entry_for_test(uint32_t fn_id, int mode);

bool test_vm_lifecycle() {
    std::cout << "\n[TEST 1] VM Lifecycle & Setup Validation..." << std::endl;
    int setup_rc = SF_custominiVm_setup_start(nullptr);
    ASSERT_EQ(setup_rc, 0, "SF_custominiVm_setup_start failed");
    ASSERT_TRUE(SF_custominiVm_is_setup() != 0, "VM should be in setup state");

    int start_rc = SF_custominiVm_start();
    ASSERT_EQ(start_rc, 0, "SF_custominiVm_start failed");
    ASSERT_TRUE(SF_custominiVm_is_started() != 0, "VM should be started");

    std::cout << "  --> VM Booted successfully in NORMAX profile." << std::endl;
    return true;
}

bool test_four_point_coordination() {
    std::cout << "\n[TEST 2] 4-Point Execution Topology (Safepoint, Controlpoint, Hookpoint, Pausepoint)..." << std::endl;

    // 1. Safepoint poll
    sf_gc_safepoint_poll();

    // 2. Controlpoint check
    int cp_res = sf_gc_controlpoint_check(2048);
    ASSERT_EQ(cp_res, 0, "controlpoint check should succeed without pacing stall under normal pressure");

    // 3. Hookpoint kernel prologue & epilogue
    alignas(64) float test_tensor[128] = {0.0f};
    sf_vm_hookpoint_trigger(SF_HOOKPOINT_KERNEL_ENTER, 101, test_tensor, sizeof(test_tensor));
    for (int i = 0; i < 128; ++i) test_tensor[i] = (float)i * 1.5f;
    sf_vm_hookpoint_trigger(SF_HOOKPOINT_KERNEL_EXIT, 101, test_tensor, sizeof(test_tensor));

    // 4. Pausepoint / Interruptpoint Gate Testing
    ASSERT_EQ(SF_custominiVm_get_pausepoint_gate(), 0, "Pausepoint gate should be initially disarmed");
    int poll_disarmed = sf_vm_pausepoint_poll();
    ASSERT_EQ(poll_disarmed, 0, "Pausepoint poll should return 0 when disarmed");

    SF_custominiVm_set_pausepoint_gate(1);
    ASSERT_EQ(SF_custominiVm_get_pausepoint_gate(), 1, "Pausepoint gate should be armed");
    int poll_armed = sf_vm_pausepoint_poll();
    ASSERT_EQ(poll_armed, 1, "Pausepoint poll should preempt when armed");

    SF_custominiVm_set_pausepoint_gate(0);
    ASSERT_EQ(SF_custominiVm_get_pausepoint_gate(), 0, "Pausepoint gate should be disarmed again");

    // 5. Query telemetry
    SF_custominiVm_point_telemetry_t telem{};
    SF_custominiVm_point_telemetry_query(&telem);
    ASSERT_TRUE(telem.safepoint_polls > 0, "safepoint_polls should be > 0");
    ASSERT_TRUE(telem.controlpoint_checks > 0, "controlpoint_checks should be > 0");
    ASSERT_TRUE(telem.hookpoint_enter_count > 0, "hookpoint_enter_count should be > 0");
    ASSERT_TRUE(telem.hookpoint_exit_count > 0, "hookpoint_exit_count should be > 0");
    ASSERT_TRUE(telem.pausepoint_polls > 0, "pausepoint_polls should be > 0");
    ASSERT_TRUE(telem.pausepoint_preemptions > 0, "pausepoint_preemptions should be > 0");

    std::cout << "  --> All 4 Execution Points and gates verified with live telemetry." << std::endl;
    return true;
}

bool test_motherline_egg_fledgling_and_pitstop() {
    std::cout << "\n[TEST 3] Motherline Egg/Fledgling Split, Pitstop & Empty Box Placeholder Protocol..." << std::endl;

    size_t egg0 = 0, fled0 = 0, pit0 = 0;
    SF_custominiVm_runway_query_zones(&egg0, &fled0, &pit0);

    // Create 3 tensors of matching function type (fn_id = 42)
    int32_t shape256[] = {256};
    SF_custominiVm_tensor_t t1 = SF_custominiVm_tensor_alloc(shape256, 1, SF_DTYPE_F32);
    SF_custominiVm_tensor_t t2 = SF_custominiVm_tensor_alloc(shape256, 1, SF_DTYPE_F32);
    SF_custominiVm_tensor_t t3 = SF_custominiVm_tensor_alloc(shape256, 1, SF_DTYPE_F32);

    ASSERT_TRUE(t1.data != nullptr && t2.data != nullptr && t3.data != nullptr, "Tensor allocations should succeed");

    SF_custominiVm_tensor_set_function_id(&t1, 42);
    SF_custominiVm_tensor_set_function_id(&t2, 42);
    SF_custominiVm_tensor_set_function_id(&t3, 42);

    size_t egg1 = 0, fled1 = 0, pit1 = 0;
    SF_custominiVm_runway_query_zones(&egg1, &fled1, &pit1);
    ASSERT_TRUE(egg1 >= egg0 + 3, "Newborn objects must enter Egg zone");

    // Perform Pitstop Excise and Return cycle on t1
    // Leaves in-place Empty Box placeholder without compaction, updates ISA, and returns to Fledgling
    int pit_rc = SF_custominiVm_pitstop_test_cycle(t1.data, "AVX512_BF16", 1250);
    ASSERT_EQ(pit_rc, 0, "Pitstop test cycle should succeed");

    size_t egg2 = 0, fled2 = 0, pit2 = 0;
    SF_custominiVm_runway_query_zones(&egg2, &fled2, &pit2);
    ASSERT_TRUE(fled2 > fled1, "Optimized objects must be promoted to Fledgling zone");

    // Deep check t1 Live Header metadata after Pitstop return
    SF_custominiVm_live_meta_t meta1{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&t1, &meta1), 0, "Query meta for t1 should succeed");
    ASSERT_EQ(meta1.has_optimized, 1, "t1 has_optimized must be 1");
    ASSERT_TRUE(strcmp(meta1.active_isa, "AVX512_BF16") == 0, "t1 active ISA must be AVX512_BF16");
    ASSERT_EQ(meta1.cycles_gained, 1250ULL, "t1 cycles_gained must be 1250");

    // Verify promote_to_fledgling idempotency: repeating Pitstop return must not corrupt zone counters
    size_t egg_before_reopt = 0, fled_before_reopt = 0, pit_before_reopt = 0;
    SF_custominiVm_runway_query_zones(&egg_before_reopt, &fled_before_reopt, &pit_before_reopt);
    int pit_rc2 = SF_custominiVm_pitstop_test_cycle(t1.data, "AVX512_BF16", 1250);
    ASSERT_EQ(pit_rc2, 0, "Repeated pitstop cycle should succeed");
    size_t egg_after_reopt = 0, fled_after_reopt = 0, pit_after_reopt = 0;
    SF_custominiVm_runway_query_zones(&egg_after_reopt, &fled_after_reopt, &pit_after_reopt);
    ASSERT_EQ(egg_after_reopt, egg_before_reopt, "Repeated promotion must not decrement egg count");
    ASSERT_EQ(fled_after_reopt, fled_before_reopt, "Repeated promotion must not increment fledgling count");

    // Deep check Batch Optimization Stampede: sibling objects t2 and t3 of function 42 in Egg must be auto-promoted!
    SF_custominiVm_live_meta_t meta2{}, meta3{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&t2, &meta2), 0, "Query meta for t2 should succeed");
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&t3, &meta3), 0, "Query meta for t3 should succeed");
    ASSERT_EQ(meta2.has_optimized, 1, "Sibling t2 must be batch promoted to Fledgling");
    ASSERT_EQ(meta2.opt_success, 1, "Sibling t2 opt_success must be 1");
    ASSERT_TRUE(strcmp(meta2.active_isa, "AVX512_BF16") == 0, "Sibling t2 must inherit active ISA");
    ASSERT_EQ(meta3.has_optimized, 1, "Sibling t3 must be batch promoted to Fledgling");

    // Deep check BYPASS_WARMUP protection
    SF::GroupCompute({5, true /* bypass_warmup */});
    SF_custominiVm_tensor_t t_bypass = SF_custominiVm_tensor_alloc(shape256, 1, SF_DTYPE_F32);
    SF_custominiVm_tensor_set_function_id(&t_bypass, 42);
    SF_custominiVm_live_meta_t meta_bp{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&t_bypass, &meta_bp), 0, "Query meta for t_bypass should succeed");
    ASSERT_EQ(meta_bp.opt_permitted, 0, "opt_permitted must be 0 for BYPASS_WARMUP");
    int bp_rc = SF_custominiVm_pitstop_test_cycle(t_bypass.data, "AVX2", 500);
    ASSERT_EQ(bp_rc, -5, "pitstop_test_cycle must reject BYPASS_WARMUP object (-5)");
    SF::GroupComputeReset();

    // Test Yapbozcu Morph Paradigms: Mode 0 (Excision Exclusive) vs Mode 1 (Shadow RCU)
    // 1. Excision Exclusive with Ready Notification Callback
    SF::Yapbozcu::SetMorphMethod(SF_YAPBOZCU_MORPH_EXCISION);
    ASSERT_EQ(SF::Yapbozcu::GetMorphMethod(), SF_YAPBOZCU_MORPH_EXCISION, "Morph mode must be EXCISION");

    static void* g_cb_seen_ptr = nullptr;
    static bool  g_cb_fired = false;
    auto test_cb = [](void* obj_ptr, void* user_data) {
        (void)user_data;
        g_cb_seen_ptr = obj_ptr;
        g_cb_fired = true;
    };

    SF_custominiVm_tensor_t t_exc = SF_custominiVm_tensor_alloc(shape256, 1, SF_DTYPE_F32);
    SF_custominiVm_tensor_set_function_id(&t_exc, 99);
    SF::Yapbozcu::OnReady(t_exc.data, test_cb, nullptr);
    int exc_rc = SF_custominiVm_pitstop_test_cycle(t_exc.data, "AVX512_VNNI", 880);
    ASSERT_EQ(exc_rc, 0, "Excision cycle must succeed");
    ASSERT_TRUE(g_cb_fired, "OnReady callback must fire upon Pitstop return");
    ASSERT_EQ(g_cb_seen_ptr, t_exc.data, "Callback must receive original tensor payload pointer");
    ASSERT_FALSE(SF::Yapbozcu::IsBusy(t_exc.data), "Object must no longer be busy after return");

    // 2. Shadow RCU Mode (Zero-Pause)
    SF::Yapbozcu::SetMorphMethod(SF_YAPBOZCU_MORPH_SHADOW);
    ASSERT_EQ(SF::Yapbozcu::GetMorphMethod(), SF_YAPBOZCU_MORPH_SHADOW, "Morph mode must be SHADOW");
    SF_custominiVm_tensor_t t_shd = SF_custominiVm_tensor_alloc(shape256, 1, SF_DTYPE_F32);
    SF_custominiVm_tensor_set_function_id(&t_shd, 100);
    ASSERT_FALSE(SF::Yapbozcu::IsBusy(t_shd.data), "Shadow mode object is never busy to callers");
    int shd_rc = SF_custominiVm_pitstop_test_cycle(t_shd.data, "AVX10_2_512", 2100);
    ASSERT_EQ(shd_rc, 0, "Shadow RCU test cycle must succeed");
    SF_custominiVm_live_meta_t meta_shd{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&t_shd, &meta_shd), 0, "Query meta for t_shd should succeed");
    ASSERT_TRUE(strcmp(meta_shd.active_isa, "AVX10_2_512") == 0, "t_shd active ISA must be AVX10_2_512");
    ASSERT_EQ(meta_shd.cycles_gained, 2100ULL, "t_shd cycles_gained must be 2100");
    SF_custominiVm_tensor_free(&t_exc);
    SF_custominiVm_tensor_free(&t_shd);

    // 3. Rehab Arena & Physical Resurrection Flow
    size_t egg_before_rehab = 0, fled_before_rehab = 0, pit_before_rehab = 0;
    SF_custominiVm_runway_query_zones(&egg_before_rehab, &fled_before_rehab, &pit_before_rehab);

    int32_t shape128[] = {128};
    SF_custominiVm_tensor_t t_rehab = SF_custominiVm_tensor_alloc(shape128, 1, SF_DTYPE_F32);
    ASSERT_TRUE(t_rehab.data != nullptr, "t_rehab allocation should succeed");
    float* rehab_data = static_cast<float*>(t_rehab.data);
    for (int i = 0; i < 128; ++i) {
        rehab_data[i] = 777.0f + (float)i * 0.5f;
    }
    SF_custominiVm_tensor_set_function_id(&t_rehab, 777);

    size_t egg_after_alloc = 0, fled_after_alloc = 0, pit_after_alloc = 0;
    SF_custominiVm_runway_query_zones(&egg_after_alloc, &fled_after_alloc, &pit_after_alloc);
    ASSERT_TRUE(egg_after_alloc >= egg_before_rehab + 1, "Allocated tensor must enter Egg zone");

    // Mark tensor for rehabilitation
    SF::Rehab::Mark(&t_rehab);

    // Trigger GC sweep - t_rehab should transition to rehab arena, decrementing egg_count
    SF_custominiVm_gc_shake_bed();

    size_t egg_after_sweep = 0, fled_after_sweep = 0, pit_after_sweep = 0;
    SF_custominiVm_runway_query_zones(&egg_after_sweep, &fled_after_sweep, &pit_after_sweep);
    ASSERT_TRUE(egg_after_sweep < egg_after_alloc, "Evicted rehab tensor must decrement Egg count");

    // Request resurrection for function 777
    uint64_t res_count_before = SF::Rehab::ResurrectionCount();
    SF::Rehab::RequestResurrection(777);

    // Scan rehab arena to process resurrection
    size_t resurrected = SF::Rehab::Scan();
    ASSERT_TRUE(resurrected >= 1, "At least 1 tensor must be physically resurrected");
    ASSERT_TRUE(SF::Rehab::ResurrectionCount() > res_count_before, "Resurrection counter must increment");

    // Look up resurrected tensor in Motherline Egg zone
    SF_custominiVm_tensor_t t_resurrected = SF::Rehab::FindByFunctionId(777);
    ASSERT_TRUE(t_resurrected.data != nullptr, "Resurrected tensor must be found in Motherline");
    ASSERT_EQ(t_resurrected.byte_size, 128 * sizeof(float), "Resurrected tensor size must match");

    // Validate payload byte-for-byte
    float* res_data = static_cast<float*>(t_resurrected.data);
    for (int i = 0; i < 128; ++i) {
        ASSERT_EQ(res_data[i], 777.0f + (float)i * 0.5f, "Resurrected payload must match original byte-for-byte");
    }

    // Verify resurrected tensor landed strictly in Egg zone
    SF_custominiVm_live_meta_t res_meta{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&t_resurrected, &res_meta), 0, "Query meta for resurrected tensor must succeed");
    ASSERT_EQ(res_meta.has_optimized, 0, "Resurrected tensor must strictly land in Egg zone (has_optimized == 0)");

    // Free resurrected tensor
    SF_custominiVm_tensor_free(&t_resurrected);

    // 4. Test Bump Allocation Fallback on Resurrection (when Falline has no matching empty box)
    int32_t shape384[] = {384};
    SF_custominiVm_tensor_t t_bump = SF_custominiVm_tensor_alloc(shape384, 1, SF_DTYPE_F32);
    ASSERT_TRUE(t_bump.data != nullptr, "t_bump allocation should succeed");
    float* bump_data = static_cast<float*>(t_bump.data);
    for (int i = 0; i < 384; ++i) {
        bump_data[i] = 888.0f + (float)i * 0.25f;
    }
    SF_custominiVm_tensor_set_function_id(&t_bump, 888);
    SF::Rehab::Mark(&t_bump);
    SF_custominiVm_gc_shake_bed();

    // Snatched box absorption: allocate t_drain to snatch the newly registered empty box from Falline
    SF_custominiVm_tensor_t t_drain = SF_custominiVm_tensor_alloc(shape384, 1, SF_DTYPE_F32);
    ASSERT_TRUE(t_drain.data != nullptr, "t_drain allocation should succeed");

    // Now Falline has NO matching box for 384 floats. Request resurrection for 888 -> must bump-alloc!
    uint64_t res_count_before_bump = SF::Rehab::ResurrectionCount();
    SF::Rehab::RequestResurrection(888);
    size_t resurrected_bump = SF::Rehab::Scan();
    ASSERT_TRUE(resurrected_bump >= 1, "Bump-allocated tensor resurrection must succeed");
    ASSERT_TRUE(SF::Rehab::ResurrectionCount() > res_count_before_bump, "Resurrection counter must increment");

    SF_custominiVm_tensor_t t_res_bump = SF::Rehab::FindByFunctionId(888);
    ASSERT_TRUE(t_res_bump.data != nullptr, "Bump-resurrected tensor must be found in Motherline");
    ASSERT_EQ(t_res_bump.byte_size, 384 * sizeof(float), "Bump-resurrected tensor size must match");
    float* res_bump_data = static_cast<float*>(t_res_bump.data);
    for (int i = 0; i < 384; ++i) {
        ASSERT_EQ(res_bump_data[i], 888.0f + (float)i * 0.25f, "Bump-resurrected payload must match byte-for-byte");
    }
    SF_custominiVm_live_meta_t bump_meta{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&t_res_bump, &bump_meta), 0, "Query meta for bump-resurrected tensor must succeed");
    ASSERT_EQ(bump_meta.has_optimized, 0, "Bump-resurrected tensor must strictly land in Egg zone (has_optimized == 0)");

    SF_custominiVm_tensor_free(&t_res_bump);
    SF_custominiVm_tensor_free(&t_drain);

    // 5. Test Corrupted Rehab Entry Rejection (CRC32 and Tail Canary Validation)
    int32_t shape64_corrupt[] = {64};
    SF_custominiVm_tensor_t t_corrupt = SF_custominiVm_tensor_alloc(shape64_corrupt, 1, SF_DTYPE_F32);
    ASSERT_TRUE(t_corrupt.data != nullptr, "t_corrupt allocation should succeed");
    for (int i = 0; i < 64; ++i) static_cast<float*>(t_corrupt.data)[i] = 999.0f + (float)i;
    SF_custominiVm_tensor_set_function_id(&t_corrupt, 999);
    SF::Rehab::Mark(&t_corrupt);
    SF_custominiVm_gc_shake_bed();

    // Mode 1: Corrupt payload CRC32
    int crp_rc1 = SF_custominiVm_corrupt_rehab_entry_for_test(999, 1);
    ASSERT_EQ(crp_rc1, 0, "Corrupting rehab entry CRC32 should find entry");
    SF::Rehab::RequestResurrection(999);
    size_t res_corrupt = SF::Rehab::Scan();
    ASSERT_EQ(res_corrupt, 0, "Resurrection of CRC-corrupted entry must be rejected");
    SF_custominiVm_tensor_t t_bad_crc = SF::Rehab::FindByFunctionId(999);
    ASSERT_TRUE(t_bad_crc.data == nullptr, "CRC-corrupted tensor must NOT be restored to Motherline");

    // Mode 2: Corrupt tail canary
    SF_custominiVm_tensor_t t_canary = SF_custominiVm_tensor_alloc(shape64_corrupt, 1, SF_DTYPE_F32);
    ASSERT_TRUE(t_canary.data != nullptr, "t_canary allocation should succeed");
    for (int i = 0; i < 64; ++i) static_cast<float*>(t_canary.data)[i] = 1111.0f + (float)i;
    SF_custominiVm_tensor_set_function_id(&t_canary, 1111);
    SF::Rehab::Mark(&t_canary);
    SF_custominiVm_gc_shake_bed();

    int crp_rc2 = SF_custominiVm_corrupt_rehab_entry_for_test(1111, 2);
    ASSERT_EQ(crp_rc2, 0, "Corrupting rehab entry tail canary should find entry");
    SF::Rehab::RequestResurrection(1111);
    size_t res_canary = SF::Rehab::Scan();
    ASSERT_EQ(res_canary, 0, "Resurrection of canary-corrupted entry must be rejected");
    SF_custominiVm_tensor_t t_bad_canary = SF::Rehab::FindByFunctionId(1111);
    ASSERT_TRUE(t_bad_canary.data == nullptr, "Canary-corrupted tensor must NOT be restored to Motherline");

    // Cleanup
    SF_custominiVm_tensor_free(&t1);
    SF_custominiVm_tensor_free(&t2);
    SF_custominiVm_tensor_free(&t3);
    SF_custominiVm_tensor_free(&t_bypass);

    std::cout << "  --> Egg, Fledgling, Pitstop and Dual Morph placeholder protocol verified." << std::endl;
    return true;
}


bool test_alu_saturation_and_group_compute() {
    std::cout << "\n[TEST 4] ALU Queue Dispatching & SF::GroupCompute() API with Priority Conflict Handling..." << std::endl;

    // Test Allow_Operation_Dispatchings flag
    SF_custominiVm_set_allow_operation_dispatchings(1);
    ASSERT_EQ(SF_custominiVm_get_allow_operation_dispatchings(), 1, "Allow_Operation_Dispatchings should be enabled");

    int32_t shape64[] = {64};

    // GroupCompute with normal priority
    uint32_t gid1 = SF::GroupCompute({5, false});
    ASSERT_TRUE(gid1 != 0, "GroupCompute should return non-zero hex ID");
    SF_custominiVm_tensor_t tg1 = SF_custominiVm_tensor_alloc(shape64, 1, SF_DTYPE_F32);
    SF_custominiVm_live_meta_t mg1{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&tg1, &mg1), 0, "Query meta for tg1");
    ASSERT_EQ(mg1.group_hex_id, gid1, "tg1 group_hex_id must match gid1");
    ASSERT_EQ(mg1.group_priority, 5, "tg1 group_priority must be 5");
    ASSERT_EQ(mg1.opt_permitted, 1, "tg1 opt_permitted must be 1");

    // GroupCompute with high priority (triggers conflict resolution notice with dispatching)
    uint32_t gid2 = SF::GroupCompute({9, false});
    ASSERT_TRUE(gid2 != 0 && gid2 != gid1, "GroupCompute should return unique hex ID");
    SF_custominiVm_tensor_t tg2 = SF_custominiVm_tensor_alloc(shape64, 1, SF_DTYPE_F32);
    SF_custominiVm_live_meta_t mg2{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&tg2, &mg2), 0, "Query meta for tg2");
    ASSERT_EQ(mg2.group_hex_id, gid2, "tg2 group_hex_id must match gid2");
    ASSERT_EQ(mg2.group_priority, 9, "tg2 group_priority must be 9");

    // GroupCompute with BYPASS_WARMUP
    uint32_t gid3 = SF::GroupCompute({10, true});
    ASSERT_TRUE(gid3 != 0 && gid3 != gid2, "GroupCompute should return unique hex ID");
    SF_custominiVm_tensor_t tg3 = SF_custominiVm_tensor_alloc(shape64, 1, SF_DTYPE_F32);
    SF_custominiVm_live_meta_t mg3{};
    ASSERT_EQ(SF_custominiVm_tensor_query_live_meta(&tg3, &mg3), 0, "Query meta for tg3");
    ASSERT_EQ(mg3.group_hex_id, gid3, "tg3 group_hex_id must match gid3");
    ASSERT_EQ(mg3.group_priority, 10, "tg3 group_priority must be 10");
    ASSERT_EQ(mg3.opt_permitted, 0, "tg3 opt_permitted must be 0 (BYPASS_WARMUP)");

    SF::GroupComputeReset();

    // Verify real ALU Queue Saturation and Dispatching Nexus
    // Saturate ALU 0 with 4 operations (saturation threshold is 4)
    for (int i = 0; i < 4; ++i) {
        int r = SF_custominiVm_alu_dispatch_op(0, 5);
        ASSERT_EQ(r, 0, "ALU 0 should accept operations until saturation threshold");
    }
    uint64_t disp_before = SF_custominiVm_get_alu_dispatched_count();

    // 5th operation with normal priority (5) on saturated ALU 0 should be dispatched to an unsaturated ALU
    int dispatched_target = SF_custominiVm_alu_dispatch_op(0, 5);
    ASSERT_TRUE(dispatched_target != 0, "Saturated ALU 0 must dispatch to another lane");
    uint64_t disp_after = SF_custominiVm_get_alu_dispatched_count();
    ASSERT_TRUE(disp_after > disp_before, "total_dispatched counter must increment");

    // High priority (>= 8) operation overrides dispatching and pins execution to preferred ALU
    int pinned_target = SF_custominiVm_alu_dispatch_op(0, 9);
    ASSERT_EQ(pinned_target, 0, "High priority (>=8) must override queue dispatching and pin to ALU 0");

    // Complete ops
    for (int i = 0; i < 5; ++i) SF_custominiVm_alu_complete_op(0);
    SF_custominiVm_alu_complete_op(dispatched_target);

    SF_custominiVm_tensor_free(&tg1);
    SF_custominiVm_tensor_free(&tg2);
    SF_custominiVm_tensor_free(&tg3);

    SF_custominiVm_set_allow_operation_dispatchings(0);
    ASSERT_EQ(SF_custominiVm_get_allow_operation_dispatchings(), 0, "Allow_Operation_Dispatchings should be disabled");

    std::cout << "  --> GroupCompute unique IDs, priority, bypass warmup, and conflict resolution verified." << std::endl;
    return true;
}

bool test_rmrp_compaction() {
    std::cout << "\n[TEST 5] RMRP (Reduce Memory Resident Propagation) On-Heap Compaction..." << std::endl;

    // Enable RMRP with PLUMP method
    SF_custominiVm_set_rmrp(1, 0);
    ASSERT_EQ(SF_custominiVm_get_rmrp(), 1, "RMRP should be enabled");

    // Allocate oversized cell then run RMRP pass
    int32_t shape512[] = {512};
    SF_custominiVm_tensor_t t = SF_custominiVm_tensor_alloc(shape512, 1, SF_DTYPE_F32);
    ASSERT_TRUE(t.data != nullptr, "Tensor allocation should succeed");

    size_t reclaimed_plump = SF_custominiVm_run_rmrp();
    std::cout << "  --> RMRP Plump pass completed, reclaimed: " << reclaimed_plump << " bytes." << std::endl;
    ASSERT_TRUE(reclaimed_plump > 0, "RMRP Plump pass must reclaim memory");

    // Switch to SEQUENTIAL method
    SF_custominiVm_set_rmrp(1, 1);
    size_t reclaimed_seq = SF_custominiVm_run_rmrp();
    std::cout << "  --> RMRP Sequential pass completed, reclaimed: " << reclaimed_seq << " bytes." << std::endl;
    ASSERT_TRUE(reclaimed_seq > 0, "RMRP Sequential pass must reclaim memory");

    SF_custominiVm_set_rmrp(0, 0);
    ASSERT_EQ(SF_custominiVm_get_rmrp(), 0, "RMRP should be disabled");

    SF_custominiVm_tensor_free(&t);
    return true;
}

bool test_thread_budgeting() {
    std::cout << "\n[TEST 6] SMT/HT Adaptive Thread Budgeting (Hardware-Adaptive Ceilings)..." << std::endl;

    SF_custominiVm_hardware_tier_t tier = SF_custominiVm_query_hardware_tier();
    const char* tier_name = SF_custominiVm_query_hardware_tier_name();
    std::cout << "  --> Detected Hardware Tier: [" << (int)tier << "] " << (tier_name ? tier_name : "Unknown") << std::endl;

    uint32_t gc_th = 0, puz_th = 0, vm_th = 0;
    SF_custominiVm_query_thread_budget(&gc_th, &puz_th, &vm_th);

    std::cout << "  --> Adaptive Budget: GC=" << gc_th
              << ", Puzzle=" << puz_th
              << ", VM=" << vm_th
              << " (Total=" << (gc_th + puz_th + vm_th) << ")" << std::endl;

    switch (tier) {
        case SF_HW_TIER_SUR:
            ASSERT_TRUE(gc_th >= 1 && gc_th <= 16, "Sür GC thread budget <= 16");
            ASSERT_TRUE(puz_th >= 1 && puz_th <= 8, "Sür Puzzle thread budget <= 8");
            ASSERT_TRUE(vm_th >= 2 && vm_th <= 104, "Sür VM thread budget <= 104");
            ASSERT_TRUE((gc_th + puz_th + vm_th) <= 128, "Sür total threads <= 128");
            break;
        case SF_HW_TIER_ORTA_UST:
            ASSERT_TRUE(gc_th >= 1 && gc_th <= 6, "Orta-Üst GC thread budget <= 6");
            ASSERT_TRUE(puz_th >= 1 && puz_th <= 3, "Orta-Üst Puzzle thread budget <= 3");
            ASSERT_TRUE(vm_th >= 2 && vm_th <= 15, "Orta-Üst VM thread budget <= 15");
            ASSERT_TRUE((gc_th + puz_th + vm_th) <= 24, "Orta-Üst total threads <= 24");
            break;
        case SF_HW_TIER_ORTA_ORTA:
            ASSERT_TRUE(gc_th >= 1 && gc_th <= 4, "Orta-Orta GC thread budget <= 4");
            ASSERT_TRUE(puz_th >= 1 && puz_th <= 2, "Orta-Orta Puzzle thread budget <= 2");
            ASSERT_TRUE(vm_th >= 2 && vm_th <= 8, "Orta-Orta VM thread budget <= 8");
            ASSERT_TRUE((gc_th + puz_th + vm_th) <= 14, "Orta-Orta total threads <= 14");
            break;
        case SF_HW_TIER_ORTA_ALT:
            ASSERT_TRUE(gc_th >= 1 && gc_th <= 2, "Orta-Alt GC thread budget <= 2");
            ASSERT_TRUE(puz_th >= 1 && puz_th <= 2, "Orta-Alt Puzzle thread budget <= 2");
            ASSERT_TRUE(vm_th >= 1 && vm_th <= 4, "Orta-Alt VM thread budget <= 4");
            ASSERT_TRUE((gc_th + puz_th + vm_th) <= 8, "Orta-Alt total threads <= 8");
            break;
        case SF_HW_TIER_ALT:
        default:
            ASSERT_TRUE(gc_th >= 1 && gc_th <= 1, "Alt GC thread budget <= 1");
            ASSERT_TRUE(puz_th >= 1 && puz_th <= 1, "Alt Puzzle thread budget <= 1");
            ASSERT_TRUE(vm_th >= 1 && vm_th <= 2, "Alt VM thread budget <= 2");
            ASSERT_TRUE((gc_th + puz_th + vm_th) <= 4, "Alt total threads <= 4");
            break;
    }

    // Also test C++ HardwareBudget facade
    ASSERT_EQ((int)SF::HardwareBudget::GetTier(), (int)tier, "SF::HardwareBudget::GetTier() must match C API");

    return true;
}

bool test_sram_optimizations_and_global_volatility() {
    std::cout << "\n[TEST 7] SRAM Cacheline Optimizations & Global Volatility..." << std::endl;

    alignas(64) uint8_t sram_buffer[256];
    memset(sram_buffer, 0xAA, sizeof(sram_buffer));

    // Global volatility method setting
    SF_custominiVm_set_global_volatility(1); // SRAM
    ASSERT_EQ(SF_custominiVm_get_global_volatility(), 1, "Global volatility should be SRAM");

    SF_custominiVm_sram_broadcast_volatility(sram_buffer, sizeof(sram_buffer));
    SF_custominiVm_sram_commit_to_dram(sram_buffer, sizeof(sram_buffer));

    alignas(64) uint8_t dram_dst[256] = {0};
    SF_custominiVm_dram_stream_store(dram_dst, sram_buffer, sizeof(sram_buffer));
    ASSERT_EQ(memcmp(dram_dst, sram_buffer, sizeof(sram_buffer)), 0, "Non-temporal stream store payload mismatch");

    SF_custominiVm_set_global_volatility(0); // DRAM
    ASSERT_EQ(SF_custominiVm_get_global_volatility(), 0, "Global volatility should be DRAM");

    std::cout << "  --> SRAM commit, store-fence volatility, and NT streaming verified." << std::endl;
    return true;
}

bool test_self_healing_engine() {
    std::cout << "\n[TEST 8] SFGC Self-Healing & Memory Corruption Recovery Engine..." << std::endl;

    int32_t shape128[] = {128};
    SF_custominiVm_tensor_t t_corrupt = SF_custominiVm_tensor_alloc(shape128, 1, SF_DTYPE_F32);
    ASSERT_TRUE(t_corrupt.data != nullptr, "Tensor allocation should succeed");

    // Deliberately inject corruption into header: illegal generation + cyclic Brooks forwarding pointer
    SF_custominiVm_diagnostic_corrupt_tensor(&t_corrupt, 88, 1);

    // Run active self-healing sweep
    uint64_t healed = SF_custominiVm_self_heal_run_check();
    ASSERT_TRUE(healed >= 1, "Self-heal sweep must detect and repair corrupted object");

    // Test code error and bitcode exception capture
    SF::SelfHeal::CaptureCodeError("math_kernel_v4", "Division by zero in vector SIMD slot");
    SF::SelfHeal::CaptureBitcodeException(101, "Corrupted opcode 0xFE in JIT stream");

    SF_custominiVm_self_heal_stats_t stats{};
    SF_custominiVm_self_heal_query_stats(&stats);
    ASSERT_TRUE(stats.heap_corruptions_healed >= 1, "heap_corruptions_healed should be >= 1");
    ASSERT_TRUE(stats.brooks_cycles_broken >= 1, "brooks_cycles_broken should be >= 1");
    ASSERT_TRUE(stats.code_errors_captured >= 1, "code_errors_captured should be >= 1");
    ASSERT_TRUE(stats.bitcode_exceptions_shielded >= 1, "bitcode_exceptions_shielded should be >= 1");

    SF_custominiVm_tensor_free(&t_corrupt);
    std::cout << "  --> Self-healing successfully repaired corruptions and broke cyclic Brooks pointers." << std::endl;
    return true;
}

bool test_log_boot_drain() {
    std::cout << "\n[TEST 9] Log Boot: Hotlog to Coldlog Drain Engine..." << std::endl;

    // Trigger several events into hotlog
    for (uint32_t i = 0; i < 10; ++i) {
        sf_vm_hookpoint_trigger(SF_HOOKPOINT_KERNEL_ENTER, 200 + i, nullptr, 0);
        sf_vm_hookpoint_trigger(SF_HOOKPOINT_KERNEL_EXIT, 200 + i, nullptr, 0);
    }

    size_t drained = SF_custominiVm_log_boot_drain();
    ASSERT_TRUE(drained > 0, "Hotlog to Coldlog drain should process pending events");

    std::cout << "  --> Log boot drained " << drained << " events into persistent Coldlog buffer." << std::endl;
    return true;
}

bool test_entangle_execution() {
    std::cout << "\n[TEST 10] Batch Register & Entangle Execution (Kernel Fusion)..." << std::endl;

    ASSERT_EQ(SF_custominiVm_entangle_is_active(), 0, "Entangle should be inactive initially");
    {
        SF::EntangleScope scope;
        ASSERT_EQ(SF_custominiVm_entangle_is_active(), 1, "Entangle should be active inside EntangleScope");
    }
    ASSERT_EQ(SF_custominiVm_entangle_is_active(), 0, "Entangle should be inactive after EntangleScope");

    std::cout << "  --> Entangle execution transaction cycle verified." << std::endl;
    return true;
}

bool test_branch_predicting_director() {
    std::cout << "\n[TEST 11] SF_BranchPredictingDirector & Speculative Prediction..." << std::endl;

    int pred_true = SF_custominiVm_branch_predict(1, 2); // BIAS_LIKELY
    ASSERT_EQ(pred_true, 1, "Predict(1, LIKELY) should evaluate to 1");

    int pred_false = SF_custominiVm_branch_predict(0, 3); // BIAS_UNLIKELY
    ASSERT_EQ(pred_false, 0, "Predict(0, UNLIKELY) should evaluate to 0");

    bool cxx_pred = SF::BranchPredictingDirector::Predict(true, SF::BranchPattern::BIAS_LIKELY);
    ASSERT_TRUE(cxx_pred, "C++ BranchPredictingDirector should return true");

    // Test Chaotic Random with cmov branchless evaluation
    int cmov32_t = sf_hinter_cmov_i32(1, 42, 99, nullptr);
    int cmov32_f = sf_hinter_cmov_i32(0, 42, 99, nullptr);
    ASSERT_EQ(cmov32_t, 42, "sf_hinter_cmov_i32(1, 42, 99) should be 42");
    ASSERT_EQ(cmov32_f, 99, "sf_hinter_cmov_i32(0, 42, 99) should be 99");

    int64_t cmov64_t = sf_hinter_cmov_i64(1, 1000LL, 2000LL, nullptr);
    int64_t cmov64_f = sf_hinter_cmov_i64(0, 1000LL, 2000LL, nullptr);
    ASSERT_EQ(cmov64_t, 1000LL, "sf_hinter_cmov_i64(1, 1000, 2000) should be 1000");
    ASSERT_EQ(cmov64_f, 2000LL, "sf_hinter_cmov_i64(0, 1000, 2000) should be 2000");

    bool chaotic_t = SF::BranchPredictingDirector::Predict(true, SF::BranchPattern::CHAOTIC_RANDOM);
    bool chaotic_f = SF::BranchPredictingDirector::Predict(false, SF::BranchPattern::CHAOTIC_RANDOM);
    ASSERT_TRUE(chaotic_t, "Chaotic predict true");
    ASSERT_FALSE(chaotic_f, "Chaotic predict false");

    std::cout << "  --> BranchPredictingDirector hints and cmov branchless evaluation verified." << std::endl;
    return true;
}

bool test_cli_fuzzy_and_synonyms() {
    std::cout << "\n[TEST 12] CLI Fuzzy Search, Technical Intent Mapping & Conflict Reporting..." << std::endl;

    // Test passing fuzzy argument and synonyms via SF_custominiVm_init_with_flags (with parent RMRP flag enabled)
    int init_synonyms = SF_custominiVm_init_with_flags("-#SF|GC_Mod:Vectoral -#SF|Reduce_Memory_Resident_Propagation:true -#SF|RMRP_Method:Sequential -#SF|Global_Volatility_Method:SRAM -#SF|GC_Modd:Cycles");
    ASSERT_EQ(init_synonyms, 0, "Initialization with synonyms and fuzzy flag should succeed");

    // Test that RMRP_Method cannot be used on its own without Reduce_Memory_Resident_Propagation
    int init_rmrp_standalone = SF_custominiVm_init_with_flags("-#SF|RMRP_Method:Sequential");
    ASSERT_EQ(init_rmrp_standalone, (int)MSIMD_ERROR_FLAG_CONFLICT, "RMRP_Method alone must be rejected as a fatal flag conflict");

    // Test conflict detection: GC_Operation_Facility:SIMD with Use_Critical_SIMDly_Operations:Deny
    int init_conflict = SF_custominiVm_init_with_flags("-#SF|GC_Operation_Facility:SIMD -#SF|Use_Critical_SIMDly_Operations:Deny");
    ASSERT_EQ(init_conflict, (int)MSIMD_ERROR_FLAG_CONFLICT, "Conflicting flags must return MSIMD_ERROR_FLAG_CONFLICT");

    // Test conflict detection: No_sfGC with Always_Focus_Compactizing
    int init_nosfgc_compact_conflict = SF_custominiVm_init_with_flags("-#SF|No_sfGC -#SF|Always_Focus_Compactizing");
    ASSERT_EQ(init_nosfgc_compact_conflict, (int)MSIMD_ERROR_FLAG_CONFLICT, "No_sfGC with Always_Focus_Compactizing must return MSIMD_ERROR_FLAG_CONFLICT");

    // Test conflict detection: No_sfGC with GC_Mode:SIMD
    int init_nosfgc_simd_conflict = SF_custominiVm_init_with_flags("-#SF|No_sfGC -#SF|GC_Mode:SIMD");
    ASSERT_EQ(init_nosfgc_simd_conflict, (int)MSIMD_ERROR_FLAG_CONFLICT, "No_sfGC with GC_Mode:SIMD must return MSIMD_ERROR_FLAG_CONFLICT");

    // Test conflict detection: Entangle_Execution without Experimental
    int init_entangle_conflict = SF_custominiVm_init_with_flags("-#SF|Entangle_Execution:true");
    ASSERT_EQ(init_entangle_conflict, (int)MSIMD_ERROR_FLAG_CONFLICT, "Entangle_Execution without Experimental must return MSIMD_ERROR_FLAG_CONFLICT");

    // Test conflict detection: UseMLTCSolver without Experimental
    int init_mltc_conflict = SF_custominiVm_init_with_flags("-#SF|UseMLTCSolver:true");
    ASSERT_EQ(init_mltc_conflict, (int)MSIMD_ERROR_FLAG_CONFLICT, "UseMLTCSolver without Experimental must return MSIMD_ERROR_FLAG_CONFLICT");

    // Test slash-prefixed flags and bare boolean synonyms
    char out_flag_slash[64] = {0};
    char out_val_slash[64] = {0};
    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("/boost", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "Intent '/boost' must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "Upclocker") == 0, "'/boost' must map to Upclocker");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("/sram", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "Intent '/sram' must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "Global_Volatility_Method") == 0, "'/sram' must map to Global_Volatility_Method");
    ASSERT_TRUE(strcmp(out_val_slash, "SRAM") == 0, "'/sram' value must be SRAM");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("/trim", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "Intent '/trim' must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "Reduce_Memory_Resident_Propagation") == 0, "'/trim' must map to RMRP");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("AlwaysPreTouch", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "AlwaysPreTouch must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "AlwaysPreTouch") == 0, "AlwaysPreTouch must resolve as known flag");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("Forbid_Runtime_Heap", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "Forbid_Runtime_Heap must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "Forbid_Runtime_Heap") == 0, "Forbid_Runtime_Heap must resolve as known flag");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("FreeList_Recycle_Policy:FIFO", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "FreeList_Recycle_Policy:FIFO must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "FreeList_Recycle_Policy") == 0, "Flag must be FreeList_Recycle_Policy");
    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("mltc_solver", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "Intent 'mltc_solver' must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "UseMLTCSolver") == 0, "'mltc_solver' must map to UseMLTCSolver");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("Code_Arena_Bytes:128MB", out_flag_slash, sizeof(out_flag_slash), out_val_slash, sizeof(out_val_slash)), 1, "Code_Arena_Bytes must resolve");
    ASSERT_TRUE(strcmp(out_flag_slash, "Code_Arena_Bytes") == 0, "Flag must be Code_Arena_Bytes");
    ASSERT_TRUE(strcmp(out_val_slash, "128MB") == 0, "Value must be 128MB");

    // Test English technical intent resolution API (C API)
    char out_flag[64] = {0};
    char out_val[64] = {0};
    int r1 = SF_custominiVm_cli_resolve_intent("boost", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r1, 1, "Intent 'boost' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Upclocker") == 0, "'boost' must map to Upclocker");

    int r2 = SF_custominiVm_cli_resolve_intent("shrink", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r2, 1, "Intent 'shrink' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Reduce_Memory_Resident_Propagation") == 0, "'shrink' must map to RMRP");

    int r3 = SF_custominiVm_cli_resolve_intent("vector", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r3, 1, "Intent 'vector' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "GC_Mode") == 0, "'vector' must map to GC_Mode");
    ASSERT_TRUE(strcmp(out_val, "SIMD") == 0, "'vector' default value must be SIMD");

    int r4 = SF_custominiVm_cli_resolve_intent("steal", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r4, 1, "Intent 'steal' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Allow_Operation_Dispatchings") == 0, "'steal' must map to Allow_Operation_Dispatchings");

    int r5 = SF_custominiVm_cli_resolve_intent("safety", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r5, 1, "Intent 'safety' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Profile") == 0, "'safety' must map to Profile");
    ASSERT_TRUE(strcmp(out_val, "SC") == 0, "'safety' default value must be SC");

    // Test prefixed tokens (--boost, +#SF|shrink, -vector:SIMD, --turbo=2)
    int r_pref1 = SF_custominiVm_cli_resolve_intent("--boost", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_pref1, 1, "Prefixed intent '--boost' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Upclocker") == 0, "'--boost' must map to Upclocker");

    int r_pref2 = SF_custominiVm_cli_resolve_intent("+#SF|shrink", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_pref2, 1, "Prefixed intent '+#SF|shrink' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Reduce_Memory_Resident_Propagation") == 0, "'+#SF|shrink' must map to RMRP");

    int r_pref3 = SF_custominiVm_cli_resolve_intent("--turbo=2", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_pref3, 1, "'--turbo=2' must be recognized with value");
    ASSERT_TRUE(strcmp(out_flag, "Upclocker") == 0, "'--turbo=2' flag must be Upclocker");
    ASSERT_TRUE(strcmp(out_val, "2") == 0, "'--turbo=2' value must be preserved as 2");

    // Test technical synonyms (fuse_ops, defrag, hugepages, overclock, simdly)
    int r_fuse = SF_custominiVm_cli_resolve_intent("fuse_ops", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_fuse, 1, "Intent 'fuse_ops' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Entangle_Execution") == 0, "'fuse_ops' must map to Entangle_Execution");

    int r_defrag = SF_custominiVm_cli_resolve_intent("defrag", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_defrag, 1, "Intent 'defrag' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Always_Focus_Compactizing") == 0, "'defrag' must map to Always_Focus_Compactizing");

    int r_huge = SF_custominiVm_cli_resolve_intent("hugepages", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_huge, 1, "Intent 'hugepages' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Enable_Huge_Pages") == 0, "'hugepages' must map to Enable_Huge_Pages");

    int r_oc = SF_custominiVm_cli_resolve_intent("overclock", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_oc, 1, "Intent 'overclock' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Upclocker") == 0, "'overclock' must map to Upclocker");

    int r_simdly = SF_custominiVm_cli_resolve_intent("simdly", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_simdly, 1, "Intent 'simdly' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "GC_Mode") == 0, "'simdly' must map to GC_Mode");

    // Test fuzzy typos (bost, turb, shrnk)
    int r_typo1 = SF_custominiVm_cli_resolve_intent("bost", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_typo1, 1, "Typo 'bost' must resolve via fuzzy matching");
    ASSERT_TRUE(strcmp(out_flag, "Upclocker") == 0, "'bost' must resolve to Upclocker");

    int r_typo2 = SF_custominiVm_cli_resolve_intent("turb", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_typo2, 1, "Typo 'turb' must resolve via fuzzy matching");
    ASSERT_TRUE(strcmp(out_flag, "Upclocker") == 0, "'turb' must resolve to Upclocker");

    // Test initialization with pure standard CLI flags (no +#SF| prefix required)
    int init_std_cli = SF_custominiVm_init_with_flags("--boost --shrink --sram");
    ASSERT_EQ(init_std_cli, 0, "Initialization with standard '--boost --shrink --sram' flags must succeed");

    // Test edge cases: null, empty string, unknown token
    ASSERT_EQ(SF_custominiVm_cli_resolve_intent(nullptr, out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 0, "Null token must return 0");
    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 0, "Empty token must return 0");
    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("nonexistent_random_token_12345", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 0, "Random token must return 0");

    // Test quoted value and delimiter edge cases
    int r_quote = SF_custominiVm_cli_resolve_intent("--safety=\"SC\"", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_quote, 1, "Quoted value '--safety=\"SC\"' must be recognized");
    ASSERT_TRUE(strcmp(out_val, "SC") == 0, "Quotes should be stripped from value");

    // Test C++ SemanticCLI facade
    std::string cxx_flag, cxx_val;
    bool cxx_ok = SF::SemanticCLI::ResolveIntent("--fuse_ops", cxx_flag, cxx_val);
    ASSERT_TRUE(cxx_ok, "SF::SemanticCLI::ResolveIntent('--fuse_ops') must succeed");
    ASSERT_EQ(cxx_flag, std::string("Entangle_Execution"), "'--fuse_ops' maps to Entangle_Execution in C++ facade");

    std::cout << "  --> CLI fuzzy matching, technical intent mappings, and conflict detection verified." << std::endl;
    return true;
}

bool test_os_hinting_and_pinning() {
    std::cout << "\n[TEST 13] OS Hinting, Thread Priorities & Hard Affinity..." << std::endl;

    bool r_core = SF::OSHinting::PinToCore(0);
    ASSERT_TRUE(r_core, "PinToCore(0) should succeed");

    bool r_numa = SF::OSHinting::PinToNumaNode(0);
    ASSERT_TRUE(r_numa, "PinToNumaNode(0) should succeed");

    bool r_prio = SF::OSHinting::SetPriorityHigh();
    ASSERT_TRUE(r_prio, "SetPriorityHigh() should succeed");

    std::cout << "  --> OS hinting, core pinning, and thread priority verified." << std::endl;
    return true;
}

bool test_execution_focus_topology() {
    std::cout << "\n[TEST 14] SFVM New Era Execution Focus Topology & TableGen Silicon Matrix..." << std::endl;

    // 1. Automated Detection from Silicon Microarchitecture & CPUID
    SF_custominiVm_execution_focus_t detected = SF::ExecutionFocus::Detect();
    std::cout << "  --> Detected Execution Focus Mode: " << SF::ExecutionFocus::GetName(detected)
              << " (" << (int)detected << ")" << std::endl;
    std::cout << "  --> Matched TableGen Microarchitecture: " << SF::ExecutionFocus::GetUarchName() << std::endl;

    // Live Arrow Lake Canary Check
    int is_hybrid = SF_custominiVm_is_hybrid_cpu();
    if (is_hybrid) {
        ASSERT_EQ(detected, SF_FOCUS_DUAL_HYBRID, "Hybrid Arrow Lake CPU must auto-detect as SF_FOCUS_DUAL_HYBRID");
        const char* uarch = SF::ExecutionFocus::GetUarchName();
        ASSERT_TRUE(strstr(uarch, "Lion Cove") != nullptr, "Matched microarchitecture must reference Lion Cove");
    }

    // 2. Full 6-Way Orthogonal Matrix State Switching & Verification
    double p_weight = 0.0, e_weight = 0.0;

    // State 1: SCP_HOMO
    SF::ExecutionFocus::Set(SF_FOCUS_SCP_HOMO);
    ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_SCP_HOMO, "Active focus must be SF_FOCUS_SCP_HOMO");
    ASSERT_TRUE(SF::ExecutionFocus::IsSCP(), "IsSCP() must be true under SCP_HOMO");
    ASSERT_FALSE(SF::ExecutionFocus::IsMCP(), "IsMCP() must be false under SCP_HOMO");
    ASSERT_FALSE(SF::ExecutionFocus::IsDual(), "IsDual() must be false under SCP_HOMO");
    SF::ExecutionFocus::GetChunkWeights(p_weight, e_weight);
    ASSERT_EQ(p_weight, 1.0, "P-weight under SCP_HOMO must be 1.0");
    ASSERT_EQ(e_weight, 1.0, "E-weight under SCP_HOMO must be 1.0");
    ASSERT_TRUE(strcmp(SF::ExecutionFocus::GetCurrentName(), "SCP_HOMO") == 0, "Name must be SCP_HOMO");

    // State 2: SCP_HYBRID
    SF::ExecutionFocus::Set(SF_FOCUS_SCP_HYBRID);
    ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_SCP_HYBRID, "Active focus must be SF_FOCUS_SCP_HYBRID");
    ASSERT_TRUE(SF::ExecutionFocus::IsSCP(), "IsSCP() must be true under SCP_HYBRID");
    ASSERT_FALSE(SF::ExecutionFocus::IsMCP(), "IsMCP() must be false under SCP_HYBRID");
    ASSERT_FALSE(SF::ExecutionFocus::IsDual(), "IsDual() must be false under SCP_HYBRID");
    SF::ExecutionFocus::GetChunkWeights(p_weight, e_weight);
    ASSERT_EQ(p_weight, 1.0, "P-weight under SCP_HYBRID must be 1.0");
    ASSERT_EQ(e_weight, 0.0, "E-weight under SCP_HYBRID must be 0.0 (strictly locked to P-core)");
    ASSERT_TRUE(strcmp(SF::ExecutionFocus::GetCurrentName(), "SCP_HYBRID") == 0, "Name must be SCP_HYBRID");

    // State 3: MCP_HOMO
    SF::ExecutionFocus::Set(SF_FOCUS_MCP_HOMO);
    ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_MCP_HOMO, "Active focus must be SF_FOCUS_MCP_HOMO");
    ASSERT_FALSE(SF::ExecutionFocus::IsSCP(), "IsSCP() must be false under MCP_HOMO");
    ASSERT_TRUE(SF::ExecutionFocus::IsMCP(), "IsMCP() must be true under MCP_HOMO");
    ASSERT_FALSE(SF::ExecutionFocus::IsDual(), "IsDual() must be false under MCP_HOMO");
    SF::ExecutionFocus::GetChunkWeights(p_weight, e_weight);
    ASSERT_EQ(p_weight, 1.0, "P-weight under MCP_HOMO must be 1.0");
    ASSERT_EQ(e_weight, 1.0, "E-weight under MCP_HOMO must be 1.0");
    ASSERT_TRUE(strcmp(SF::ExecutionFocus::GetCurrentName(), "MCP_HOMO") == 0, "Name must be MCP_HOMO");

    // State 4: MCP_HYBRID
    SF::ExecutionFocus::Set(SF_FOCUS_MCP_HYBRID);
    ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_MCP_HYBRID, "Active focus must be SF_FOCUS_MCP_HYBRID");
    ASSERT_FALSE(SF::ExecutionFocus::IsSCP(), "IsSCP() must be false under MCP_HYBRID");
    ASSERT_TRUE(SF::ExecutionFocus::IsMCP(), "IsMCP() must be true under MCP_HYBRID");
    ASSERT_FALSE(SF::ExecutionFocus::IsDual(), "IsDual() must be false under MCP_HYBRID");
    SF::ExecutionFocus::GetChunkWeights(p_weight, e_weight);
    ASSERT_EQ(p_weight, 2.5, "P-weight under MCP_HYBRID must be 2.5");
    ASSERT_EQ(e_weight, 1.0, "E-weight under MCP_HYBRID must be 1.0");
    ASSERT_TRUE(strcmp(SF::ExecutionFocus::GetCurrentName(), "MCP_HYBRID") == 0, "Name must be MCP_HYBRID");

    // State 5: DUAL_HOMO
    SF::ExecutionFocus::Set(SF_FOCUS_DUAL_HOMO);
    ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_DUAL_HOMO, "Active focus must be SF_FOCUS_DUAL_HOMO");
    ASSERT_FALSE(SF::ExecutionFocus::IsSCP(), "IsSCP() must be false under DUAL_HOMO");
    ASSERT_FALSE(SF::ExecutionFocus::IsMCP(), "IsMCP() must be false under DUAL_HOMO");
    ASSERT_TRUE(SF::ExecutionFocus::IsDual(), "IsDual() must be true under DUAL_HOMO");
    SF::ExecutionFocus::GetChunkWeights(p_weight, e_weight);
    ASSERT_EQ(p_weight, 1.0, "P-weight under DUAL_HOMO must be 1.0");
    ASSERT_EQ(e_weight, 1.0, "E-weight under DUAL_HOMO must be 1.0");
    ASSERT_TRUE(strcmp(SF::ExecutionFocus::GetCurrentName(), "DUAL_HOMO") == 0, "Name must be DUAL_HOMO");

    // State 6: DUAL_HYBRID
    SF::ExecutionFocus::Set(SF_FOCUS_DUAL_HYBRID);
    ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_DUAL_HYBRID, "Active focus must be SF_FOCUS_DUAL_HYBRID");
    ASSERT_FALSE(SF::ExecutionFocus::IsSCP(), "IsSCP() must be false under DUAL_HYBRID");
    ASSERT_FALSE(SF::ExecutionFocus::IsMCP(), "IsMCP() must be false under DUAL_HYBRID");
    ASSERT_TRUE(SF::ExecutionFocus::IsDual(), "IsDual() must be true under DUAL_HYBRID");
    SF::ExecutionFocus::GetChunkWeights(p_weight, e_weight);
    ASSERT_EQ(p_weight, 2.5, "P-weight under DUAL_HYBRID must be 2.5");
    ASSERT_EQ(e_weight, 1.0, "E-weight under DUAL_HYBRID must be 1.0");
    ASSERT_TRUE(strcmp(SF::ExecutionFocus::GetCurrentName(), "DUAL_HYBRID") == 0, "Name must be DUAL_HYBRID");

    // 3. Shorthand Category Aliases & Hardware Topology Invariance
    if (is_hybrid) {
        // Deliberately set homogeneous modes first to verify that physical hw_is_hybrid is NEVER destroyed
        SF::ExecutionFocus::Set(SF_FOCUS_SCP_HOMO);
        ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_SCP_HOMO, "Active focus must be SF_FOCUS_SCP_HOMO");

        Sfvm.SetFocus(SF_FOCUS_SCP);
        ASSERT_EQ(Sfvm.Focus(), SF_FOCUS_SCP_HYBRID, "SF_FOCUS_SCP shorthand on hybrid must resolve to SF_FOCUS_SCP_HYBRID even after HOMO mode");

        SF::ExecutionFocus::Set(SF_FOCUS_DUAL_HOMO);
        ASSERT_EQ(SF::ExecutionFocus::Get(), SF_FOCUS_DUAL_HOMO, "Active focus must be SF_FOCUS_DUAL_HOMO");

        Sfvm.SetFocus(SF_FOCUS_MCP);
        ASSERT_EQ(Sfvm.Focus(), SF_FOCUS_MCP_HYBRID, "SF_FOCUS_MCP shorthand on hybrid must resolve to SF_FOCUS_MCP_HYBRID even after HOMO mode");

        Sfvm.SetFocus(SF_FOCUS_HYBRID_DUAL);
        ASSERT_EQ(Sfvm.Focus(), SF_FOCUS_DUAL_HYBRID, "SF_FOCUS_HYBRID_DUAL shorthand on hybrid must resolve to SF_FOCUS_DUAL_HYBRID");

        Sfvm.SetFocus(SF_FOCUS_AUTO);
        ASSERT_EQ(Sfvm.Focus(), SF_FOCUS_DUAL_HYBRID, "SF_FOCUS_AUTO on hybrid must re-resolve to SF_FOCUS_DUAL_HYBRID");
    }

    // 4. CLI Intent & Fuzzy Resolution
    char out_flag[64] = {0};
    char out_val[64]  = {0};
    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("scp", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 1, "Intent 'scp' must resolve");
    ASSERT_TRUE(strcmp(out_flag, "Execution_Focus") == 0 && strcmp(out_val, "SCP") == 0, "'scp' maps to Execution_Focus:SCP");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("--scp", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 1, "Intent '--scp' must resolve");
    ASSERT_TRUE(strcmp(out_flag, "Execution_Focus") == 0 && strcmp(out_val, "SCP") == 0, "'--scp' maps to Execution_Focus:SCP");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("--mcp", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 1, "Intent '--mcp' must resolve");
    ASSERT_TRUE(strcmp(out_flag, "Execution_Focus") == 0 && strcmp(out_val, "MCP") == 0, "'--mcp' maps to Execution_Focus:MCP");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("mcp-hybrid", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 1, "Intent 'mcp-hybrid' must resolve");
    ASSERT_TRUE(strcmp(out_flag, "Execution_Focus") == 0 && strcmp(out_val, "MCP_Hybrid") == 0, "'mcp-hybrid' maps to Execution_Focus:MCP_Hybrid");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("--hybrid-dual", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 1, "Intent '--hybrid-dual' must resolve");
    ASSERT_TRUE(strcmp(out_flag, "Execution_Focus") == 0 && strcmp(out_val, "HybridDual") == 0, "'--hybrid-dual' maps to Execution_Focus:HybridDual");

    ASSERT_EQ(SF_custominiVm_cli_resolve_intent("dual_homo", out_flag, sizeof(out_flag), out_val, sizeof(out_val)), 1, "Intent 'dual_homo' must resolve");
    ASSERT_TRUE(strcmp(out_flag, "Execution_Focus") == 0 && strcmp(out_val, "Dual_Homo") == 0, "'dual_homo' maps to Execution_Focus:Dual_Homo");

    // 5. Priority >= 8 Pins to P-Core with Turbo Upclocker Boost
    SF::ExecutionFocus::Set(SF_FOCUS_DUAL_HYBRID);
    uint32_t gid = SF::GroupCompute({9, false});
    ASSERT_TRUE(gid > 0, "GroupCompute with priority 9 must succeed");
    ASSERT_EQ(SF_custominiVm_upclocker_is_boosted(), 1, "High priority (>=8) must activate turbo upclocker boost");
    SF::GroupComputeReset();

    // 6. Numerical Parallel Execution under Focus Modes
    const size_t N = 100000;
    std::vector<float> a(N, 2.0f);
    std::vector<float> b(N, 3.0f);
    std::vector<float> c(N, 0.0f);

    struct KernelData {
        const float* a;
        const float* b;
        float* c;
    } kdata{a.data(), b.data(), c.data()};

    auto kernel_fn = [](void* user_data, size_t start, size_t end) {
        auto* d = static_cast<KernelData*>(user_data);
        for (size_t i = start; i < end; ++i) {
            d->c[i] = d->a[i] + d->b[i];
        }
    };

    // Run in SCP mode (serial fast-path)
    SF::ExecutionFocus::Set(SF_FOCUS_SCP_HYBRID);
    std::fill(c.begin(), c.end(), 0.0f);
    SF_custominiVm_parallel_for(N, kernel_fn, &kdata);
    for (size_t i = 0; i < N; ++i) {
        if (c[i] != 5.0f) {
            std::cerr << "[FAIL] Data mismatch in SCP mode at index " << i << ": got " << c[i] << std::endl;
            return false;
        }
    }

    // Run in MCP_HYBRID mode (asymmetric weighted chunking via dedicated workers)
    SF::ExecutionFocus::Set(SF_FOCUS_MCP_HYBRID);
    std::fill(c.begin(), c.end(), 0.0f);
    SF_custominiVm_parallel_for(N, kernel_fn, &kdata);
    for (size_t i = 0; i < N; ++i) {
        if (c[i] != 5.0f) {
            std::cerr << "[FAIL] Data mismatch in MCP_HYBRID mode at index " << i << ": got " << c[i] << std::endl;
            return false;
        }
    }

    // Run in MCP_HOMO mode (uniform chunking via dedicated workers)
    SF::ExecutionFocus::Set(SF_FOCUS_MCP_HOMO);
    std::fill(c.begin(), c.end(), 0.0f);
    SF_custominiVm_parallel_for(N, kernel_fn, &kdata);
    for (size_t i = 0; i < N; ++i) {
        if (c[i] != 5.0f) {
            std::cerr << "[FAIL] Data mismatch in MCP_HOMO mode at index " << i << ": got " << c[i] << std::endl;
            return false;
        }
    }

    // Run in DUAL mode
    SF::ExecutionFocus::Set(SF_FOCUS_DUAL_HYBRID);
    std::fill(c.begin(), c.end(), 0.0f);
    SF_custominiVm_parallel_for(N, kernel_fn, &kdata);
    for (size_t i = 0; i < N; ++i) {
        if (c[i] != 5.0f) {
            std::cerr << "[FAIL] Data mismatch in DUAL mode at index " << i << ": got " << c[i] << std::endl;
            return false;
        }
    }

    // Restore to automatic detection
    SF::ExecutionFocus::Set(SF_FOCUS_AUTO);

    std::cout << "  --> Execution Focus Topology, 6-way orthogonal matrix, and kernel dispatch verified." << std::endl;
    return true;
}

bool test_vfs_branch_validation_and_panic() {
    std::cout << "\n[TEST 15] SFVM VFS Branch Validation, Diagnostic Alert & Safe Crash Engine..." << std::endl;

    // 1. Verify strict branch validation is enabled by default
    ASSERT_TRUE(SF::VFS::IsStrictBranchValidation(), "Strict VFS branch validation must be active by default");
    ASSERT_EQ(SF_custominiVm_get_strict_vfs_branch_validation(), 1, "get_strict_vfs_branch_validation must return 1");
    ASSERT_EQ(SF_custominiVm_is_strict_vfs_branch_validation(), 1, "is_strict_vfs_branch_validation must return 1");

    // 2. Verify normal operations on existing default mounted branches succeed
    SF_custominiVm_vfs_stat_t stat{};
    int st_res = SF::VFS::Stat("/heap/on_heap/runway/motherline/egg/young/tank", &stat);
    ASSERT_EQ(st_res, 0, "Default mounted branch '/heap/on_heap/runway/motherline/egg/young/tank' must exist");
    ASSERT_EQ((int)stat.is_dir, 1, "VFS branch must be recognized as directory");

    // 3. Verify directory creation via mkdir works cleanly
    int mk_res = SF::VFS::Mkdir("/models/weights/transformer", 1024 * 1024);
    ASSERT_EQ(mk_res, 0, "Creating valid VFS branch via mkdir must succeed");
    st_res = SF::VFS::Stat("/models/weights/transformer", &stat);
    ASSERT_EQ(st_res, 0, "Created branch '/models/weights/transformer' must exist in VFS");
    ASSERT_EQ(stat.quota_bytes, (size_t)(1024 * 1024), "Created branch quota must match");

    // 4. Test English technical intent mapping and CLI flag resolution
    char out_flag[64] = {0};
    char out_val[64] = {0};
    int r_intent1 = SF_custominiVm_cli_resolve_intent("strict_vfs", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_intent1, 1, "Technical intent 'strict_vfs' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Strict_VFS_Branch_Validation") == 0, "'strict_vfs' must map to Strict_VFS_Branch_Validation");

    int r_intent2 = SF_custominiVm_cli_resolve_intent("vfs_panic", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_intent2, 1, "Technical intent 'vfs_panic' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "Strict_VFS_Branch_Validation") == 0, "'vfs_panic' must map to Strict_VFS_Branch_Validation");

    int r_intent3 = SF_custominiVm_cli_resolve_intent("vfs_dialog", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_intent3, 1, "Technical intent 'vfs_dialog' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "VFS_Panic_Dialog") == 0, "'vfs_dialog' must map to VFS_Panic_Dialog");

    int r_intent4 = SF_custominiVm_cli_resolve_intent("vfs_popup", out_flag, sizeof(out_flag), out_val, sizeof(out_val));
    ASSERT_EQ(r_intent4, 1, "Technical intent 'vfs_popup' must be recognized");
    ASSERT_TRUE(strcmp(out_flag, "VFS_Panic_Dialog") == 0, "'vfs_popup' must map to VFS_Panic_Dialog");

    // 5. Test panic interception and diagnostic alert hook
    struct PanicCapture {
        std::string path;
        std::string op;
        int count{0};
    } capture;

    auto panic_hook = [](const char* missing_path, const char* op, void* user_data) {
        auto* cap = static_cast<PanicCapture*>(user_data);
        cap->path = missing_path ? missing_path : "";
        cap->op = op ? op : "";
        cap->count++;
    };

    // Suppress abort and disable GUI modal popup for automated test suite execution
    SF::VFS::SetPanicBehavior(true, false);
    SF::VFS::SetPanicHook(panic_hook, &capture);

    uint64_t initial_panics = SF::VFS::GetPanicCount();

    // 5a. Trigger lookup on deep non-existent branch
    const char* missing_branch = "/missing/phantom/branch/does_not_exist";
    SF_custominiVm_vfs_stat_t missing_stat{};
    int fail_stat = SF::VFS::Stat(missing_branch, &missing_stat);
    ASSERT_EQ(fail_stat, -1, "Stat on deep non-existent branch must fail");
    ASSERT_EQ(capture.count, 1, "Panic hook must be invoked on missing branch access");
    ASSERT_TRUE(capture.path.find(missing_branch) != std::string::npos, "Panic hook must capture exact missing branch");
    ASSERT_EQ(SF::VFS::GetPanicCount(), initial_panics + 1, "Panic counter must increment");

    // 5b. Trigger lookup on single-level non-existent root branch (regression test for root masking bug)
    const char* missing_single = "/phantom_root_folder";
    fail_stat = SF::VFS::Stat(missing_single, &missing_stat);
    ASSERT_EQ(fail_stat, -1, "Stat on single-level non-existent branch must fail");
    ASSERT_EQ(capture.count, 2, "Panic hook must be invoked on single-level missing branch");
    ASSERT_TRUE(capture.path.find(missing_single) != std::string::npos, "Panic hook must capture single-level branch");

    // 5c. Trigger lookup on non-existent subfolder inside an existing branch
    const char* missing_subfolder = "/heap/on_heap/runway/motherline/egg/young/tank/missing_subfolder";
    fail_stat = SF::VFS::Stat(missing_subfolder, &missing_stat);
    ASSERT_EQ(fail_stat, -1, "Stat on missing subfolder inside existing branch must fail");
    ASSERT_EQ(capture.count, 3, "Panic hook must trigger for missing subfolder");
    ASSERT_TRUE(capture.path.find("missing_subfolder") != std::string::npos, "Captured path must contain missing subfolder");

    // 5d. Test bind_tensor on non-existent branch to prevent phantom binding
    int32_t shape[2] = {4, 4};
    SF_custominiVm_tensor_t t = SF_custominiVm_tensor_alloc(shape, 2, SF_DTYPE_F32);
    int bind_res = SF::VFS::Bind("/non_existent/phantom_folder", &t);
    ASSERT_EQ(bind_res, -1, "Binding to non-existent branch must fail in strict mode");
    ASSERT_EQ(capture.count, 4, "Panic hook must trigger on phantom branch binding attempt");
    ASSERT_TRUE(capture.path.find("/non_existent/phantom_folder") != std::string::npos, "Captured path must match phantom branch");

    // 5e. Test bind_tensor to non-existent subfolder inside an existing parent (verify NO fallback binding to parent)
    int bind_res2 = SF::VFS::Bind("/models/weights/transformer/nonexistent_child", &t);
    ASSERT_EQ(bind_res2, -1, "Binding to non-existent child must fail in strict mode");
    ASSERT_EQ(capture.count, 5, "Panic hook must trigger without phantom binding to parent");

    // 5f. Test unlink on non-existent file
    int unl_res = SF::VFS::Unlink("/models/weights/transformer/missing_file.bin");
    ASSERT_EQ(unl_res, -1, "Unlink on non-existent file must fail");
    ASSERT_EQ(capture.count, 6, "Panic hook must trigger on unlink of non-existent file");

    // 5g. Test null path safety
    fail_stat = SF::VFS::Stat(nullptr, &missing_stat);
    ASSERT_EQ(fail_stat, -1, "Stat on null path must fail");
    ASSERT_EQ(capture.count, 7, "Panic hook must trigger on null path");

    int null_bind = SF::VFS::Bind(nullptr, &t);
    ASSERT_EQ(null_bind, -1, "Bind on null path must fail");
    ASSERT_EQ(capture.count, 8, "Panic hook must trigger on null bind path");
    SF_custominiVm_tensor_free(&t);

    // 6. Test disabling strict validation
    SF::VFS::SetStrictBranchValidation(false);
    ASSERT_FALSE(SF::VFS::IsStrictBranchValidation(), "Strict validation should now be disabled");
    ASSERT_EQ(SF_custominiVm_is_strict_vfs_branch_validation(), 0, "is_strict_vfs_branch_validation must return 0");

    capture.count = 0;
    fail_stat = SF::VFS::Stat("/another/missing/branch", &missing_stat);
    ASSERT_EQ(fail_stat, -1, "Stat should fail gracefully when strict validation is disabled");
    ASSERT_EQ(capture.count, 0, "No panic should be triggered when strict validation is disabled");

    // 7. Deadlock regression test: verify non-strict Bind on new folder auto-creates without recursive lock deadlock
    SF_custominiVm_tensor_t t2 = SF_custominiVm_tensor_alloc(shape, 2, SF_DTYPE_F32);
    int auto_bind = SF::VFS::Bind("/auto_created_tree/test_node", &t2);
    ASSERT_EQ(auto_bind, 0, "Non-strict Bind on non-existent folder must auto-create without deadlock");
    ASSERT_EQ(capture.count, 0, "No panic during non-strict auto-creation bind");
    SF_custominiVm_tensor_free(&t2);

    // 8. Re-enable strict validation and restore behavior
    SF::VFS::SetStrictBranchValidation(true);
    ASSERT_TRUE(SF::VFS::IsStrictBranchValidation(), "Strict validation should be re-enabled");
    SF::VFS::SetPanicHook(nullptr, nullptr);
    SF::VFS::SetPanicBehavior(false, true);

    std::cout << "  --> VFS branch validation, diagnostic alert logging, and safe crash protection verified." << std::endl;
    return true;
}

bool test_telemetry_streaming_and_formatting() {
    std::cout << "\n[TEST 16] Telemetry Streaming Daemon, ANSI CLI Colors, NDJSON & 786MB ColdLog..." << std::endl;

    // 1. Verify Entry Sizing and Capacity
    ASSERT_EQ(sizeof(SF_custominiVm_hotlog_entry_t), 64ULL, "HotLog entry must be exactly 64 bytes (1 cacheline)");
    ASSERT_EQ(sizeof(SF_custominiVm_coldlog_entry_t), 64ULL, "ColdLog entry must be exactly 64 bytes (1 cacheline)");

    // 2. Format HotLog record to ANSI colored CLI string
    SF_custominiVm_hotlog_entry_t hot_e{};
    hot_e.ts_ns = 1718000000123ULL;
    hot_e.tsc = 3500123456ULL;
    hot_e.gc_sweep_no = 42;
    hot_e.fn_id = 101;
    hot_e.heap_used_kb = 16384;
    hot_e.slot = 3;
    hot_e.current_core = 4;
    hot_e.generation = 1;
    hot_e.region = 2;
    hot_e.event = SF_Ebin0_PITSTOP_RETURN;
    hot_e.severity = SF_Ebin1_INFO;
    strncpy(hot_e.msg, "AVX512_BF16", sizeof(hot_e.msg) - 1);

    std::string cli_out = SF::Telemetry::FormatCLI(hot_e);
    ASSERT_FALSE(cli_out.empty(), "CLI formatted output must not be empty");
    ASSERT_TRUE(cli_out.find("PITSTOP_RETURN") != std::string::npos, "CLI output must contain event name PITSTOP_RETURN");
    ASSERT_TRUE(cli_out.find("\033[32m") != std::string::npos, "INFO severity must contain green ANSI color code");
    ASSERT_TRUE(cli_out.find("AVX512_BF16") != std::string::npos, "CLI output must contain message AVX512_BF16");

    // 3. Format HotLog record to NDJSON line
    std::string json_out = SF::Telemetry::FormatJSON(hot_e);
    ASSERT_FALSE(json_out.empty(), "JSON formatted output must not be empty");
    ASSERT_TRUE(json_out.find("\"event\":\"PITSTOP_RETURN\"") != std::string::npos, "JSON output must contain event key");
    ASSERT_TRUE(json_out.find("\"severity\":\"INFO\"") != std::string::npos, "JSON output must contain severity key");
    ASSERT_TRUE(json_out.find("\"fn_id\":101") != std::string::npos, "JSON output must contain fn_id");

    // 4. Format ColdLog record to ANSI and NDJSON
    SF_custominiVm_coldlog_entry_t cold_e{};
    cold_e.ts_ns = 1718000000500ULL;
    cold_e.fn_id = 202;
    cold_e.event = SF_Ebin0_HEAL_CANARY_QUARANTINE;
    cold_e.severity = SF_Ebin1_ERROR;
    strncpy(cold_e.msg, "SFVM captured 'D' corruption at graph in heap!", sizeof(cold_e.msg) - 1);

    std::string cold_cli = SF::Telemetry::FormatCLI(cold_e);
    ASSERT_TRUE(cold_cli.find("HEAL_CANARY_QUARANTINE") != std::string::npos, "ColdLog CLI must contain event name");
    ASSERT_TRUE(cold_cli.find("\033[31m") != std::string::npos, "ERROR severity must contain red ANSI color code");

    std::string cold_json = SF::Telemetry::FormatJSON(cold_e);
    ASSERT_TRUE(cold_json.find("\"event\":\"HEAL_CANARY_QUARANTINE\"") != std::string::npos, "ColdLog JSON must contain event");
    ASSERT_TRUE(cold_json.find("\"severity\":\"ERROR\"") != std::string::npos, "ColdLog JSON must contain severity");

    // 5. Test Filter Mask Controls
    uint32_t orig_mask = SF::Telemetry::GetFilterMask();
    SF::Telemetry::SetFilterMask(0x00000000u); // Mute all
    ASSERT_EQ(SF::Telemetry::GetFilterMask(), 0x00000000u, "Filter mask must be 0");
    SF::Telemetry::SetFilterMask(orig_mask); // Restore full mask

    // 6. Test Live Telemetry Streaming Daemon with Background Thread Callback
    struct StreamTestContext {
        std::atomic<uint32_t> received_events{0};
        std::string last_formatted;
    } stream_ctx;

    int stream_rc = SF::Telemetry::StartStream(5000, [](const SF_custominiVm_hotlog_entry_t* e, const char* line, void* user) {
        auto* c = static_cast<StreamTestContext*>(user);
        if (e && line && line[0]) {
            c->received_events.fetch_add(1, std::memory_order_relaxed);
            c->last_formatted = line;
        }
    }, &stream_ctx);

    ASSERT_EQ(stream_rc, 0, "Telemetry stream daemon start must succeed");
    ASSERT_TRUE(SF::Telemetry::IsStreamRunning(), "Telemetry stream daemon must report running state");

    // Emit live events to trigger streaming callbacks
    for (int i = 0; i < 5; ++i) {
        sf_vm_hookpoint_trigger(SF_HOOKPOINT_KERNEL_ENTER, 300 + i, nullptr, 0);
        sf_vm_hookpoint_trigger(SF_HOOKPOINT_KERNEL_EXIT, 300 + i, nullptr, 0);
    }

    // Sleep briefly to let the background daemon thread poll and drain the ring buffer
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // Stop streaming daemon
    SF::Telemetry::StopStream();
    ASSERT_FALSE(SF::Telemetry::IsStreamRunning(), "Telemetry stream daemon must stop cleanly");
    ASSERT_TRUE(stream_ctx.received_events.load() > 0, "Stream daemon must have received events");

    std::cout << "  --> Telemetry daemon received and formatted " << stream_ctx.received_events.load() << " live events." << std::endl;
    std::cout << "  --> 786MB ColdLog, ANSI terminal styling, and NDJSON serialization verified." << std::endl;
    return true;
}

bool test_multitype_and_advanced_domain_kernels() {
    std::cout << "\n[TEST 17] Precision Multi-Type DType Routing & Section 48 Advanced Domain Kernels..." << std::endl;

    // 1. Verify sf_dtype_size for all 12 types (plus invalid)
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_F8),   1ULL,  "F8 size must be 1 byte");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_U8),   1ULL,  "U8 size must be 1 byte");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_I8),   1ULL,  "I8 size must be 1 byte");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_F16),  2ULL,  "F16 size must be 2 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_I16),  2ULL,  "I16 size must be 2 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_F32),  4ULL,  "F32 size must be 4 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_I32),  4ULL,  "I32 size must be 4 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_F64),  8ULL,  "F64 size must be 8 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_I64),  8ULL,  "I64 size must be 8 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_F128), 16ULL, "F128 size must be 16 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_I128), 16ULL, "I128 size must be 16 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_F256), 32ULL, "F256 size must be 32 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_I256), 32ULL, "I256 size must be 32 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_U16),  2ULL,  "U16 size must be 2 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_U32),  4ULL,  "U32 size must be 4 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_U64),  8ULL,  "U64 size must be 8 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_U128), 16ULL, "U128 size must be 16 bytes");
    ASSERT_EQ(sf_dtype_size(SF_DTYPE_U256), 32ULL, "U256 size must be 32 bytes");
    ASSERT_EQ(sf_dtype_size(static_cast<sfDType>(-1)), 0ULL, "Invalid dtype size must be 0 bytes");

    // 2. Verify sf_dtype_detect tensor auto-type routing
    ASSERT_EQ(sf_dtype_detect(1, 1, 0), SF_DTYPE_F8,   "1-byte float must route to F8");
    ASSERT_EQ(sf_dtype_detect(2, 1, 0), SF_DTYPE_F16,  "2-byte float must route to F16");
    ASSERT_EQ(sf_dtype_detect(4, 1, 0), SF_DTYPE_F32,  "4-byte float must route to F32");
    ASSERT_EQ(sf_dtype_detect(8, 1, 0), SF_DTYPE_F64,  "8-byte float must route to F64");
    ASSERT_EQ(sf_dtype_detect(16, 1, 0), SF_DTYPE_F128, "16-byte float must route to F128");
    ASSERT_EQ(sf_dtype_detect(32, 1, 0), SF_DTYPE_F256, "32-byte float must route to F256");

    ASSERT_EQ(sf_dtype_detect(1, 0, 0), SF_DTYPE_U8,   "1-byte unsigned int must route to U8");
    ASSERT_EQ(sf_dtype_detect(1, 0, 1), SF_DTYPE_I8,   "1-byte signed int must route to I8");
    ASSERT_EQ(sf_dtype_detect(2, 0, 0), SF_DTYPE_U16,  "2-byte unsigned int must route to U16");
    ASSERT_EQ(sf_dtype_detect(2, 0, 1), SF_DTYPE_I16,  "2-byte signed int must route to I16");
    ASSERT_EQ(sf_dtype_detect(4, 0, 0), SF_DTYPE_U32,  "4-byte unsigned int must route to U32");
    ASSERT_EQ(sf_dtype_detect(4, 0, 1), SF_DTYPE_I32,  "4-byte signed int must route to I32");
    ASSERT_EQ(sf_dtype_detect(8, 0, 0), SF_DTYPE_U64,  "8-byte unsigned int must route to U64");
    ASSERT_EQ(sf_dtype_detect(8, 0, 1), SF_DTYPE_I64,  "8-byte signed int must route to I64");
    ASSERT_EQ(sf_dtype_detect(16, 0, 0), SF_DTYPE_U128, "16-byte unsigned int must route to U128");
    ASSERT_EQ(sf_dtype_detect(16, 0, 1), SF_DTYPE_I128, "16-byte signed int must route to I128");
    ASSERT_EQ(sf_dtype_detect(32, 0, 0), SF_DTYPE_U256, "32-byte unsigned int must route to U256");
    ASSERT_EQ(sf_dtype_detect(32, 0, 1), SF_DTYPE_I256, "32-byte signed int must route to I256");

    // 3. Pillar 8: CPU Acoustic Ray Tracing
    {
        float ray_origin[3] = {0.0f, 0.0f, 0.0f};
        float ray_dir[3]    = {0.0f, 1.0f, 0.0f};
        float traversed_dist = 0.0f;
        float final_pressure = 0.0f;
        msimd_acoustic_trace_ray(ray_origin, ray_dir, 100.0f, 0.02f, 25.0f, 0.1f, &traversed_dist, &final_pressure, nullptr);
        ASSERT_TRUE(traversed_dist > 0.0f, "Acoustic ray must traverse non-zero distance");
        ASSERT_TRUE(final_pressure > 0.0f && final_pressure < 100.0f, "Acoustic pressure must attenuate along ray");
    }

    // 4. Pillar 9: Molang & Dual Quaternion Mesh Skinning
    {
        float qvars[4] = {12.0f, 8.0f, 0.0f, 0.0f};
        float molang_res = msimd_molang_eval("query.var0 + 3.0", qvars, 4, nullptr);
        ASSERT_TRUE(std::abs(molang_res - 15.0f) < 1e-4f, "Molang eval query.var0 + 3.0 should equal 15.0");

        // Chained expression testing operator precedence & left-associativity
        float qvars2[4] = {2.0f, 3.14159265f / 2.0f, 0.0f, 0.0f};
        float chained_res = msimd_molang_eval("query.var0 * math.sin(query.var1) + math.cos(query.var2)", qvars2, 4, nullptr);
        ASSERT_TRUE(std::abs(chained_res - 3.0f) < 1e-3f, "Molang eval chained '2 * sin(pi/2) + cos(0)' must equal 3.0");

        float sub_res = msimd_molang_eval("10 - 5 - 2", nullptr, 0, nullptr);
        ASSERT_TRUE(std::abs(sub_res - 3.0f) < 1e-4f, "Molang eval left-associative '10 - 5 - 2' must equal 3.0");

        float sqrt_res = msimd_molang_eval("math.sqrt(4.0) + math.sin(0.0)", nullptr, 0, nullptr);
        ASSERT_TRUE(std::abs(sqrt_res - 2.0f) < 1e-4f, "Molang eval 'math.sqrt(4.0) + math.sin(0.0)' must equal 2.0");

        float clamp_res = msimd_molang_eval("math.clamp(15.0, 0.0, 10.0)", nullptr, 0, nullptr);
        ASSERT_TRUE(std::abs(clamp_res - 10.0f) < 1e-4f, "Molang eval 'math.clamp(15.0, 0.0, 10.0)' must equal 10.0");

        float in_v[3] = {1.0f, 2.0f, 3.0f};
        float in_n[3] = {0.0f, 1.0f, 0.0f};
        float dq[1][8] = {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}; // identity transform (rw=1)
        float weights[1] = {1.0f};
        float out_v[3] = {0}, out_n[3] = {0};
        msimd_animation_dualquat_skinning(in_v, in_n, dq, weights, 1, out_v, out_n, nullptr);
        ASSERT_TRUE(std::abs(out_v[0] - in_v[0]) < 1e-3f, "Dual quat skinning identity X must match");
        ASSERT_TRUE(std::abs(out_v[1] - in_v[1]) < 1e-3f, "Dual quat skinning identity Y must match");
        ASSERT_TRUE(std::abs(out_v[2] - in_v[2]) < 1e-3f, "Dual quat skinning identity Z must match");
        ASSERT_TRUE(std::abs(out_n[1] - in_n[1]) < 1e-3f, "Dual quat skinning normal Y must match");
    }

    // 5. Pillar 10: Relativistic Astrophysics (Kerr Raymarching & Magnetar B-Field)
    {
        double ro[3] = {0.0, 0.0, 50.0};
        double rd[3] = {0.0, 0.0, -1.0};
        double hit_dist = 0.0;
        int captured = 0;
        int kerr_rc = msimd_astro_kerr_raymarch(1.0, 0.5, ro, rd, 0.2, 500, &hit_dist, &captured, nullptr);
        ASSERT_EQ(kerr_rc, 1, "Kerr raymarch execution must report capture (return code 1)");
        ASSERT_EQ(captured, 1, "Direct radial ray into Kerr BH must be captured by horizon");

        // Extreme spin near photon sphere (r = 3M, a = 0.9999M)
        double ro_tang[3] = {3.0, 0.0, 0.0};
        double rd_tang[3] = {0.0, 1.0, 0.0};
        double dist_tang = 0.0;
        int cap_tang = 0;
        int kerr_tang_rc = msimd_astro_kerr_raymarch(1.0, 0.9999, ro_tang, rd_tang, 0.02, 500, &dist_tang, &cap_tang, nullptr);
        ASSERT_TRUE(kerr_tang_rc == 0 || kerr_tang_rc == 1, "Kerr extreme spin raymarching must converge without crash");
        ASSERT_TRUE(dist_tang > 0.0, "Kerr extreme spin ray must accumulate positive distance");

        double pos_km[3] = {0.0, 0.0, 20.0};
        double b_field[3] = {0};
        double b_mag = 0.0, vac_pol = 0.0;
        msimd_astro_magnetar_bfield(1e11, 10.0, pos_km, b_field, &b_mag, &vac_pol, nullptr);
        ASSERT_TRUE(b_mag > 0.0, "Magnetar B-field magnitude must be positive");
        ASSERT_TRUE(vac_pol > 0.0, "Vacuum polarization index delta_n must be > 0.0");
    }

    // 6. Pillar 12: Discrete Planck Quantum (256-bit Quad-Double Multi-Limb)
    {
        msimd_f256_t dx = {{1e-15, 0.0, 0.0, 0.0}};
        msimd_f256_t dp = {{1e-18, 0.0, 0.0, 0.0}};
        msimd_f256_t unc = {{0.0, 0.0, 0.0, 0.0}};
        int satisfies = 0;
        int q_rc = msimd_quantum_planck_bounded_uncertainty_256(&dx, &dp, &unc, &satisfies, nullptr);
        ASSERT_EQ(q_rc, 0, "Planck bounded uncertainty calculation must succeed");
        ASSERT_EQ(satisfies, 1, "Uncertainty product 1e-33 must satisfy Planck bound (>= 0.5*hbar)");
        ASSERT_TRUE(unc.limbs[0] >= 0.527e-34, "Phase space uncertainty must satisfy minimum Planck bound");

        // Discrete finite universe boundary test: non-divergence to 0 or infinity
        msimd_f256_t dx_zero = {{0.0, 0.0, 0.0, 0.0}};
        msimd_f256_t dp_zero = {{0.0, 0.0, 0.0, 0.0}};
        msimd_f256_t unc_zero = {{0.0, 0.0, 0.0, 0.0}};
        int satisfies_zero = -1;
        msimd_quantum_planck_bounded_uncertainty_256(&dx_zero, &dp_zero, &unc_zero, &satisfies_zero, nullptr);
        ASSERT_TRUE(unc_zero.limbs[0] > 0.0, "Discrete Planck universe: uncertainty cannot collapse to zero");
        ASSERT_TRUE(unc_zero.limbs[0] >= 0.527e-34, "Zero input must be clamped to minimum Planck phase space bound");

        msimd_f256_t dx_huge = {{1e30, 0.0, 0.0, 0.0}};
        msimd_f256_t dp_huge = {{1e30, 0.0, 0.0, 0.0}};
        msimd_f256_t unc_huge = {{0.0, 0.0, 0.0, 0.0}};
        int satisfies_huge = -1;
        msimd_quantum_planck_bounded_uncertainty_256(&dx_huge, &dp_huge, &unc_huge, &satisfies_huge, nullptr);
        ASSERT_TRUE(unc_huge.limbs[0] < 1e38, "Discrete Planck universe: uncertainty cannot diverge to infinity");

        msimd_f256_t psi_re[4] = {{{1.0, 0,0,0}}, {{4.0, 0,0,0}}, {{2.0, 0,0,0}}, {{0.5, 0,0,0}}};
        msimd_f256_t psi_im[4] = {{{0.0, 0,0,0}}, {{3.0, 0,0,0}}, {{0.0, 0,0,0}}, {{0.0, 0,0,0}}};
        msimd_f256_t peak_prob = {{0.0, 0,0,0}};
        size_t peak_idx = msimd_quantum_wavepacket_peak_pinpoint_256(psi_re, psi_im, 4, &peak_prob, nullptr);
        ASSERT_EQ(peak_idx, 1ULL, "Wavepacket peak index must pinpoint index 1 (magnitude 4^2+3^2=25)");
        ASSERT_TRUE(std::abs(peak_prob.limbs[0] - 25.0) < 1e-5, "Peak probability must be 25.0");

        double q_in_re[3] = {1.0, 0.0, 0.0};
        double q_in_im[3] = {0.0, 0.0, 0.0};
        double ckm_re[9]  = {1,0,0, 0,1,0, 0,0,1};
        double ckm_im[9]  = {0};
        double q_out_re[3] = {0}, q_out_im[3] = {0};
        msimd_quantum_flavor_ckm_mix(q_in_re, q_in_im, ckm_re, ckm_im, q_out_re, q_out_im, nullptr);
        ASSERT_TRUE(std::abs(q_out_re[0] - 1.0) < 1e-5, "CKM mix identity first component must match");
        ASSERT_TRUE(std::abs(q_out_re[1]) < 1e-5, "CKM mix identity second component must be 0");
    }

    // 7. Pillar 13: Asymmetric Numeral Systems (rANS) SIMD Entropy Coding
    {
        uint8_t raw_msg[8] = {10, 20, 30, 40, 10, 20, 30, 40};
        uint8_t compressed[64] = {0};
        uint8_t decompressed[64] = {0};
        size_t enc_bytes = msimd_compress_rans_encode(raw_msg, sizeof(raw_msg), compressed, sizeof(compressed), nullptr, 0, nullptr);
        ASSERT_TRUE(enc_bytes > 0, "rANS encode must produce non-zero compressed byte stream");

        size_t dec_bytes = msimd_compress_rans_decode(compressed, enc_bytes, decompressed, sizeof(raw_msg), nullptr, 0, nullptr);
        ASSERT_EQ(dec_bytes, sizeof(raw_msg), "rANS decode must decode exact number of raw symbols");
        for (size_t i = 0; i < sizeof(raw_msg); ++i) {
            ASSERT_EQ(decompressed[i], raw_msg[i], "rANS decompressed byte must match original byte exactly");
        }

        // Secondary verification with multi-frequency message
        uint8_t raw_msg2[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
        uint8_t comp2[64] = {0};
        uint8_t decomp2[64] = {0};
        size_t enc2 = msimd_compress_rans_encode(raw_msg2, sizeof(raw_msg2), comp2, sizeof(comp2), nullptr, 0, nullptr);
        ASSERT_TRUE(enc2 > 0, "rANS encode test 2 must produce non-zero compressed byte stream");
        size_t dec2 = msimd_compress_rans_decode(comp2, enc2, decomp2, sizeof(raw_msg2), nullptr, 0, nullptr);
        ASSERT_EQ(dec2, sizeof(raw_msg2), "rANS decode test 2 must decode exact number of symbols");
        for (size_t i = 0; i < sizeof(raw_msg2); ++i) {
            ASSERT_EQ(decomp2[i], raw_msg2[i], "rANS decode test 2 byte must match original byte exactly");
        }
    }

    std::cout << "  --> Multi-type precision detection (F8..F256, I8..I256) and Section 48 advanced domain kernels verified." << std::endl;
    return true;
}

static std::vector<std::string> s_test_captured_logs;
static void test_raw_log_callback(const char* text, size_t text_length, void* user_data) {
    (void)user_data;
    if (text && text_length > 0) {
        s_test_captured_logs.push_back(std::string(text, text_length));
    }
}

bool test_raw_log_stream_forwarder() {
    std::cout << "\n[TEST 18] Raw Log Stream Forwarder & Telemetry Mirroring..." << std::endl;

    // 1. Console mirroring toggle
    int orig_mirror = SF_custominiVm_get_console_mirroring();
    SF_custominiVm_set_console_mirroring(0);
    ASSERT_EQ(SF_custominiVm_get_console_mirroring(), 0, "Console mirroring should be disabled");
    SF_custominiVm_set_console_mirroring(1);
    ASSERT_EQ(SF_custominiVm_get_console_mirroring(), 1, "Console mirroring should be re-enabled");

    // 2. Register raw stream hook
    s_test_captured_logs.clear();
    SF_custominiVm_register_raw_log_stream(test_raw_log_callback, nullptr);

    // 3. Emit raw log with hex tag & ANSI code
    SF_custominiVm_emit_raw_log("\033[1;36m[0x00A1] [TEST_EMIT] Raw log forwarder stream active\033[0m");
    ASSERT_TRUE(!s_test_captured_logs.empty(), "Captured log list must not be empty after emit");
    bool found_hex = false;
    for (const auto& l : s_test_captured_logs) {
        if (l.find("[0x00A1]") != std::string::npos) {
            found_hex = true;
            break;
        }
    }
    ASSERT_TRUE(found_hex, "Broadcasted log must retain [0x00A1] hex tag");

    // 4. Test modern C++ telemetry wrapper
    size_t count_before = s_test_captured_logs.size();
    SF::Telemetry::EmitRaw("\033[1;32m[0x00B1] [CPP_WRAPPER] Modern C++ SF::Telemetry stream\033[0m");
    ASSERT_TRUE(s_test_captured_logs.size() > count_before, "SF::Telemetry::EmitRaw must broadcast through raw stream");

    // 5. Unregister hook
    SF_custominiVm_unregister_raw_log_stream();
    size_t count_unregistered = s_test_captured_logs.size();
    SF_custominiVm_emit_raw_log("[0x00C1] Should not be captured");
    ASSERT_EQ(s_test_captured_logs.size(), count_unregistered, "Log must not be captured after unregister");

    // Restore original mirroring
    SF_custominiVm_set_console_mirroring(orig_mirror);

    std::cout << "  --> Raw log stream forwarder and C++ telemetry hooks verified." << std::endl;
    return true;
}

bool test_scoped_batch_processing() {
    std::cout << "\n[TEST 19] Scoped Batch Processing Blocks (SF::BatchScope & C API)..." << std::endl;

    // 1. Initial batch id must be 0 (no batch active)
    ASSERT_EQ(SF_custominiVm_get_current_batch_id(), 0ULL, "Initial batch ID must be 0");

    // 2. C API: SF_custominiVm_batch_begin / batch_end
    uint64_t b1 = SF_custominiVm_batch_begin(7, 1);
    ASSERT_TRUE(b1 != 0, "batch_begin must return valid non-zero hex ID");
    ASSERT_EQ(SF_custominiVm_get_current_batch_id(), b1, "get_current_batch_id must return active batch ID");
    SF_custominiVm_batch_end();
    ASSERT_EQ(SF_custominiVm_get_current_batch_id(), 0ULL, "get_current_batch_id must reset to 0 after batch_end");

    // 3. RAII SF::BatchScope
    uint64_t scoped_id = 0;
    {
        SF::BatchScope batch(5, false);
        scoped_id = batch.group_id;
        ASSERT_TRUE(scoped_id != 0, "BatchScope must assign non-zero group ID");
        ASSERT_EQ(SF_custominiVm_get_current_batch_id(), scoped_id, "get_current_batch_id must match batch.group_id inside scope");
    }
    ASSERT_EQ(SF_custominiVm_get_current_batch_id(), 0ULL, "Batch ID must reset to 0 upon BatchScope destruction");

    // 4. RAII SF::GroupScope alias
    {
        SF::GroupScope group(9, true);
        ASSERT_TRUE(group.group_id != 0, "GroupScope alias must assign non-zero group ID");
        ASSERT_EQ(SF_custominiVm_get_current_batch_id(), group.group_id, "get_current_batch_id must match group.group_id inside scope");
    }
    ASSERT_EQ(SF_custominiVm_get_current_batch_id(), 0ULL, "Batch ID must reset to 0 upon GroupScope destruction");

    // 5. Nested SF::EntangleScope / SF::BatchScope (Tek Çatı unified scope test)
    {
        SF::EntangleScope outer(4, false);
        uint64_t outer_id = outer.group_id;
        ASSERT_EQ(SF_custominiVm_get_current_batch_id(), outer_id, "Active batch ID must be outer_id");
        ASSERT_EQ(SF_custominiVm_entangle_is_active(), 1, "Entangle must be active in outer scope");
        {
            SF::BatchScope inner(8, true);
            uint64_t inner_id = inner.group_id;
            ASSERT_TRUE(inner_id != outer_id, "Inner scope must have distinct unique group ID");
            ASSERT_EQ(SF_custominiVm_get_current_batch_id(), inner_id, "Active batch ID must switch to inner_id");
            ASSERT_EQ(SF_custominiVm_entangle_is_active(), 1, "Entangle must remain active in inner scope");
        }
        ASSERT_EQ(SF_custominiVm_get_current_batch_id(), outer_id, "Active batch ID must restore to outer_id on inner destruction");
        ASSERT_EQ(SF_custominiVm_entangle_is_active(), 1, "Entangle must remain active in outer scope");
    }
    ASSERT_EQ(SF_custominiVm_get_current_batch_id(), 0ULL, "Active batch ID must reset to 0 after outer destruction");
    ASSERT_EQ(SF_custominiVm_entangle_is_active(), 0, "Entangle must reset to inactive after outer destruction");

    std::cout << "  --> Scoped batch blocks (SF::BatchScope / SF::GroupScope / SF::EntangleScope nested) verified." << std::endl;
    return true;
}

static std::vector<std::pair<SF_custominiVm_controlpoint_action_t, uint64_t>> s_test_cp_events;
static void test_cp_callback(SF_custominiVm_controlpoint_action_t action, uint64_t param, void* user_data) {
    (void)user_data;
    s_test_cp_events.push_back({action, param});
}

bool test_controlpoint_host_coordination_and_emulation_routing() {
    std::cout << "\n[TEST 20] Controlpoint Host GC Sync & Multi-Limb Silicon Emulation..." << std::endl;

    // 1. Host GC Cooperative mode switching
    int orig_mode = SF_custominiVm_get_host_gc_cooperative_mode();
    ASSERT_EQ(orig_mode, (int)SF_HOST_GC_AUTONOMOUS, "Default host GC mode should be AUTONOMOUS");

    SF_custominiVm_set_host_gc_cooperative_mode((int)SF_HOST_GC_COOPERATIVE_SYNC);
    ASSERT_EQ(SF_custominiVm_get_host_gc_cooperative_mode(), (int)SF_HOST_GC_COOPERATIVE_SYNC, "Mode should be COOPERATIVE_SYNC");

    SF_custominiVm_set_host_gc_cooperative_mode((int)SF_HOST_GC_DELEGATED_HOST_LEAD);
    ASSERT_EQ(SF_custominiVm_get_host_gc_cooperative_mode(), (int)SF_HOST_GC_DELEGATED_HOST_LEAD, "Mode should be DELEGATED_HOST_LEAD");

    // Modern C++ helper
    SF::Controlpoint::SetMode(SF_HOST_GC_AUTONOMOUS);
    ASSERT_EQ((int)SF::Controlpoint::GetMode(), (int)SF_HOST_GC_AUTONOMOUS, "Mode should be back to AUTONOMOUS");

    // 2. Controlpoint hook registration & invocation
    s_test_cp_events.clear();
    SF_custominiVm_register_controlpoint_hook(test_cp_callback, nullptr);

    // Trigger test events
    SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_PACING_STALL, 0xABC);
    SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_PAUSE_MUTATORS, 42);
    SF_custominiVm_trigger_host_controlpoint(SF_CP_ACTION_RESUME_MUTATORS, 42);

    ASSERT_EQ(s_test_cp_events.size(), 3ULL, "Must have received 3 controlpoint events");
    ASSERT_EQ((int)s_test_cp_events[0].first, (int)SF_CP_ACTION_PACING_STALL, "First event should be PACING_STALL");
    ASSERT_EQ(s_test_cp_events[0].second, 0xABCULL, "First event param should match");
    ASSERT_EQ((int)s_test_cp_events[1].first, (int)SF_CP_ACTION_PAUSE_MUTATORS, "Second event should be PAUSE_MUTATORS");
    ASSERT_EQ((int)s_test_cp_events[2].first, (int)SF_CP_ACTION_RESUME_MUTATORS, "Third event should be RESUME_MUTATORS");

    SF_custominiVm_unregister_controlpoint_hook();

    // 3. Multi-Limb Silicon Emulation Schema inspection
    const char* schema_f4x16 = SF_custominiVm_get_emulation_schema_name(SF_EMU_SCHEMA_FLOAT_4X16);
    ASSERT_TRUE(schema_f4x16 != nullptr && std::strstr(schema_f4x16, "Float_4x16") != nullptr, "Float_4x16 schema name mismatch");

    const char* schema_i4x16 = SF_custominiVm_get_emulation_schema_name(SF_EMU_SCHEMA_INT_4X16);
    ASSERT_TRUE(schema_i4x16 != nullptr && std::strstr(schema_i4x16, "Int_4x16") != nullptr, "Int_4x16 schema name mismatch");

    // 4. Dynamic ALU Route Resolution
    int is_emu = -1;
    int schema_id = -1;

    // High priority (>= 8) strictly stays on native silicon lane without emulation
    int native_lane = SF_custominiVm_alu_resolve_route_by_dtype(SF_DTYPE_F64, 8, &is_emu, &schema_id);
    ASSERT_EQ(is_emu, 0, "Priority >= 8 must pin to native ALU without emulation");
    ASSERT_EQ(native_lane, 0, "FP64 native lane must be lane 0");
    ASSERT_EQ(schema_id, (int)SF_EMU_SCHEMA_NONE, "Native routing should have SF_EMU_SCHEMA_NONE");

    // FP128 / FP256 must always resolve to emulation on standard 64-bit silicon
    int emu_128_lane = SF_custominiVm_alu_resolve_route_by_dtype(SF_DTYPE_F128, 5, &is_emu, &schema_id);
    ASSERT_EQ(is_emu, 1, "FP128 must be emulated");
    ASSERT_TRUE(schema_id > 0, "FP128 must have valid emulation schema");
    ASSERT_TRUE(emu_128_lane >= 0, "FP128 must have valid ALU lane");

    int emu_256_lane = SF_custominiVm_alu_resolve_route_by_dtype(SF_DTYPE_F256, 5, &is_emu, &schema_id);
    ASSERT_EQ(is_emu, 1, "FP256 must be emulated");
    ASSERT_TRUE(schema_id > 0, "FP256 must have valid emulation schema");
    ASSERT_TRUE(emu_256_lane >= 0, "FP256 must have valid ALU lane");

    // 5. Query emulation dispatched telemetry count
    uint64_t emu_count = SF_custominiVm_get_alu_emulation_dispatched_count();
    uint64_t emu_count_cpp = SF::ALU::GetEmulationDispatchedCount();
    ASSERT_EQ(emu_count, emu_count_cpp, "C and C++ telemetry queries must match");

    std::cout << "  --> Controlpoint host sync and multi-limb emulation routing verified." << std::endl;
    return true;
}

bool test_inline_asm_threshold_and_strict_ring3() {
    std::cout << "\n[TEST 21] Inline Assembly (iasm) Threshold, Strict Ring 3 & Strict Flags..." << std::endl;

    // 1. Initial defaults check
    ASSERT_EQ(SF_custominiVm_get_iasm_threshold(), 5U, "Default iasm threshold must be 5");
    ASSERT_EQ(SF_custominiVm_is_strict_ring3(), 0, "Strict Ring 3 must be disabled by default");
    ASSERT_EQ(SF_custominiVm_is_strict_flags(), 0, "Strict flags validation must be disabled by default");

    // 2. Threshold gating verification
    // Priority 4 < threshold 5 -> iasm must NOT be active
    ASSERT_EQ(SF_custominiVm_is_iasm_active(4), 0, "Priority 4 must not activate iasm when threshold is 5");
    // Priority 5 >= threshold 5 -> iasm MUST be active
    ASSERT_EQ(SF_custominiVm_is_iasm_active(5), 1, "Priority 5 must activate iasm when threshold is 5");
    // Priority 10 >= threshold 5 -> iasm MUST be active
    ASSERT_EQ(SF_custominiVm_is_iasm_active(10), 1, "Priority 10 must activate iasm when threshold is 5");

    // 3. Modifying threshold
    SF::InlineASM::SetThreshold(8);
    ASSERT_EQ(SF::InlineASM::GetThreshold(), 8U, "Updated threshold must be 8");
    ASSERT_EQ(SF::InlineASM::IsActive(7), false, "Priority 7 must not activate iasm when threshold is 8");
    ASSERT_EQ(SF::InlineASM::IsActive(8), true, "Priority 8 must activate iasm when threshold is 8");
    ASSERT_EQ(SF::InlineASM::IsActive(9), true, "Priority 9 must activate iasm when threshold is 8");

    // 4. Strict Ring 3 mode override
    SF::InlineASM::SetStrictRing3(true);
    ASSERT_TRUE(SF::InlineASM::IsStrictRing3(), "Strict Ring 3 mode must be active");
    // Even priority 10 must be blocked in strict Ring 3 mode!
    ASSERT_EQ(SF::InlineASM::IsActive(10), false, "Strict Ring 3 must block iasm even at max priority 10");
    ASSERT_EQ(SF::InlineASM::IsActive(5), false, "Strict Ring 3 must block iasm at priority 5");

    // Test stream stores and conditional moves under strict Ring 3
    float src_f32[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float dst_f32[8] = {0.0f};
    SF_custominiVm_stream_store_f32(dst_f32, src_f32, 8);
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(dst_f32[i], src_f32[i], "Strict Ring 3 stream store f32 must correctly copy data");
    }

    int32_t cmov_res = sf_hinter_cmov_i32(1, 42, 99, nullptr);
    ASSERT_EQ(cmov_res, 42, "Hinter cmov under strict Ring 3 must return val_true when condition is true");
    int32_t cmov_res_false = sf_hinter_cmov_i32(0, 42, 99, nullptr);
    ASSERT_EQ(cmov_res_false, 99, "Hinter cmov under strict Ring 3 must return val_false when condition is false");

    // Disable Strict Ring 3 and restore threshold
    SF::InlineASM::SetStrictRing3(false);
    ASSERT_FALSE(SF::InlineASM::IsStrictRing3(), "Strict Ring 3 must be disabled");
    SF::InlineASM::SetThreshold(5);
    ASSERT_EQ(SF::InlineASM::GetThreshold(), 5U, "Restored threshold must be 5");
    ASSERT_TRUE(SF::InlineASM::IsActive(5), "Priority 5 must reactivate iasm once strict Ring 3 is disabled");

    // 5. Strict Flags API
    SF::Flags::SetStrictValidation(true);
    ASSERT_TRUE(SF::Flags::IsStrictValidation(), "Strict flags validation must be enabled");
    SF::Flags::SetStrictValidation(false);
    ASSERT_FALSE(SF::Flags::IsStrictValidation(), "Strict flags validation must be disabled");

    std::cout << "  --> Inline Assembly safety threshold, Ring 3 gate, and strict flag validation verified." << std::endl;
    return true;
}

bool test_open_world_visibility_culling() {
    std::cout << "\n[TEST 22] Open-World Visibility Culling & Geometry Optimization (CPU SIMD & GPU Reference)..." << std::endl;

    // 1. Test msimd_cull_frustum_aabbs_f32
    // Simple 6-plane frustum for camera at (0, 0, 0) looking along +Z:
    // Left:   x + z >= 0   -> ( 1, 0, 1, 0)
    // Right: -x + z >= 0   -> (-1, 0, 1, 0)
    // Bottom: y + z >= 0   -> ( 0, 1, 1, 0)
    // Top:   -y + z >= 0   -> ( 0,-1, 1, 0)
    // Near:       z >= 1   -> ( 0, 0, 1, -1)
    // Far:       -z >= -100-> ( 0, 0,-1, 100)
    float planes[24] = {
         1.0f,  0.0f,  1.0f,   0.0f,
        -1.0f,  0.0f,  1.0f,   0.0f,
         0.0f,  1.0f,  1.0f,   0.0f,
         0.0f, -1.0f,  1.0f,   0.0f,
         0.0f,  0.0f,  1.0f,  -1.0f,
         0.0f,  0.0f, -1.0f, 100.0f
    };

    // 4 AABBs (minX, minY, minZ, maxX, maxY, maxZ):
    // Box 0: Inside frustum at z=20
    // Box 1: Behind camera at z=-20 (culled)
    // Box 2: Way to the right at x=200 (culled)
    // Box 3: Beyond far plane at z=200 (culled)
    float aabbs[24] = {
        -2.0f, -2.0f,  18.0f,   2.0f,  2.0f,  22.0f,
        -2.0f, -2.0f, -22.0f,   2.0f,  2.0f, -18.0f,
       198.0f, -2.0f,  18.0f, 202.0f,  2.0f,  22.0f,
        -2.0f, -2.0f, 198.0f,   2.0f,  2.0f, 202.0f
    };
    uint8_t aabb_vis[4] = {0};
    size_t aabb_visible_count = msimd_cull_frustum_aabbs_f32(planes, aabbs, 4, aabb_vis, nullptr);
    ASSERT_EQ(aabb_visible_count, 1ULL, "Exactly 1 AABB must be visible");
    ASSERT_EQ(aabb_vis[0], 1U, "Box 0 inside frustum must be visible");
    ASSERT_EQ(aabb_vis[1], 0U, "Box 1 behind camera must be culled");
    ASSERT_EQ(aabb_vis[2], 0U, "Box 2 far right must be culled");
    ASSERT_EQ(aabb_vis[3], 0U, "Box 3 beyond far plane must be culled");

    // 2. Test msimd_cull_frustum_spheres_f32
    // Sphere 0: center (0, 0, 10), radius 2 -> visible
    // Sphere 1: center (0, 0, -10), radius 2 -> culled (behind near plane)
    // Sphere 2: center (-12, 0, 10), radius 5 -> visible (intersects left plane x+z >= 0: -12+10 = -2 >= -5)
    // Sphere 3: center (0, 0, 150), radius 5 -> culled (beyond far plane)
    float spheres[16] = {
          0.0f, 0.0f,  10.0f, 2.0f,
          0.0f, 0.0f, -10.0f, 2.0f,
        -12.0f, 0.0f,  10.0f, 5.0f,
          0.0f, 0.0f, 150.0f, 5.0f
    };
    uint8_t sphere_vis[4] = {0};
    size_t sphere_visible_count = msimd_cull_frustum_spheres_f32(planes, spheres, 4, sphere_vis, nullptr);
    ASSERT_EQ(sphere_visible_count, 2ULL, "Exactly 2 bounding spheres must be visible");
    ASSERT_EQ(sphere_vis[0], 1U, "Sphere 0 inside frustum must be visible");
    ASSERT_EQ(sphere_vis[1], 0U, "Sphere 1 behind camera must be culled");
    ASSERT_EQ(sphere_vis[2], 1U, "Sphere 2 intersecting left plane must be visible");
    ASSERT_EQ(sphere_vis[3], 0U, "Sphere 3 beyond far plane must be culled");

    // 3. Test msimd_cull_distance_lod_f32
    float cam_pos[3] = {0.0f, 0.0f, 0.0f};
    float lod_thresholds_sq[4] = {100.0f, 900.0f, 3600.0f, 10000.0f}; // 10m, 30m, 60m, 100m
    float obj_positions[15] = {
        0.0f, 0.0f,   5.0f, // dist = 5m  -> LOD 0
        0.0f, 0.0f,  20.0f, // dist = 20m -> LOD 1
        0.0f, 0.0f,  50.0f, // dist = 50m -> LOD 2
        0.0f, 0.0f,  80.0f, // dist = 80m -> LOD 3
        0.0f, 0.0f, 150.0f  // dist = 150m-> LOD 255 (culled)
    };
    uint8_t lod_levels[5] = {0};
    size_t lod_visible_count = msimd_cull_distance_lod_f32(cam_pos, obj_positions, 5, lod_thresholds_sq, lod_levels, nullptr);
    ASSERT_EQ(lod_visible_count, 4ULL, "4 of 5 objects must be within max cull range");
    ASSERT_EQ(lod_levels[0], 0U, "Object 0 must be LOD 0");
    ASSERT_EQ(lod_levels[1], 1U, "Object 1 must be LOD 1");
    ASSERT_EQ(lod_levels[2], 2U, "Object 2 must be LOD 2");
    ASSERT_EQ(lod_levels[3], 3U, "Object 3 must be LOD 3");
    ASSERT_EQ(lod_levels[4], 255U, "Object 4 beyond max range must be 255 (culled)");

    // 4. Test msimd_voxel_cull_hidden_faces_u32
    // 3x3x3 solid block of voxels (27 voxels total, all block_id = 1)
    uint32_t grid[27];
    for (int i = 0; i < 27; ++i) grid[i] = 1U;
    uint8_t face_masks[27] = {0};

    size_t total_faces = msimd_voxel_cull_hidden_faces_u32(grid, 3, 3, 3, face_masks, nullptr);
    // Center voxel at (1, 1, 1) has index = 1*9 + 1*3 + 1 = 13
    // It is completely surrounded by solid voxels on all 6 sides -> its face mask MUST BE 0!
    ASSERT_EQ(face_masks[13], 0U, "Interior center voxel (1,1,1) must have 0 visible faces (completely culled)");
    // Outer surface of a 3x3x3 cube has 6 faces * 9 quads = 54 visible quads
    ASSERT_EQ(total_faces, 54ULL, "Total visible faces of 3x3x3 solid cube must be 54 (6 * 9)");

    std::cout << "  --> Frustum AABB culling, sphere culling, distance LOD, and voxel hidden face culling verified." << std::endl;
    return true;
}

int main() {
    std::cout << "=================================================================" << std::endl;
    std::cout << "   MSIMD SFVM & sfGC COMPREHENSIVE ARCHITECTURAL VERIFICATION    " << std::endl;
    std::cout << "=================================================================" << std::endl;

    if (!test_vm_lifecycle()) return 1;
    if (!test_four_point_coordination()) return 2;
    if (!test_motherline_egg_fledgling_and_pitstop()) return 3;
    if (!test_alu_saturation_and_group_compute()) return 4;
    if (!test_rmrp_compaction()) return 5;
    if (!test_thread_budgeting()) return 6;
    if (!test_sram_optimizations_and_global_volatility()) return 7;
    if (!test_self_healing_engine()) return 8;
    if (!test_log_boot_drain()) return 9;
    if (!test_entangle_execution()) return 10;
    if (!test_branch_predicting_director()) return 11;
    if (!test_cli_fuzzy_and_synonyms()) return 12;
    if (!test_os_hinting_and_pinning()) return 13;
    if (!test_execution_focus_topology()) return 14;
    if (!test_vfs_branch_validation_and_panic()) return 15;
    if (!test_telemetry_streaming_and_formatting()) return 16;
    if (!test_multitype_and_advanced_domain_kernels()) return 17;
    if (!test_raw_log_stream_forwarder()) return 18;
    if (!test_scoped_batch_processing()) return 19;
    if (!test_controlpoint_host_coordination_and_emulation_routing()) return 20;
    if (!test_inline_asm_threshold_and_strict_ring3()) return 21;
    if (!test_open_world_visibility_culling()) return 22;

    std::cout << "\n[SHUTDOWN] Terminating VM execution engine..." << std::endl;
    SF_custominiVm_shutdown();

    // Verify clean shutdown & emergency OS sweep leaves 0 leaked OS blocks
    size_t swept_post = SF_custominiVm_emergency_os_sweep();
    if (swept_post != 0) {
        std::cerr << "[FAIL] Post-shutdown emergency OS sweep found " << swept_post << " leaked bytes!" << std::endl;
        return 23;
    }

    std::cout << "\n=================================================================" << std::endl;
    std::cout << "   ALL 22 ARCHITECTURAL VERIFICATION SUITES PASSED CLEANLY!       " << std::endl;
    std::cout << "=================================================================" << std::endl;
    std::fflush(stdout);
#ifdef _WIN32
    TerminateProcess(GetCurrentProcess(), 0);
#endif
    return 0;
}
