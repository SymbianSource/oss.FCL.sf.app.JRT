/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  FileUtilities - Provied file utilities
 *
*/


#ifndef FILEUTILITIES_H
#define FILEUTILITIES_H

#include <list>
#include <string>

namespace java
{
namespace fileutils
{

OS_NONSHARABLE_CLASS(FileUtilities)
{
public:

    /**
     * Returns boolean value indicating if the path specified pointed to a file.
     *
     * @param[in] aPath: Absolute path of the file/directory
     * @return true if the path specified was a file. false otherwise.
     */
    OS_IMPORT static bool isFile(const std::wstring& aPath);

    /**
     * Returns boolean value indicating if the path specified pointed to a directory.
     *
     * @param[in] aPath: Absolute path of the file/directory
     * @return true if the path specified was a directory. false otherwise.
     */
    OS_IMPORT static bool isDirectory(const std::wstring& aPath);

    /**
     * Checks to see if the path specified exists.
     *
     * @param[in] aPath: Absolute path of the file/directory
     * @return true if the path specified exists. false otherwise.
     */
    OS_IMPORT static bool exists(const std::wstring& aPath);

    /**
     * Checks to see if the path specified can be read.
     *
     * @param[in] aPath: Absolute path of the file/directory
     * @return true if the path specified can be read. false otherwise.
     */
    OS_IMPORT static bool canRead(const std::wstring& aPath);

    /**
     * Checks to see if the path specified can be written into.
     *
     * @param[in] aPath: Absolute path of the file/directory
     * @return true if the path specified can be written. false otherwise.
     */
    OS_IMPORT static bool canWrite(const std::wstring& aPath);

    /**
     * Returns the size of the file.
     *
     * @param[in] aPath: Absolute path of the file
     * @return size of the file in bytes.
     * @throws errno value in case of error
     */
    OS_IMPORT static long fileSize(const std::wstring& aPath);

    /**
     * Returns the last modified time of the file/directory
     *
     * @param[in] aPath: Absolute path of the file/directory
     * @return last modified time of the file.
     * @throws errno value in case of error
     */
    OS_IMPORT static long lastModified(const std::wstring& aPath);

    /**
     * Changes the readable attribute of the file based on the aReadable attribute
     *
     * @param[in] aFile: Absolute path of the file/directory
     * @param[in] aReadable: boolean indicating if read must be allowed or not.
     * @return 0 in case of success, errno value in case of failure
     */
    OS_IMPORT static int setReadable(const std::wstring& aFile, bool aReadable);

    /**
     * Changes the writable attribute of the file based on the aWritable attribute
     *
     * @param[in] aFile: Absolute path of the file/directory
     * @param[in] aWritable: boolean indicating if write must be allowed or not.
     * @return 0 in case of success, errno value in case of failure
     */
    OS_IMPORT static int setWritable(const std::wstring& aFile, bool aWritable);

    /**
     * Returns the available size of the file system.
     *
     * @param[in] aFile: Absolute path of the file.
     * @return Available size of File System, -1 if error during access or
     *         process does not have access to the file system.
     */
    OS_IMPORT static long long availableSize(const std::wstring& aFile);

    /**
     * Returns the Total Size of the File System.
     *
     * @param[in] aFile: Absolute path of the file.
     * @return Total Size of File System, -1 if error during access or
     *         process does not have access to the file system.
     */
    OS_IMPORT static long long totalSize(const std::wstring& aFile);

    /**
     * Returns the Used Size of the File System.
     *
     * @param[in] aFile: Absolute path of the file.
     * @return Used Size in File System, -1 if error during access or
     *         process does not have access to the file system.
     */
    OS_IMPORT static long long usedSize(const std::wstring& aFile);

    /**
     * Checks if the File/Directory is hidden.
     * In Linux, it is simply a check to see if the File begins with a "."
     * In File Systems that rely on attributes to differentiate between hidden
     * and visible files, the method checks those attributes.
     *
     * @param[in] aFile: Absolute path of the file.
     * @return true of the file is hidden, false otherwise
     */
    OS_IMPORT static bool isHidden(const std::wstring& aPath);

    /**
     * Lists the roots of the File System. Roots are synonymous to mount points
     * in Linux file system.
     *
     * @return string containing the roots. Delimited by "*" Character.
     *         The implementation using this is responsible to split it into
     *         multiple tokens.
     */
    OS_IMPORT static std::wstring listRoots();

    /**
     * Method to get the contents of a Directory. By default, hidden files are
     * also included.
     *
     * @param[in] dirName: Directory name with path included.
     * @return string of names delimited by "*"
     */
    OS_IMPORT static std::wstring getDirContents(const std::wstring& aDirName);

    /**
     * Method to get the contents of a Directory.
     *
     * @param[in] aDirName: Directory name with path included.
     * @param[in] aIncludeHidden: specifies if hidden files must be included
     * @return string of names delimited by "*"
     */
    OS_IMPORT static std::wstring getDirContents(const std::wstring& aDirName, const bool aIncludeHidden);

    /**
     * Method to get the contents of a Directory.
     *
     * @param[in] aDirName: Directory name with path included.
     * @return a list of strings containing the names of the contents
     */
    OS_IMPORT static std::list<std::wstring> getDirContentsList(const std::wstring& aDirName);

    /**
     * Returns the size of the Directory
     *
     * @param[in] aDirName: Directory name with complete path
     * @param[in] aIncludeSubDirs: To specify if the contents of the
     *            sub-directories also needs to be taken while calculating the
     *            size of the file.
     * @return the size of the Directory
     */
    OS_IMPORT static long getDirSize(const std::wstring& aDirName, const bool aIncludeSubDirs);

    /**
     * Creates a directory tree specified.
     *
     * @param[in] aDirPath: Directory name with complete path
     * @return 0 on success, -1 on failure
     */
    OS_IMPORT static int makeDirAll(const std::wstring& aDirPath);

}; //end class

} //end namespace fileutils
} //end namespace java

#endif // FILEUTILITIES_H
