void test_config(void);
void test_throttle_calibration(void);
void test_core_architecture(void);
void test_hal_interface(void);

int main(void)
{
    test_config();
    test_throttle_calibration();
    test_core_architecture();
    test_hal_interface();
    return 0;
}
