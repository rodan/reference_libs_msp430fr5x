
TESTS := clock_system cypress_fm24 edge gt9xx honeywell_hsc_ssc i2c_testboard_pins maxim_ds3231 maxim_ds3234 sensirion_sht1x timer_a0 uart0 vishay_tsop  # uart0_cpp

all:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var);)

clean:
	@$(foreach var, $(TESTS), $(MAKE) -C tests/$(var) clean ;)

