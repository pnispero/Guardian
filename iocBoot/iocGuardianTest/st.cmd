#!../../bin/rhel7-x86_64/Guardian

#- You may have to change Guardian to something else
#- everywhere it appears in this file

< envPaths

cd "${TOP}"

## Register all support components
dbLoadDatabase "dbd/Guardian.dbd"
Guardian_registerRecordDeviceDriver pdbbase

## Load record instances
#dbLoadRecords("db/xxx.db","user=pnispero")
dbLoadRecords("db/counter.db", "USER=pnispero,PORT=pnispero,ADDR=0,TIMEOUT=0") # PATRICK TODO: Temporarily add counter


## Configure Guardian driver
GuardianDriverConfigure("pnispero")
## Configure counter driver
# CounterDriverConfigure("pnispero") # PATRICK TODO: Temporarily add counter


cd "${TOP}/iocBoot/${IOC}"
iocInit

## Start any sequence programs
#seq sncxxx,"user=pnispero"
