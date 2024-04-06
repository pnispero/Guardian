#!/bin/sh
# Used to create parameters for the display

# Run MSI tool to grab macros from guardian_device_data.substitutions 
# https://epics.anl.gov/base/R3-15/1-docs/msi.html
 
subs=./GuardianApp/Db/guardian_device_data.substitutions
template=./GuardianApp/Db/guardian_display_device.template

msi -V -o ./generated/devices_msi.txt -S $subs $template

subs=./GuardianApp/Db/guardian_tolerance_data.substitutions
template=./GuardianApp/Db/guardian_display_tolerance.template

msi -V -o ./generated/tolerances_msi.txt -S $subs $template

python create_display_parameters.py
