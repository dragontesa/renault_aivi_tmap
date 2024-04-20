#!/bin/bash
#
# Script to Dump various config items from KDS based on different FORMATS required by client.
#

# History:
#   180427 0v1, mpr3hi

function echoERR(){
	echo -e "\033[31mERR : $1\033[0m"
}
function echoWARN(){
	echo -e "\033[34mWARN: $1\033[0m"
}
function echoINFO(){
	echo -e "\033[33m$1\033[0m"
}

function convert_oemtype {
	local oemtype=$1
	brand="unknown"	

	if [ "06" == "${oemtype}" ] ||
		[ "16" == "${oemtype}" ] ||
		[ "17" == "${oemtype}" ];
	then
		brand="Nissan"
	elif [ "08" == "${oemtype}" ] ||
		[ "13" == "${oemtype}" ] ||
		[ "14" == "${oemtype}" ] ||
		[ "15" == "${oemtype}" ];
	then
		brand="Renault"
	fi
	echo $brand
}

function convert_productline {
	local typeselector=$1
	productline="null"
	
	if [ "0" == "${typeselector}" ];
	then
		productline="ENTRY"
	else
		productline="MID"
	fi
}

function convert_variant {
	local variant=$1
	devicevariant="null"

	if [ "1" == "${variant}" ];
	then
		devicevariant="Navi"
	elif [ "2" == "${variant}" ];
	then
		devicevariant="DA"
	fi		
}

function showInfo { 
	echo "Script to DUMP KDS values for certain config items"

	cp configreaderapp_out.out  /opt/bosch/base/bin/configreaderapp_out.out
	chmod 550 /opt/bosch/base/bin/configreaderapp_out.out 

	echoINFO "-- IVI_Id -- "
	/opt/bosch/base/bin/configreaderapp_out.out -e ECUSerialNumber -l 7 -f /tmp/out &> /dev/null
	line=$(head -n 1 /tmp/out)
	strECUSerialNumber=${line// /\\x}
	printf "ECUSerialNumber $strECUSerialNumber ASCII \n"
	rm /tmp/out

	/opt/bosch/base/bin/configreaderapp_out.out -e NavUnitID -l 8 -f /tmp/out &> /dev/null
	line=$(head -n 1 /tmp/out)
	strNavUnitID=${line// /\\x}
	printf "NavUnitID $strNavUnitID ASCII \n"
	rm /tmp/out

	echoINFO "\n-- Hardware Ref --"
	/opt/bosch/base/bin/configreaderapp_out.out -e CustomerRefA  -l 10 -f /tmp/out &> /dev/null
	line=$(head -n 1 /tmp/out)
	strCustomerRefA=${line// /\\x}
	printf "CustomerRefA $strCustomerRefA ASCII \n"
	rm /tmp/out

	echoINFO "\n-- Part Number Ref --"
	/opt/bosch/base/bin/configreaderapp_out.out -e CustomerECUPartNumber  -l 10 -f /tmp/out &> /dev/null
	line=$(head -n 1 /tmp/out)
	strCustomerECUPartNumber=${line// /\\x}
	printf "CustomerECUPartNumber $strCustomerECUPartNumber ASCII \n"
	rm /tmp/out

	echoINFO "\n-- SW_Version --"	
	strSW_Version=$(cat /opt/bosch/base/registry/BuildVersion.reg | grep "BUILDVERSION_CUSTVERSTRING"  | awk -F"=" '{print $2}' | sed 's/"//g')
	printf "CustomerrSWVersion $strSW_Version ASCII \n"

	echoINFO "\n-- FCID --"
	/opt/bosch/base/bin/configreaderapp_out.out -e SWUPD_VariantInfo  -l 16 -f /tmp/out &> /dev/null
	hexSWUPD_VariantInfo=$(head -n 1 /tmp/out)
	fcid=$(echo "$hexSWUPD_VariantInfo" | cut -f3-6 -d " ")
	printf "SWUPD_VariantInfo $hexSWUPD_VariantInfo HEX \n"
	printf "FCID is: $fcid\n"
	rm /tmp/out

	echoINFO "\n-- Region --"
	echoERR "Config Item not specified"

	echoINFO "\n-- IVI brand --"
	/opt/bosch/base/bin/configreaderapp_out.out -e CMVariantCoding  -l 32 -f /tmp/out &> /dev/null
	hexCMVariantCoding=$(head -n 1 /tmp/out)
	printf "CMVariantCoding $hexCMVariantCoding HEX \n"
	hexOEMType=$(echo $hexCMVariantCoding | awk '{ print $4 }')
	convert_oemtype $hexOEMType
	printf "OEMType $hexOEMType HEX \n"
	printf "Brand is: ${brand}"   

	echoINFO "\n-- ProductLine --"
	/opt/bosch/base/bin/configreaderapp_out.out -e AIVIVariantCoding -i VariantSelection &> /dev/null
	retval=$?
	convert_variant $retval
	printf "VariantSelection $retval DEC\n"
	printf "Device is: ${devicevariant}\n"

	/opt/bosch/base/bin/configreaderapp_out.out -e AIVIVariantCoding -i SystemTypeSelector &> /dev/null
	retval=$?
	convert_productline $retval
	printf "SystemTypeSelector $retval DEC\n"
	printf "ProductLine: ${productline}\n"

	echoINFO "\n-- MapComponentd --"
	echoERR "TomTomComponentID doesnt exist in dpIfKdsKeyValueAccess"

}
showInfo

