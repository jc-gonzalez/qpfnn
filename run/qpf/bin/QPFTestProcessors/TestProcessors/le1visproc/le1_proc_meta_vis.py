#!/usr/bin/env python
"""
le1_proc_meta_vis

Defines class LE1MetadataCollector

@author J C Gonzalez
"""
import os, sys, shutil

from TestProcessors.basetestproc.base_test_processor import BaseTestProcessor, get_info


class LE1MetadataCollector(BaseTestProcessor):
    def __init__(self, base_dir = "./", file_ext = None):
        BaseTestProcessor.__init__(self, base_dir, file_ext)
        self.base_name = ''
        self.no_files_processed = 0
        self.in_files  = []
        self.out_files = []
        self.log_files = []

    def process_file(self, the_file=None):
        # Just extract OS information about the system
        print("\nProcessing file {} . . .".format(the_file))
        log = "LE1 Generated product: {}".format(the_file)
        for key, value in get_info(the_file).items():
            log += "\n  - {} : {}".format(key, value)
        file_base_name = os.path.basename(the_file)
        base_name_parts = os.path.splitext(file_base_name)
        self.base_name = base_name_parts[0]
        self.in_files.append(file_base_name)

    def generate_outputs(self, the_file=None):
        # ... and copy file to outputs folder (as if it was generated)
        meta_file_name = (self.base_name + '.xml').replace("_INFO", "_LE1_META")
        self.out_files.append(meta_file_name)
        meta_file_name = self.out_dir + meta_file_name

        #shutil.copy2(the_file, meta_file_name)

        # Read metadata file name to send
        with open(the_file, "r") as handle:
            file_content = handle.read()
        new_file = open(meta_file_name, "w")
        new_file.write(file_content.replace("_RAW", "_LE1"))
        new_file.close()

        print(" .. saving metadata to file {}".format(meta_file_name))
        self.no_files_processed += 1

    def generate_summary(self):
        # Finally, generate a log summary
        self.no_files_processed = self.no_files_processed + 1
        sum = "Summary of LE1 Processing for VIS_INFO data files on "
        sum += self.base_dir + '\n'
        sum += '=' * 78 + '\n' + '\n'
        sum += '# Processed files: ' + str(self.no_files_processed) + '\n'
        sum += '  - ' + '\n  - '.join(self.in_files) + '\n' + '\n'
        sum += '# Output files: \n'
        sum += '  - ' + '\n  - '.join(self.out_files) + '\n' + '\n'
        sum += '\n'
        print(sum)
        sum_file_name = self.log_dir + 'summary-meta.log'
        print(sum_file_name)
        with open(name=sum_file_name, mode="w") as sum_file:
            sum_file.write(sum)


def main():
    def usage():
        print('''
%s - Processes all RAW input data files to generate LE1 metadata

Usage:
  %s <folder>''' % (os.path.basename(sys.argv[0]), os.path.basename(sys.argv[0])))

    if len(sys.argv) < 2:
        usage()
        exit

    base_path = sys.argv[1]

    # Create metadata collector
    le1Meta = LE1MetadataCollector(base_dir=base_path, file_ext=".xml")
    le1Meta.run()


if __name__ == '__main__':
    main()
