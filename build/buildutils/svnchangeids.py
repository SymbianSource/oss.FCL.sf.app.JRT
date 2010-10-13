#!/usr/bin/python
#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Script for parsing TeleLogic Change ids and svn revisions from svn log.
#

import os, re, sys, traceback
from optparse import OptionParser

def main():
    parser = OptionParser(
        usage = "python -u %prog [options] <svn_url> <svn_rev_1> <svn_rev_2>")
    parser.add_option("-r", "--release", dest="release",
                      help="release to which the change is targeted, e.g. 92 or mcl")
    (opts, args) = parser.parse_args()
    svn_url = args[0]
    svn_rev_1 = args[1]
    svn_rev_2 = args[2]
    if len(args) > 3:
        print "ERROR: Too many arguments"
        sys.exit(1)

    try:
        svn_change_ids = get_svn_change_ids(opts.release, svn_url, svn_rev_1, svn_rev_2)
        change_ids = svn_change_ids.keys()
        change_ids.sort()
        for change_id in change_ids:
            print "%s %s" % (change_id, ",".join(svn_change_ids[change_id]))
    except:
        print "ERROR: Unexpected exception"
        traceback.print_exc()
        sys.exit(1)

def get_svn_change_ids(release, url, rev_1, rev_2):
    """Return svn change id dictionary.

    Returns a dictionary where keys are change ids and values
    are list of svn revisions where the change is made.
    """

    change_id_keyword = "changeid\s*" # Matches to change ids with no release.
    if not release is None:
        # Matches to change ids for given release only.
        change_id_keyword = "changeid[_-]?" + release + "\s*"
    re_rev = re.compile("^r(\d+) \| ", re.I | re.M)
    re_change_id = re.compile("(\w+\#\d+)\s*,?", re.I | re.M)
    re_change_id_keyword = re.compile(change_id_keyword + "[:=]?\s*(\w+\#\d+)",
                                      re.I | re.M)
    svn_change_ids = {}
    cmd = "svn log -r " + rev_2 + ":" + rev_1 + " " + url
    #print cmd
    for line in os.popen(cmd).readlines():
        rev = re_rev.findall(line)
        if rev:
            current_rev = rev[0]
        change_ids = re_change_id_keyword.findall(line)
        while change_ids:
            for change_id in change_ids:
                if change_id in svn_change_ids:
                    if not current_rev in svn_change_ids[change_id]:
                        svn_change_ids[change_id] += [current_rev]
                else:
                    svn_change_ids[change_id] = [current_rev]
            line = re_change_id.sub("", line, 1)
            change_ids = re_change_id_keyword.findall(line)
    return svn_change_ids

if __name__ == "__main__":
    main()
