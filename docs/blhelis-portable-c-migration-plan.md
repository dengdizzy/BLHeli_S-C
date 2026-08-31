# BLHeliS Portable Standard C Migration Plan

## Scope and constraints

This document records the repository analysis and defines an incremental migration
from the current BLHeliS Silicon Labs 8051 assembly implementation to:

```text
BLHeliS ESC control core
            |
            v
Hardware abstraction layer (HAL)
            |
            v
MCU-specific implementation
```

The assembly sources under `BLHeli_S SiLabs/` are the behavioral reference.
The migration must preserve commutation order, startup behavior, BEMF behavior,
timing, integer truncation, saturation, thresholds, and operation ordering.
The legacy implementation remains in the repository throughout the migration.
No behavior should be guessed or optimized as part of the migration.

## Repository analysis

### Current implementation

The primary BLHeliS implementation is:

- `BLHeli_S SiLabs/BLHeli_S.asm`
- `BLHeli_S SiLabs/*.inc`
- `BLHeli_S SiLabs/SI_EFM8BB*_Defs.inc`
- `BLHeli_S SiLabs/BLHeliPgm.inc`
- `BLHeli_S SiLabs/BLHeliBootLoad.inc`

The `.inc` files contain board mappings, FET polarity, timer/PCA/ADC/comparator
macros, MCU setup, and register definitions. The `SI_EFM8BB*_Defs.inc` files
define Silicon Labs SFRs and interrupt numbers.

Older Atmel and SiLabs BLHeli implementations are separate legacy references
and are not part of the first BLHeliS migration.

The repository already contains an initial portable-C scaffold:

- `core/throttle_calibration.c/.h`
- `config/blheli_s_config.c/.h`
- `hal/esc_hal.h`
- `storage/storage.h`
- `communication/input_protocol.h`
- `tests/*`
- `Makefile`

`docs/migration-baseline.md` explicitly identifies the assembly as the reference
and warns against changing observable transitions or adding unsupported voltage
protection.

### Entry point and control flow

- Reset vector: `BLHeli_S.asm:4593-4600`, jumping to `pgm_start`.
- Initialization: `pgm_start`, including watchdog, clock, ports, crossbar,
  storage, beeper, and LED setup.
- Input detection and protocol selection: `init_no_signal` and
  `validate_rcp_start`.
- Arming and wait-for-command flow: `arming_start`,
  `program_by_tx_checked`, and `wait_for_power_on`.
- Motor start: `init_start`.
- Closed-loop execution: `run1` through `run6`.
- Stop, stall, and direction-change handling:
  `run_to_wait_for_power_on`, `run6_check_dir`, and `run6_check_speed`.

There is no conventional C main loop. The main loop is implemented as assembly
labels, calls, blocking timer waits, and jumps between state-machine paths.

### Interrupt handlers and timers

| Handler | Current use |
| --- | --- |
| `t0_int` | Extends Timer 0 for high-frequency input timing |
| `t1_int` | DShot frame synchronization and decoding |
| `t2_int` | Periodic RC timeout and stop-counter maintenance |
| `t3_int` | Commutation timing deadline and timer-wrap flag |
| `int0_int` | RC pulse/DShot edge capture and throttle processing |
| `int1_int` | DShot frame edge capture |
| `pca_int` | Safe PWM register transfer |

Timer usage is documented in the assembly header:

- Timer 0: RC pulse measurement and DShot edge timing.
- Timer 1: DShot frame synchronization.
- Timer 2: RC timeout counts and elapsed commutation timing.
- Timer 3: Commutation, advance, BEMF scan, and timeout scheduling.
- PCA0: Hardware PWM generation and PWM update timing.

The C8051 does not automatically disable interrupts on entry. The assembly
therefore explicitly masks and restores interrupts around timing-critical
operations, especially FET switching, PWM updates, timer reads, and beeps.

### Motor-control algorithm

- Six electrical commutation states are implemented by
  `comm1comm2` through `comm6comm1`, with separate reverse paths.
