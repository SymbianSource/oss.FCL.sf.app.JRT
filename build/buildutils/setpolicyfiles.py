#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#!/usr/bin/python

import datetime, os, os.path, sys, traceback
from optparse import OptionParser

"""This script sets up S60 distribution policy files."""

class PolicyData:
    def __init__(self):
        self.root_policy = None
        self.policies = []
        self.created_policies = 0
        self.existing_policies = 0
        self.warning_count = 0
        self.start_time = datetime.datetime.now()
        self.stop_time = None
    def __str__(self):
        if self.stop_time is None:
            self.stop_time = datetime.datetime.now()
        msg = "Distribution policies generation time " + str(self.stop_time - self.start_time)
        msg += "\nCreated policies: " + str(self.created_policies)
        msg += "\nExisting policies: " + str(self.existing_policies)
        if self.warning_count > 0:
            msg += "\nWarnings: " + str(self.warning_count)
        return msg

policy_data = PolicyData()

def main():
    parser = OptionParser(usage = "python -u %prog [options] <path>")
    (opts, args) = parser.parse_args()
    root_path = args[0]
    if not os.path.isdir(root_path):
        print "ERROR: %s is not a directory" % root_path
        sys.exit(1);

    try:
        global policy_data
        print "Started " + str(policy_data.start_time)
        parseConfigurationFile(policy_data)
        print "Generating S60 distribution policies to", root_path
        print "Policy config:", str(policy_data.policies)
        print "Policy for root dir:", str(policy_data.root_policy)
        writePolicies(root_path, policy_data)
        policy_data.stop_time = datetime.datetime.now()
        print "Finished " + str(policy_data.stop_time)
        print str(policy_data)
    except:
        print "ERROR: Unexpected exception"
        traceback.print_exc()
        print str(policy_data)
        sys.exit(1)

def parseConfigurationFile(policy_data):
    config_file = "distribution.policy.s60.configuration.txt"
    f = open(config_file)
    try:
        default_policy = None
        line_count = 0;
        for line in f.readlines():
            line_count += 1
            line = line.strip()
            if len(line) == 0 or line.find("#") == 0:
                continue
            tokens = line.split(':')
            if len(tokens) != 2:
                print "ERROR: Invalid line", line_count, "at file", config_file
                sys.exit(1)
            dir = tokens[0].strip()
            dir = dir.replace("/", os.sep)
            policy = tokens[1].strip()
            if dir == "DEFAULT_POLICY":
                default_policy = policy
            elif dir == "ROOT_POLICY":
                policy_data.root_policy = policy
            else:
                policy_data.policies += [(dir, policy)]
        if default_policy is None:
            print "ERROR: Default policy not defined at", config_file
            sys.exit(1)
        policy_data.policies += [("", default_policy)]
    finally:
        f.close()

def writePolicies(root_path, policy_data):
    # Write policy for the root directory.
    if (not policy_data.root_policy is None) and \
            (policy_data.root_policy != "IGNORE"):
        writePolicy(root_path, policy_data.root_policy)
    # Write policies for the subdirectories.
    for root, dirs, files in os.walk(root_path):
        for dir in dirs:
            path = os.path.join(root, dir)
            policy = getPolicyForPath(path.replace(root_path, ""), policy_data)
            if policy != "IGNORE":
                writePolicy(path, policy)

def writePolicy(path, policy):
    global policy_data
    policy_file = os.path.join(path, 'distribution.policy.s60')

    if os.path.exists(policy_file):
        existing_policy = readPolicy(policy_file)
        if policy == existing_policy:
            print "Policy %s already exists in %s" % (existing_policy, policy_file)
        else:
            print "WARNING: Policy %s (should be %s) already exists in %s"  % (existing_policy, policy, policy_file)
            policy_data.warning_count += 1
        policy_data.existing_policies += 1
        return

    f = open(policy_file, "w+")
    try:
        print "Writing policy", policy, "to", policy_file
        f.write(policy)
        policy_data.created_policies += 1
    except IOError:
        print "ERROR: Writing policy to", policy_file, "failed"
        sys.exit(1)
    finally:
        f.close

def readPolicy(policy_file):
    policy = None
    f = open(policy_file)
    try:
        for line in f.readlines():
            # Take the last line from the policy.
            policy = line.strip()
    finally:
        f.close
    return policy

def getPolicyForPath(target_path, policy_data):
    for path, policy in policy_data.policies:
        match_path = target_path
        if path.find("ROOT" + os.sep) == 0:
            # Add keyword ROOT to target_path so that only
            # paths in root directory will match.
            match_path = "ROOT" + target_path
        if match_path.find(path + os.sep) != -1 or \
                match_path.find(path, -len(path)) != -1:
            return policy
    return None

if __name__ == "__main__":
    main()
