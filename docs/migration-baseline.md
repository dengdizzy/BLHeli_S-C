# BLHeli_S migration baseline

The assembly sources under `BLHeli_S SiLabs/` are the behavioral reference.
No portable-C change may change an observable state transition, integer
truncation, saturation, threshold, or timing-critical operation order.

| Area | Reference |
| --- | --- |
| Configuration defaults and decoding | `BLHeli_S.asm:311-330`, `3028-3238` |
| Pulse throttle calibration | `BLHeli_S.asm:3113-3148`, `3250-3418` |
| Input processing | `BLHeli_S.asm:621-941`, `1028-1539` |
| Commutation timing | `BLHeli_S.asm:1902-2422` |
| BEMF and demag | `BLHeli_S.asm:2434-2775` |
| Six-step commutation | `BLHeli_S.asm:2787-2924`, `4336-4418` |

`UNKNOWN`: the routine named `check_temp_voltage_and_limit_power` describes
voltage behavior, while the selected ADC source is the temperature sensor.
Portable code must not add voltage protection until evidence establishes its
required behavior.
