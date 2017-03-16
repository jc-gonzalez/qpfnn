#!/usr/bin/env python
"""
base_test_processor

Provides the class BaseTestProcessor, as a base for test processors to be used
in the tests of the QLA Processing Framework

@author J C Gonzalez
"""

import os, sys, datetime

from os import listdir
from stat import *
from datetime import *
from time import strftime, gmtime

def str_filesize(size):
    """
    >>> print str_filesize(0)
    0
    >>> print str_filesize(1023)
    1023
    >>> print str_filesize(1024)
    1K
    >>> print str_filesize(1024*2)
    2K
    >>> print str_filesize(1024**2-1)
    1023K
    >>> print str_filesize(1024**2)
    1M
    """
    import bisect

    d = [(1024-1,'K'), (1024**2-1,'M'), (1024**3-1,'G'), (1024**4-1,'T')]
    s = [x[0] for x in d]

    index = bisect.bisect_left(s, size) - 1
    if index == -1:
        return str(size)
    else:
        b, u = d[index]
    return str(size / (b+1)) + u

def find_files(path_to_dir=None, suffix=None):
    """Returns list of files with a given extension, under a directory
    and all its subdirectories
    """
    file_list = []
    for root, dirs, files in os.walk(path_to_dir):
        for a_file in files:
            if a_file.endswith(suffix):
                file_list.append(os.path.join(root, a_file))
    return file_list

def get_info(filename):
    """Gets file info like file-size etc. from filename.
    Returns a dictionary keys.
    """
    get_time = lambda f, s: strftime(f, gmtime(s))
    strtime = lambda t: datetime.fromtimestamp(t).strftime('%Y-%m-%d %H:%M:%S')
    fileinfo = os.stat(filename)
    size = fileinfo[ST_SIZE]
    accessed = fileinfo[ST_ATIME]
    modified = fileinfo[ST_MTIME]
    created = fileinfo[ST_CTIME]
    return ({
        "size" : unicode(size),
        'file_size': str_filesize(size),
        'file_size_bytes': unicode(size),
        'file_size_kb': u'%d KB' % long(size / 1024),
        'file_size_mb': u'%.2f MB' % (size / 1024.0**2),
        "created": strtime(created),
        'file_create_date': get_time('%Y-%m-%d', created),
        'file_create_datetime':
          get_time('%Y-%m-%d %H:%M:%S', created),
        'file_create_datetime_raw': unicode(created),
        "modified": strtime(modified),
        'file_mod_date': get_time('%Y-%m-%d', modified),
        'file_mod_datetime': get_time('%Y-%m-%d %H:%M:%S', modified),
        'file_mod_datetime_raw': unicode(modified),
        'accessed': strtime(accessed),
        'file_access_date': get_time('%Y-%m-%d', accessed),
        'file_access_datetime':
          get_time('%Y-%m-%d %H:%M:%S', accessed),
        'file_access_datetime_raw': unicode(accessed)
        })

class BaseTestProcessor:
    """Base class for all test QLA Framework LE1 processes.
    """
    def __init__(self, base_dir = "./", file_ext = None):
        self.base_dir = base_dir
        self.in_dir = self.base_dir + "in/"
        self.out_dir = self.base_dir + "out/"
        self.log_dir = self.base_dir + "log/"
        self.ext = file_ext
        self.log = []

    def get_input_files(self):
        return find_files(self.in_dir, self.ext)

    def get_output_files(self):
        return find_files(self.out_dir, self.ext)

    def process_file(self, the_file=None):
        pass

    def generate_outputs(self, the_file=None):
        pass

    def generate_summary(self):
        pass

    def run(self):
        for a_file in self.get_input_files():
            self.process_file(the_file = a_file)
            self.generate_outputs(the_file = a_file)
        self.generate_summary()


def main():
    def usage():
        print('''
%s - Lists all files with a given extension under a folder

Usage:
  %s <folder> <extension>''' % (os.path.basename(sys.argv[0]), os.path.basename(sys.argv[0])))

    if len(sys.argv) < 3:
        usage()
    else:
        #file_list_generator = BaseTestProcessor(base_dir = sys.argv[1], file_ext = sys.argv[2])
        file_list = find_files(path_to_dir=sys.argv[1], suffix=sys.argv[2])
        print(file_list)

if __name__ == '__main__':
    main()
