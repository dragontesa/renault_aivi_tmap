@echo off
rem check putty
rem call config.cmd

set PRODUCTION_NAME=korea
set NAVI_SOFTWARE_NAME=navigation_kor

rem remount fs to rw
putty\plink.exe -batch root@172.17.0.1 rwrfs
if not errorlevel 0 goto ERROREXIT

@echo copy LXC container
putty\pscp -batch Bosch\thirdparty-navi-container_armv7a.ipk root@172.17.0.1:/home/root/thirdparty-navi-container_armv7a.ipk
if not errorlevel 0 goto ERROREXIT
putty\pscp -batch Bosch\links_extnav.xml root@172.17.0.1:/home/root/links_extnav.xml
if not errorlevel 0 goto ERROREXIT
putty\pscp -batch Bosch\create_container_links.pl root@172.17.0.1:/home/root/create_container_links.pl
if not errorlevel 0 goto ERROREXIT


rem copy bosch service which for third party navigation
@echo copy bosch service for third party navigation to device...

putty\plink.exe -batch root@172.17.0.1 mkdir -p /opt/bosch/extnav

putty\pscp -batch Bosch\navigationhall_ext-%PRODUCTION_NAME%_out.out root@172.17.0.1:/opt/bosch/base/bin/navigationhall_ext-%PRODUCTION_NAME%_out.out
if not errorlevel 0 goto ERROREXIT
putty\pscp -batch Bosch\NavigationHall_ext_%PRODUCTION_NAME%_logConfig.json root@172.17.0.1:/opt/bosch/base/bin/NavigationHall_ext_%PRODUCTION_NAME%_logConfig.json
if not errorlevel 0 goto ERROREXIT
putty\pscp -batch Bosch\libhmisdkbase_so.so root@172.17.0.1:/opt/bosch/hmibase/lib/libhmisdkbase_so.so
if not errorlevel 0 goto ERROREXIT
putty\pscp -batch Bosch\rbcm-navigationhall_ext_%PRODUCTION_NAME%.service root@172.17.0.1:/lib/systemd/system/rbcm-navigationhall_ext_%PRODUCTION_NAME%.service
if not errorlevel 0 goto ERROREXIT

rem copy third party navigation
@echo copy bosch service for third party navigation to device...
putty\plink.exe -batch root@172.17.0.1 mkdir -p /opt/bosch/%NAVI_SOFTWARE_NAME%
putty\plink.exe -batch root@172.17.0.1 mv /opt/bosch/%NAVI_SOFTWARE_NAME%/lib /opt/bosch/%NAVI_SOFTWARE_NAME%/lib_original
putty\plink.exe -batch root@172.17.0.1 mv /opt/bosch/%NAVI_SOFTWARE_NAME%/res /opt/bosch/%NAVI_SOFTWARE_NAME%/res_original

putty\pscp -batch -r thirdpartynavi\lib root@172.17.0.1:/opt/bosch/%NAVI_SOFTWARE_NAME%/
if not errorlevel 0 goto ERROREXIT
putty\pscp -batch -r thirdpartynavi\res root@172.17.0.1:/opt/bosch/%NAVI_SOFTWARE_NAME%/
if not errorlevel 0 goto ERROREXIT

putty\pscp -batch -r thirdpartynavi\bin\thirdpartynavigation_out.out root@172.17.0.1:/opt/bosch/extnav/thirdpartynavigation_out.out
if not errorlevel 0 goto ERROREXIT



@echo set permissions
putty\plink.exe -batch root@172.17.0.1 chmod +x /opt/bosch/base/bin/navigationhall_ext-%PRODUCTION_NAME%_out.out
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 chmod +x /opt/bosch/hmibase/lib/libhmisdkbase_so.so
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 chmod +x /home/root/thirdparty-navi-container_armv7a.ipk
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 chmod +x /home/root/create_container_links.pl
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 chmod -R +x /opt/bosch/%NAVI_SOFTWARE_NAME%/lib/
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 chmod +x /opt/bosch/extnav/thirdpartynavigation_out.out
if not errorlevel 0 goto ERROREXIT
putty\plink.exe -batch root@172.17.0.1 sync
if not errorlevel 0 goto ERROREXIT

rem setup container
putty\plink.exe -batch root@172.17.0.1 opkg install --force-reinstall /home/root/thirdparty-navi-container_armv7a.ipk
if not errorlevel 0 goto ERROREXIT
rem putty\plink.exe -batch root@172.17.0.1 perl create_container_links.pl --list links_extnav.xml --force
rem if not errorlevel 0 goto ERROREXIT
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
