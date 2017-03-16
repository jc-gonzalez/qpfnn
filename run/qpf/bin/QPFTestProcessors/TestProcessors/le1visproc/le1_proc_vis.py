#!/usr/bin/env python
"""
le1_proc_vis

Defines class LE1Processor

@author J C Gonzalez
"""
import os, sys, shutil

from TestProcessors.basetestproc.base_test_processor import BaseTestProcessor, get_info


class LE1Processor(BaseTestProcessor):
    def __init__(self, base_dir = "./", file_ext = None):
        BaseTestProcessor.__init__(self, base_dir, file_ext)
        self.base_name = ''
        self.no_files_processed = 0
        self.in_files  = []
        self.out_files = []
        self.log_files = []
        self.log_file_name = ''

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

        # Save log to file
        self.log_file_name = (self.base_name + '.log').replace("_RAW", "_LE1_LOG")
        self.log_files.append(self.log_file_name)
        self.log_file_name = self.log_dir + self.log_file_name
        print(" .. writing log to file {}".format(self.log_file_name))
        with open(name=self.log_file_name, mode="w") as log_file:
            log_file.write(log + "\n\n")

    def generate_outputs(self, the_file=None):
        # ... and move file to outputs folder (as if it was generated)
        out_file_name = (self.base_name + '.fits').replace("_RAW", "_LE1")
        self.out_files.append(out_file_name)
        out_file_name = self.out_dir + out_file_name

        #os.rename(the_file, out_file_name)
	#os.symlink(the_file, out_file_name)

        # Write output file
        #with open(the_file, "r") as handle:
        #    file_content = handle.read()
        file_content = self.base_name + '.fits' + '\n'
        new_file = open(out_file_name, "w")
        new_file.write(file_content)
        new_file.close()

        print(" .. saving output to file {}".format(out_file_name))
        self.no_files_processed += 1

    def generate_summary(self):
        # Finally, generate a log summary
        self.no_files_processed = self.no_files_processed + 1
        sum = "Summary of LE1 Processing for VIS_RAW data files on "
        sum += self.base_dir + '\n'
        sum += '=' * 78 + '\n' + '\n'
        sum += '# Processed files: ' + str(self.no_files_processed) + '\n'
        sum += '  - ' + '\n  - '.join(self.in_files) + '\n' + '\n'
        sum += '# Log files: \n'
        sum += '  - ' + '\n  - '.join(self.log_files) + '\n' + '\n'
        sum += '# Output files: \n'
        sum += '  - ' + '\n  - '.join(self.out_files) + '\n' + '\n'
        sum += '\n'
        print(sum)
        sum_file_name = self.log_dir + 'summary.log'
        print(sum_file_name)
        with open(name=sum_file_name, mode="w") as sum_file:
            sum_file.write(sum)


def main():
    def usage():
        print('''
%s - Processes all RAW input data files to generate LE1 productcs

Usage:
  %s <folder>''' % (os.path.basename(sys.argv[0]), os.path.basename(sys.argv[0])))

    if len(sys.argv) < 2:
        usage()
        exit

    base_path = sys.argv[1]

    # Create processor
    le1Proc = LE1Processor(base_dir=base_path, file_ext=".fits")
    le1Proc.run()


if __name__ == '__main__':
    main()
