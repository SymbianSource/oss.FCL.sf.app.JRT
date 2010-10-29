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
# Script for copying directory contents or SVN project
# to Telelogic Synergy (CCM) project.
#
# The target directory specified in <ccm_path> parameter must
# exist in ccm version control before this script is executed.
#
# If SVN URL and revision are specified the <svn_path>
# directory must not exist.
#
# Script performs the following steps:
# 1) If SVN URL and revision are specified, exports SVN project
#    to <svn_path> directory.
# 2) Creates S60 distribution policies to <svn_path> directory
#    using setpolicyfiles.py script.
# 3) Makes CCM sync and reconf for the CCM project.
# 4) Synchronizes the <svn_path> and <ccm_path> directories.
# 5) If there were any changes, creates CCM default task,
#    reconciles CCM project to database and commits CCM
#    default task.
#
# The script execution aborts immediately if any error occurs.
# In this case the user must manually delete the ccm task and
# reconcile the ccm work area so that it is again
# in the same level as it was before script execution.
#
# Usage:
# python -u svn2ccm.py [-c "<comment_string>"] [-u <url>] [-r <revision>] <svn_path> <ccm_path> <ccm_project_id> <ccm_project_release>
# For example:
# python -u svn2ccm.py -c "Update from SVN release 7010" -u http://host/path -r 7010 X:\java C:\ccm_wa\java jrt-java s60_release > svn2ccm.log 2>&1
#

import datetime, filecmp, os, re
import shutil, subprocess, stat, sys, traceback, zlib
from optparse import OptionParser

class CcmCounter:
    def __init__(self):
        self.added_dirs = 0
        self.added_files = 0
        self.removed_dirs = 0
        self.removed_files = 0
        self.modified_files = 0
        self.unmodified_files = 0
        self.visited_dirs = -1; # Do not count the root dir.
        self.start_time = datetime.datetime.now()
        self.stop_time = None
    def __str__(self):
        if self.stop_time is None:
            self.stop_time = datetime.datetime.now()
        msg = "Execution time " + str(self.stop_time - self.start_time) + "\n"
        msg += "Summary of CCM changes:\n"
        msg += "Added dirs: " + str(self.added_dirs) + "\n"
        msg += "Added files: " + str(self.added_files) + "\n"
        msg += "Removed dirs: " + str(self.removed_dirs) + "\n"
        msg += "Removed files: " + str(self.removed_files) + "\n"
        msg += "Modified files: " + str(self.modified_files) + "\n"
        msg += "Unmodified files: " + str(self.unmodified_files) + "\n"
        msg += "Total number of dirs: " + str(self.visited_dirs) + "\n"
        msg += "Total number of files: "
        msg += str(self.added_files + self.unmodified_files + self.modified_files)
        return msg
    def changes_made(self):
        if self.added_dirs > 0 or self.added_files > 0 or \
                self.removed_dirs > 0 or self.removed_files > 0 or \
                self.modified_files > 0:
            return True
        else:
            return False

# Filetype mappings which override the default ones for Synergy.
ccm_filetypes = {
    "\.cdtproject$": "xml",
    "\.checkstyle$": "xml",
    "\.classpath$": "xml",
    "\.classpath_qt$": "xml",
    "\.classpath_qt_j2me$": "xml",
    "\.confml$": "xml",
    "\.cproject$": "xml",
    "\.crml$": "xml",
    "\.gcfml$": "xml",
    "\.der$": "binary",
    "\.dm$": "binary",
    "\.dr$": "binary",
    "\.flm$": "makefile",
    "\.javaversion$": "ascii",
    "\.jupiter$": "xml",
    "\.launch$": "xml",
    "\.meta$": "makefile",
    "\.metadata$": "ascii",
    "\.mmh$": "makefile",
    "\.odc$": "ascii",
    "\.plugin$": "ascii",
    "\.prefs$": "ascii",
    "\.pri$": "ascii",
    "\.prj$": "makefile",
    "\.prf$": "ascii",
    "\.project$": "xml",
    "\.project_classpath_builder$": "xml",
    "\.project_normal$": "xml",
    "\.review$": "xml",
    "\.ser$": "binary",
    "\.sps$": "ascii",
    "\.vcs$": "ascii",
    "cc_launch$": "ascii",
    "findtr$": "ascii",
    "launch$": "ascii",
    "new$": "ascii",
    "option$": "ascii",
    "syncqt$": "ascii",
    "unknowncert$": "binary",
    "IJG_README$": "ascii",
    "README$": "ascii",
    "package-list$": "html",
}

