#!/bin/bash
# Used to automate for guardian testing.
declare -a arr=("SIOC:B34:GD_PATRICK:SNAPSHOT_TRG_EN"
               "SIOC:B34:GD_PATRICK:ARM"
               "SIOC:B34:GD_PATRICK:TEST_TRG_EN" )
for i in "${arr[@]}"
do
   sleep 1
   caput "$i" 1
done
