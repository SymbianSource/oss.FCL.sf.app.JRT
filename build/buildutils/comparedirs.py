#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
# Script for comparing contents of two directories.
#
# Usage:
# python -u comparedirs.py <source_path> <target_path>
#

import datetime, filecmp, os, sys, traceback, zlib
from optparse import OptionParser

class ResultCounter:
    def __init__(self):
        self.source_only_dirs = 0
        self.source_only_files = 0
        self.target_only_dirs = 0
        self.target_only_files = 0
        self.diff_case_dirs = 0
        self.diff_case_files = 0
        self.diff_files = 0
        self.identical_files = 0
        self.start_time = datetime.datetime.now()
        self.stop_time = None
    def __str__(self):
        if self.stop_time is None:
            self.stop_time = datetime.datetime.now()
        msg = "Execution time " + str(self.stop_time - self.start_time) + "\n"
        msg += "Summary of differences:\n"
        if self.source_only_dirs > 0:
            msg += "Source only dirs: " + str(self.source_only_dirs) + "\n"
        if self.source_only_files > 0:
            msg += "Source only files: " + str(self.source_only_files) + "\n"
        if self.target_only_dirs > 0:
            msg += "Target only dirs: " + str(self.target_only_dirs) + "\n"
        if self.target_only_files > 0:
            msg += "Target only files: " + str(self.target_only_files) + "\n"
        if self.diff_case_dirs > 0:
            msg += "Different case dirs: " + str(self.diff_case_dirs) + "\n"
        if self.diff_case_files > 0:
            msg += "Different case files: " + str(self.diff_case_files) + "\n"
        if self.diff_files > 0:
            msg += "Different files: " + str(self.diff_files) + "\n"
        msg += "Identical files: " + str(self.identical_files)
        return msg
    def differences_found(self):
        if self.source_only_dirs > 0 or \
                self.source_only_files > 0 or \
                self.target_only_dirs > 0 or \
                self.target_only_files > 0 or \
                self.diff_case_dirs > 0 or \
                self.diff_case_files > 0 or \
                self.diff_files > 0:
            return True
        else:
            return False

# Counter for differences.
result_counter = ResultCounter()

# Command line options.
opts = None

def main():
    global opts, result_counter
    # Parse command line options and arguments.
    parser = OptionParser(
        usage = "python -u %prog [options] <source_path> <target_path>")
    parser.add_option("-i", "--ignore", dest="ignore", action="append",
                      help="dir or file name to be ignored in the root directory")
    parser.add_option("--ignore_all", dest="ignore_all", action="append",
                      help="dir or file name to be ignored in all directories")
    parser.add_option("-v", "--verbose", dest="verbose",
                      action="store_true", default=False,
                      help="verbose output, lists also identical files")
    (opts, args) = parser.parse_args()
    if opts.ignore is None:
        opts.ignore = []
    if opts.ignore_all is None:
        opts.ignore_all = []
    source_path = args[0]
    target_path = args[1]
    if len(args) > 2:
        print "ERROR: Too many arguments"
        sys.exit(1)

    try:
        # Compare the source and target directories.
        print "%s: Comparing source %s and target %s" % \
            (str(datetime.datetime.now()), source_path, target_path)
        if len(opts.ignore) > 0:
            print "Ignoring from root directory: " + ", ".join(opts.ignore)
        if len(opts.ignore_all) > 0:
            print "Ignoring from all directories: " + ", ".join(opts.ignore_all)
        compare_dirs(source_path, target_path, opts.ignore, opts.ignore_all + [".svn", "_ccmwaid.inf"])

        # Comparison finished, display results.
        result_counter.stop_time = datetime.datetime.now()
        print "%s: Finished" % str(result_counter.stop_time)
        print str(result_counter)
    except:
        print "ERROR: Unexpected exception", datetime.datetime.now()
        traceback.print_exc()
        print str(result_counter)
        sys.exit(1)

    # if differences were found, exit with error status.
    if result_counter.differences_found():
        print "Directories differ."
        sys.exit(1)
    else:
        print "Directories are identical."

def compare_dirs(source, target, ignore, ignore_all):
    global opts, result_counter
    dircmp = filecmp.dircmp(source, target, ignore + ignore_all)
    #dircmp.report()
    filelist = get_filelist(target)
    # Loop the common files.
    for p in dircmp.common:
        sp = os.path.join(source, p)
        tp = os.path.join(target, p)
        tp_old = os.path.join(target, get_filename_case(p, filelist))
        # Check if source has a file and target has a dir or vice versa,
        # or if the dir/file name case is different.
        if os.path.isdir(sp) and os.path.isfile(tp_old):
            print "Source dir, target file: %s, %s" % (sp, tp_old)
            result_counter.source_only_dirs += 1
            result_counter.target_only_files += 1
        elif os.path.isfile(sp) and os.path.isdir(tp_old):
            print "Source file, target dir: %s, %s" % (sp, tp_old)
            result_counter.source_only_files += 1
            result_counter.target_only_dirs += 1
        elif not tp == tp_old:
            if os.path.isdir(tp_old):
                print "Dir cases differ: %s, %s" %  (sp, tp_old)
                result_counter.diff_case_dirs += 1
            else:
                print "File cases differ: %s, %s" %  (sp, tp_old)
                result_counter.diff_case_files += 1
        # Compare existing dir or count existing file.
        elif os.path.isdir(sp) and os.path.isdir(tp):
            compare_dirs(sp, tp, [], ignore_all)
        else:
            if not files_equal(sp, tp):
                # Files are different.
                print "Files differ: %s, %s" % (sp, tp_old)
                result_counter.diff_files += 1
            else:
                # Files are identical.
                if opts.verbose:
                    print "Identical files: %s, %s" % (sp, tp_old)
                result_counter.identical_files += 1
    # Loop the files which only exist in source.
    for p in dircmp.left_only:
        sp = os.path.join(source, p)
        if os.path.isdir(sp):
            print "Source only dir: %s" % sp
            result_counter.source_only_dirs += 1
        else:
            print "Source only file: %s" % sp
            result_counter.source_only_files += 1
    # Loop the files which only exist in target.
    for p in dircmp.right_only:
        tp = os.path.join(target, p)
        if os.path.isdir(tp):
            print "Target only dir: %s" % tp
            result_counter.target_only_dirs += 1
        else:
            print "Target only file: %s" % tp
            result_counter.target_only_files += 1

def get_filelist(path):
    if os.path.isdir(path):
        return os.listdir(path)
    else:
        return None

def get_filename_case(filename, filelist):
    filename = os.path.basename(filename)
    if filelist is None:
        result = [filename]
    else:
        result = [f for f in filelist if f.lower() == filename.lower()]
    return result[0]

def files_equal(source, target):
    if digest(source) == digest(target):
        return True
    else:
        return False

def digest(file):
    f = open(file, "rb")
    try:
        h = zlib.crc32(f.read())
    finally:
        f.close()
    return h

if __name__ == "__main__":
    main()
