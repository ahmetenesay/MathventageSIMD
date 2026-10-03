
#include "MathventageSIMD.h"
#include "msimd_internal.h"

#ifdef USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/task_arena.h>
#include <tbb/partitioner.h>
#include <tbb/blocked_range.h>
#endif

#ifdef USE_OPENVINO
#include <openvino/openvino.hpp>
#endif

#include <cstdlib>

#ifdef USE_TBB
struct TbbBackendContext {
    tbb::task_arena arena;
    tbb::affinity_partitioner partitioner;

    TbbBackendContext(int cores) : arena(cores > 0 ? cores : tbb::task_arena::automatic) {}
};
#endif

extern "C" {

msimd_status_t msimd_backend_accelerated_init(msimd_context ctx) {
    if (!ctx) return MSIMD_ERROR_INVALID_ARGUMENT;

#ifdef USE_TBB
    try {
        int cores = ctx->cpu_info.num_physical_cores;
        TbbBackendContext* tbb_ctx = new TbbBackendContext(cores);
        ctx->backend_handle = tbb_ctx;
        return MSIMD_SUCCESS;
    } catch (...) {
        return MSIMD_ERROR_BACKEND_FAILURE;
    }
#else

    return MSIMD_ERROR_UNSUPPORTED_HARDWARE;
#endif
}

void msimd_backend_accelerated_destroy(msimd_context ctx) {
    if (!ctx || !ctx->backend_handle) return;
#ifdef USE_TBB
    TbbBackendContext* tbb_ctx = static_cast<TbbBackendContext*>(ctx->backend_handle);
    delete tbb_ctx;
    ctx->backend_handle = nullptr;
#endif
}

void msimd_backend_accelerated_parallel_for(msimd_context ctx, size_t total, msimd_parallel_for_fn fn, void* user_data) {
    if (!ctx || total == 0 || !fn) return;

#ifdef USE_TBB
    TbbBackendContext* tbb_ctx = static_cast<TbbBackendContext*>(ctx->backend_handle);
    if (!tbb_ctx) {
        fn(user_data, 0, total);
        return;
    }

    tbb_ctx->arena.execute([&]() {
        tbb::parallel_for(
            tbb::blocked_range<size_t>(0, total),
            [&](const tbb::blocked_range<size_t>& r) {
                fn(user_data, r.begin(), r.end());
            },
            tbb_ctx->partitioner
        );
    });
#else
    fn(user_data, 0, total);
#endif
}

}
