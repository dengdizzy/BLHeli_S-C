CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -pedantic

CORE_SOURCES = core/throttle_calibration.c config/blheli_s_config.c
TEST_SOURCES = tests/test_main.c tests/test_config.c tests/test_throttle_calibration.c
TEST_BINARY = build/blheli_s_core_tests

.PHONY: test check-core clean

test: $(TEST_BINARY)
	$(TEST_BINARY)

$(TEST_BINARY): $(CORE_SOURCES) $(TEST_SOURCES)
	mkdir -p build
	$(CC) $(CFLAGS) -I. $(CORE_SOURCES) $(TEST_SOURCES) -o $@

check-core:
	! grep -REn '#include.*(mcu|hal)|\b(volatile|PCA0|TMR[0-9]|ADC0|CMP0|SFR|MOVX|MOVC)\b' core

clean:
	rm -rf build
