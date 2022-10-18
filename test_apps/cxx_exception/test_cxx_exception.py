def test_cxx_exception(dut):
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[exception_utils]')
    dut.expect_unity_test_output(timeout=30)

def test_test_throw_fail(dut):
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[test_throw_fail]')
    dut.expect("2 Tests 2 Failures 0 Ignored")
    dut.expect("FAIL")
