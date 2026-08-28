# Algorithm trace format

Each migrated module is verified with a deterministic trace. Record one
newline-delimited record per externally observable action:

```
sequence,event,input,state_before,output,state_after,tick
```

`event` is one of `input`, `timer`, `comparator`, `pwm_request`,
`gate_change`, `deadline`, `fault`, or `storage`. `tick` uses the hardware
timer tick only when that unit is evidenced by the original implementation;
otherwise it must be `UNKNOWN`.

For timing-critical paths, trace ordering is normative. A matching final state
is insufficient if the gate, comparator, or timer action sequence differs.
