# SXRSS Guardian - Test IOC

## Description
This is a test IOC, not the actual Guardian. NOT MEANT TO BE MERGED WITH MAIN

This test IOC serves as a simulation of all the devices the Guardian is monitoring. It provides values for the Guardian to grab from and use to test functionality.

## How to run
### Preliminary steps
*** NOTE: This test IOC can only be ran on dev ***
#### Guardian test steps:
1. `git clone https://github.com/pnispero/Guardian.git`
2. `mv Guardian Guardian_test`
3. `cd Guardian_test/`
4. `git checkout guardian_test`
5. `make`
6. Ensure the guardian_device_test_data.substitutions (test) matches the guardian_device_data.substitutions (main).
    Same for tolerance and condition substitution files.
#### Guardian main steps:
1. `git clone https://github.com/pnispero/Guardian.git`
2. `cd Guardian/`
3. `git checkout main`
5. `make TEST=1`

### Run tests
1. Run the test IOC and the guardian IOC in seperate terminals
```
cd iocBoot/iocGuardianTest
./st.cmd
```
2. Wait for initialization to finish on both SIOCS
3. Trigger a snapshot ``` caput SIOC:MCC0:MP00:GUARD_SNAPSHOT_TRG_EN 1 ```
4. Arm the guardian ``` caput SIOC:MCC0:MP00:GUARD_ARM 1 ```
5. Trigger the tests ``` caput SIOC:MCC0:MP00:GUARD_TEST_TRG_EN 1 ```
6. Optional - You can also skip 3-5 and run the guardian_test.sh script.
7. Thats it, then you should see if the test cases pass/fail on the test IOC.
8. To test SC (default is NC), unarm the guardian ``` caput SIOC:MCC0:MP00:GUARD_ARM 0 ```
    1. then change the mode ``` caput SIOC:MCC0:MP00:GUARD_MODE 1 ```
    2. Repeat step 6 and 7.

## Resources
More Details:
https://confluence.slac.stanford.edu/display/~pnispero/Guardian+Test+Cases+Information

Original SXRSS Guardian: $MAT/toolbox/FELpulseEnergyMonitor.m
Confluence: https://confluence.slac.stanford.edu/display/~pnispero/SXRSS+Guardian
