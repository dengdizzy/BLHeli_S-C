# C migration completion matrix

The assembly under `BLHeli_S SiLabs/` remains the behavioral oracle.  This
matrix records implementation evidence, rather than declaring target or
hardware equivalence.

| Assembly path | C ownership | Integrated into core control | HAL-connected | Deterministic host evidence | Hardware evidence |
| --- | --- | --- | --- | --- | --- |
| `init_start` (`BLHeli_S.asm:4274-4326`) | `core/esc_control.c`, `core/startup.c` | Startup state, action trace, PWM, and timing initialization models | No | Startup unit tests | No |
| `run1`–`run6` (`4336-4544`) | `core/esc_control.c`, `core/run_control.c` | Per-run event, phase transitions, run-step descriptor, and ordered closed-loop trace | No | Core orchestration, run-step descriptor, and trace tests | No |
| Commutation (`2787-2924`) | `core/commutation.c`, `hal/phase_mapping.c` | Forward action trace and state sequencing | Mapping only | Table/trace tests | No |
| Comparator/BEMF and demag (`2434-2775`) | `core/bemf.c`, `core/zero_crossing.c`, `core/demag.c` | Comparator wait, zero-crossing deadline, demag metric, timeout extension, and power-cut intent models | Interface/model only | Unit/trace tests | No |
| PCA PWM transfer (`pca_int`) | `core/pwm_control.c`, platform PWM model | Deferred-transfer model only | Model only | Unit tests for pending/current state and PCA update windows | No |
| Timing advance (`1919-2438`) | `core/timing_control.c` | Host-level commutation period, timing advance, wait calculation, and Timer3 intent model | No | Timing unit tests | No |
| Input interrupts (`int0_int`, `int1_int`, `t1_int`) | throttle and DShot modules | Host-level throttle qualification, scaling, counters, and DShot frame decode | Interface/model only | Throttle input and decoder tests | No |
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

`core/run_control.c` now exposes deterministic descriptors and ordered host
traces for the assembly `run1` through `run6` path map: zero-cross scan intent,
comparator wait direction, comparator-integrity evaluation, commutation wait,
the commutation transition, next timing calculation, and the special `run2` and
`run6` side-effect intents.  This is trace metadata only; it does not perform
FET, PWM, comparator, ADC, timer, interrupt, or protection hardware operations.

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

`core/demag.c` models the `wait_for_comm` demag metric update, threshold-based
power cut intent, comparator wrong-read timeout extension intent, high-RPM
versus low-RPM extension reload intent, startup/initial-run demag suppression,
and the need to reapply power on a later commutation.  It does not write Timer 3
reload registers or switch PWM/FET hardware.

`core/zero_crossing.c` models the `wait_before_zc_scan` and
`setup_zc_scan_timeout` deadline state without touching Timer 3 registers.  It
tracks scan delay, timeout duration, startup/initial-run long timeout scaling,
startup timeout extension countdown, comparator-read count, and Timer 3 action
intent for deterministic host tests.  Real Timer 3 reload values, interrupt
latency, and comparator polling remain platform/HAL work.

`core/timing_control.c` models the `calc_next_comm_timing`,
`calc_new_wait_times`, fast-path, and `wait_advance_timing` intent at host
level.  It tracks previous timestamps, startup versus normal period updates,
high-RPM fast-path selection, demag timing compensation, minimum wait clamping,
startup wait overrides, and the Timer 3 action that would arm zero-cross scan.
It does not read Timer 2, write Timer 3 reload registers, or provide hardware
timing evidence.

`core/throttle_input.c` models host-level input qualification for the
`int0_int`/`t1_int` path: protocol scaling intent for PPM, OneShot125,
OneShot42, MultiShot, and DShot frame-level input, 900us/2235us range checks,
outside-range count, stop count, timeout countdown, bidirectional deadband and
direction state, and startup boost intent.  It does not install interrupt
vectors, read timer capture registers, or model DShot edge-buffer timing.

The assembly increments `Startup_Cnt` during comparator integrity evaluation.
At 24 it enters initial run and initializes the rotation count to 12; the same
`run6` pass then decrements that count.  The core preserves that observable
transition.  Completion of each six-step cycle is identified when the portable
run state returns to step 1.

`check_temp_voltage_and_limit_power` remains **UNKNOWN**: its selected ADC
source is temperature, so no voltage-protection behavior has been inferred or
added.
