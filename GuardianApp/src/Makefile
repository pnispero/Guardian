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

PROD_IOC = GuardianTest
# GuardianTest.dbd will be created and installed
DBD += GuardianTest.dbd

# GuardianTest.dbd will be made up from these files:
GuardianTest_DBD += base.dbd
GuardianTest_DBD += Init.dbd

# Include dbd files from all support applications:
GuardianTest_DBD += asyn.dbd

# Add all the support libraries needed by this IOC
#GuardianTest_LIBS += xxx

# GuardianTest_registerRecordDeviceDriver.cpp derives from GuardianTest.dbd
GuardianTest_SRCS += GuardianTest_registerRecordDeviceDriver.cpp
GuardianTest_SRCS += GuardianTestDriver.cpp 

# Build the main IOC entry point on workstation OSs.
GuardianTest_SRCS_DEFAULT += GuardianTestMain.cpp
GuardianTest_SRCS_vxWorks += -nil-

# Add support from base/src/vxWorks if needed
#GuardianTest_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

# Finally link to the EPICS Base libraries
GuardianTest_LIBS += $(EPICS_BASE_IOC_LIBS)
# GuardianTest_LIBS += cpsw 
GuardianTest_LIBS += asyn
# GuardianTest_LIBS += yamlLoader
# GuardianTest_LIBS += yamlDownloader

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

