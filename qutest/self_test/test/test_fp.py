# test-script for QUTest unit testing harness
# see https://www.state-machine.com/qtools/qutest.html

include("test_include.pyi")

# tests...
test("FP output")
command("CMD_Z", 0, 3, 7)
expect("@timestamp COMMAND CMD_Z   4e-01  -6e+23")
expect("@timestamp Trg-Done QS_RX_COMMAND")
command("CMD_Z", 4, (-3 & 0xFFFFFFFF), 7)
expect("@timestamp COMMAND CMD_Z  -4.2857e-01  -6.0221e+23")
expect("@timestamp Trg-Done QS_RX_COMMAND")
