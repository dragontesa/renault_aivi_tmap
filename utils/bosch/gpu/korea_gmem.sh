#!/bin/sh

ps -xaf > /var/opt/bosch/dynamic/gmem_info.log 

while [ true  ]
do
 NOW=$(date +"%H-%M-%S")
 echo $NOW >> /var/opt/bosch/dynamic/gmem_info.log 
 
 gmem_info >> /var/opt/bosch/dynamic/gmem_info.log
 sleep 10
 
done