# Counter for ccm changes.
ccm_counter = CcmCounter()

# Command line options.
opts = None

def main():
    global opts, ccm_counter
    # Parse command line options and arguments.
    parser = OptionParser(
        usage = "python -u %prog [options] <svn_path> <ccm_path> <ccm_project_id> <ccm_project_release>")
    parser.add_option("-c", "--comment", dest="ccm_comment",
                      help="comment string for CCM objects")
    parser.add_option("-d", "--description_file", dest="ccm_description_file",
                      help="description file for CCM task")
    parser.add_option("-i", "--ignore", dest="ignore", action="append",
                      help="dir or file name to be ignored in the root directory")
    parser.add_option("--ignore_all", dest="ignore_all", action="append",
                      help="dir or file name to be ignored in all directories")
    parser.add_option("--remove_dir", dest="remove_dir", action="append",
                      help="before synchronising directories, remove specified dir from <svn_path>")
    parser.add_option("--no_commit", dest="no_commit",
                      action="store_true", default=False,
                      help="do not commit CCM task in the end")
    parser.add_option("--no_dp", dest="no_dp",
                      action="store_true", default=False,
                      help="do not generate distribution policies")
    parser.add_option("--no_exe", dest="no_exe",
                      action="store_true", default=False,
                      help="do not execute any SVN and CCM commands, only print them")
    parser.add_option("--no_reconf", dest="no_reconf",
                      action="store_true", default=False,
                      help="do not reconfigure CCM project")
    parser.add_option("--no_sync", dest="no_sync",
                      action="store_true", default=False,
                      help="do not sync CCM project")
    parser.add_option("-u", "--url", dest="svn_url",
                      help="SVN project URL")
    parser.add_option("-r", "--revision", dest="svn_revision",
                      help="SVN project revision")
    parser.add_option("-q", "--quiet", dest="quiet",
                      action="store_true", default=False,
                      help="quieter output")
    (opts, args) = parser.parse_args()
    if opts.ignore is None:
        opts.ignore = []
    if opts.ignore_all is None:
        opts.ignore_all = []
    if opts.remove_dir is None:
        opts.remove_dir = []
    svn_path = args[0]
    ccm_path = args[1]
    ccm_project_id = args[2]
    ccm_project_release = args[3]
    if len(args) > 4:
        print "ERROR: Too many arguments"
        sys.exit(1)

    # Check the validity of options and arguments.
    if opts.svn_revision and opts.svn_url and os.path.exists(svn_path):
        print "SVN2CCM: ERROR: SVN URL and revision specified but %s already exists" % svn_path
        sys.exit(1);
    if not os.path.isdir(ccm_path):
        print "SVN2CCM: ERROR: %s is not a directory" % ccm_path
        sys.exit(1);
    if not opts.svn_url and opts.svn_revision:
        print "SVN2CCM: ERROR: Must specify SVN URL for the revision"
        sys.exit(1);
    if not opts.svn_revision and opts.svn_url:
        print "SVN2CCM: ERROR: Must specify revision for the SVN URL"
        sys.exit(1);
    if not opts.ccm_comment and not opts.svn_revision:
        print "SVN2CCM: ERROR: Must specify either CCM comment or SVN URL and revision"
        sys.exit(1);

    try:
        print "SVN2CCM: Started " + str(ccm_counter.start_time)
        print "SVN2CCM: Arguments " + str(sys.argv)
        if opts.svn_revision and opts.svn_url:
            # Export SVN project.
            print "SVN2CCM: Exporting %s revision %s to %s" % \
                (opts.svn_url, opts.svn_revision, svn_path)
            execute(["svn", "export"] + get_quiet_option() + ["-r", opts.svn_revision, quote_str(opts.svn_url), quote_str(svn_path)])

        # Check if there are directories which must be removed.
        for dir in opts.remove_dir:
            dir_path = os.path.join(svn_path, dir)
            print "SVN2CCM: Removing %s" % dir_path
            shutil.rmtree(dir_path)

        if not opts.no_dp:
            # Create S60 distribution policy files.
            # Set arguments to setpolicyfiles.main() with sys.argv.
            old_args = sys.argv
            sys.argv = ["setpolicyfiles.py", svn_path]
            print "SVN2CCM: Calling", sys.argv
            import setpolicyfiles
            setpolicyfiles.main()
            sys.argv = old_args

        if not opts.no_sync:
            # Sync CCM project.
            print "SVN2CCM: Syncing CCM project", datetime.datetime.now()
            execute(["ccm", "sync", "-p", quote_str(ccm_project_id)])

        if not opts.no_reconf:
            # Reconfigure CCM project.
            print "SVN2CCM: Reconfiguring CCM project", datetime.datetime.now()
            execute(["ccm", "reconf", "-r", "-p", quote_str(ccm_project_id)])

        # Synchronize the SVN and CCM directories.
        print "SVN2CCM: Synchronizing from %s to %s %s" % \
            (svn_path, ccm_path, str(datetime.datetime.now()))
        if len(opts.ignore) > 0:
            print "SVN2CCM: Ignoring from root directory: " + ", ".join(opts.ignore)
        if len(opts.ignore_all) > 0:
            print "SVN2CCM: Ignoring from all directories: " + ", ".join(opts.ignore_all)
        sync_dirs(svn_path, ccm_path, opts.ignore, opts.ignore_all + [".svn"])

        if ccm_counter.changes_made():
            # Create CCM task.
            print "SVN2CCM: Creating CCM task", datetime.datetime.now()
            if opts.ccm_description_file:
                execute(["ccm", "task", "-create", "-default", "-release", quote_str(ccm_project_release), "-descriptionfile", quote_str(opts.ccm_description_file), "-synopsis", get_comment_string()])
            else:
                execute(["ccm", "task", "-create", "-default", "-release", quote_str(ccm_project_release), "-description", get_comment_string(), "-synopsis", get_comment_string()])
            # Reconcile CCM project.
            print "SVN2CCM: Reconciling CCM project", datetime.datetime.now()
            execute(["ccm", "reconcile", "-r", "-cu", "-mwaf", "-update_db", "-p", quote_str(ccm_project_id)])
            if opts.no_commit:
                print "SVN2CCM: WARNING: No commit, leaving CCM task open"
            else:
                # Commit the default CCM task.
                print "SVN2CCM: Committing CCM task", datetime.datetime.now()
                execute(["ccm", "task", "-ci", "default"])
        else:
            # No changes, do not reconcile or commit.
            print "SVN2CCM: WARNING: No changes found"

        # Finished.
        ccm_counter.stop_time = datetime.datetime.now()
        print "SVN2CCM: Finished " + str(ccm_counter.stop_time)
        print str(ccm_counter)
    except:
        print "SVN2CCM: ERROR: Unexpected exception", datetime.datetime.now()
        traceback.print_exc()
        print str(ccm_counter)
        sys.exit(1)

