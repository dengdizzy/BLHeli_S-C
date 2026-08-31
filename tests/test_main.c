void test_config(void);
void test_throttle_calibration(void);
void test_core_architecture(void);
void test_hal_interface(void);
void test_state_layout(void);
void test_commutation(void);
void test_pwm_control(void);
void test_bemf(void);
void test_zero_crossing(void);
void test_startup(void);
void test_run_control(void);
void test_throttle_input(void);
void test_timing_control(void);
void test_demag(void);

int main(void)
{
    test_config();
    test_throttle_calibration();
    test_core_architecture();
    test_hal_interface();
    test_state_layout();
    test_commutation();
    test_pwm_control();
    test_bemf();
    test_zero_crossing();
    test_startup();
    test_run_control();
    test_throttle_input();
    test_timing_control();
    test_demag();
    return 0;
}
