#!/bin/bash

source /etc/network/network_envvars.txt



BEHAVIOR="ACCEPT"
#BEHAVIOR="DROP"


################ prelude: set up chains and default behavior

## flush tables
iptables -F 
iptables -F -t nat
iptables -F -t mangle

## set default behaviord
iptables -P INPUT $BEHAVIOR
iptables -P OUTPUT $BEHAVIOR
iptables -P FORWARD $BEHAVIOR

## create all required chains
iptables -N AntiSpoofINPUT
iptables -N AntiSpoofOUTPUT
iptables -N AntiSpoofFORWARD
iptables -N LocalServices    
iptables -N RemServicesNative
iptables -N RemServicesFwd


################ early: make sure most traffic does not need to pass through chain (fast-tracking)
                                         
## start with early rules                 
# lo should always be accepted
iptables -A INPUT -i lo -j ACCEPT  
iptables -A OUTPUT -o lo -j ACCEPT 
# related traffic has been checked when it first came in (NEW)
iptables -A INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A OUTPUT -m state --state ESTABLISHED,RELATED -j ACCEPT
iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT  



        
                                                             
################ anti-spoof: ensure that packets we get come exactly from where we expect them from		
														  
## install anti-spoof rules                                        
iptables -A INPUT -j AntiSpoofINPUT                             
iptables -A OUTPUT -j AntiSpoofOUTPUT                           
iptables -A FORWARD -j AntiSpoofFORWARD                         
                                                                  
## anti-spoof for local incoming (for services running in native) 
#iptables -A AntiSpoofINPUT -i wlan0 -p udp --sport 67 --dport 68 -j ACCEPT
#iptables -A AntiSpoofINPUT -i bnep+ -p udp --sport 67 --dport 68 -j ACCEPT  
iptables -A AntiSpoofINPUT -i wlan1 -p udp --dport 67 --sport 68 -j ACCEPT                               
iptables -A AntiSpoofINPUT -i inc-scc -s 192.168.203.0/24 -j LocalServices 
iptables -A AntiSpoofINPUT -i inc-adr3 -s 192.168.204.0/24 -j LocalServices
iptables -A AntiSpoofINPUT -i inc-fake0 -s 192.168.223.0/24 -j LocalServices
iptables -A AntiSpoofINPUT -i inc-fake1 -s 192.168.224.0/24 -j LocalServices
iptables -A AntiSpoofINPUT -i ${CN_PRJ_USBNETDEV}.2 -s 10.19.229.244/30 -j LocalServices   
iptables -A AntiSpoofINPUT -i ${CN_PRJ_USBNETDEV}.3 -s 10.19.229.248/30 -j LocalServices   
iptables -A AntiSpoofINPUT -i ${CN_PRJ_USBNETDEV}.4 -s 10.19.229.252/30 -j LocalServices   
iptables -A AntiSpoofINPUT -i wlan1 -s 10.19.229.0/26 -j LocalServices      
iptables -A AntiSpoofINPUT -i wlan1 -s 10.19.229.64/27 -j LocalServices     
iptables -A AntiSpoofINPUT -i wlan1 -s 10.19.229.96/28 -j LocalServices
iptables -A AntiSpoofINPUT -i brwan -s 10.19.229.128/26 -j LocalServices     
# we currently do not configure .112  to keep number of rules smaller                                         
# iptables -A AntiSpoofINPUT -i wlan1 -s 10.19.229.112 -j LocalServices
iptables -A AntiSpoofINPUT -i wlan0 -j LocalServices                        
iptables -A AntiSpoofINPUT -i bnep+ -j LocalServices 

   
                   
iptables -A AntiSpoofINPUT -j LOG -m limit --limit 1/s --log-prefix "FWAntiSpoofINPUT: " --log-level 7
if [ "$BEHAVIOR" == "ACCEPT" ]; then
	iptables -A AntiSpoofINPUT -j LocalServices;
else
	iptables -A AntiSpoofINPUT -j $BEHAVIOR;
fi

                                               
## anti-spoof for local outgoing  (for services we call from native)  
iptables -A AntiSpoofOUTPUT -o wlan1 -p udp --dport 68 --sport 67 -j ACCEPT                                                                     
#iptables -A AntiSpoofOUTPUT -o wlan0 -p udp --sport 68 --dport 67 -j ACCEPT
#iptables -A AntiSpoofOUTPUT -o bnep+ -p udp --sport 68 --dport 67 -j ACCEPT
iptables -A AntiSpoofOUTPUT -o inc-scc -s 192.168.203.1 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o inc-adr3 -s 192.168.204.1 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o inc-fake0 -s 192.168.223.1 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o inc-fake1 -s 192.168.224.1 -j RemServicesNative                     
iptables -A AntiSpoofOUTPUT -o ${CN_PRJ_USBNETDEV}.2 -s 10.19.229.246 -j RemServicesNative                  
iptables -A AntiSpoofOUTPUT -o ${CN_PRJ_USBNETDEV}.3 -s 10.19.229.250 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o ${CN_PRJ_USBNETDEV}.4 -s 10.19.229.254 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o wlan1 -s 10.19.229.1 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o brwan -s 10.19.229.129 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o wlan0 -j RemServicesNative
iptables -A AntiSpoofOUTPUT -o bnep+ -j RemServicesNative



