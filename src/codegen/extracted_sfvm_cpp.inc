#include <type_traits>

#include "SacculusForge.h"

/* ========================================================================= */
/* CONTINUNTIL.FINISH & STEPPING_MTP TAGS & CONSTANTS                        */
/* ========================================================================= */

/**
 * @struct _sf_continuntil_tag
 * @brief Tag type enabling ergonomic `CONTINUNTIL.FINISH` syntax for continuous GC execution.
 */
struct _sf_continuntil_tag {
    static constexpr SF_custominiVm_gc_run_mode_t FINISH = SF_GC_RUN_CONTINUE_FINISH;
    constexpr SF_custominiVm_gc_run_mode_t operator()() const { return FINISH; }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/**
 * @brief Global tag instance enabling intuitive syntax: `Sfvm.Gc(CONTINUNTIL.FINISH)`.
 */
inline constexpr _sf_continuntil_tag CONTINUNTIL{};
#else
static const _sf_continuntil_tag CONTINUNTIL{};
#endif

/** @brief Stepping mode constant for Manual Termination Point (MTP) execution. */
static constexpr SF_custominiVm_gc_run_mode_t STEPPING_MTP = SF_GC_RUN_STEPPING_MTP;
/** @brief Stepping mode constant for single-cycle incremental GC stepping. */
static constexpr SF_custominiVm_gc_run_mode_t STEP_SINGLE  = SF_GC_RUN_SINGLE_STEP;

/* ========================================================================= */
/* C++ SFVM RUNTIME & MANUAL GC HELPER (Sfvm.Gc() / Sfvm::Gc())              */
/* ========================================================================= */

/**
 * @struct sfvm_runtime_t
 * @brief High-level modern C++ static interface for SFVM garbage collection, memory telemetry, and heuristics.
 *
 * Provides an idiomatic C++ API wrapping low-level C SF_custominiVm_* functions. Supports single-step
 * collection, continuous draining to finish, predicate-controlled stepping with C++ lambdas,
 * and comprehensive Caretaker Manifest reporting.
 */
struct sfvm_runtime_t {
    /**
     * @brief Executes a single incremental garbage collection cycle.
     *
     * @param[in] owp One-Way Pacing flag: if true, applies strict allocation backpressure pacing.
     * @return Volume of memory reclaimed during the step in bytes.
     */
    static inline size_t Gc(bool owp = false) {
        return SF_custominiVm_gc_run(SF_GC_RUN_SINGLE_STEP, owp ? 1 : 0, nullptr, nullptr);
    }

    /**
     * @brief Executes garbage collection under a specified operational stepping mode.
     *
     * Example: `Sfvm.Gc(CONTINUNTIL.FINISH)` or `Sfvm.Gc(STEPPING_MTP, true)`.
     *
     * @param[in] mode Stepping mode (SF_GC_RUN_SINGLE_STEP, CONTINUE_FINISH, or STEPPING_MTP).
     * @param[in] owp  One-Way Pacing flag.
     * @return Total volume of memory reclaimed in bytes.
     */
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp = false) {
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, nullptr, nullptr);
    }

    /**
     * @brief Executes continuous collection until the heap is fully drained using the CONTINUNTIL tag.
     *
     * Example: `Sfvm.Gc(CONTINUNTIL)`.
     *
     * @param[in] tag Unused tag parameter for overload resolution.
     * @param[in] owp One-Way Pacing flag.
     * @return Total volume of memory reclaimed in bytes.
     */
    static inline size_t Gc(const _sf_continuntil_tag&, bool owp = false) {
        return SF_custominiVm_gc_run(SF_GC_RUN_CONTINUE_FINISH, owp ? 1 : 0, nullptr, nullptr);
    }

    /**
     * @brief Executes stepping collection evaluating a C-style function pointer MTP predicate.
     *
     * @param[in] mode      Target stepping mode.
     * @param[in] owp       One-Way Pacing flag.
     * @param[in] pred      Function pointer predicate returning 1 to terminate, 0 to continue.
     * @param[in] user_data Context pointer forwarded to pred.
     * @return Reclaimed bytes.
     */
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp,
                            SF_custominiVm_mtp_predicate_fn pred, void* user_data = nullptr) {
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, pred, user_data);
    }

    /**
     * @brief Executes stepping collection evaluating a modern C++ lambda or callable predicate.
     *
     * Example:
     * @code
     * Sfvm.Gc(STEPPING_MTP, false, [](const sfAnatomyStat* s, const sfEnvChange* e) {
     *     return s->falline_emptybox_bytes < 1024 * 1024;
     * });
     * @endcode
     *
     * @tparam Predicate Callable object type matching `bool(const sfAnatomyStat*, const sfEnvChange*)`.
     * @param[in] mode Target stepping mode.
     * @param[in] owp  One-Way Pacing flag.
     * @param[in] pred Lambda or functor predicate.
     * @return Reclaimed bytes.
     */
    template <typename Predicate, typename = typename std::enable_if<!std::is_convertible<Predicate, bool>::value && !std::is_convertible<Predicate, SF_custominiVm_gc_run_mode_t>::value>::type>
    static inline size_t Gc(SF_custominiVm_gc_run_mode_t mode, bool owp, Predicate pred) {
        auto thunk = [](const SF_custominiVm_anatomy_stat_t* s, const SF_custominiVm_env_change_t* e, void* ctx) -> int {
            auto* p = static_cast<Predicate*>(ctx);
            return (*p)(s, e) ? 1 : 0;
        };
        return SF_custominiVm_gc_run(mode, owp ? 1 : 0, thunk, &pred);
    }

    /**
     * @brief Triggers a deterministic deep clean sweep ("Shake the Bed and Sleep") and returns the full audit manifest.
     * @return Completed Caretaker Manifest audit record.
     */
    static inline SF_custominiVm_gc_manifest_t GcReport() {
        return SF_custominiVm_gc_shake_bed();
    }

    /**
     * @brief Retrieves the audit manifest generated by the most recent deep clean sweep.
     * @return Last recorded Caretaker Manifest.
     */
    static inline SF_custominiVm_gc_manifest_t LastManifest() {
        SF_custominiVm_gc_manifest_t m{};
        SF_custominiVm_gc_get_last_manifest(&m);
        return m;
    }

    /**
     * @brief Configures the autonomous garbage collection heuristic model.
     * @param[in] h Selected heuristic (Adaptive, Aggressive, Bureaucratic, or Disabled).
     */
    static inline void SetHeuristic(SF_custominiVm_gc_heuristic_t h) {
        SF_custominiVm_gc_set_heuristic(h);
    }

    /**
     * @brief Queries the currently configured autonomous garbage collection heuristic model.
     * @return Active heuristic enum value.
     */
    static inline SF_custominiVm_gc_heuristic_t GetHeuristic() {
        return SF_custominiVm_gc_get_heuristic();
    }

    /**
     * @brief Enables or disables No-sfGC (Zero-GC) execution mode.
     * @param[in] enabled Set to true for Zero-GC mode, false to restore active collection.
     */
    static inline void SetNoGc(bool enabled) {
        SF_custominiVm_gc_set_no_sfgc(enabled ? 1 : 0);
    }

    /**
     * @brief Checks whether the engine is currently executing in No-sfGC (Zero-GC) mode.
     * @return True if in Zero-GC mode, false otherwise.
     */
    static inline bool IsNoGc() {
        return SF_custominiVm_gc_is_no_sfgc() != 0;
    }

    /** @brief Returns total active on-heap memory volume in bytes. */
    static inline size_t HeapBytes() { return SF_custominiVm_heap_bytes(); }
    /** @brief Returns cumulative rebated memory returned to active runways in bytes. */
    static inline size_t RebatedBytes() { return SF_custominiVm_heap_rebated_bytes(); }
    /** @brief Returns cumulative count of completed GC sweep cycles. */
    static inline uint64_t SweepCount() { return SF_custominiVm_gc_sweep_count(); }
    /** @brief Shuts down the SFVM runtime engine and frees all allocated resources. */
    static inline void Shutdown() { SF_custominiVm_shutdown(); }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/**
 * @brief Global singleton instance of the modern C++ SFVM runtime helper.
 */
