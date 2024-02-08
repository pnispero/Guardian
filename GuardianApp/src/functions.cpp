/* This is a temporary file to list out the functions in the matlab script ($MAT/tools)
    That we have to replicate here in cpp */

/*
Note: These first 2 functions are the 'meat' of the script. The rest is mostly standard epics features.
0) FELpulseEnergyMonitor() lines 7 - 337
    Definition: Within script
    What: This is the 'main' function, and after initalization, and initalizes a snapshot of current values 
            and stores them into 'stats' it then goes into a 'while forever' loop that 
            consistently grabs updates from all of its pvs and stores them into 'stored'.
            They are then passed into 'trip_logic(stored, stats)' returns boolean 'trip', and string 'out'.
            Finally, write 'trip' value to the MPS:UNDS:1:SXRSS_GUARDIAN_OK and the 'out' message to display.
    Conversion: seems like they run the script after they've set their values for the devices. We can make a pv
                that controls whether or not this guardian runs. And just use that as a conditional in the while loop.

1) trip_logic() lines 712 - 1002
    Definition: Within script
    What: Most of the trip logic is just the stats.<parameter> value in 'stored' comparing to 'stats' x +/- 1.10 (10%)
            Besides a few special cases that do a bit more comparison. 
            Finally, function returns 'trip' and 'out' based off the comparison.  
    Conversion: There seems to be a good amount of repeated code here, would be good to make a dictionary
                of all the parameters, and just pass into a function that does the comparison.

1) generate_pv_list() lines 340 - 708
    Definition: Within script
    What: 1) Creates 56 'storage' pvs using setup_pv(), including the input pv to the MPS. 
            2) Then at line 475, 'these are actual data PVs' is where the input pvs we are reading the device data from
            3) Then at line 649, 'control PVs, for tolerances' is where the tolerance pvs are created using setup_pv()
    Conversion: 1) Since the  storage pvs are already created on $APP/MatlabSupport/db/matlabAOSupport.db. We don't have to make templates for them. 
                But we do have to set the fields. Which we can create a dictionary for, and just loop over them. 
                ACTUALLY: instead of using the matlabAOSupport.db we can make our own db and just use templates and substitution files.
                2) Then for the device data PVs, 
                we can make them asyn parameters as well like how we did centralNodeIoc, and just reference those.  
                3) Same as number 1.
    Uses: setup_pv()
        1.1) setup_pv() lines 1006 - 1029 - Function that doesnt create pvs in epics, but sets the fields, such as 'DESC', 'EGU', 'PREC'

2) lcaSetSeverityWarnLevel()
    Definition: $MAT/src/

3) watchdog() 
    Defintion: $MAT/tools/watchdog.m
    What: C
    Conversion:

4) watchdog_run()
    Defintion: $MAT/tools/watchdog_run.m
    What: C
    Conversion:

5) lcaGetSmart()
    Definition: $MAT/tools/lcaGetSmart.m
    What:
    Conversion:

6) lcaSetMonitor()
    Definition: $MAT/src/

7) cell()
    Definition: Standard MATLAB function
    What:
    Conversion: Likely a c++ version exists 

8) zeros()
    Definition: Standard MATLAB function
    What:
    Conversion: Likely a c++ version exists 

9) 

q's: 
Check matlab standard library
1) what is 'nQS' line 33
2) 

*/

/* Matlab notes 
1) You can declare a structure without explicity declaring structure
data.x = 5
data.y = [1,2]
This is legal, so the 'stats' and 'output' variable are 
2) Array indexing and function calling both use parenthesis

*/