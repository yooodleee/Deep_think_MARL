import warnings
warnings.filterwarnings("ignore", category=DeprecationWarning)

# imports from python standard library
import grading
import imp
import optparse
import os
import pprint
import re
import sys
import projectParams
import random
random.seed(0)
try:
    from pacman import GameState
except:
    pass

# register arguments and set default values
def readCommand(argv):
    parser = optparse.OptionParser(
        description='Run public tests on student code'
    )
    parser.set_defaults(
        generateSolutions=False,
        edxOutput=False,
        gsOutput=False,
        muteOutput=False,
        printTestCase=False,
        noGraphics=False,
    )
    parser.add_option(
        '--test-directory',
        dest='testRoot',
        default='test_cases',
        help='Root test directory which contains subdirectories corresponding to each question',
    )
    