- PWM is damped-light/complementary PWM. Power and damping compare values are
  calculated in the RC interrupt path and transferred safely by the PCA
  interrupt.
- BEMF is detected by selecting the floating phase with `Set_Comp_Phase_*`,
  then polling the comparator in `wait_for_comp_out_low` or
  `wait_for_comp_out_high`.
- Zero-crossing qualification uses expected comparator polarity, repeated
  samples, scan delays, timeout deadlines, and comparator-integrity checks.
- Timing is calculated by `calc_next_comm_timing`,
  `calc_next_comm_timing_fast`, `calc_new_wait_times`, and
  `calc_new_wait_times_fast`.
- The nominal sequence is a commutation wait, timing advance wait, BEMF scan
  delay, and comparator scan. Startup uses different delays and qualification.

### Startup and closed-loop operation

`init_start`:

1. Switches all power off.
2. Reads the initial ADC/temperature value.
3. Calculates startup PWM and initial limits.
4. Selects the requested direction.
5. Performs initial commutations (`comm5comm6`, `comm6comm1`).
6. Initializes and samples virtual commutation timing.
7. Enters `STARTUP_PHASE`.

The startup phase requires 24 commutations before entering
`INITIAL_RUN_PHASE`; the initial-run phase then requires 12 rotations before
normal operation. Stall counters and timeout behavior determine recovery.

### Throttle and communication

Input processing is in `int0_int`, `int1_int`, and `t1_int`.
The implementation supports:

- Regular PWM/PPM input.
- OneShot125.
- OneShot42.
- Multishot.
- DShot150, DShot300, and DShot600.

Protocol detection is performed by trying calibrated timing ranges and checking
accepted-pulse counts. DShot decoding includes checksum validation, command
repeat counting, beeps, direction changes, and saving settings.

Throttle calibration and gain calculation are in `average_throttle`,
`find_throttle_gains`, and `find_throttle_gain`. The existing C equivalents are
`core/throttle_calibration.c/.h`.

### Protection, brake, storage, and programming

- Low-RPM power limiting: `set_pwm_limit_low_rpm`.
- High-RPM limiting: `set_pwm_limit_high_rpm`.
- Temperature/ADC processing: `start_adc_conversion` and
  `check_temp_voltage_and_limit_power`.
- Demag metric and power cut: `wait_for_comm` and comparator routines.
- Stop brake and direction-change brake:
  `run_to_wait_for_power_on` and `run6_check_dir_change`.
- Defaults and parameter decoding:
  `set_default_parameters`, `decode_settings`, and `config/*`.
- Flash-backed EEPROM-like parameter storage:
  the data segment around `BLHeli_S.asm:529-588`, plus
  `BLHeliPgm.inc`.
- TX programming and bootloader:
  `BLHeliPgm.inc`, `BLHeliBootLoad.inc`, and `init_no_signal`.

The routine name `check_temp_voltage_and_limit_power` is ambiguous; the current
ADC source is the temperature sensor. Voltage protection must not be added until
the required behavior is established from evidence.

## Component classification