iptables -A AntiSpoofOUTPUT -j LOG -m limit --limit 1/s --log-prefix "FWAntiSpoofOUTPUT: " --log-level 7
if [ "$BEHAVIOR" == "ACCEPT" ]; then
	iptables -A AntiSpoofOUTPUT -j RemServicesNative;
else
	iptables -A AntiSpoofOUTPUT -j $BEHAVIOR;
fi

iptables -t filter -A AntiSpoofFORWARD -o wlan1 -j ACCEPT
iptables -t filter -A AntiSpoofFORWARD -i wlan1 -j ACCEPT 

## anti-spoof for forwarded traffic (for traffic we forward from one side to another)
#here we need only to forward from wlan1 to ipmc container??
#this irule s for tethering AP -> IVC
iptables -A AntiSpoofFORWARD -i wlan1 -s 10.19.229.0/26 -j RemServicesFwd    
#iptables -A AntiSpoofFORWARD -i wlan1 -s 10.19.229.64/27 -j RemServicesFwd
#iptables -A AntiSpoofFORWARD -i wlan1 -s 10.19.229.96/28 -j RemServicesFwd

# we likely want the following rule to apply to individual bridge interfaces (n<XX>-<X>), but this needs testing for bridge support
iptables -A AntiSpoofFORWARD -i brwan -s 10.19.229.128/26 -j RemServicesFwd
#iptables -A AntiSpoofFORWARD -i brwan -s 10.19.229.192/27 -j RemServicesFwd 
# disabled for now as IPs from this range have not been allocated yet
#iptables -A AntiSpoofFORWARD -i brwan -s 10.19.229.224/28 -j RemServicesFwd    
#iptables -A AntiSpoofFORWARD -i brwan -s 10.19.229.240/29 -j RemServicesFwd    

iptables -A AntiSpoofFORWARD -j LOG -m limit --limit 1/s --log-prefix "FWAntiSpoofFORWARD: " --log-level 7
if [ "$BEHAVIOR" == "ACCEPT" ]; then
	iptables -A AntiSpoofFORWARD -j RemServicesFwd;
else
	iptables -A AntiSpoofFORWARD -j $BEHAVIOR;
fi






################ services

## allowed local services in native domain (that we run)
# DHCP for WiFi AP
iptables -A LocalServices -i wlan1 -p udp --dport 67:68 -j ACCEPT
# Apple CarPlay (please verify ports)
iptables -A LocalServices -i wlan1 -p tcp --dport 5000 -j ACCEPT # RTSPControl
iptables -A LocalServices -i wlan1 -p tcp --dport 5001 -j ACCEPT # RTSPEvents
iptables -A LocalServices -i wlan1 -p udp --dport 5020 -j ACCEPT # KeepAlive
iptables -A LocalServices -i wlan1 -p udp --dport 6000 -j ACCEPT # RTPAudio
iptables -A LocalServices -i wlan1 -p tcp --dport 6030 -j ACCEPT # screen data
iptables -A LocalServices -i wlan1 -p udp --dport 6003 -j ACCEPT # altaudio data
iptables -A LocalServices -i wlan1 -p udp --dport 6011 -j ACCEPT # mainhighaudio control (RTCP) requests
iptables -A LocalServices -i wlan1 -p tcp --dport 7000 -j ACCEPT # media control
# ICMP
iptables -A LocalServices -i eth0 -p icmp -m icmp --icmp-type 8 -j ACCEPT
iptables -A LocalServices -i ${CN_PRJ_USBNETDEV}.2 -p icmp -m icmp --icmp-type 8 -j ACCEPT
iptables -A LocalServices -i ${CN_PRJ_USBNETDEV}.3 -p icmp -m icmp --icmp-type 8 -j ACCEPT
iptables -A LocalServices -i ${CN_PRJ_USBNETDEV}.4 -p icmp -m icmp --icmp-type 8 -j ACCEPT
# multicast DNS
iptables -A LocalServices -i wlan1 -p udp --dport 5353 -j ACCEPT
iptables -A LocalServices -i wlan0 -p udp --dport 5353 -j ACCEPT
iptables -A LocalServices -i bnep+ -p udp --dport 5353 -j ACCEPT
# DNS for containers
iptables -A LocalServices -i brwan -d 10.19.229.129 -p udp --dport 53 -j ACCEPT
iptables -A LocalServices -i brwan -d 10.19.229.129 -p tcp --dport 53 -j ACCEPT

