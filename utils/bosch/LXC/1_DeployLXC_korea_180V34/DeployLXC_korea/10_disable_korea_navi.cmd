@echo off
rem check putty
call config.cmd


rem Disable Korea navigation firstly

@echo disble Korea navigation firstly and restart system

putty\plink.exe -batch root@172.17.0.1 rwrfs
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 mv /lib/systemd/system/rbcm-navigationhall_ext_korea.service /lib/systemd/system/rbcm-navigationhall_ext_korea_orignal.service
putty\plink.exe -batch root@172.17.0.1 mv /opt/bosch/base/bin/navigationhall_ext-korea_out.out /opt/bosch/base/bin/navigationhall_ext-korea_original_out.out
putty\plink.exe -batch root@172.17.0.1 mv /opt/bosch/base/bin/NavigationHall_ext_korea_logConfig.json /opt/bosch/base/bin/NavigationHall_ext_korea_test_logConfig.json
putty\plink.exe -batch root@172.17.0.1 mv /opt/bosch/hmibase/lib/libhmisdkbase_so.so /opt/bosch/hmibase/lib/libhmisdkbase_original_so.so
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 sync
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 reboot -f
if not errorlevel 0 goto ERROREXIT

:SUCCESEXIT
@pause
goto EOF

:ERROREXIT
echo something error
@pause

:EOF
