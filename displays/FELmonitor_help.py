from os import path
from pydm import Display
from epics import caget
import time
from pydm.widgets.slider import PyDMSlider
from pydm.widgets.label import PyDMLabel
from pyqtgraph import InfiniteLine
from pydm.widgets.channel import PyDMChannel
import numpy as np

class FELMonitorHelp(Display):
    def __init__(self, parent=None, args=[], macros=None):
        super(FELMonitorHelp, self).__init__(parent=parent, args=args, macros=macros)

        logic_type_name = "ca://{DEVICE}:LOGIC_TYPE".format(**macros)
        self.logic_type_pv = PyDMChannel(address=logic_type_name, value_slot=self.logic_type_val_change) 
        self.logic_type_pv.connect()

        # TODO: fill in formula type, and actual formula
        

        # TODO: fill in special case conditions

    def logic_type_val_change(self, new_val):
        self.logic_type_val = new_val
        self.fill_in_formula_labels()

    def fill_in_formula_labels(self):
        if self.logic_type_val == 0: # TODO add special case conditions
            self.ui.formula_name_label.setText("Special Case")
            self.ui.formula_label.setText('''
tolerance = tolerance * 0.01                                // Percentage\n
current value > (stored value + stored value * tolerance)   // Upper limit\n
current value < (stored value - stored value * tolerance)   // Lower limit''')
        if self.logic_type_val == 1:
            self.ui.formula_name_label.setText("Outside Percentage Tolerance")
            self.ui.formula_label.setText('''
tolerance = tolerance * 0.01                                // Percentage\n
current value > (stored value + stored value * tolerance)   // Upper limit\n
current value < (stored value - stored value * tolerance)   // Lower limit''')
        if self.logic_type_val == 2:
            self.ui.formula_name_label.setText("Outside Absolute Percentage Tolerance")
            self.ui.formula_label.setText('''
tolerance = tolerance * 0.01                                        // Percentage\n
|current value| > (|stored value| + |stored value| * tolerance)     // Upper Limit\n
|current value| < (|stored value| - |stored value| * tolerance)     // Lower Limit''')
        if self.logic_type_val == 3:
            self.ui.formula_name_label.setText("Outside Absolute Value Tolerance")
            self.ui.formula_label.setText('''
|current value| > (|stored value| + tolerance)      // Upper Limit\n
|current value| < (|stored value| - tolerance)      // Lower Limit''')
        if self.logic_type_val == 4:
            self.ui.formula_name_label.setText("Outside Absolute Difference Percentage Tolerance")
            self.ui.formula_label.setText('''
tolerance = tolerance * 0.01                                    // Percentage\n
|stored value - current value| > |tolerance * stored value|     // Upper & Lower limit''')

    @staticmethod
    def ui_filename():
        return 'FELmonitor_help.ui'

    def ui_filepath(self):
        return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())