iptables -A LocalServices -j LOG -m limit --limit 1/s --log-prefix "FWLocalServices: " --log-level 7
iptables -A LocalServices -j $BEHAVIOR

## allowed remote services for native domain
# DHCP for WiFi STA
iptables -A RemServicesNative -o wlan0 -p udp --dport 67:68 -j ACCEPT
# DHCP for WiFi AP (not usually required, but connection state may be missed)
iptables -A RemServicesNative -o wlan1 -p udp --dport 67:68 -j ACCEPT
# DHCP for Bluetooth
iptables -A RemServicesNative -o bnep+ -p udp --dport 67:68 -j ACCEPT
# DNS (dnsmasq running on brwan interface)
iptables -A RemServicesNative -o ${CN_PRJ_USBNETDEV}.2 -p udp -d 10.19.229.245 --dport 53 -j ACCEPT
iptables -A RemServicesNative -o ${CN_PRJ_USBNETDEV}.2 -p tcp -d 10.19.229.245 --dport 53 -j ACCEPT
iptables -A RemServicesNative -o ${CN_PRJ_USBNETDEV}.3 -p udp -d 10.19.229.249 --dport 53 -j ACCEPT
iptables -A RemServicesNative -o ${CN_PRJ_USBNETDEV}.3 -p tcp -d 10.19.229.249 --dport 53 -j ACCEPT
#CHECK if sensible within review allows ssl traffic to travel aivc0.2 (vNext interface) / wlan STA
iptables -A RemServicesNative -o ${CN_PRJ_USBNETDEV}.2 -p tcp --dport 443 -j ACCEPT
iptables -A RemServicesNative -o wlan0 -p tcp --dport 443 -j ACCEPT
#CHECK if sensible within review allows secure MQTT traffic to travel aivc0.2 (vNext interface) 
iptables -A RemServicesNative -o ${CN_PRJ_USBNETDEV}.2 -p tcp --dport 8883 -j ACCEPT
iptables -A RemServicesNative -o wlan0 -p tcp --dport 8883 -j ACCEPT
#CHECK if sensible within review allows secure MQTT traffic to travel aivc0.2 (vNext interface) 
iptables -A RemServicesNative -o ${CN_PRJ_USBNETDEV}.2 -p udp --dport 8883 -j ACCEPT
iptables -A RemServicesNative -o wlan0 -p udp --dport 53 -j ACCEPT
iptables -A RemServicesNative -o bnep+ -p udp --dport 53 -j ACCEPT
iptables -A RemServicesNative -o wlan0 -p tcp --dport 53 -j ACCEPT
iptables -A RemServicesNative -o bnep+ -p tcp --dport 53 -j ACCEPT
# ICMP
iptables -A RemServicesNative -p icmp -m icmp --icmp-type 8 -j ACCEPT
# multicast DNS
iptables -A RemServicesNative -o wlan0 -p udp --dport 5353 -j ACCEPT
iptables -A RemServicesNative -o wlan1 -p udp --dport 5353 -j ACCEPT
iptables -A RemServicesNative -o bnep+ -p udp --dport 5353 -j ACCEPT
# Apple CarPlay
iptables -A RemServicesNative -o wlan1 -p udp --dport 6002 -j ACCEPT # timing sync
iptables -A RemServicesNative -j LOG -m limit --limit 1/s --log-prefix "FWRemServicesNative: " --log-level 7
iptables -A RemServicesNative -j $BEHAVIOR


## allowed remote services for forwarded traffic from tethering and containers
# tethering (no real filtering)
iptables -A RemServicesFwd -i wlan1 -o ${CN_PRJ_USBNETDEV}.3 -d 10.19.229.249 -p udp --dport 53 -j ACCEPT
iptables -A RemServicesFwd -i wlan1 -o ${CN_PRJ_USBNETDEV}.3 -d 10.19.229.249 -p tcp --dport 53 -j ACCEPT
iptables -A RemServicesFwd -i wlan1 -o ${CN_PRJ_USBNETDEV}.3 ! -d 10.19.229.248/30 -j ACCEPT
# Secure MQTT to vNext
iptables -A RemServicesFwd -i brwan -o ${CN_PRJ_USBNETDEV}.2 -p tcp --dport 8883 -j ACCEPT
iptables -A RemServicesFwd -i brwan -o wlan0 -p tcp --dport 8883 -j ACCEPT
# HTTPS to vNext
iptables -A RemServicesFwd -i brwan -o ${CN_PRJ_USBNETDEV}.2 -p tcp --dport 443 -j ACCEPT
iptables -A RemServicesFwd -i brwan -o wlan0 -p tcp --dport 443 -j ACCEPT
# HTTPS to Internet
iptables -A RemServicesFwd -i brwan -o ${CN_PRJ_USBNETDEV}.3 -p tcp --dport 443 -j ACCEPT
iptables -A RemServicesFwd -i brwan -o ${CN_PRJ_USBNETDEV}.3 -p tcp --dport 443 -j ACCEPT

