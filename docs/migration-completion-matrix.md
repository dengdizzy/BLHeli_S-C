# C migration completion matrix

The assembly under `BLHeli_S SiLabs/` remains the behavioral oracle.  This
matrix records implementation evidence, rather than declaring target or
hardware equivalence.

| Assembly path | C ownership | Integrated into core control | HAL-connected | Deterministic host evidence | Hardware evidence |
| --- | --- | --- | --- | --- | --- |
| `init_start` (`BLHeli_S.asm:4274-4326`) | `core/esc_control.c`, `core/startup.c` | Startup state, action trace, PWM, and timing initialization models | No | Startup unit tests | No |
| `run1`–`run6` (`4336-4544`) | `core/esc_control.c`, `core/run_control.c` | Per-run event, phase transitions, and run-step descriptor | No | Core orchestration and run-step descriptor tests | No |
| Commutation (`2787-2924`) | `core/commutation.c`, `hal/phase_mapping.c` | Forward action trace and state sequencing | Mapping only | Table/trace tests | No |
| Comparator/BEMF and demag (`2434-2775`) | `core/bemf.c`, `core/zero_crossing.c`, `core/demag.c` | Comparator wait and zero-crossing deadline models | Interface/model only | Unit/trace tests | No |
| PCA PWM transfer (`pca_int`) | `core/pwm_control.c`, platform PWM model | Deferred-transfer model only | Model only | Unit tests for pending/current state and PCA update windows | No |
| Input interrupts (`int0_int`, `int1_int`, `t1_int`) | throttle and DShot modules | No | Interface/model only | Decoder tests | No |
| TX programming and bootloader | No complete C equivalent | No | No | No | No |

## `init_start` / `run1`–`run6` path map

`init_start` clears motor state, performs initial ADC/temperature processing,
calculates startup PWM, selects direction, executes `comm5comm6` then
`comm6comm1`, runs `initialize_timing` around the virtual timing calculations,
and enters `STARTUP_PHASE`.
The portable core records this as a startup action trace and models
`set_startup_pwm` plus the `initialize_timing` value of `Comm_Period4x=0xf000`.
ADC, clock, FET, PWM, comparator, interrupt, and real timer actions remain
platform/HAL work.

Each `runN` performs comparator qualification, `wait_for_comm`, the matching
commutation operation, and timing calculation.  `run2` adjusts the RPM power
limit and `run6` schedules ADC/protection plus startup, initial-run, stop, and
direction checks.  The core event API models only the already-migrated
comparator result, demag status, timeout status, and throttle value after that
ordered hardware sequence has occurred.

`core/run_control.c` now exposes a deterministic descriptor for the assembly
`run1` through `run6` path map: the powered phase, PWM phase, comparator phase,
expected comparator transition, next commutation step, and the special `run2`
and `run6` side effects.  This is trace metadata only; it does not perform FET,
PWM, comparator, ADC, timer, interrupt, or protection hardware operations.

`core/commutation.c` exposes a forward-only action trace for `comm1comm2`
through `comm6comm1`, preserving the observed order of RPM output update,
interrupt mask, FET-off, FET/PWM reapplication, interrupt restore, and
comparator phase selection.  Reverse paths are present in the assembly, but the
portable action-trace API currently fails closed for reverse direction until the
reverse table is separately reviewed against the hardware variant macros.

`core/pwm_control.c` models the PWM register values staged by the input path and
the deferred transfer performed by `pca_int`.  The host model tracks pending and
current PWM state, the current power compare high byte, FETON_DELAY zero versus
non-zero behavior, and the PCA counter high-byte windows that gate transfers.
It does not write PCA registers or provide hardware timing evidence.

`core/bemf.c` models the `wait_for_comp_out_low` and
`wait_for_comp_out_high` comparator expectations without reading hardware.  The
model records expected polarity, direction-change brake inversion, startup and
high-RPM sample counts, timeout input, and demag flag state for host tests.
Timer 3 scheduling, comparator register reads, and hardware polling latency
remain HAL/platform work.

`core/zero_crossing.c` models the `wait_before_zc_scan` and
`setup_zc_scan_timeout` deadline state without touching Timer 3 registers.  It
tracks scan delay, timeout duration, startup/initial-run long timeout scaling,
startup timeout extension countdown, comparator-read count, and Timer 3 action
intent for deterministic host tests.  Real Timer 3 reload values, interrupt
latency, and comparator polling remain platform/HAL work.

The assembly increments `Startup_Cnt` during comparator integrity evaluation.
At 24 it enters initial run and initializes the rotation count to 12; the same
`run6` pass then decrements that count.  The core preserves that observable
transition.  Completion of each six-step cycle is identified when the portable
run state returns to step 1.

`check_temp_voltage_and_limit_power` remains **UNKNOWN**: its selected ADC
source is temperature, so no voltage-protection behavior has been inferred or
added.
