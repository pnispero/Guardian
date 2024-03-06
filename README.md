# SXRSS Guardian - Test IOC

## Description
This is a test IOC, not the actual Guardian. NOT MEANT TO BE MERGED WITH MAIN

## How to RUN:
1. git clone https://github.com/pnispero/Guardian.git
2. cd Guardian/
3. git checkout guardian_test
3. make
4. cd iocBoot/iocGuardianTest/
5. chmod +x st.cmd
6. ./st.cmd

7. add fields to guardian_tolernace_data.template
    field(OMSL, "closed_loop") # TODO: TEMP field for testing
    field(DOL, "$(BASE):MPS_PERMIT CPP") # # TODO: TEMP field for testing


## Resources
Original SXRSS Guardian: $MAT/toolbox/FELpulseEnergyMonitor.m
Confluence: https://confluence.slac.stanford.edu/display/~pnispero/SXRSS+Guardian
