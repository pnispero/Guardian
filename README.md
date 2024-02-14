# SXRSS Guardian - Example

## Description
Recreating the original SXRSS Guardian to an SIOC with extended functionality for SC.

This branch is used as an ASYN Port Driver example, it does not have any guardian logic. 
Keep this branch for future reference. 

## How to RUN:
1. git clone https://github.com/pnispero/Guardian.git
2. cd Guardian/
3. make
4. cd iocBoot/iocGuardianTest/
5. chmod +x st.cmd
6. ./st.cmd
7. Then in the shell you can 'dbl' to get pvs
8. And in another terminal write to 'device' pv. $ caput FBCK:FB02:GN01:S2DES_TEST 5
9. Trigger the 'trigger' pv. $ caput SIOC:B34:GD_PATRICK:SNAPSHOT_TRG_EN 1 
    The code will then takeSnapshot(). And your output should be 5


## Resources
Original SXRSS Guardian: $MAT/toolbox/FELpulseEnergyMonitor.m
Confluence: https://confluence.slac.stanford.edu/display/~pnispero/SXRSS+Guardian
