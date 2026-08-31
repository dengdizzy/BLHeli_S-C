# Phase 19 qualification gate

Phase 19 is conditional. The legacy implementation remains the behavioral
oracle and is not removed by the normal build or test targets.

The root default build is portable C only. The preserved legacy build is
available explicitly with `make legacy-reference-build` and is never invoked
by `make`, `make test`, or `make phase19-check`.

The legacy tree is archived as a read-only reference. Run
`make legacy-reference-immutable` to verify the tracked files against
`docs/phase19-legacy.sha256`. If the check fails, restore the archive from its
approved tag before using it for behavioral comparison.

Run `make phase19-check` only after the following evidence has been completed
and recorded by the project maintainer:

| Gate | Required evidence |
| --- | --- |
| Host validation | `make test` and `make check-core` pass |
| Target validation | A real EFM8 target build passes with warnings enabled |
| Behavioral equivalence | C and assembly traces match for every migrated path |
| Timing equivalence | Measured gate, PWM, deadline, comparator, and interrupt timing has no regression |
| Hardware validation | Motor bench tests pass for startup, closed loop, reversal, brake, protection, and stall |
| Compatibility | Storage layout and communication behavior remain compatible |
| Recovery | A tagged or archived copy of the legacy reference is available |
| Approval | The project maintainer explicitly authorizes the current removal step |

The gate fails closed unless every `PHASE19_*` variable is set to `1`:

```sh
PHASE19_TARGET=1 \
PHASE19_BEHAVIOR=1 \
PHASE19_TIMING=1 \
PHASE19_HARDWARE=1 \
PHASE19_COMPATIBILITY=1 \
PHASE19_RECOVERY=1 \
PHASE19_APPROVAL=1 \
make phase19-check
```

This gate does not delete, rename, or modify the legacy sources. Removal must
be performed as separate, explicitly reviewed commits after the gate passes.

When the project maintainer has accepted the required real-target, timing,
hardware, behavioral, and compatibility evidence, the repository provides
`make phase19-qualified` as the explicit readiness step. It still preserves
the legacy source tree; any removal remains a separate reviewed change.