| Component | Classification | Primary references |
| --- | --- | --- |
| Motor state machine | `CORE_ALGORITHM` | `init_start`, `run1`-`run6` |
| Six-step commutation | `CORE_ALGORITHM` + `HARDWARE` | `comm1comm2`-`comm6comm1` |
| BEMF and zero crossing | `CORE_ALGORITHM` + `HARDWARE` | `wait_for_comp_*`, `setup_zc_scan_timeout` |
| Startup and closed-loop control | `CORE_ALGORITHM` | `init_start`, `run1`-`run6` |
| Timing advance and commutation timing | `CORE_ALGORITHM` | `calc_next_comm_timing*`, `calc_new_wait_times*` |
| Demag and brake | `CORE_ALGORITHM` + `HARDWARE` | `wait_for_comm`, brake paths |
| Throttle scaling and calibration | `CORE_ALGORITHM` | `int0_int`, `find_throttle_gain` |
| PPM/OneShot/Multishot/DShot | `COMMUNICATION` + `HARDWARE` | input interrupt handlers |
| DShot commands and TX programming | `COMMUNICATION` + `STORAGE` | `dshot_*`, `BLHeliPgm.inc` |
| PWM calculation and damping | `CORE_ALGORITHM` + `HARDWARE` | PWM registers and `pca_int` |
| ADC and comparator peripherals | `HARDWARE` | ADC/comparator macros |
| FET/phase/LED/beeper mappings | `CONFIG` + `HARDWARE` | variant `.inc` files |
| Parameter defaults and decoding | `CONFIG` | `config/*`, `decode_settings` |
| Flash/EEPROM-like storage | `STORAGE` + `MCU_SPECIFIC` | EEPROM segment and programming code |
| Reset, clock, ports, crossbar, watchdog | `MCU_SPECIFIC` | `pgm_start`, variant `.inc` files |
| SFR/register definitions | `MCU_SPECIFIC` | `SI_EFM8BB*_Defs.inc` |
| Assembly macros and directives | `MCU_SPECIFIC` | all BLHeliS `.inc` files |
| Global RAM flags, counters, and registers | `CORE_ALGORITHM` + `MCU_SPECIFIC` | `BLHeli_S.asm:347-526` |
| Interrupt masking and atomic sections | `HARDWARE` + `MCU_SPECIFIC` | interrupt and commutation routines |
| Bootloader handoff | `MCU_SPECIFIC` | `init_no_signal`, `BLHeliBootLoad.inc` |

## Incremental migration phases

Each phase must retain the original assembly and use it as a behavioral oracle.
New code should be introduced alongside the legacy build, not as a replacement.

### Phase 0 — Repository analysis

- **Files:** `README.md`, `docs/migration-baseline.md`, BLHeliS assembly and
  includes, Makefiles, portable C scaffold, and tests.
- **Functions:** All labels, macros, interrupt handlers, and register accesses.
- **New files:** None.
- **Dependencies:** None.
- **Expected changes:** Record behavior, timing units, ownership, state
  transitions, and unknown behavior.
- **Validation:** Cross-check the inventory against labels, macros, registers,
  and build targets.
- **Risks:** Misreading undocumented behavior or mixing legacy BLHeli with
  BLHeliS.
- **Kind:** Analysis.

### Phase 1 — Portable C architecture

- **Files:** `core/`, `config/`, `hal/`, `storage/`, `communication/`,
  `tests/`, and `Makefile`.
- **Functions:** Existing configuration and throttle functions define initial
  portable-C conventions.
- **New files:** `core/esc_control.h/.c`, `core/esc_state.h`,
  `core/esc_types.h`, `core/esc_events.h`.
- **Dependencies:** Standard C11 build and tests.
- **Expected changes:** Introduce an explicit context/state API and prevent new
  core code from depending on implicit hardware globals.
- **Validation:** `make test`, `make check-core`, strict C11 compilation.
- **Risks:** Changed widths, signedness, truncation, or transition order.
- **Kind:** Architectural and algorithmic boundary.

### Phase 2 — HAL interface

- **Files:** `hal/esc_hal.h`, assembly hardware macros, and variant includes.
- **Functions:** PWM commit, FET disable, comparator phase/read, time read,
  deadline scheduling, ADC, input capture, interrupts, clock, LEDs, beeper,
  and storage.
- **New files:** `hal/esc_hal_motor.h`, `hal/esc_hal_input.h`,
  `hal/esc_hal_adc.h`, `hal/esc_hal_storage.h`,
  `hal/esc_hal_platform.h`.
- **Dependencies:** Phase 1 types and context.
- **Expected changes:** Define exact units, atomicity, timing semantics, and
  valid ranges for HAL calls.
- **Validation:** Mock-HAL tests and `make check-core`.
- **Risks:** An abstraction that hides or alters required timing.
- **Kind:** Hardware interface.

### Phase 3 — Data structures

- **Files:** `BLHeli_S.asm:347-526`, `config/*`, and throttle structures.
- **Functions:** All assembly global flags, counters, timers, PWM values, and
  configuration fields.
