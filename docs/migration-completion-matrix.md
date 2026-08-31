# C migration completion matrix

The assembly under `BLHeli_S SiLabs/` remains the behavioral oracle.  This
matrix records implementation evidence, rather than declaring target or
hardware equivalence.

| Assembly path | C ownership | Integrated into core control | HAL-connected | Deterministic host evidence | Hardware evidence |
| --- | --- | --- | --- | --- | --- |
| `init_start` (`BLHeli_S.asm:4274-4326`) | `core/esc_control.c`, `core/startup.c` | Startup state initialization only | No | Startup unit tests | No |
| `run1`–`run6` (`4336-4544`) | `core/esc_control.c`, `core/run_control.c` | Per-run event, phase transitions, and run-step descriptor | No | Core orchestration and run-step descriptor tests | No |
| Commutation (`2787-2924`) | `core/commutation.c`, `hal/phase_mapping.c` | State sequencing only | Mapping only | Table/trace tests | No |
| Comparator/BEMF and demag (`2434-2775`) | `core/bemf.c`, `core/zero_crossing.c`, `core/demag.c` | Event inputs only | Interface/model only | Unit/trace tests | No |
| PCA PWM transfer (`pca_int`) | `core/pwm_control.c`, platform PWM model | No | Model only | Unit tests | No |
| Input interrupts (`int0_int`, `int1_int`, `t1_int`) | throttle and DShot modules | No | Interface/model only | Decoder tests | No |
| TX programming and bootloader | No complete C equivalent | No | No | No | No |

## `init_start` / `run1`–`run6` path map

`init_start` clears motor state, performs initial ADC/temperature processing,
calculates startup PWM, selects direction, executes `comm5comm6` then
`comm6comm1`, initializes virtual timing twice, and enters `STARTUP_PHASE`.
The portable core currently represents only the state initialization; ADC,
clock, FET, PWM, comparator, interrupt, and timing actions remain platform/HAL
work.

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

The assembly increments `Startup_Cnt` during comparator integrity evaluation.
At 24 it enters initial run and initializes the rotation count to 12; the same
`run6` pass then decrements that count.  The core preserves that observable
transition.  Completion of each six-step cycle is identified when the portable
run state returns to step 1.

`check_temp_voltage_and_limit_power` remains **UNKNOWN**: its selected ADC
source is temperature, so no voltage-protection behavior has been inferred or
added.