iptables -A RemServicesFwd -j LOG -m limit --limit 1/s --log-prefix "FWRemServicesFwd: " --log-level 7
iptables -A RemServicesFwd -j $BEHAVIOR




	


################ masquerading and port forwarding

## destination NAT (masquerading)
iptables -t nat -A POSTROUTING -o bnep0 -j MASQUERADE
iptables -t nat -A POSTROUTING -o wlan0 -j MASQUERADE
iptables -t nat -A POSTROUTING -o ${CN_PRJ_USBNETDEV}.2 -j SNAT --to-source 10.19.229.246
iptables -t nat -A POSTROUTING -o ${CN_PRJ_USBNETDEV}.3 -j SNAT --to-source 10.19.229.250
iptables -t nat -A POSTROUTING -o ${CN_PRJ_USBNETDEV}.4 -j SNAT --to-source 10.19.229.254

## port forwarding
# TFTP server for FOTA
# customer specific TFTP port 2008 (standard is 69)
# dynamic range set as 60100-60110 (needs to be configured in TFTP server!)
iptables -t nat -A PREROUTING -p udp -m udp -i ${CN_PRJ_USBNETDEV}.4 -s 10.19.229.253 -d 10.19.229.254 --dport 2008 -j DNAT --to-destination 10.19.229.141:2008
iptables -t nat -A PREROUTING -p udp -m udp -i ${CN_PRJ_USBNETDEV}.4 -s 10.19.229.253 -d 10.19.229.254 --dport 60100:60110 -j DNAT --to-destination 10.19.229.141:60100-60110












################ development rules ####################################
##### this should only be active in unlocked target ###################
iptables -I AntiSpoofINPUT 1 -i eth0 -s 172.17.0.0/16 -j LocalServices        
iptables -I AntiSpoofOUTPUT 1 -o eth0 -s 172.17.0.1 -j RemServicesNative  

iptables -I LocalServices 1 -i eth0 -p tcp --dport 22 -j ACCEPT
iptables -I LocalServices 1 -i eth0 -p tcp --dport 3490 -j ACCEPT
#GDB debugging
iptables -I LocalServices 1 -i eth0 -p tcp --dport 2345 -j ACCEPT

#this rules are needed for testing required by KP Kollai
iptables -I LocalServices 1 -i eth0 -p tcp --dport 2355 -j ACCEPT

iptables -I RemServicesNative 1 -o eth0 -p tcp -s 172.17.0.1 --sport 2355 -j ACCEPT
#/end KPK
iptables -I RemServicesNative 1 -o eth0 -p tcp -s 172.17.0.1 --sport 3490 -j ACCEPT # should not be needed, but DLT is sometimes missed
iptables -I LocalServices 1 -i eth0 -p icmp -m icmp --icmp-type 8 -j ACCEPT

# MQTT to vNext
iptables -I RemServicesFwd 1 -i wlan0 -o brwan -p tcp --dport 1883 -j ACCEPT
iptables -I RemServicesFwd 1 -o wlan0 -i brwan -p tcp --dport 1883 -j ACCEPT
iptables -I RemServicesFwd 1 -i brwan -o wlan0 -p tcp --dport 1883 -j ACCEPT


iptables -I RemServicesFwd 1 -i brwan -o ${CN_PRJ_USBNETDEV}.2 -p tcp --dport 1883 -j ACCEPT
# HTTP to vNext
iptables -I RemServicesFwd 1 -i brwan -o ${CN_PRJ_USBNETDEV}.2 -p tcp --dport 80 -j ACCEPT
iptables -I RemServicesFwd 1 -i brwan -o ${CN_PRJ_USBNETDEV}.2 -p tcp --dport 80 -j ACCEPT
# HTTP to Internet
iptables -I RemServicesFwd 1 -i brwan -o ${CN_PRJ_USBNETDEV}.3 -p tcp --dport 80 -j ACCEPT
iptables -I RemServicesFwd 1 -i brwan -o ${CN_PRJ_USBNETDEV}.3 -p tcp --dport 80 -j ACCEPT
# Chrome debugging
iptables -t nat -A PREROUTING -p tcp -m tcp  -i eth0 -s 172.17.0.0/16 -d 172.17.0.1 --dport 9223:9224 -j DNAT --to-destination 10.19.229.139:9223-9224
