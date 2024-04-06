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
dbLoadRecords("db/guardian_controls.db", "BASE=SIOC:MCC0:MP00")
dbLoadRecords("db/guardian_metadata.db", "BASE=SIOC:MCC0:MP00")
dbLoadRecords("db/guardian_device_condition.db", "BASE=SIOC:MCC0:MP00")
dbLoadRecords("db/guardian_device_data.db", "BASE=SIOC:MCC0:MP00")
dbLoadRecords("db/guardian_special_device.db", "BASE=SIOC:MCC0:MP00") 
dbLoadRecords("db/guardian_tolerance_data.db", "BASE=SIOC:MCC0:MP00") 
dbLoadRecords("db/guardian_mps.db", "BASE=SIOC:MCC0:MP00") 


## Configure Guardian driver
# GuardianDriverConfigure(
#    Port Name,                 # Name given to this port driver
GuardianDriverConfigure("GUARDIAN")

cd "${TOP}/iocBoot/${IOC}"
iocInit

# Wait for IOC to initialize then Initialize values for trip_msg waveform records 
system("./guardian_trip_msg.sh")

## Start any sequence programs
#seq sncxxx,"user=GUARDIAN"
