/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef JAVAOSLAYER_H
#define JAVAOSLAYER_H

#include <string>
#include <list>
#include <vector>
#include "javaosheaders.h"

namespace java
{
namespace util
{

// Runtime specific bootclasspath identifier.
const int BOOT_CLASSPATH_MIDP = 1;
const int BOOT_CLASSPATH_INSTALLER = 2;
const int BOOT_CLASSPATH_TCKRUNNER = 3;

class DriveId;

class JavaOsLayer
{
public:
    OS_IMPORT static
    void getOsSpecificLibName(std::string& result, const char* libName);

    OS_IMPORT static void getOsSpecificJavaRootPath(std::string& path);

    /**
    * Utility method for getting the root of OMJ binaries. By default the
    * method will set the path without drive letter (in file systems that
    * uses drive letters). The utility will append the leading slash to the
    * path(e.g. the result could be "\sys\bin\" not "\sys\bin").
    * If the append flag is set, then it is assumed that provided argument
    * 'path' includes the drive letter and the implentation will append ':'
    * and the valid path.
    * The utility will return a refrence to the path given as argument just
    * for helping e.g. concatenation.
    * <p>
    * Example in S60:
    *   std::string path;
    *   getBinRoot(path); //This will set path to contain "\sys\bin"
    *   std::string path = 'C';
    *   getBinRoot(path, true); //This will set path to contain "C:\sys\bin"
    *
    * @param[in/out] path Utility will either set or prepend the binary root
    *                depending on the value of argument 'append'
    * @param[in/out] append Whether to append the result into the argument
    *                'path' or override the content. Look the description
    *                above for further instructions.
    * @return Reference to argument 'path'.
    */
    OS_IMPORT static std::string& getBinRoot(std::string& path, bool append = false);

    /**
    * Utility method for getting the root of OMJ resources. The behavior of this
    * method is very similar to getBinRoot, so look for further information from
    * the description of the JavaOsLayer::getBinRoot()
    *
    * @param[in/out] path Utility will either set or prepend the resource root
    *                depending on the value of argument 'append'
    * @param[in/out] append Whether to append the result into the argument
    *                'path' or override the content. Look the description
    *                of JavaOsLayer::getBinRoot() for further instructions.
    * @return Reference to argument 'path'.
    */
    OS_IMPORT static std::string& getResRoot(std::string& path, bool append = false);

    /**
    * Utility method for getting the root of Midp work directory. The behavior of
    * thismethod is very similar to getBinRoot, so look for further information
    * from the description of the JavaOsLayer::getBinRoot()
    *
    * @param[in/out] path Utility will either set or prepend the Midp work
                          directory root depending on the value of argument
                          'append'
    * @param[in/out] append Whether to append the result into the argument
    *                       'path' or override the content. Look the
                             description of JavaOsLayer::getBinRoot() for
    *                        further instructions.
    * @return Reference to argument 'path'.
    */
    OS_IMPORT static std::string& getMidpRoot(std::string& path, bool append = false);

    /**
    * Utility method for getting the root of Java Captain's work directory.
    * The behavior of this method is very similar to getBinRoot, so look for
    * further information from the description of the JavaOsLayer::getBinRoot()
    *
    * @param[in/out] path Utility will either set or prepend the Java Captain's
    *                     work directory root depending on the value of argument
                          'append'
    * @param[in/out] append Whether to append the result into the argument
    *                       'path' or override the content. Look the
                            description of JavaOsLayer::getBinRoot() for
    *                       further instructions.
    * @return Reference to argument 'path'.
    */
    OS_IMPORT static std::string& getJavaCaptainRoot(std::string& path, bool append = false);

    /**
    * Utility method for getting the drive where the MIDP is installed in
    * systems supporting drive letters.
    * @return Instance of class DriveLetter conatining the drive letter as char and
    *         as integer. For further information see DriveLetter class description.
    */
    OS_IMPORT static DriveId  getMidpDrive();
    OS_IMPORT static void*    dlopen(const char* file);
    OS_IMPORT static void*    dlsym(void* handle, const char* name, bool containsByteCode = false);
    OS_IMPORT static int      dlclose(void* handle);

