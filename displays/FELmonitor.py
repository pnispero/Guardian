from os import path
from pydm import Display
from epics import caget
import time
from pydm.widgets.slider import PyDMSlider
from pydm.widgets.label import PyDMLabel
from pyqtgraph import InfiniteLine
from pydm.widgets.channel import PyDMChannel
import numpy as np

class FELMonitorMain(Display):
    def __init__(self, parent=None, args=[], macros=None):
        super(FELMonitorMain, self).__init__(parent=parent, args=args, macros=macros)

        guardian_arm_name = "ca://{BASE}:ARM".format(**macros)
        self.guardian_arm_pv = PyDMChannel(address=guardian_arm_name, value_slot=self.guardian_arm_val_change) 
        self.guardian_arm_pv.connect()

        # self.ui.whats_this_mode_button.clicked.connect(self.button_state)


    def guardian_arm_val_change(self, new_val):
        self.guardian_arm_val = new_val
        if (self.guardian_arm_val == 1):
            # Grey out buttons while guardian is running
            self.ui.snapshot_button.setEnabled(False)
            self.ui.snapshot_button.setStyleSheet('''color: rgb(161, 161, 161);''')
            self.ui.nc_mode_button.setEnabled(False)
            self.ui.nc_mode_button.setStyleSheet('''color: rgb(161, 161, 161);''')
            self.ui.sc_mode_button.setEnabled(False)
            self.ui.sc_mode_button.setStyleSheet('''color: rgb(161, 161, 161);''')
        else: # Set back to normal
            self.ui.snapshot_button.setEnabled(True)
            self.ui.snapshot_button.setStyleSheet('''''')
            self.ui.nc_mode_button.setEnabled(True)
            self.ui.nc_mode_button.setStyleSheet('''''')
            self.ui.sc_mode_button.setEnabled(True)
            self.ui.sc_mode_button.setStyleSheet('''''')

    # def button_state(self):
    #     if self.whats_this_mode_button.isChecked():
    #         print("button pressed")
    #         self.enterWhatsThisMode()

    @staticmethod
    def ui_filename():
        return 'FELmonitor.ui'

    def ui_filepath(self):
        return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())