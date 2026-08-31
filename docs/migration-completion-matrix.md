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
| Brake and stop (`3012-3016`, `4481-4574`) | `core/brake.c` | Host-level stop brake, direction-change brake, switch-power-off, and brake-on-stop intent model | No | Brake unit tests | No |
| Temperature/power protection (`1758-1868`) | `core/protection.c` | Host-level ADC cadence, temperature average, PWM temperature limit, and PWM recovery intent model | No | Protection unit tests | No |
| Input interrupts and DShot commands (`int0_int`, `int1_int`, `t1_int`, `3988-4267`) | throttle and DShot modules | Host-level throttle qualification, scaling, counters, DShot frame decode, command latch, repeat count, and command execution intents | Interface/model only | Throttle input, decoder, and command tests | No |
| Parameters and settings (`3029-3338`) | `config/blheli_s_config.c`, `storage/blheli_s_storage.c` | Host-level parameter offsets, defaults, signature checks, record encode/decode, and `decode_settings` derived fields | No | Config and storage tests | No |
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

`communication/dshot_decoder.c` validates the DShot checksum and separates
normal throttle values from the special command range.  The command model
records the telemetry-bit gate, latched command, repeat count, beep intents,
direction-change intents, temporary direction intents, save-settings intent,
and clear versus don't-clear command behavior.  It does not execute real
beeper, FET, flash, EEPROM, TX programming, bootloader, or DShot edge-timing
hardware operations.

`core/brake.c` models the `run6` stop and direction-change brake decisions plus
the `switch_power_off` action order.  It records stop thresholds, RC timeout
stop intent, direction-change brake start and completion, initial-run restart
count, PWM limit restore intent, all-FET-off ordering, and brake-on-stop
all-comFET-on intent.  It does not switch real FETs or write PWM hardware.

`core/protection.c` models the `start_adc_conversion` intent and the
`check_temp_voltage_and_limit_power` host-level cadence.  It records ADC
conversion-count increments, the every-eighth-call temperature path, ADC
complete wait/read/stop/reset intents, temperature-average update, temperature
PWM limit steps, and the non-temperature-path `Pwm_Limit += 16` recovery with
saturation.  It does not read real ADC hardware, start or stop an ADC
peripheral, or provide voltage/power hardware evidence.

`storage/parameter_layout.h` and `storage/blheli_s_storage.c` model the
currently used EEPROM parameter record offsets, layout revision, signatures,
reserved placeholder bytes, and blank name field.  `config/blheli_s_config.c`
models default settings and the host-visible `decode_settings` derived fields:
direction flags, decoded startup power, low-RPM power slope, demag power-off
threshold, temperature protection limit, and the switch-power-off intent at the
end of settings decode.  Governor, BEC, PWM-frequency, dither, TX programming
writes, flash/EEPROM HAL behavior, and bootloader behavior remain unmodeled.

The assembly increments `Startup_Cnt` during comparator integrity evaluation.
At 24 it enters initial run and initializes the rotation count to 12; the same
`run6` pass then decrements that count.  The core preserves that observable
transition.  Completion of each six-step cycle is identified when the portable
run state returns to step 1.

`check_temp_voltage_and_limit_power` voltage behavior remains **UNKNOWN** beyond
the observed host-level `Pwm_Limit` recovery increment; no voltage-protection
ADC threshold or hardware behavior has been inferred or added.