    /**
     * Utility method for tracing the startup time.
     * @param[in] header The header of the trace.
     * @param[in] freeMem The amount of free memory of the Java heap. Set to -1
     *                    if unknown.
     * @param[in] totalMem The size of the Java heap. Set to -1 if unknown.
     */
    OS_IMPORT static void startUpTrace(const std::string& header,
                                       int freeMem, int totalMem);

    /**
     * Populate bootclasspath ODC package names. If not found or error
     * it is left empty.
     *
     * @param[out] odcFiles   list of ODC files for bootclasspath.
     * @param[out] bcpEntites list of other bootclasspath enties than ODC
                              files e.g jar files.
     * @param[in] type bootclasspath type. If not in valid range midp
     *                 bootclasspath is set.
     */
    OS_IMPORT static void bootClassPath(std::list<std::wstring>& odcFiles,
                                        std::list<std::wstring>& bcpEntites,
                                        const int type);


private: //Methods
    JavaOsLayer();                                //No default constructor allowed
    JavaOsLayer(const JavaOsLayer&);              //No copy constructor allowed
    JavaOsLayer& operator= (const JavaOsLayer&);  //No Assignment operator allowed

    /**
     * List directory entries.
     *
     * @param[in] dirPath direcotry path.
     * @param[out] entries directory entries.
     */
    static void listDirectory(std::string& dirPath, std::vector<std::string>& entries);

    /**
     * List odc file contents.
     *
     * @param[in] filePath path to file.
     * @param[out] entries odc file entries.
     */
    static void listOdcFile(std::string& filePath, std::vector<std::string>& entries);

    /**
     * Populate Classpath entries from given vector. This separetes ODC files from other
     * classpath entries and add full path of the items. Vector contains only file names
     * and postfixes i.e. myfile.odc or mygreat.jar.
     *
     * @param[in] pathPrefix Path prefix used to created full path.
     * @param[in] entries Class path entries.
     * @param[out] odcFiles All odc files from entries.
     * @param[out] bcpEntites All other entities.
     */
    static void populateCPEntries(const std::string& pathPrefix,
                                  const std::vector<std::string>& entries,
                                  std::list<std::wstring>& odcFiles,
                                  std::list<std::wstring>& bcpEntites);

    /**
     * Check all rom entries and if they are not matching to iad entries
     * adds them to deltaEntries.
     *
     * @param romEntries[in] ROM classpath entries.
     * @param iadEntries[in] IAD classpath entries.
     * @param deltaEntries[out] all ROM entries not deployed with IAD package.
     */
    static void deltaCPEntries(const std::vector<std::string>& romEntries,
                               const std::vector<std::string>& iadEntries,
                               std::vector<std::string>& deltaEntries);

};

/**
 * A class for storing both drive letter as char and as integer.
 * The integer value 0 means dirve letter A, 1 means drive letter B etc.
 */
class DriveId
{
public:

    /**
     * The constructor that sets the object with default values. Useful for
     * platforms that don't have drive letter concept.
     */
    DriveId() : mLetter(0), mIndex(-1)
    {
        //Set the drive letter to be empty;
        mLetter = new char[1];
        mLetter[0]=0;
    }

    /**
     * The constructor with specified arguments.
     * @param[in] driveLetter Drive letter as char*. This can't be const, since
                  it will be deleted in the destructor;
     * @param[in] driveIndex  Drive letter as integer. 0='A', 1='B'...25='Z'
     */
    DriveId(char* driveLetter, int driveIndex) : mLetter(driveLetter),
            mIndex(driveIndex) {}


    /**
     * The destructor.
     */
    ~DriveId()
    {
        delete[] mLetter;
    }
    /**
     * Getter for the drive letter.
     * @return drive letter as char. The charcter case is unspecified. If
     *         some error has happened when solving the drive letter, the
     *         content is empty ''.
     */
    inline const char* getDriveLetter() const
    {
        return mLetter;
    }

    /**
     * Getter for the drive letter index.
     * @return drive letter index. If some error has happened when solving
     *         the drive letter or the system doesn't support drive letters
     *         -1 is returned.
     */
    inline int getDriveIndex() const
    {
        return mIndex;
    }

private:
    char*   mLetter;
    int     mIndex;
};


} //end namespace util
} //end namespace java

#endif // JAVAOSLAYER_H