- **New files:** `core/esc_context.h`, `core/motor_state.h`,
  `core/timing_state.h`, `core/input_state.h`,
  `core/protection_state.h`, `storage/parameter_layout.h`.
- **Dependencies:** Phases 1–2.
- **Expected changes:** Map every variable to fixed-width C fields while
  preserving byte order and reset/persistence behavior.
- **Validation:** Layout checks, initialization tests, overflow tests, and
  trace comparisons.
- **Risks:** C promotion and structure-layout differences.
- **Kind:** Algorithmic data modeling.

### Phase 4 — Six-step commutation

- **Files:** `BLHeli_S.asm:2787-2924` and phase/FET variant macros.
- **Functions:** Six forward and reverse commutation routines.
- **New files:** `core/commutation.c/.h`, `hal/phase_mapping.h`.
- **Dependencies:** Phases 1–3 and phase/PWM HAL operations.
- **Expected changes:** Encode the exact sequence, including FET off-before-on,
  comparator selection, RPM output, PWM reapplication, and interrupt masking.
- **Validation:** Table-driven sequence tests and waveform capture.
- **Risks:** Shoot-through, wrong phase order, polarity, or reverse mapping.
- **Kind:** Algorithmic sequence plus hardware actuation.

### Phase 5 — PWM

- **Files:** `int0_int_set_pwm_registers`, `pca_int`, and PWM macros.
- **Functions:** Power/damping calculation, limits, and safe PCA transfer.
- **New files:** `core/pwm_control.c/.h`.
- **Dependencies:** Phases 2–4.
- **Expected changes:** Preserve compare polarity, damped-light behavior,
  FET-on delay, saturation, and deferred register updates.
- **Validation:** Arithmetic vectors, register traces, and oscilloscope tests.
- **Risks:** Inversion, unsafe update timing, deadtime, or output glitches.
- **Kind:** Algorithmic calculation and hardware timing.

### Phase 6 — BEMF

- **Files:** `wait_for_comp_out_low/high`, comparator macros, and initialization.
- **Functions:** Comparator selection, expected polarity, and sample handling.
- **New files:** `core/bemf.c/.h`, `hal/comparator.h`.
- **Dependencies:** Phases 2–5.
- **Expected changes:** Move comparator decisions into core; keep configuration and
  reads in HAL.
- **Validation:** Synthetic comparator streams, timeout/demag tests, and
  hardware comparator traces.
- **Risks:** Wrong polarity, stale reads, or polling latency.
- **Kind:** Algorithmic detection with hardware input.

### Phase 7 — Zero crossing

- **Files:** `wait_before_zc_scan`, `setup_zc_scan_timeout`, comparator logic,
  and Timer 3 handling.
- **Functions:** Scan delay, timeout, qualification count, and integrity checks.
- **New files:** `core/zero_crossing.c/.h`, `hal/timing_deadline.h`.
- **Dependencies:** Phases 2, 3, and 6.
- **Expected changes:** Represent zero-crossing states and deadlines explicitly,
  preserving startup-specific counts and extensions.
- **Validation:** Deterministic event traces and timing-instrumented hardware.
- **Risks:** Off-by-one timing, false crossings, or lost synchronization.
- **Kind:** Timing-critical algorithm.

### Phase 8 — Startup

- **Files:** `init_start`, `set_startup_pwm`, `initialize_timing`, startup
  branches in `run1`-`run6`.
- **Functions:** Initial ADC, startup PWM, initial commutations, counters, and
  startup-to-initial-run transition.
- **New files:** `core/startup.c/.h`.
- **Dependencies:** Phases 3–7.
- **Expected changes:** Preserve startup power, virtual commutation setup,
  24-commutation transition, timeout, and stall handling.
- **Validation:** Simulated startup, stall, throttle, and bench waveform tests.
- **Risks:** Twitch, wrong direction, missed transition, or altered acceleration.
- **Kind:** Algorithmic and hardware timing.

### Phase 9 — Closed-loop operation

- **Files:** `run1`-`run6`, timing routines, `wait_for_comm`, and comparator
  integrity logic.
