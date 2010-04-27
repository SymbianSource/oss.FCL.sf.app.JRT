#!/usr/bin/python
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
#   Searches all the distribution.file.s60 files recursively starting from 
#   a directory given as parameter, reports about missing files, generates
#   usage report per distribution policy ID and generates a summary.

import os, sys

def main():

    missingList = []    # List containing directories without policy file
    idLists = {}        # Set containing lists of directories for each distribution policy ID

    findFiles(sys.argv[1], missingList, idLists)
    
    missingList.sort();
    
    for id in idLists:
        idLists[id].sort
    
    ids = idLists.keys()
    ids.sort()
    
    print "Analysis started from directory: " + sys.argv[1]
    print ""
    
    print "Directories without distribution policy file:"
    for dir in missingList:
        print "    " + dir
    print ""
    
    for id in ids:
        print "Directories with distribution policy ID " + id + ":"
        list = idLists[id]
        for dir in list:
            print "    " + dir
        print ""
        
    
# Go through the directories recursively
#   If some directory doesn't contain policy file, add that directory to the list of "directories without policy file"
#   Else read the policy ID from the file and add the directory to the list corresponding to the found ID


def findFiles(startDir, missingList, idLists):

    def visitFun(arg, dirname, names):
        try:
            f = open(dirname + "\\distribution.policy.s60")
            id = f.readline()
            if id[len(id)-1] == '\n':
                id = id[0:len(id)-1]
            f.close()
            
            idList = idLists.get(id);
            if idList == None:
                idLists[id] = [ dirname ]
            else:
                idList.append(dirname)
                
        except IOError:
            # file was not found
            missingList.append(dirname)            
            
            
    os.path.walk(startDir, visitFun, None)


if __name__ == "__main__":
    main()
