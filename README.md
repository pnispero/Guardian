# SXRSS Guardian

## Description
Recreating the original SXRSS Guardian to an SIOC with extended functionality for SC.

## How to run 
** NOTE: Tis repo is a standalone repo of the guardian, the production/actual guardian runs in MpsSupport apart of sioc-mcc0-mp00 **
1. Enter iocBoot/iocGuardianTest/ then ```./st.cmd```
2. Wait a few seconds for IOC to boot up until it says "Finished Initialization"
3. IOC is now 'unarmed' meaning, no device monitoring is taking place. In this state, op may take a snapshot (store the current device values as desired values) or 'arm' the guardian meaning, devices are monitored and guardian will trip if needed.
4. To take a snapshot: ```caput SIOC:MCC0:MP00:GUARD_SNAPSHOT_TRG_EN 1```, note that snapshots can only be taken when guardian is unarmed.
5. To arm the guardian: ```caput SIOC:MCC0:MP00:GUARD_ARM 1```, to unarm write 0 instead. (In this stage, nothing is going to happen since there are no values. If you want to test, please refer to [guardian_test branch](https://github.com/pnispero/Guardian/tree/guardian_test).)
6. Optional - Steps 4-5 could also be done using the display ```pydm -m BASE=SIOC:MCC0:MP00 displays/guardian.py ```

## How to add/delete devices
Depending on the comparison needed, adding/deleting a device can be as simple as adding/deleting lines in substitutions files. See [LOGIC_TYPES.md](LOGIC_TYPES.md) if the tolerance check already exists.

### I. Add device using any of the 4 predefined logical comparisons (Common)
1. If device has a non-zero tolerance, then open [GuardianApp/Db/guardian_device_tolerance_data.substitutions](GuardianApp/Db/guardian_device_tolerance_data.substitutions)
2. Add a new tolerance pv at end of list, ensure ID is incremented by 1 from previous ID
3. Open  [GuardianApp/Db/guardian_device_data.substitutions](GuardianApp/Db/guardian_device_data.substitutions)
4. Add device at end of list using the defined pattern [NAME, DESC, EGU, PREC, MSG, LOGIC_TYPE, TOLERANCE_ID, CONDITION_ID, ID]
~~~
# NAME - Name of device data pv
# DESC - Description of device data pv
# EGU - Engineering units
# PREC - Precision
# MSG - Trip message to GUI (For some special cases, some devices may have the same msg as other devices)
# LOGIC_TYPE - logic type number (0 - special case, 1 - outsideTolerancePercentage ...)
# TOLERANCE_ID - tolerance pv ID (Refer to guardian_tolerance_data.substitutions)
# CONDITION_ID - [OPTIONAL, Default = 0] condition pv ID. where 0 means no condition
# ID - ASYN ID of device data pv (increment by 1)
~~~
5. Ensure the ID is incremented by 1 from the previous ID.
6. Open [GuardianApp/Db/guardian_metadata.substitutions](GuardianApp/Db/guardian_metadata.substitutions)
7. Increment the DEVICE_PARAM_SIZE by 1
8. Optional - If you want the device on the pydm display, run this script [create_display_parameters.sh](create_display_parameters.sh). (Please note the instructions after running script)
9. Optional - If want to test (you should), then add/modify the same lines to the [guardian_test branch](https://github.com/pnispero/Guardian/tree/guardian_test). And add a simulation val for both device and tolerance.
10. Build at top of repo:```make```

### II. Delete device using any of the 4 predefined logical comparisons
1. Open [GuardianApp/Db/guardian_device_data.substitutions](GuardianApp/Db/guardian_device_data.substitutions)
2. Before deleting the device, take note of the tolerance id and condition id if no other devices use the same tolerance id and condition id
3. Delete, then ensure all device entries below the device you deleted get an updated ID (decremented by 1)
4. Open [GuardianApp/Db/guardian_metadata.substitutions](GuardianApp/Db/guardian_metadata.substitutions), decrement the DEVICE_PARAM_SIZE by 1
5. Optional - If want to test (you should), then delete the same lines to the [guardian_test branch](https://github.com/pnispero/Guardian/tree/guardian_test).
6. If on step 2 you found tol/condition ids not being used, then follow this step. Devices rely on correct tolerance ID and condition ID, deleting them from their substitutions files may take some work. When you delete one, and decrement those below it by 1. Ensure the tolerance/condition IDs in the  device_data.substitutions also get updated. Specifically the ones that you decremented ID by 1. 
7. Delete the device on the pydm display using [guardian_nc_parameters.json](displays/guardian_nc_parameters.json) or [guardian_sc_parameters.json](displays/guardian_sc_parameters.json), if it exists
8. Build at top of repo:```make```

### III. Add device with a unique logical comparison (Special case)
This can be more work, as special cases are hardcoded
1. Using the same steps in previous "Add device using any of the 5 predefined logical comparisons (Common)" section EXCEPT for step 2, add the device just before the "END SPECIAL CASES" line. 
2. Open [GuardianApp/src/GuardianDriver.cpp](GuardianApp/src/GuardianDriver.cpp) and enter your logic in function tripSpecialCase()

### IV. Delete device with a unique logical comparison
This can be more work, as special cases are hardcoded
1. Using the same steps in previous "Delete device using any of the 4 predefined logical comparisons" section AND in step 3 take note of all the special case device IDs that were decremented.
2. For the special case device ID's, tolerance ID's, and condition ID's that were decremented, update the values in function tripSpecialCase(). 

## Testing
Please refer to [guardian_test branch](https://github.com/pnispero/Guardian/tree/guardian_test).

## Development
Tips
1. Development is best done on this repo. The actual place this guardian runs is in MpsSupport, and
doing development on the guardian within that repo is more steps (Because building builds the entire MpsSupport). So keep it simple and make your changes on this repo, and when you're finished, move the changes to MpsSupport.
2. Since you're developing on this repo, you may want to keep all the test fields in place. But when moving it over to MpsSupport you can remove the test fields.
3. The official displays are located in $PYDM/mps

## Resources
Original SXRSS Guardian: ```$MAT/toolbox/FELpulseEnergyMonitor.m```
I also added a heavily commented version in this repo under the same name.

Confluence: https://confluence.slac.stanford.edu/display/~pnispero/SXRSS+Guardian

Jira Ticket: https://jira.slac.stanford.edu/browse/EEDSW-12
