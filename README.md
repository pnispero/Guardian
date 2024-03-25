# SXRSS Guardian - Test IOC

## Description
This is a test IOC, not the actual Guardian. NOT MEANT TO BE MERGED WITH MAIN

## How to run (todo)
### Preliminary steps
*** NOTE: This will be updated once finalized and integrated into MPS ***
1. git clone https://github.com/pnispero/Guardian.git
2. cd Guardian/
3. git checkout guardian_test
3. make
4. In real guardian add fields to both nc/sc guardian_tolerance_data.templates, record(ao, "$(NAME):TOL")
    1. field(OMSL, "closed_loop") # TODO: TEMP field for testing
    2. field(DOL, "$(BASE):MPS_PERMIT CPP") # TODO: TEMP field for testing
5. And add field to both nc/sc guardian_device_condition.templates, record(bo, "$(NAME):CONDITION")
    1. field(DOL, "$(NAME):TEST:CONDITION_RBV CPP")  # TODO: Temporarily set as test, For PROD its just $(NAME)

6. Ensure the guardian_device_test_data.substitutions matches the guardian_device_data.substitutions.
    Same for tolerance and condition substitution files.

7. Build the real guardian with additional flag TEST=1
```make CPPFLAGS+=-DTEST=1```

### Run tests
1. Run the test IOC and the guardian IOC in seperate terminals
```
cd iocBoot/iocGuardianTest
./st.cmd
```
2. Wait for initialization to finish on both SIOCS
3. Trigger a snapshot ``` caput SIOC:B34:GD_PATRICK:SNAPSHOT_TRG_EN 1 ```
4. Arm the guardian ``` caput SIOC:B34:GD_PATRICK:ARM 1 ```
5. Trigger the tests ``` caput SIOC:B34:GD_PATRICK:TEST_TRG_EN 1 ```
6. Optional - You can also skip 3-5 and run the guardian_test.sh script.
7. Thats it, then you should see if the test cases pass/fail on the test IOC.
8. To test SC (default is NC), unarm the guardian ``` caput SIOC:B34:GD_PATRICK:ARM 0 ```
9.  then change the mode ``` caput SIOC:B34:GD_PATRICK:GUARDIAN_MODE 1 ```
10.  Repeat step 6 and 7.


## Resources
More Details:
https://confluence.slac.stanford.edu/display/~pnispero/Guardian+Test+Cases+Information

Original SXRSS Guardian: $MAT/toolbox/FELpulseEnergyMonitor.m
Confluence: https://confluence.slac.stanford.edu/display/~pnispero/SXRSS+Guardian
