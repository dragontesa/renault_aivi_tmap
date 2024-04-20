#!/bin/bash
###############################################################################
# FILE       start_zram.sh
# PROJECT    RN-A-AIVI derived from GM GEN3
# COMPONENT  CM-AI/EPD1 / Architecture & Base Functions
#------------------------------------------------------------------------------
# DESCRIPTION
# The zram script tries to load the zram kernel module at system startup
# It creates a zram swap memory which is used by the kernel memory management
# to swap out least used pages when the system gets into hight memory situation
#
# The basic idea of enabling zram is that the current Gen3G system having less than 1GB
# memory isn't sufficient for the memory demands of RN-A-AIVI project and it
# is used to prevent memory thrashing an eventually OOM.
#
# it sends out traces on console (if found) and EM in case of errors.
# - ...
#
#------------------------------------------------------------------------------
# COPYRIGHT  (C) 2014-2015 Robert Bosch Car Multimedia GmbH, Hildesheim
#------------------------------------------------------------------------------
# HISTORY
# Version   | Date        | Author                 | Modification
#  1.0      | 19 Feb 2014 | CM-AI/EPD1 Sachin      | Initial Version
#  1.1      | 07 Oct 2015 | CM-AI/EPB2 Welz        | Size adaptation for RN-A-IVI
#  1.2      | 13 Nov 2017 | CM-AI/ERN-1ESW BCE2HI  | RAM dynamically
#------------------------------------------------------------------------------

ERRMEM=/dev/errmem
CONSOLE=$(cat /proc/consoles | grep "tty.*[W].*(E" | tail -n 1 | cut "-d " -f 1)
if [[ ! -z $CONSOLE ]] && [[ -w /dev/$CONSOLE ]]
then
    CONSOLE="/dev/$CONSOLE"
fi



#-----------   set size of RAM dynamically ---------------
cpu=`cat /proc/cpuinfo | grep -w Hardware | tr -s ' ' | cut -f4 -d ' '`
mem=`cat /proc/meminfo | grep -w MemTotal | tr -s ' ' | cut -f2 -d ' '`       

if [ ${mem} -le 512000 ]
then 
# there is a DL with 512 MB 
   let "ZRAM_DISK  = 512*1024*1024"
   let "ZRAM_LIMIT = 256*1024*1024"

elif [ ${mem} -le 1024000 ]
then 
# DL with 1 GB 
   let "ZRAM_DISK  = 1024*1024*1024"
   let "ZRAM_LIMIT = 512*1024*1024"
   
else 
# 2 or 4 GB 
  let "ZRAM_DISK  = 512*1024*1024"
  let "ZRAM_LIMIT = 256*1024*1024"
fi





ZRAM_RETRY_COUNTER=1
create_zram_swap()
{
     # ------- load zram kernel module ---------
     modprobe zram
     zram_exists=$(lsmod | grep -c zram)
     echo "zram_exists $zram_exists"
     if [ $zram_exists -ne 0 ]
     then

        echo ${ZRAM_DISK} > /sys/block/zram0/disksize
        echo ${ZRAM_LIMIT} > /sys/block/zram0/mem_limit
        sysctl vm.swappiness=60
        # ------- create zram swap memory ---------
        mkswap /dev/zram0

        zram_swap_sxists=$(blkid /dev/zram0 | grep -c zram)
        if [ $zram_swap_sxists  -ne 0 ]
        then
           # ------- enable the zram swap ---------
           swapon /dev/zram0
           swap_on=$(swapon -s | grep -c zram)
           if [ $swap_on  -ne 0 ]
           then
             echo "Created zram swap with 128 MBytes" >> $CONSOLE
           else
              if [ $ZRAM_RETRY_COUNTER -le 3 ]
              then
                 echo "failed to enable zram swap, retrying ... retry count value $ZRAM_RETRY_COUNTER" >> $ERRMEM
                 ZRAM_RETRY_COUNTER=$((ZRAM_RETRY_COUNTER+1))
                 sleep 1
                 create_zram_swap
              else
                 echo "****** failed to enable zram swap memory, no swap will be created for this ignition cycle *****" >> $ERRMEM
             fi
           fi
        else
           if [ $ZRAM_RETRY_COUNTER -le 3 ]
           then
             echo "failed to create a zram swap, retrying ... retry count value $ZRAM_RETRY_COUNTER" >> $ERRMEM
             ZRAM_RETRY_COUNTER=$((ZRAM_RETRY_COUNTER+1))
             sleep 1
             create_zram_swap
           else
             echo "****** failed to create a zram swap memory, no swap will be created for this ignition cycle *****" >> $ERRMEM
           fi
        fi
     else
        if [ $ZRAM_RETRY_COUNTER -le 3 ]
        then
          echo "failed to load zram kernel module, retrying ... retry count value $ZRAM_RETRY_COUNTER" >> $ERRMEM
          ZRAM_RETRY_COUNTER=$((ZRAM_RETRY_COUNTER+1))
          sleep 1
          create_zram_swap
        else
          echo "****** failed to load zram kernel module, no swap will be created for this ignition cycle *****" >> $ERRMEM
        fi
     fi
}

create_zram_swap &
