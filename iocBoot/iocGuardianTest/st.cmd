#!../../bin/rhel7-x86_64/Guardian

#- You may have to change Guardian to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/Guardian.dbd"
Guardian_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadRecords("db/xxx.db","user=GUARDIAN")
dbLoadRecords("db/test.db") # PATRICK TODO: Temp here for testing
dbLoadRecords("db/guardian_snapshot.db", "BASE=SIOC:B34:GD_PATRICK") # PATRICK TODO: Temp add patrick so its unique
dbLoadRecords("db/guardian_device_data.db", "BASE=SIOC:B34:GD_PATRICK") # PATRICK TODO: Temp add patrick so its unique
dbLoadRecords("db/guardian_tolerance_data.db", "BASE=SIOC:B34:GD_PATRICK") # PATRICK TODO: Temp add patrick so its unique


## Configure Guardian driver
# GuardianDriverConfigure(
#    Port Name,                 # Name given to this port driver
GuardianDriverConfigure("GUARDIAN")

cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=GUARDIAN"
