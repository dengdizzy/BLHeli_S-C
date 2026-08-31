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
void test_brake(void);
void test_protection(void);
void test_storage(void);
void test_dshot_decoder(void);
void test_tx_programming(void);
void test_bootloader_api(void);
void test_silabs_platform(void);
void test_trace_replay(void);

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
    test_brake();
    test_protection();
    test_storage();
    test_dshot_decoder();
    test_tx_programming();
    test_bootloader_api();
    test_silabs_platform();
    test_trace_replay();
    return 0;
}
