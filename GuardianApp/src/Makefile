TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# #=============================
# # Build the IOC application
# # =====================================================
# # Path to "NON EPICS" External PACKAGES: USER INCLUDES
# # =====================================================
USR_CXXFLAGS += -std=c++0x

# USR_INCLUDES += -I$(BOOST_INCLUDE)
# USR_INCLUDES += -I$(CPSW_FRAMEWORK_INCLUDE) 
# USR_INCLUDES += -I$(YAML_INCLUDE)
# USR_INCLUDES += -I$(YAMLREADER_INCLUDE)
# USR_INCLUDES += -I$(DEVICELIB_INCLUDE)

# # =====================================================

# # ======================================================
# #PATH TO "NON EPICS" EXTERNAL PACKAGES: USER LIBRARIES
# # ======================================================
# cpsw_DIR         = $(CPSW_FRAMEWORK_LIB) 
# yaml-cpp_DIR     = $(YAML_LIB)
# deviceLib_DIR    = $(DEVICELIB_LIB)
# yamlReader_DIR   = $(YAMLREADER_LIB)
# boost_system_DIR = $(BOOST_LIB)
# # ======================================================

# # ======================================================
# # LINK "NON EPICS" EXTERNAL PACKAGE LIBRARIES STATICALLY
# # ======================================================

# # USR_LIBS_Linux += cpsw 
# USR_LIBS_Linux += boost_system
# USR_LIBS_Linux += yaml-cpp
# USR_LIBS_Linux += yamlReader 
# USR_LIBS_Linux += deviceLib

#=============================
# Build the IOC application

PROD_IOC = Guardian
# Guardian.dbd will be created and installed
DBD += Guardian.dbd

# Guardian.dbd will be made up from these files:
Guardian_DBD += base.dbd
Guardian_DBD += Init.dbd

# Include dbd files from all support applications:
Guardian_DBD += asyn.dbd

# Add all the support libraries needed by this IOC
#Guardian_LIBS += xxx

# Guardian_registerRecordDeviceDriver.cpp derives from Guardian.dbd
Guardian_SRCS += Guardian_registerRecordDeviceDriver.cpp
Guardian_SRCS += GuardianDriver.cpp 

# Build the main IOC entry point on workstation OSs.
Guardian_SRCS_DEFAULT += GuardianMain.cpp
Guardian_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#Guardian_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
Guardian_LIBS += $(EPICS_BASE_IOC_LIBS)
# Guardian_LIBS += cpsw 
Guardian_LIBS += asyn
# Guardian_LIBS += yamlLoader
# Guardian_LIBS += yamlDownloader

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

