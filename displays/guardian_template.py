from os import path
from pydm import Display
from epics import caget
import time
from pydm.widgets.slider import PyDMSlider
from pydm.widgets.label import PyDMLabel
from pyqtgraph import InfiniteLine
from pydm.widgets.channel import PyDMChannel
import numpy as np

from PyQt5.QtGui import QIcon

class GuardianTemplate(Display):
    def __init__(self, parent=None, args=[], macros=None):
        super(GuardianTemplate, self).__init__(parent=parent, args=args, macros=macros)

        # What we want in the FELmonitor_template.ui (used for template repeater)
        # Have a seperate thread that updates the qlabels of Lower tol and Upper tol
            # 1) Get the tolerance value of the certain device 
                # 1.1) The tolerance pv can come from the FELmonitor_nc_parameters.json with macros
                # 1.2) Use channel access to grab tolerance value
            # 2) Calculate the lower and upper tolerance using the tolerance pv
                # 2.1) Use channel access to grab stored value
                # 2.2) Then add/subtract it from the tolerance value
                # 2.3) otherwise multiply 0.01 if percentage (check the egu)
            # 3) Set the qlabel text to the calculated lower and upper tolernace
            # 4) Make the channel of the tol labels the tolerance pv, but keep the actual text as the calculated
                # lower and upper tolerance
        
        # macros are grabbed from FELmonitor_nc_parameters.json
        # Adding a value_slot field creates a thread that points to a function, and calls it anytime this channel changes value
        stored_pv_name = "ca://{DEVICE}_GUARD_LATCHED_RBV".format(**macros)
        self.stored_val_pv = PyDMChannel(address=stored_pv_name, value_slot=self.stored_val_change) 
        self.stored_val_pv.connect()
        
        # add tolerance pv 
        tolerance_pv_name = "ca://{TOLERANCE}".format(**macros)
        self.tolerance_val_pv = PyDMChannel(address=tolerance_pv_name, value_slot=self.tolerance_val_change) 
        self.tolerance_val_pv.connect()

        # add tolerance pv for EGU
        tolerance_egu_name = "ca://{TOLERANCE}.EGU".format(**macros)
        self.tolerance_val_pv = PyDMChannel(address=tolerance_egu_name, value_slot=self.tolerance_egu_change) 
        self.tolerance_val_pv.connect()

        # Grab the precision from the stored pv
        stored_prec_name = "ca://{DEVICE}_GUARD_LATCHED_RBV.PREC".format(**macros)
        self.tolerance_val_pv = PyDMChannel(address=stored_prec_name, value_slot=self.stored_prec_change) 
        self.tolerance_val_pv.connect()

        # add logic type because we need the absolute values
        # If logic type 3 then set the lower tolerance to 0 if its calculated to
        # to be below 0 (since those can't be under 0 it won't be evaluated since its
        # the comparison takes the absolute value of the current)
        logic_type_name = "ca://{DEVICE}_GUARD_LOGIC_TYPE".format(**macros)
        self.logic_type_pv = PyDMChannel(address=logic_type_name, value_slot=self.logic_type_change) 
        self.logic_type_pv.connect()

        # add automatic boxing of the tripped device
        tripped_pv_name = "ca://{BASE}:GUARD_TRIP_ID".format(**macros)
        self.device_id =  "{ID}".format(**macros)
        self.tripped_id_pv = PyDMChannel(address=tripped_pv_name, value_slot=self.tripped_val_change) 
        self.tripped_id_pv.connect()

        # Add logic for when the snapshot is triggered, then trigger the tolerance_val_change
        snapshot_pv_name = "ca://{BASE}:GUARD_SNAPSHOT_TRG_RBV".format(**macros)
        self.snapshot_pv = PyDMChannel(address=snapshot_pv_name, value_slot=self.snapshot_val_change) 
        self.snapshot_pv.connect()

        # may consider making lower/upper tol a pv calculated from guardian (not gonna work cause of 3)
        # cons:
        # 1) wont update unless running (solution: we can make the logic evaluate but
        #  dont make boolean to signal not to trip)
        # 2) May slow down guardian evaluation since were writing to multiple pvs each time 
        # (solution: probably wont)
        # 3) will not calculate the rest of the parameters if it trips on an early parameter
        # (solution: nothing unless you change trip logic dramatically to not return after a trip)

        # Disable the icon for help button
        self.ui.help_button_overlay.setIcon(QIcon())

    def stored_val_change(self, new_val):
        self.stored_val = new_val
        self.set_tolerance_label() # set the labels when new stored value occurs

    def tolerance_egu_change(self, new_val):
        self.tol_val_egu = new_val

    def logic_type_change(self, new_val):
        self.logic_type = new_val

    def snapshot_val_change(self):
        self.set_tolerance_label() # set the labels

    def stored_prec_change(self, new_val):
        self.ui.lower_tol.precision = new_val
        self.ui.upper_tol.precision = new_val

    def tripped_val_change(self, new_val):
        self.tripped_val = new_val
        if (str(self.device_id) == str(self.tripped_val)):
            # create red box around tripped device
            self.ui.border.setStyleSheet('''border: 2px solid;
                                                 border-color: rgb(255, 0, 0);''')
        else:
            self.ui.border.setStyleSheet('''background:transparent;''')

    def tolerance_val_change(self, new_val):
        self.tol_val = new_val
        self.set_tolerance_label()

    def set_tolerance_label(self):
        try:
            if (self.tol_val == 0):
                upper_tol = self.stored_val
                lower_tol = self.stored_val
            elif (self.tol_val_egu == '%'):
                tol_percent = self.tol_val * 0.01
                upper_tol = self.stored_val + (self.stored_val * tol_percent)
                lower_tol = self.stored_val - (self.stored_val * tol_percent)
            else:
                upper_tol = self.stored_val + self.tol_val
                lower_tol = self.stored_val - self.tol_val
            if (self.stored_val < 0): # Switch lower/upper if stored val was negative
                self.ui.lower_tol.value_changed(upper_tol)
                self.ui.upper_tol.value_changed(lower_tol)
            else:
                self.ui.lower_tol.value_changed(lower_tol)
                self.ui.upper_tol.value_changed(upper_tol)
            if (self.logic_type == 3):
                if (lower_tol < 0):
                    self.ui.lower_tol.value_changed(0)
        except AttributeError:
            # Skip attribute error (usually only at initilzation because values initialize
            # at different times)
            pass

    @staticmethod
    def ui_filename():
        return 'guardian_template.ui'

    def ui_filepath(self):
        return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())