def sync_dirs(source, target, ignore, ignore_all):
    global ccm_counter
    ccm_counter.visited_dirs += 1
    dircmp = filecmp.dircmp(source, target, ignore + ignore_all)
    filelist = get_filelist(target)
    # Update the common files from source to target.
    for p in dircmp.common:
        sp = os.path.join(source, p)
        tp = os.path.join(target, p)
        tp_old = os.path.join(target, get_filename_case(p, filelist))
        # Check if file has been changed to be a dir or vice versa,
        # or if the dir/file name case has been changed.
        add_target = False
        if os.path.isdir(sp) and os.path.isfile(tp_old):
            add_target = True
            remove_ccm_file(tp_old)
        elif os.path.isfile(sp) and os.path.isdir(tp_old):
            add_target = True
            remove_ccm_dir(tp_old)
        elif not tp == tp_old:
            add_target = True
            if os.path.isdir(tp_old):
                remove_ccm_dir(tp_old)
            else:
                remove_ccm_file(tp_old)
        # Synchronize existing dir or update existing file.
        if os.path.isdir(sp):
            if add_target:
                add_dir(tp)
            sync_dirs(sp, tp, [], ignore_all)
        else:
            if add_target:
                add_file(sp, tp)
            elif not files_equal(sp, tp):
                # Files are different, make an update.
                update_file(sp, tp)
            else:
                # Files are identical, update counter.
                ccm_counter.unmodified_files += 1
    # Add files which only exist in source.
    for p in dircmp.left_only:
        sp = os.path.join(source, p)
        tp = os.path.join(target, p)
        if os.path.isdir(sp):
            add_dir(tp)
            sync_dirs(sp, tp, [], ignore_all)
        else:
            add_file(sp, tp)
    # Remove files which only exist in target.
    for p in dircmp.right_only:
        sp = os.path.join(source, p)
        tp = os.path.join(target, p)
        if os.path.isdir(tp):
            remove_dir(tp)
        else:
            remove_file(tp)