inline constexpr sfvm_runtime_t Sfvm{};
#else
static const sfvm_runtime_t Sfvm{};
#endif

/* ========================================================================= */
/* TWO-STAGE LIFECYCLE C++ WRAPPERS (SFVM_Setup & SFVM_VM)                   */
/* ========================================================================= */

/**
 * @struct sfvm_setup_t
 * @brief C++ Stage 1 Initializer: Discovers hardware topology, allocates virtual memory arenas, and mounts VFS.
 */
struct sfvm_setup_t {
    /**
     * @brief Executes Stage 1 initialization.
     * @param[in] cfg Optional custom configuration descriptor (default nullptr).
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int Start(const void* cfg = nullptr) {
        return SF_custominiVm_setup_start(cfg);
    }

    /**
     * @brief Checks whether Stage 1 setup has completed successfully.
     * @return True if setup is complete, false otherwise.
     */
    static inline bool IsReady() {
        return SF_custominiVm_is_setup() != 0;
    }

    /** @brief Function call operator overload executing Start(cfg). */
    inline int operator()(const void* cfg = nullptr) const {
        return Start(cfg);
    }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/** @brief Global callable Stage 1 initializer object: `SFVM_Setup()`. */
inline constexpr sfvm_setup_t SFVM_Setup{};
#else
static const sfvm_setup_t SFVM_Setup{};
#endif

/**
 * @struct sfvm_vm_t
 * @brief C++ Stage 2 Initializer: Ingests bitcode, decompresses code heaps, and boots JIT engine.
 */
struct sfvm_vm_t {
    /**
     * @brief Executes Stage 2 initialization and launches the execution engine.
     * @return MSIMD_SUCCESS (0) on success, or an error status code.
     */
    static inline int Start() {
        return SF_custominiVm_start();
    }

    /**
     * @brief Checks whether the VM has started and is actively executing.
     * @return True if actively running, false otherwise.
     */
    static inline bool IsRunning() {
        return SF_custominiVm_is_started() != 0;
    }

    /** @brief Shuts down the VM execution engine. */
    static inline void Shutdown() {
        SF_custominiVm_shutdown();
    }

    /** @brief Function call operator overload executing Start(). */
    inline int operator()() const {
        return Start();
    }
};

#if defined(__cplusplus) && __cplusplus >= 201703L
/** @brief Global callable Stage 2 initializer object: `SFVM_VM()`. */
inline constexpr sfvm_vm_t SFVM_VM{};
#else
static const sfvm_vm_t SFVM_VM{};
#endif
