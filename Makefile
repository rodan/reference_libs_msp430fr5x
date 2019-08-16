
all:
	+$(MAKE) -C tests/clock_system
	+$(MAKE) -C tests/cypress_fm24
	+$(MAKE) -C tests/gt9xx
	+$(MAKE) -C tests/honeywell_hsc_ssc
	+$(MAKE) -C tests/maxim_ds3231
	+$(MAKE) -C tests/maxim_ds3234
	+$(MAKE) -C tests/sensirion_sht1x
	+$(MAKE) -C tests/timer_a0
	+$(MAKE) -C tests/uart0
	#+$(MAKE) -C tests/uart0_cpp
	+$(MAKE) -C tests/vishay_tsop

clean:
	+$(MAKE) -C tests/clock_system clean
	+$(MAKE) -C tests/cypress_fm24 clean
	+$(MAKE) -C tests/gt9xx clean
	+$(MAKE) -C tests/honeywell_hsc_ssc clean
	+$(MAKE) -C tests/maxim_ds3231 clean
	+$(MAKE) -C tests/maxim_ds3234 clean
	+$(MAKE) -C tests/sensirion_sht1x clean
	+$(MAKE) -C tests/timer_a0 clean
	+$(MAKE) -C tests/uart0 clean
	#+$(MAKE) -C tests/uart0_cpp clean
	+$(MAKE) -C tests/vishay_tsop clean

