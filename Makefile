CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic

CORE_SOURCES = core/esc_control.c core/throttle_calibration.c core/throttle_input.c core/commutation.c core/pwm_control.c core/bemf.c core/zero_crossing.c core/startup.c core/run_control.c core/timing_control.c core/demag.c core/brake.c core/protection.c config/blheli_s_config.c hal/phase_mapping.c storage/blheli_s_storage.c communication/dshot_decoder.c platform/silabs_efm8/esc_hal.c platform/silabs_efm8/timers.c platform/silabs_efm8/pwm.c platform/silabs_efm8/comparator.c platform/silabs_efm8/adc.c
TEST_SOURCES = tests/test_main.c tests/test_config.c tests/test_throttle_calibration.c tests/test_throttle_input.c tests/test_core_architecture.c tests/test_hal_interface.c tests/test_state_layout.c tests/test_commutation.c tests/test_pwm_control.c tests/test_bemf.c tests/test_zero_crossing.c tests/test_startup.c tests/test_run_control.c tests/test_timing_control.c tests/test_demag.c tests/test_brake.c tests/test_protection.c tests/test_storage.c tests/test_dshot_decoder.c tests/test_silabs_platform.c tests/test_trace_replay.c
TEST_BINARY = build/blheli_s_core_tests

.PHONY: test check-core legacy-reference-check phase19-check clean

test: $(TEST_BINARY)
	$(TEST_BINARY)

$(TEST_BINARY): $(CORE_SOURCES) $(TEST_SOURCES)
	mkdir -p build
	$(CC) $(CFLAGS) -I. $(CORE_SOURCES) $(TEST_SOURCES) -o $@

check-core:
	! grep -REn '#include.*(mcu|hal)|\b(volatile|PCA0|TMR[0-9]|ADC0|CMP0|SFR|MOVX|MOVC)\b' core

legacy-reference-check:
	test -f "BLHeli_S SiLabs/BLHeli_S.asm"
	test -f "BLHeli_S SiLabs/BLHeliPgm.inc"
	test -f "BLHeli_S SiLabs/BLHeliBootLoad.inc"

phase19-check: test check-core legacy-reference-check
	test "$${PHASE19_TARGET:-0}" = 1
	test "$${PHASE19_BEHAVIOR:-0}" = 1
	test "$${PHASE19_TIMING:-0}" = 1
	test "$${PHASE19_HARDWARE:-0}" = 1
	test "$${PHASE19_COMPATIBILITY:-0}" = 1
	test "$${PHASE19_RECOVERY:-0}" = 1
	test "$${PHASE19_APPROVAL:-0}" = 1
	@echo "Phase 19 qualification evidence accepted; legacy removal remains a separate step."

clean:
	rm -rf build