- **Functions:** Run-state transitions and synchronization failure recovery.
- **New files:** `core/run_control.c/.h`.
- **Dependencies:** Phases 4–8.
- **Expected changes:** Implement an event-driven equivalent without changing
  operation order.
- **Validation:** Golden traces, synthetic motor model, sync-loss tests, and
  hardware motor tests.
- **Risks:** Moving PWM, ADC, timing, or commutation operations.
- **Kind:** Core algorithm.

### Phase 10 — Throttle

- **Files:** `int0_int`, `t1_int`, `int1_int`, throttle calibration routines,
  and `core/throttle_calibration.c/.h`.
- **Functions:** Protocol scaling, qualification, gain, deadband, calibration,
  timeout, and stop counting.
- **New files:** `core/throttle_input.c/.h`,
  `communication/ppm_decoder.c/.h`, `communication/dshot_decoder.c/.h`.
- **Dependencies:** Phases 2–3 and existing config/throttle code.
- **Expected changes:** Preserve all constants, units, gain loops, saturation,
  and direction transitions.
- **Validation:** Existing tests, captured input vectors, and timing tests.
- **Risks:** Any arithmetic or unit-conversion change affects throttle response.
- **Kind:** Core algorithm with hardware capture dependency.

### Phase 11 — Timing advance

- **Files:** `calc_new_wait_times`, fast path, `wait_advance_timing`, and
  commutation timing routines.
- **Functions:** Averaging, advance, scan, timeout, high-RPM, and minimum waits.
- **New files:** `core/timing_control.c/.h`.
- **Dependencies:** Phases 3, 7, and 9.
- **Expected changes:** Preserve 8/16-bit arithmetic and exact timing branches.
- **Validation:** Exhaustive timing-vector comparison with assembly behavior.
- **Risks:** Integer promotion or signed-negation differences.
- **Kind:** Timing-critical algorithm.

### Phase 12 — Demag

- **Files:** Comparator routines, `wait_for_comm`, and timing calculations.
- **Functions:** Demag flag, sliding metric, threshold power cut, and compensation.
- **New files:** `core/demag.c/.h`.
- **Dependencies:** Phases 6–9 and configuration decoding.
- **Expected changes:** Preserve thresholds 130/160/255, metric minimum 120,
  update ordering, and PWM cut/reapplication.
- **Validation:** Synthetic demag sequences and hard-acceleration tests.
- **Risks:** Excessive cuts or synchronization loss.
- **Kind:** Core algorithm.

### Phase 13 — Brake

- **Files:** Direction-change, stop, and `switch_power_off` paths.
- **Functions:** Stop brake, direction-change brake, termination thresholds,
  and all-FET activation.
- **New files:** `core/brake.c/.h`.
- **Dependencies:** Phases 4, 8, 9, and 10.
- **Expected changes:** Preserve brake settings, direction sequence, speed
  thresholds, and FET ordering.
- **Validation:** Stop and reversal tests with brake enabled and disabled.
- **Risks:** Excessive current, wrong reversal, or unsafe FET state.
- **Kind:** Algorithmic and hardware actuation.

### Phase 14 — Protection

- **Files:** ADC and temperature routines plus low/high-RPM limit routines.
- **Functions:** ADC scheduling, temperature averaging, thermal PWM limits,
  and RPM power limits.
- **New files:** `core/protection.c/.h`, `hal/adc.h`.
- **Dependencies:** Phases 2–3, 5, 9, and 12.
- **Expected changes:** Preserve current behavior; do not infer or add voltage
  protection from the ambiguous routine name.
- **Validation:** ADC vectors, thermal tests, RPM-limit tests, and target ADC
  calibration.
- **Risks:** Misinterpreted sensor behavior or changed safety thresholds.
- **Kind:** Algorithmic policy plus hardware measurement.

### Phase 15 — Storage/configuration

- **Files:** `set_default_parameters`, `decode_settings`, EEPROM segment,
  `BLHeliPgm.inc`, `config/*`, and `storage/storage.h`.
- **Functions:** Defaults, signature validation, decoding, flash read/write.
- **New files:** `storage/blheli_s_storage.c/.h`,
  `storage/blheli_s_layout.c/.h`, `config/blheli_s_parameters.c/.h`.
