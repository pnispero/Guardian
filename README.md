# SXRSS Guardian - Test IOC

## Description
This is a test IOC, not the actual Guardian. NOT MEANT TO BE MERGED WITH MAIN

## How to run (todo)
### Preliminary steps
*** NOTE: This will be updated once finalized and integrated into MPS ***
1. git clone https://github.com/pnispero/Guardian.git
2. cd Guardian/
3. git checkout guardian_test
4. make
5. In real guardian add fields to both nc/sc guardian_tolerance_data.templates, record(ao, "$(NAME)")
    1. field(OMSL, "closed_loop") # TODO: TEMP field for testing, For PROD delete this field
    2. field(DOL, "$(BASE):MPS_PERMIT CPP") # TODO: TEMP field for testing, For PROD delete this field
6. In real guardian add field to both nc/sc guardian_device_condition.templates, record(bo, "$(NAME):CONDITION")
    1. field(DOL, "$(NAME)_TEST_GUARD_CONDITION_RBV CPP")  # TODO: Temporarily set as test, For PROD its just $(NAME)
7. In real guardian change OUT field guardian_mps.template
    
    1. record(bo, "$(BASE):GUARD_MPS_PERMIT_OUT"),
        1. field(OUT, "MPS:UNDS:1:SXRSS_GUARDIAN_OK:TEST") # TODO: Temporarily set as test, For PROD remove ":TEST"
    2. record(bo, "$(BASE):GUARD_ON_OUT")
        1. field(OUT, "MPS:UNDS:1:SXRSS_GUARDIAN_ON:TEST") # TODO: Temporarily set as test, For PROD remove ":TEST"
8. In real guardian change DOL field to both nc/sc guardian_device_data.templates, record(ao, "$(NAME)_GUARD_ACT")
    1. field(DOL, "$(NAME)_TEST_GUARD_ACT_RBV CPP")  # TODO: Temporarily set as test_rbv, For PROD its just $(NAME)
9. In real guardian change DOL field guardian_special_device.template
    1. record(ao, "FOIL:LI24:804:MOTR_GUARD_ACT")
        1. field(DOL, "FOIL:LI24:804:MOTR_TEST_GUARD_ACT_RBV CPP")  # TODO: Temporarily set as test_rbv, for PROD its "FOIL:LI24:804:MOTR.RBV"
    2. record(ao, "FOIL:LI24:807:MOTR_GUARD_ACT")
        1. field(DOL, "FOIL:LI24:807:MOTR_TEST_GUARD_ACT_RBV CPP")  # TODO: Temporarily set as test_rbv, for PROD its "FOIL:LI24:807:MOTR.RBV"

10. Ensure the guardian_device_test_data.substitutions matches the guardian_device_data.substitutions.
    Same for tolerance and condition substitution files.

11. Build the real guardian with additional flag TEST=1
```make CPPFLAGS+=-DTEST=1```

### Run tests
1. Run the test IOC and the guardian IOC in seperate terminals
```
cd iocBoot/iocGuardianTest
./st.cmd
```
2. Wait for initialization to finish on both SIOCS
3. Trigger a snapshot ``` caput SIOC:MCC0:MP00:SNAPSHOT_TRG_EN 1 ```
4. Arm the guardian ``` caput SIOC:MCC0:MP00:ARM 1 ```
5. Trigger the tests ``` caput SIOC:MCC0:MP00:TEST_TRG_EN 1 ```
6. Optional - You can also skip 3-5 and run the guardian_test.sh script.
7. Thats it, then you should see if the test cases pass/fail on the test IOC.
8. To test SC (default is NC), unarm the guardian ``` caput SIOC:MCC0:MP00:ARM 0 ```
9.  then change the mode ``` caput SIOC:MCC0:MP00:GUARDIAN_MODE 1 ```
10.  Repeat step 6 and 7.


## Resources
More Details:
https://confluence.slac.stanford.edu/display/~pnispero/Guardian+Test+Cases+Information

Original SXRSS Guardian: $MAT/toolbox/FELpulseEnergyMonitor.m
Confluence: https://confluence.slac.stanford.edu/display/~pnispero/SXRSS+Guardian
