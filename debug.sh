/opt/openocd/bin/openocd -d 3 -l debug/openocd.log -f debug/openjtag.cfg -f debug/stm32f103vct6.cfg -f debug/init.cfg &
ls -l
telnet localhost 4444
