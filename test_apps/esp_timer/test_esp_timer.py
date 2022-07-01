def test_app(dut):
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[ESPTimer]')
    dut.expect_unity_test_output(timeout=30)