- **Dependencies:** Phases 1–3 and storage HAL.
- **Expected changes:** Define a versioned layout and preserve legacy layout
  compatibility and safe write transactions.
- **Validation:** Binary layout, corruption, round-trip, and target flash tests.
- **Risks:** Flash corruption or incompatible existing configuration.
- **Kind:** Storage/configuration.

### Phase 16 — Communication

- **Files:** `communication/input_protocol.h`, DShot routines,
  `BLHeliPgm.inc`, and `BLHeliBootLoad.inc`.
- **Functions:** Protocol framing, DShot commands, TX programming, and bootloader
  handoff.
- **New files:** `communication/input_capture.c/.h`,
  `communication/dshot_commands.c/.h`,
  `communication/tx_programming.c/.h`, `communication/bootloader_api.h`.
- **Dependencies:** Phases 2, 3, 10, and 15.
- **Expected changes:** Separate framing, throttle interpretation, commands,
  and storage operations.
- **Validation:** Protocol vectors, signal replay, command repeat tests, and
  bootloader integration.
- **Risks:** Incorrect auto-detection or unintended writes.
- **Kind:** Communication with hardware timing dependency.

### Phase 17 — MCU implementation

- **Files:** Silicon Labs includes, register definitions, and existing build
  targets.
- **Functions:** Reset, timers, PCA PWM, comparator, ADC, GPIO/FET mapping,
  interrupts, clock switching, and flash storage.
- **New files:** `platform/silabs_efm8/esc_hal.c`,
  `interrupts.c`, `timers.c`, `pwm.c`, `comparator.c`, and `adc.c`.
  Future STM32, AT32, and GD32 implementations should be separate platforms.
- **Dependencies:** Stable HAL and migrated core.
- **Expected changes:** Move register access and interrupt vectors into the
  platform layer.
- **Validation:** Target builds, interrupt-latency measurements, waveform
  capture, and motor bench tests.
- **Risks:** Peripheral semantics and latency differ between MCUs.
- **Kind:** Hardware/MCU-specific.

### Phase 18 — Testing

- **Files:** Existing tests, Makefile, and all migrated modules.
- **Functions:** Every migrated algorithm, state transition, timing calculation,
  commutation table, protocol decoder, and protection path.
- **New files:** Tests for commutation, timing, BEMF, startup, PWM, protocols,
  protection, and trace replay.
- **Dependencies:** All prior phases.
- **Expected changes:** Establish golden vectors and traces derived from the
  assembly reference.
- **Validation:** Host tests, strict core checks, replay tests, hardware-in-loop
  tests, and oscilloscope/logic-analyzer captures.
- **Risks:** Tests may validate a mistaken C interpretation rather than legacy
  behavior.
- **Kind:** Verification.

### Phase 19 — Removal of legacy implementation

- **Files:** Original BLHeliS assembly and legacy build targets.
- **Functions:** Legacy entry point, handlers, commutation, PWM, storage, and
  programming paths.
- **New files:** None unless compatibility packaging requires them.
- **Dependencies:** Successful Phase 18 validation on every supported target.
- **Expected changes:** Remove or archive legacy implementation only after
  feature parity and production qualification.
- **Validation:** Full regression, clean builds, binary/behavioral comparison,
  and production hardware qualification.
- **Risks:** Removing the reference too early eliminates the behavioral oracle.
- **Kind:** Final repository/build migration.

## Final acceptance criteria

1. The original BLHeliS behavior remains available for comparison until Phase 19.
2. Portable core code compiles as standard C11 without MCU headers, SFR names,
   assembly, or direct register access.
3. All hardware operations are mediated through HAL interfaces.
4. MCU-specific code is isolated under platform implementations.
5. Commutation sequence, startup, BEMF, zero crossing, timing, throttle,
   demag, brake, protection, storage, and communication behavior match the
   assembly reference.
6. Host tests, trace replay, target builds, timing measurements, and motor
   hardware validation pass before legacy removal.
