#!/bin/bash
# Used to automate for guardian testing.
declare -a arr=("SIOC:MCC0:MP00:SNAPSHOT_TRG_EN"
               "SIOC:MCC0:MP00:ARM"
               "SIOC:MCC0:MP00:TEST_TRG_EN" )
for i in "${arr[@]}"
do
   sleep 1
   caput "$i" 1
done
