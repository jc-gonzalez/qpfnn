#!/usr/bin/env python
'''
QLA Processing Framework - LE1 Test Processing and Injection for VIS
====================================================================

This script uses the classes in the TestProcessors package to test
processing of raw data, metadata generation, as well as archival of
output products into the DSS archive and Registration (injection) of
the corresponding metadata into the EAS archive.
'''

from TestProcessors.le1visproc.le1_proc_vis      import LE1Processor
from TestProcessors.le1visproc.le1_proc_meta_vis import LE1MetadataCollector

import argparse
import time

__author__ = "J C Gonzalez"


def get_args():
    # Command line parser
    parser = argparse.ArgumentParser(description='RAW and INFO data processor')
    parser.add_argument('-p', '--input_path', help='Path to input RAW data folder',
                        dest='input_folder', required=True)
    parser.add_argument('-1', '--level1', help='Process RAW data to generate LE1 products',
                        dest='generate_level1', action='store_true')
    parser.add_argument('-m', '--meta', help='Process INFO data to generate Metadata files',
                        dest='generate_metadata', action='store_true')
    return parser.parse_args()


def main():
    args = get_args()

    if (args.generate_level1):
        print("Processing RAW data products ...")
        le1Proc = LE1Processor(base_dir=args.input_folder, file_ext=".fits")
        le1Proc.run()

    if (args.generate_metadata):
        print("Generated associated Metadata ...")
        le1Meta = LE1MetadataCollector(base_dir=args.input_folder, file_ext=".xml")
        le1Meta.run()

    time.sleep(10)

    print "Done."


if __name__ == '__main__':
    main()
