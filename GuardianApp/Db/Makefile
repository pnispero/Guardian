TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
# Create and install (or just install) into <top>/db
# databases, templates, substitutions like this
#DB += xxx.db
DB += guardian_controls.db
DB += guardian_metadata.db
DB += guardian_device_data.db
DB += guardian_special_device.db
DB += guardian_device_condition.db
DB += guardian_tolerance_data.db
DB += guardian_mps.db

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_template = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

ifeq ($(TEST),1)
  # make TEST=1: enable test line, disable normal line
  USR_DBFLAGS += -M "TEST=" -M "PROD=\#"
else
  # make: enable normal line, disable test line
  USR_DBFLAGS += -M "TEST=\#" -M "PROD="
endif

# $(warning In Db/Makefile: TEST='$(TEST)')
# $(warning In Db/Makefile: USR_DBFLAGS='$(USR_DBFLAGS)')
# $(warning In Db/Makefile: DBFLAGS='$(DBFLAGS)')
# $(warning In Db/Makefile: MSI_FLAGS='$(MSI_FLAGS)')
# $(warning In Db/Makefile: USR_MSI_FLAGS='$(USR_MSI_FLAGS)')