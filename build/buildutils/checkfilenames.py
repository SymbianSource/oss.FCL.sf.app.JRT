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
# Checks filenames in specified paths for maximum path length, illegal
# characters, uppercase characters and unknown filename extensions.
#
# http://developer.symbian.org/wiki/index.php/Coding_Standards_and_Conventions#File_and_Directory
#

import os, re, sys, traceback
from optparse import OptionParser

# Patterns for matching illegal characters:
# & ^ + - @ $ % * ( ) | / [ ] { } < > ? ; : , " '
# Space and dash omitted:
re_illegal_chars = re.compile(
    ".*[\&\^\+\@\$\%\*\(\)\|\[\]\{\}\<\>\?\;\:\,\"\'].*")
# Space and dash included:
re_illegal_strictchars = re.compile(
    ".*[\s\&\^\+\-\@\$\%\*\(\)\|\[\]\{\}\<\>\?\;\:\,\"\'].*")

# List of known filename extensions in lowercase and in alphabetical order.
known_exts = [
    ".3gp",
    ".bat", ".bin", ".bmp",
    ".c", ".cat", ".cer", ".cfg", ".class", ".classpath",
     ".classpath_qt", ".classpath_qt_j2me",
    ".cmd", ".comp", ".conf", ".css", ".confml", ".cpp", ".cproject", ".crml",
    ".cvsignore", ".dat", ".dcf", ".def", ".defs", ".dll", ".der", ".doc",
    ".dr", ".dso", ".dsp", ".dsw", ".dtd",
    ".emx", ".exe", ".flm", ".gcfml", ".gif",
    ".h", ".hrh", ".htm", ".html", ".iby", ".inf", ".ini", ".inl",
    ".jad", ".jar", ".java", ".javaversion", ".jpeg", ".jpg", ".js", ".jsp", ".jupiter",
    ".key", ".ks", ".launch", ".lib", ".loc", ".log",
    ".mak", ".make", "makefile", "makefile_akn", "makefile_qt",  "manifest", ".mbm", ".mdl",
    ".meta", ".metadata", ".metaxml", ".mf", ".mif", ".mk", ".mmh", ".mmp",
    ".m3g", ".nonqt", ".odc",
    ".pem", ".pkg", ".pl", ".plugin", ".png", ".prefs", ".prf", ".pri", ".pro",
    ".project", ".project_classpath_builder", ".project_normal", ".mtj",
    ".eclipseme", ".properties", ".py",
    "readme", ".reg", ".result", ".review", ".rh", ".rsg", ".rss",
    ".ser", ".sh", ".sis", ".sisx",
    ".sps", ".subsystem", ".svg", ".svgb", ".s60",
    ".template", ".txt", ".vcf", ".vcs",
    ".wav", ".xls", ".xml", ".xpf", ".xsl", ".zip",
    ".rm", ".aac", ".amr", ".awb", ".au", ".jts", ".mid", ".mp3", ".mp4",
    ".ra", ".wma", ".mpg", 
# Peculiar filenames under jrt/tools directory:
    "exemakefile", "ijg_readme", "objmakefile", "staticlibmakefile", "testexemakefile",
    "findtr", "syncqt", "new",
# Other peculiar filenames:
    "installerodclist", "installerodclist30", "midpodclist", "midpodclist30",
    "notice", "package-list", "tckrunnerodclist", "tckrunnerodclist30", "unknowncert","midprootslist",
]

# Command line options.
opts = None
# Status code to be returned when script exits.
status_code = 0

def main():
    global opts, status_code
    # Parse command line options and arguments.
    parser = OptionParser(
        usage = "python -u %prog [options] <path> ...",
        description = "Checks filenames in specified paths for maximum " + \
            "path length, illegal characters, uppercase characters and " + \
            "unknown filename extensions. By default makes --length, --chars " + \
            "and --exts checks.")
    parser.add_option("--length", dest="length",
                      action="store_true", default=False,
                      help="check path lengths")
    parser.add_option("--chars", dest="chars",
                      action="store_true", default=False,
                      help="check for illegal characters, ignoring spaces and dashes")
    parser.add_option("--strictchars", dest="strictchars",
                      action="store_true", default=False,
                      help="check for all illegal characters")
    parser.add_option("--uppercase", dest="uppercase",
                      action="store_true", default=False,
                      help="check for uppercase characters")
    parser.add_option("--exts", dest="exts",
                      action="store_true", default=False,
                      help="check for unknown filename extensions")
    (opts, args) = parser.parse_args()
    if not (opts.length or opts.chars or opts.strictchars or \
            opts.uppercase or opts.exts):
        # No checks specified, make all checks except uppercase and strictchars.
        opts.length = True
        opts.chars = True
        opts.strictchars = False
        opts.uppercase = False
        opts.exts = True
    paths = []
    for arg in args:
        paths += [arg]

    try:
        # Check all filenames under paths that were given as arguments.
        prefix_path = "jrt"
        for path in paths:
            check_filenames(prefix_path, path)
    except:
        print "ERROR: Unexpected exception"
        traceback.print_exc()
        sys.exit(1)

    # If no exceptions occurred, exit with status_code.
    sys.exit(status_code)

def check_filenames(prefix_path, path):
    for root, dirs, files in os.walk(path):
        if root.find(path + os.sep) == 0:
            repl_prefix = path + os.sep
        else:
            repl_prefix = path
        dir_path = os.path.join(prefix_path, root.replace(repl_prefix, ""))
        for dir in dirs:
            dp_path = os.path.join(dir_path, dir, "distribution.policy.s60")
            if not os.path.exists(dp_path):
                check_filename(dp_path)
            check_dirname(os.path.join(root, dir))
        for file in files:
            file_path = os.path.join(dir_path, file)
            check_filename(file_path)

def check_dirname(dirname):
    # Ignore .svn paths.
    if dirname.find(os.sep + ".svn" + os.sep) != -1:
        return
    global status_code
    if not os.listdir(dirname):
        print "Empty directory " + dirname
        status_code = 1

def check_filename(filename):
    # Ignore .svn paths.
    if filename.find(os.sep + ".svn" + os.sep) != -1:
        return
    global opts, re_illegal_chars, re_illegal_strictchars, known_exts, status_code
    if opts.length:
        # Check maximum path length.
        l = len(filename)
        if l > 140:
            print "Too long path (" + str(l) + "): " + filename
            status_code = 1
    if opts.strictchars:
        # Check for all illegal characters.
        if re_illegal_strictchars.match(filename):
            print "Illegal character: " + filename
            status_code = 1
    elif opts.chars:
        # Check for illegal characters.
        if re_illegal_chars.match(filename):
            print "Illegal character: " + filename
            status_code = 1
    path = os.path.dirname(filename)
    (root, ext) = os.path.splitext(filename)
    if opts.uppercase:
        if ext == ".java" or ext == ".class":
            # Check for uppercase characters in dir name.
            if path.lower() != path:
                print "Uppercase character: " + filename
                status_code = 1
        else:
            # Check for uppercase characters.
            if filename.lower() != filename:
                print "Uppercase character: " + filename
                status_code = 1
    if opts.exts:
        # Check for unknown extensions.
        if ext == "":
            ext = os.path.basename(filename)
        if not ext.lower() in known_exts:
            print "Unknown extension: " + filename
            status_code = 1

if __name__ == "__main__":
    main()
