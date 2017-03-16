'''
QLA Processing Framework - Test Processors and Injection

This package includes some classes to test processing of raw data,
metadata generation, as well as archival of output products into
the DSS archive and Registration (injection) of the corresponding
metadata into the EAS archive.
'''

from .version import version
import os

__author__ = 'J C Gonzalez'
__version__ = version
__top_dir__ = os.path.dirname(os.path.realpath(__file__))
