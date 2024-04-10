#!/bin/sh
# Used to scrape and initialize epics waveform trip messages
# Only ran on initialization apart of st.cmd

# Run MSI tool to grab macros from guardian_device_data.substitutions 
# https://epics.anl.gov/base/R3-15/1-docs/msi.html
# Note - Only works when ran off of IOC because $TOP is populated
subs=${TOP}/GuardianApp/Db/guardian_device_data.substitutions
template=${TOP}/GuardianApp/Db/guardian_trip_msg.template

msi -V -o bash_trip_msg.txt -S $subs $template
sleep 8 # Wait for IOC to initialize
while IFS=, read -r PV_NAME TRIP_MSG; do
    caput -S ${PV_NAME}_GUARD_TRIP_MSG "$TRIP_MSG" # -S is string
    # echo $PV_NAME:TRIP_MSG "$TRIP_MSG"
done < bash_trip_msg.txt
rm bash_trip_msg.txt
echo "Finished initialization"