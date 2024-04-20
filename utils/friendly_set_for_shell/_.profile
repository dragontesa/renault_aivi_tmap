alias ls='ls --color=auto'
alias ll='ls -al'
alias grep='grep --color=auto'
alias egrep='egrep --color=auto'
alias lxc='su lxc_iddbus'
alias sshot='LayerManagerControl dump screen 1 to'
alias dmon='dbus-monitor'
alias extnav='lxc-attach -n extnav'

alias navstop='systemctl stop rbcm-lxc-extnav.service'
alias navstart='systemctl start rbcm-lxc-extnav.service'
alias navstatus='systemctl status rbcm-lxc-extnav.service'
alias navrestart='systemctl restart rbcm-lxc-extnav.service'

alias navbackup='mv /lib/systemd/system/rbcm-lxc-extnav.service /lib/systemd/system/_rbcm-lxc-extnav.service'
alias navrestore='mv /lib/systemd/system/_rbcm-lxc-extnav.service /lib/systemd/system/rbcm-lxc-extnav.service'

alias _navstop='systemctl stop _rbcm-lxc-extnav.service'
alias _navstart='systemctl start _rbcm-lxc-extnav.service'
alias _navstatus='systemctl status _rbcm-lxc-extnav.service'
alias _navrestart='systemctl restart _rbcm-lxc-extnav.service'


eval `cat /tmp/dbus_session.conf`; export DBUS_SESSION_BUS_ADDRESS

export SDSPATH='/com/bosch/cm/ai/navigationsds'
export NSPATH='/com/bosch/cm/ai/navigation'
export EXTPATH='/com/bosch/cm/ai/navigation/ext'
export AGENTPATH='/com/bosch/cm/ai/navigation/ext/agent'
export DRPATH='/com/bosch/DrPositionService'
export GUIPATH='/bosch/cm/ai/hmi/hmiappctrl'
export PROPATH='/com/bosch/cm/profileMngr'
export ZONEPATH='/bosch/cm/ai/nissan/hmi/zoneDataService'

export APPDEST='App.Core.AppHmi_Navigation'
export EXTDEST='App.Core.AppHmi_Navigation_Ext'
export AUTODEST='com.bosch.AutomotiveProxy'

export PATH=$PATH:$(pwd)
