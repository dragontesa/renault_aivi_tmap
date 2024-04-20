#####################################################################################
#!/bin/sh
# Please note that this script is just to simulate the imporatant acitivties of UC_01
# This is a test script to simulate UC_01 and test UC_02 with real server.
#####################################################################################


#Install the SDC keys
rwrfs

#Check if CN is passed as a parameter
if [ -z "$1" ] ; then
	echo "Invalid Paramaters"
	echo "Please pass ECU Serial Number (or CN Name) as the 1st parameter"
	echo "Usage: "sh Simulate_UC_01.sh 123456700000000", where 123456700000000 is the CN or the ECU Serial Number"
	exit 1
fi

#Check if private key exists
if [ ! -f ./ECM_Setup/crypto/id/device_priv_key.pem ] ; then
	echo "Private key is missing"
	echo "Please place it in the location ./ECM_Setup/crypto/id/device_priv_key.pem"
	exit 1
fi

#remove Keys
sdc_ctl remove -k 9101 
sdc_ctl remove -k 15072
sdc_ctl remove -k 15000

#Install the required SDC keys
sdc_ctl install -k 9101 -g pj_ecm -r -b 32 -p
sdc_ctl install -k 15072 -g pj_partAuth -r -b 32 -p
sdc_ctl install -k 15000 -g aid_devicekey -p -f ./ECM_Setup/crypto/id/device_priv_key.pem --rsa-pri-pem -B 2048
if [ $? != 0 ] ; then
	openssl rsa -in /ECM_Setup/crypto/id/device_priv_key.pem -out ./ECM_Setup/crypto/id/device_priv_key.der -outform DER
	sdc_ctl install -k 15000 -g aid_devicekey -f ./ECM_Setup/crypto/id/device_priv_key.der -p --rsa-pri-der -B 2048
fi


#==========================================
# Start--> Handling of the Private Key
#==========================================
#Generate the public key from private key (This is required for comparing this public key against the downloaded certificate's public key)
openssl rsa -in ./ECM_Setup/crypto/id/device_priv_key.pem -out ./ECM_Setup/crypto/id/device_pub.pem -pubout

#Wrap the private key for ECM (in the location /var/opt/bosch/persistent/id/device_priv_key.pem.wrap)
sdc_ctl fwrap -k 9101 -f ./ECM_Setup/crypto/id/device_priv_key.pem -o /var/opt/bosch/persistent/id/device_priv_key.pem.wrap

#Wrap the publik key for ECM (in the location /var/opt/bosch/persistent/id/device_pub.pem.wrap)
sdc_ctl fwrap -k 15072 -f ./ECM_Setup/crypto/id/device_pub.pem -o /var/opt/bosch/persistent/id/device_pub.pem.wrap

#Update the owner as per ECM/Security rule
chown aid_diagnosis:pj_partAuth /var/opt/bosch/persistent/id/device_pub.pem.wrap
chown aid_diagnosis:pj_ecm /var/opt/bosch/persistent/id/device_priv_key.pem.wrap
chmod 664 /var/opt/bosch/persistent/id/device_pub.pem.wrap

#==========================================
# End --> Handling of the Private Key 
#==========================================

#==========================================
# Start--> Handling of the CSR 
#==========================================
# Please note that just an empty CSR is required to start the ECM (Note: This script is not intended to use UC_01. Intention is to test UC_02)
touch ./ECM_Setup/crypto/id/device_csr.pem

#Wrap the CSR
sdc_ctl fwrap -k 15072 -f ./ECM_Setup/crypto/id/device_csr.pem -o /var/opt/bosch/persistent/id/device_csr.pem.wrap

#Update the group and permissions
chgrp pj_partAuth /var/opt/bosch/persistent/id/device_csr.pem.wrap
chmod 664 /var/opt/bosch/persistent/id/device_csr.pem.wrap

cp ./ECM_Setup/crypto/id/device_csr.pem /var/opt/bosch/persistent/id/device_csr.pem
chgrp pj_partAuth /var/opt/bosch/persistent/id/device_csr.pem
#==========================================
# End--> Handling of the CSR
#==========================================

#==========================================
# Start--> Handling of the Common Name
#==========================================
echo $1 > /var/opt/bosch/persistent/id/nuid
# echo 000111701Z000V1 > /var/opt/bosch/persistent/id/nuid
chgrp pj_partAuth /var/opt/bosch/persistent/id/nuid
sdc_ctl fwrap -k 15072 -f /var/opt/bosch/persistent/id/nuid -o /var/opt/bosch/persistent/id/nuid.wrap
#==========================================
# End--> Handling of the Common Name
#==========================================


#========================================================================
# Start--> Handling of the certificates, Root CAs and Intermediate CA's
#========================================================================
sdc_ctl fwrap -k 15072 -f ./ECM_Setup/crypto/certs/A-VNext_CA.pem -o /var/opt/bosch/persistent/certs/A-VNext_CA.pem.wrap
sdc_ctl fwrap -k 15072 -f ./ECM_Setup/crypto/certs/device_cert.crt -o /var/opt/bosch/persistent/certs/device_cert.crt.wrap
sdc_ctl fwrap -k 15072 -f ./ECM_Setup/crypto/certs/NissanCentricCarPlatformStagingCA.crt -o /var/opt/bosch/persistent/certs/NissanCentricCarPlatformStagingCA.crt.wrap
sdc_ctl fwrap -k 15072 -f ./ECM_Setup/crypto/certs/NissanVehiclesStagingCA.crt -o /var/opt/bosch/persistent/certs/NissanVehiclesStagingCA.crt.wrap
#========================================================================
# End--> Handling of the certificates
#========================================================================


#========================================================================
# Start--> Handling of the CRLs
#========================================================================
cp ./ECM_Setup/crypto/certs/*.crl /var/opt/bosch/persistent/certs/
#========================================================================
# End--> Handling of the CRLs
#========================================================================


chgrp pj_ecm /var/opt/bosch/persistent/id/device_priv_key.pem.wrap
chgrp pj_partAuth /var/opt/bosch/persistent/certs/device_cert.crt.wrap
chgrp pj_partAuth /var/opt/bosch/persistent/id/nuid.wrap
chgrp pj_partAuth /var/opt/bosch/persistent/certs/A-VNext_CA.pem.wrap
chgrp pj_partAuth /var/opt/bosch/persistent/id/device_csr.pem.wrap
#chgrp pj_partAuth /var/opt/bosch/persistent/certs/intermediate*.crt.wrap # FAILED
chgrp pj_certs /var/opt/bosch/persistent/certs
chown pj_ecm:pj_ecm /var/opt/bosch/persistent/id
chmod 775 /var/opt/bosch/persistent/certs
chmod 775 /var/opt/bosch/persistent/id

sync