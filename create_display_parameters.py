# PYTHON script to automatically create .json file

# 1) Create bash script that will call msi twice and call this python script (15m)
#  1.1) msi devices_msi.txt for device ids, device names, tolerance ids
#  1.2) msi tolerances_msi.txt for tolerance ids, tolerance names
#  1.3) Call this python script to do the rest

warning_message = "**ATTENTION** Please add special devices (15,16) manually to guardian_nc_parameters.json since they are not in the guardian_device_data.substitutions\n\
                        You can grab it from the existing guardian_nc_parameters.json\n\
**ATTENTION** Please manually move undulator pvs to guardian_nc_und_parameters.json\n\
**ATTENTION** Please replace every \"'\" with a quotation mark instead \"\n\
**ATTENTION** Please keep the existing SPECIAL_CASE_DESC for each device from guardian_nc_parameters.json (unless you changed a special case)"

#  1.4) split devices_msi.txt and tolerances_msi.txt into 2 seperate (1 for sc and 1 for nc)
    #  1.4.1) loop through each line until you find the next id of 0, then add in 'split here'
    #  1.4.2) Split the file based off 'split here'
def split_nc_sc_files(filename): # filename without the .txt
    full_filename = filename + ".txt"
    with open(full_filename, "r") as in_file:
        first_line = in_file.readline()
        buf = in_file.readlines()

# weird bug that deletes the original line when you add a new one
    with open(full_filename, "w") as out_file:
        out_file.write(first_line)
        for line in buf:
            if line.startswith("0,"):
                line = "split here\n" + line
            out_file.write(line)

    with open(full_filename) as f:
        contents1, sentinel, contents2 = f.read().partition("split here\n")

    with open(filename + "_sc.txt", "w") as f:
        f.write(contents1)
    with open(filename + "_nc.txt", "w") as f:
        f.write(contents2)

# 2) Parse the SPLIT msi generated files into 2 dictionaries
# 2.1) Get each device in devices_msi.txt [device_id, device_name, tol_id]
# 2.2) Get each device in tolerances_msi.txt [tol_id, tol_name]
def process_devices(devices_filename, tolerances_filename, output_json_filename):
    # Parse the devices
    devices_file = open(devices_filename, "r")
    devices_read = devices_file.read().splitlines()
    device_ids = []
    device_names = []
    device_tol_ids = []
    for device_line in devices_read:
        device_line_contents = device_line.split(',') # [device_id, device_name, tol_id]
        device_ids.append(device_line_contents[0])
        device_names.append(device_line_contents[1])
        device_tol_ids.append(device_line_contents[2])

    devices_list = list(zip(device_ids, device_names, device_tol_ids))
    devices_file.close()

    # Parse the tolerances
    tolerances_file = open(tolerances_filename, "r") 
    tolerances_read = tolerances_file.read().splitlines()
    tolerance_ids = []
    tolerance_names = []
    for tolerance_line in tolerances_read:
        tolerance_line_contents = tolerance_line.split(',') # [tol_id, tol_name]
        tolerance_ids.append(tolerance_line_contents[0])
        tolerance_names.append(tolerance_line_contents[1])

    tolerances_list = list(zip(tolerance_ids, tolerance_names))

    tolerances_file.close()

# 3) Get the devices affected for each tolerance and add it to dictionary
# 3.1) Loop through each tolerance, and grab the device name by matching tol_dict[tol_id] == device_dict[tol_id] 
    # this can be more than 1 device
    tolerance_devices_affected_full_list = []
    tolerance_devices_affected_list = []
    for tolerance in tolerances_list:
        if (str(tolerance[0]) == "0"): # Make it skip tol 0, since thats just a default 0
            tolerance_devices_affected_full_list.append([])
            continue
        for device in devices_list:
            if (tolerance[0] == device[2]):
                tolerance_devices_affected = device[1]
                tolerance_devices_affected_list.append(tolerance_devices_affected)
        # print("tolerance[0]: " + str(tolerance[0])) 
        # print(tolerance_devices_affected_list)
        tolerance_devices_affected_full_list.append(tolerance_devices_affected_list)
        tolerance_devices_affected_list = [] # Reset

    if (len(tolerance_devices_affected_full_list) == 0): # Used for sc right now since its empty
        tolerance_devices_affected_full_list.append([])
    
# Goal: "TOLERANCE_DEVICES_AFFECTED": [device1, device2]

# 4) Create a final dictionary with structure [ID, DEVICE, TOLERANCE]
    # 4.1) Loop through each device, and grab the tolerance name by matching device_dict[tol_id] == tol_dict[tol_id]
    # 4.2) Then fill in [ID, DEVICE, TOLERANCE, TOLERANCE_AFFECTED_DEVICES}
    final_list = []
    tol_index = 0
    for device in devices_list:
        for tolerance in tolerances_list:
            if (device[2] == tolerance[0]):
                device_tol_temp_list = [device[0], device[1], tolerance[1], tolerance_devices_affected_full_list[tol_index]]
                final_list.append(device_tol_temp_list)  
            tol_index += 1      
        tol_index = 0


# 5) Write the final dictionary to JSON
    # Format ex: {"ID": 2, "DEVICE": "FBCK:BCI0:1:CHRGSP", "TOLERANCE": "SIOC:MCC0:MP00:GUARD_TOL0", "TOLERANCE_AFFECTED_DEVICES": ["", ""]},
    full_output_filename = output_json_filename
    with open(full_output_filename, "w") as out_file:
        first_line = "[\n"
        out_file.write(first_line)
        for current_line in range(len(final_list)):
            write_line = "{\"ID\": " + final_list[current_line][0] + ", \"DEVICE\": \"" + \
                            final_list[current_line][1] + "\", \"TOLERANCE\": \"" + final_list[current_line][2] + \
                            "\", \"TOLERANCE_AFFECTED_DEVICES\": " + str(final_list[current_line][3]) + "},\n"
            if (current_line == len(final_list) - 1): # dont write a comma for last item
                write_line = "{\"ID\": " + final_list[current_line][0] + ", \"DEVICE\": \"" + \
                            final_list[current_line][1] + "\", \"TOLERANCE\": \"" + final_list[current_line][2] + \
                            "\", \"TOLERANCE_AFFECTED_DEVICES\": " + str(final_list[current_line][3]) + "}\n"

            out_file.write(write_line)
            # print(write_line)
        last_line = "]\n"
        out_file.write(last_line)
        out_file.write(warning_message)

    # Spit message to user to manually add in special devices (15,16)
    print("Successfully generated " + str(full_output_filename) + ". Can almost directly replace guardian_nc_parameters.json/guardian_sc_parameters.json with some caveats below")

split_nc_sc_files("./generated/devices_msi")
split_nc_sc_files("./generated/tolerances_msi")
process_devices("./generated/devices_msi_sc.txt", "./generated/tolerances_msi_sc.txt", "./generated/generated_guardian_sc_parameters.json")
process_devices("./generated/devices_msi_nc.txt", "./generated/tolerances_msi_nc.txt", "./generated/generated_guardian_nc_parameters.json")
print(warning_message)
