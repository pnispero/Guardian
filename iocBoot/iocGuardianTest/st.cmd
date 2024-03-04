#!../../bin/rhel7-x86_64/GuardianTest

#- You may have to change GuardianTest to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"
## Register all support components
dbLoadDatabase "dbd/GuardianTest.dbd"
GuardianTest_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadRecords("db/xxx.db","user=GUARDIAN_TEST")
dbLoadRecords("db/guardian_snapshot.db", "BASE=SIOC:B34:GD_TEST") 
dbLoadRecords("db/guardian_device_test_data.db", "BASE=SIOC:B34:GD_TEST") 


## Configure GuardianTest driver
# GuardianTestDriverConfigure(
#    Port Name,                 # Name given to this port driver
GuardianTestDriverConfigure("GUARDIAN_TEST")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=GUARDIAN_TEST"
