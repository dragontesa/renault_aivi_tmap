#!/bin/bash

if [ $# -lt 1 ]; then
	echo "execute with options that prefix of log filename."
	echo
	echo "ex) $ ./logscript.sh 20190101_235959"
	echo "then will save the log filename that '20190101_235959_dbus_NavigationService.log"
	echo
	exit
else
	PREFIX=$1
fi

DMON="dbus-monitor --address unix:path=/tmp/shared/iddbus/lxcdbus"

echo "start for logging of dbus-monitor..."
echo /tmp/${PREFIX}_dbus_NavigationService.log
echo /tmp/${PREFIX}_dbus_NavigationSDSService.log
echo /tmp/${PREFIX}_dbus_GuiControl.log
echo /tmp/${PREFIX}_dbus_DrPositionService.log
echo /tmp/${PREFIX}_dbus_ExtAgent.log
echo /tmp/${PREFIX}_dbus_ExtService.log
echo
echo "if you want to stop logging, execute to below:"
echo "$ ./killscript.sh"
echo

${DMON} path=/com/bosch/cm/ai/navigation > /tmp/${PREFIX}_dbus_NavigationService.log &
${DMON} path=/com/bosch/cm/ai/navigationsds > /tmp/${PREFIX}_dbus_NavigationSDSService.log &
${DMON} path=/bosch/gui/cm/ai/guicontrol > /tmp/${PREFIX}_dbus_GuiControl.log &
${DMON} path=/com/bosch/DrPositionService > /tmp/${PREFIX}_dbus_DrPositionService.log &
${DMON} path=/com/bosch/cm/ai/navigation/ext/agent > /tmp/${PREFIX}_dbus_ExtAgent.log &
${DMON} path=/com/bosch/cm/ai/navigation/ext > /tmp/${PREFIX}_dbus_ExtService.log &

