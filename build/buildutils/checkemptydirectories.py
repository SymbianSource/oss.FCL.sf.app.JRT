#!/usr/bin/python
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
#   Checks for empty directories (ignoring .svn directories) recursively 
#   starting from the given path. This check is done because Mercurial does 
#   not support empty directories.

import sys, os, re

def main():

    def visitFun(arg, dirname, names):

        # Skip SVN directories
        if dirname.find("\\.svn") != -1:
            return
        
        # Check if the directory is empty (except for the .svn subdirectory)
        if len(names) == 0 or (len(names) == 1 and names[0] == ".svn"):
            print dirname

    root = sys.argv[1]
    os.path.walk(root, visitFun, None)

    
if __name__ == "__main__":
    main()