def add_dir(target):
    print "Add dir " + target
    os.mkdir(target)
    global ccm_counter
    ccm_counter.added_dirs += 1

def add_file(source, target):
    print "Add file " + target
    shutil.copy(source, target)
    global ccm_counter
    ccm_counter.added_files += 1

def update_file(source, target):
    print "Update file " + target
    os.chmod(target, stat.S_IWRITE);
    shutil.copy(source, target)
    global ccm_counter
    ccm_counter.modified_files += 1

def remove_dir(target):
    print "Remove dir " + target
    remove_read_only_attribute(target)
    shutil.rmtree(target)
    global ccm_counter
    ccm_counter.removed_dirs += 1

def remove_file(target):
    if os.path.basename(target) == "_ccmwaid.inf":
        # Do not remove CCM special files.
        return
    print "Remove file " + target
    os.chmod(target, stat.S_IWRITE);
    os.remove(target)
    ccm_counter.removed_files += 1

def remove_ccm_dir(target):
    execute(["ccm", "unuse", quote_str(target)])
    global ccm_counter
    ccm_counter.removed_dirs += 1

def remove_ccm_file(target):
    if os.path.basename(target) == "_ccmwaid.inf":
        # Do not remove CCM special files.
        return
    execute(["ccm", "unuse", quote_str(target)])
    ccm_counter.removed_files += 1

def remove_read_only_attribute(target):
    os.chmod(target, stat.S_IWRITE)
    if os.path.isdir(target):
        for f in os.listdir(target):
            new_target = os.path.join(target,f)
            remove_read_only_attribute(new_target)

def get_comment_option():
    global opts
    comment_str = get_comment_string()
    if comment_str is None:
        return []
    else:
        return ["-c", comment_str]

def get_comment_string():
    global opts
    if opts.ccm_comment is None:
        if opts.svn_revision is None:
            return None
        else:
            return quote_str("Update from SVN revision %s" % opts.svn_revision)
    else:
        return quote_str(opts.ccm_comment)

def get_quiet_option():
    global opts
    if opts.quiet:
        return ["-q"]
    else:
        return []

def get_filetype_option(path):
    global ccm_filetypes
    for ext, type in ccm_filetypes.iteritems():
        if re.search(ext, path, re.I):
            return ["-type", type]
    return []

def execute(args):
    global opts, ccm_counter
    cmd = " ".join(args)
    if not opts.quiet:
        print cmd
    if opts.no_exe:
        return
    retcode = subprocess.call(cmd)
    if retcode != 0:
        print "SVN2CCM: ERROR: Command '%s' failed with code %d" % (cmd, retcode)
        sys.exit(retcode)

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

def quote_str(s):
    return '"' + str(s) + '"'

if __name__ == "__main__":
    main()
