void test_config(void);
void test_throttle_calibration(void);
void test_core_architecture(void);

int main(void)
{
    test_config();
    test_throttle_calibration();
    test_core_architecture();
    return 0;
}
