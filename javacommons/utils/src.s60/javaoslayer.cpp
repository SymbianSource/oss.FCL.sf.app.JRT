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

#include <dirent.h>
#include <memory>
#include <string.h> // Needed for strcmp
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <e32err.h>
#include <f32file.h>
#include <hal.h>
#include <flogger.h>
#include "logger.h"
#include "javaoslayer.h"
#include "javasymbianoslayer.h"
#include "libraryloaderexception.h"
#include "dynamiclibloader.h"
#include "javacommonutils.h"
#include "s60commonutils.h"

using namespace java::util;

const char * const BOOT_CLASSPATH_MIDP_FILE = "midpodclist";
const char * const BOOT_CLASSPATH_INSTALLER_FILE = "installerodclist";
const char * const BOOT_CLASSPATH_TCKRUNNER_FILE = "tckrunnerodclist";
const char * const BOOT_CLASSPATH_JAVACONTROLPANEL_FILE = "javacontrolpanelodclist";

_LIT(KStartUpTraceDir, "java\\full");
_LIT(KStartUpTraceFile, "startup.txt");
_LIT(KTraceSpaceExtra, "                                         ");

OS_EXPORT void JavaOsLayer::startUpTrace(const std::string& header,
        int freeMem, int totalMem)
{
    if (header.length() == 0)
    {
        _LIT(KDash, "-");
        RFileLogger::Write(KStartUpTraceDir, KStartUpTraceFile,
                           EFileLoggingModeAppend, KDash);
    }
    else
    {
        TTime time;
        time.HomeTime();
        const int maxStrLen = 41;
        TBuf<maxStrLen>  str;
        _LIT(KFormatStr, "%-B%:0%J%:1%T%:2%S%.%*C2%:3%+B");
        TRAP_IGNORE(time.FormatL(str, KFormatStr));
        int mem = 0;
        int err = HAL::Get(HALData::EMemoryRAMFree, mem);
        if (err != KErrNone)
        {
            mem = err;
        }
        HBufC* buf = stringToDes(header.c_str());
        const int maxLen = 41;
        int headerLen = Min(buf->Length(), maxLen);
        TPtrC headerPtr = buf->Left(headerLen);
        TPtrC spacePtr = KTraceSpaceExtra().Right(maxLen - headerLen);

        int heapUsageTotal;
        User::AllocSize(heapUsageTotal);

        _LIT(KOutStr, "%S%S Time(%S), SysMem(%d), pUsedHeap(%d), free(%d), total(%d)");
        RFileLogger::WriteFormat(KStartUpTraceDir, KStartUpTraceFile,
                                 EFileLoggingModeAppend,
                                 KOutStr,
                                 &headerPtr, &spacePtr, &str, mem, heapUsageTotal, freeMem, totalMem);
        delete buf;
    }
}


OS_EXPORT void JavaOsLayer::getOsSpecificLibName(std::string& result,
        const char* libName)
{
//    JELOG2(EUtils);
    if (libName)
    {
        result = libName;
    }
    else
    {
        ELOG(EUtils, "JavaOsLayer::getOsSpecificLibName() libname was null");
    }
}

OS_EXPORT void JavaOsLayer::getOsSpecificJavaRootPath(std::string& path)
{
//    JELOG2(EUtils);
    path = "c:\\java";
}

OS_EXPORT std::string& JavaOsLayer::getBinRoot(std::string& path, bool append)
{
//    JELOG2(EUtils);
    const char* start = ":\\sys\\bin\\";
    if (append)
    {
        path += start;
    }
    else
    {
        path = start+1;
    }
    return path;
}

OS_EXPORT std::string& JavaOsLayer::getResRoot(std::string& path, bool append)
{
//    JELOG2(EUtils);
    const char* start = ":\\resource\\java\\";
    if (append)
    {
        path += start;
    }
    else
    {
        path = start+1;
    }
    return path;
}

OS_EXPORT std::string& JavaOsLayer::getMidpRoot(std::string& path, bool append)
{
//    JELOG2(EUtils);
    const char* start = ":\\private\\102033E6\\";
//    const char* start = ":\\private\\200211D9\\";
    if (append)
    {
        path += start;
    }
    else
    {
        path = start+1;
    }
    /*
        TUint32 uid = RProcess().SecureId().iId;
        std::ostringstream oss;
        oss<<std::hex<<uid;
        path += oss.str();
        path += "]\\";
    */
    return path;
}

OS_EXPORT std::string& JavaOsLayer::getJavaCaptainRoot(std::string& path, bool append)
{
//    JELOG2(EUtils);
    const char* start = ":\\private\\200211DC\\";
    if (append)
    {
        path += start;
    }
    else
    {
        path = start+1;
    }
    return path;
}

OS_EXPORT DriveId JavaOsLayer::getMidpDrive()
{
//    JELOG2(EUtils);
    int driveNum = 1;
    TUint16 driveLetter = RProcess().FileName()[0];
    RFs::CharToDrive(driveLetter, driveNum);
    char* drive = new char[2];
    drive[0] = *((char*)&driveLetter);
    drive[1] = 0;
    return DriveId(drive, driveNum);
}


OS_EXPORT void* JavaOsLayer::dlopen(const char* libName)
{
    JELOG2(EUtils);
    RLibrary* libAccess = 0;
    if (libName)
    {
        libAccess = new RLibrary();
        if (libAccess)
        {
            HBufC* buf = stringToDes(libName);
            if (buf)
            {
                int status = libAccess->Load(*buf);
                LOG2(EUtils, EInfo, "JavaOsLayer::dlopen(%s), Handle: %X",
                     libName, libAccess);
                delete buf;
                if (status != KErrNone)
                {
                    delete libAccess;
                    libAccess = 0;
                    throw LibraryLoaderException(OPENING_LIBRARY_FAILED, status,
                                                 "Error opening Symbian lib.",
                                                 __FILE__, __FUNCTION__, __LINE__);

                }
                else
                {
                    //The code below makes RLibrary accessible from any thread.
                    // copies the handle without doing Open()
                    RLibrary duplicate=*libAccess;
                    // overwrite the original
                    status = libAccess->Duplicate(RThread());
                    duplicate.Close(); // close the original
                }
            }
        }
    }
    else
    {
        ELOG(EUtils, "JavaOsLayer::dlopen() libname was null");
    }
    return libAccess;
}

OS_EXPORT void* JavaOsLayer::dlsym(void* handle, const char* name,
                                   bool containsByteCode)
{
    JELOG2(EUtils);
    void* func = 0;
    if (handle && name)
    {
        RLibrary*  libAccess = reinterpret_cast<RLibrary*>(handle);
        LookupFunc lookup = (LookupFunc)libAccess->Lookup(containsByteCode?2:1);
        if (lookup)
        {
            func = (void*) lookup(name);
            LOG2(EUtils, EInfo, "JavaOsLayer::dlsym() fuction name: %s, "
                 "fPtr: %X", name, func);
            if (func == 0)
            {
                ELOG1(EUtils, "dlsym failed for %s.", name);
            }
        }
        else
        {
            ELOG1(EUtils,
                  "No lookup method in ordinal 1 when looking for %s.", name);
        }
    }
    else
    {
        ELOG2(EUtils, "JavaOsLayer::dlsym() null argument handle: %X, name %X",
              handle, name);
    }
    return func;
}

OS_EXPORT int JavaOsLayer::dlclose(void* handle)
{
    JELOG2(EUtils);
    if (handle)
    {
        RLibrary*  libAccess = reinterpret_cast<RLibrary*>(handle);
        libAccess->Close();
        delete libAccess;
    }
    else
    {
        ELOG(EUtils, "dlclose failed, null handle");
    }
    return 0;
}
OS_EXPORT void JavaOsLayer::bootClassPath(std::list<std::wstring>& odcFiles,
        std::list<std::wstring>& bcpEntites,
        const int pathType)
{
    /*
     * getResRoot requires drive letter but VerifiedFileNameL adds correct one.
     * C drive used that it is checked first and if file is found that is used
     * after that z is checked.
     */
    std::string path = "c";
    JavaOsLayer::getResRoot(path, true);

    if (BOOT_CLASSPATH_MIDP == pathType)
    {
        path.append(BOOT_CLASSPATH_MIDP_FILE);
    }
    else if (BOOT_CLASSPATH_INSTALLER == pathType)
    {
        path.append(BOOT_CLASSPATH_INSTALLER_FILE);
    }
    else if (BOOT_CLASSPATH_TCKRUNNER == pathType)
    {
        path.append(BOOT_CLASSPATH_TCKRUNNER_FILE);
    }
    else if (BOOT_CLASSPATH_JAVACONTROLPANEL == pathType)
    {
        path.append(BOOT_CLASSPATH_JAVACONTROLPANEL_FILE);
    }
    else
    {
        path.append(BOOT_CLASSPATH_MIDP_FILE);
    }

    std::auto_ptr<HBufC>pathDes(stringToDes(path.c_str()));
    TFileName realPathName;
    TRAPD(err, realPathName = S60CommonUtils::VerifiedFileNameL(*pathDes));

    if (KErrNone == err)
    {
        std::wstring temp((wchar_t*)realPathName.PtrZ());
        std::string odcFileName(temp.begin(), temp.end());

        std::vector<std::string> romClasspathEntries;
        std::string dir_path("Z:\\resource\\java\\jvm\\lib\\jrt");

        listDirectory(dir_path, romClasspathEntries);  // This is always needed.

        if (odcFileName[0] == 'c' || odcFileName[0] == 'C')  // IAD deployed
        {
            std::vector<std::string> updatedClasspathEntries;
            listOdcFile(path, updatedClasspathEntries);  // Read all ODC files from odc file

            std::string::size_type const delimiter = path.rfind("\\");
            path = path.substr(0, delimiter);  // IAD ODC file directory

            populateCPEntries((path + "\\jvm\\lib\\jrt\\"), updatedClasspathEntries, odcFiles, bcpEntites);

            std::vector<std::string> romDeltaEntries;

            // Add all ROM entries that are not deployed using IAD.
            deltaCPEntries(romClasspathEntries, updatedClasspathEntries, romDeltaEntries);

            // Add ROM only entries
            populateCPEntries((dir_path + "\\"), romDeltaEntries, odcFiles, bcpEntites);
        }
        else  // Classpath only at ROM so using dir listing.
        {
            populateCPEntries((dir_path + "\\"), romClasspathEntries, odcFiles, bcpEntites);
        }
    }
    else
    {
        ELOG1(EUtils, "Cannot get real path name for: '%s'", path.c_str());
    }
}

void JavaOsLayer::listDirectory(std::string& aDirPath, std::vector<std::string>& aEntries)
{
    DIR *pDIR = opendir(aDirPath.c_str());    // open directory

    if (pDIR == NULL)
    {
        std::string errorMsg("Cannot read ROM ODC files: ");
        errorMsg.append(strerror(errno));
        errorMsg.append(" : dir: ");
        errorMsg.append(aDirPath.c_str());
        ELOG(EUtils, errorMsg.c_str());
        // Suppres error if no Java at all on device ROM.
    }

    struct dirent* pDirEnt = readdir(pDIR);
    while (pDirEnt != NULL)
    {
        aEntries.push_back(std::string(pDirEnt->d_name));
        pDirEnt = readdir(pDIR);
    }

    closedir(pDIR);    // Release the open directory
}

void JavaOsLayer::listOdcFile(std::string& filePath, std::vector<std::string>& entries)
{
    std::ifstream odcFile;
    odcFile.open(filePath.c_str(), std::ifstream::in);

    if (odcFile)
    {
        std::string line;
        while (std::getline(odcFile, line))
        {
            // Trim enter if exists.
            std::string::size_type const enter = line.find_last_not_of("\n");
            entries.push_back(line.substr(0, enter));
        }
        odcFile.close();
    }
    else
    {
        ELOG1(EUtils, "Cannot read file: '%s'", filePath.c_str());
    }
}

void JavaOsLayer::populateCPEntries(const std::string& pathPrefix,
                                    const std::vector<std::string>& entries,
                                    std::list<std::wstring>& odcFiles,
                                    std::list<std::wstring>& bcpEntites)
{
    std::vector<std::string>::const_iterator iter;

    for (iter = entries.begin(); iter != entries.end(); ++iter)
    {
        std::string entryName((*iter));
        entryName.insert(0, pathPrefix);    // Full path needed.

        std::wstring wideEntryName = L"";

        try
        {
            wideEntryName = JavaCommonUtils::utf8ToWstring(entryName.c_str());
        }
        catch (ExceptionBase& eb)
        {
            // Suppress error to skip invalid ones.
            ELOG2(EUtils, "utf8->wstring failed on cp entry: '%s' exp: %s ",
                  entryName.c_str(), eb.toString().c_str());
        }
        size_t pos = wideEntryName.rfind(L".odc");
        bool isOdcFile = (pos != std::string::npos) &&
                         (pos == (wideEntryName.length() - 4));
        if (isOdcFile)
        {
            odcFiles.push_back(wideEntryName);
        }
        else
        {
            bcpEntites.push_back(wideEntryName);
        }
    }
}

void JavaOsLayer::deltaCPEntries(const std::vector<std::string>& romEntries,
                                 const std::vector<std::string>& iadEntries,
                                 std::vector<std::string>& deltaEntries)
{
    std::vector<std::string>::const_iterator iter;  // ROM CP iter
    std::vector<std::string>::const_iterator matchIter; // IAD CP iter
    std::vector<std::string> romDeltaEntries;

    // Iterate through all rom entries
    for (iter = romEntries.begin(); iter != romEntries.end(); ++iter)
    {
        bool match = false;
        // and check if they do not match to IAD ones add to romDeltaEntries.
        for (matchIter = iadEntries.begin(); matchIter != iadEntries.end(); ++matchIter)
        {
            if ((*matchIter) == (*iter))
            {
                match = true;
                break;
            }
        }

        if (!match)
        {
            deltaEntries.push_back((*iter));
        }
    }
}

OS_EXPORT FuncPtr findMethod(const char* funcName,
                             const FuncTable funcTable[],
                             int tableSize)
{
//    JELOG2(EUtils);
    int       res = 0;
    int       mid = 0;
    int       top = 0;
    int       bottom = tableSize-1;

    if (funcName == 0)
    {
        ELOG(EUtils, "findMethod failed, null funcName");
        return 0;
    }
    if (funcTable == 0)
    {
        ELOG(EUtils, "findMethod failed, null funcTable");
        return 0;
    }

    // Loop while the number of the items left in the list is greater
    // than 2.  Each iteration will split the number of items left to search
    // in half
    while ((bottom - top) > 1)
    {
        // This case handles the normal serach case where the number of
        // items left to search is greater than 2
        mid = (top + bottom) / 2;
        res = strcmp(funcName, funcTable[mid].mFuncName);
        if (res == 0)
        {
            return((FuncPtr) funcTable[mid].mFuncAddr);
        }
        if (res > 0)
        {
            top = mid;
        }
        else
        {
            bottom = mid;
        }
    }

    // If there are two items left in the list then the bottom item should be
    // checked for a match
    if (bottom != top)
    {
        // Check the bottom item to see if it is a match
        res=strcmp(funcName, funcTable[bottom].mFuncName);
        if (res == 0)
        {
            return ((FuncPtr) funcTable[bottom].mFuncAddr);
        }
    }

    // Check the top item to see if it is a match
    res=strcmp(funcName, funcTable[top].mFuncName);

    if (res == 0)
    {
        return ((FuncPtr) funcTable[top].mFuncAddr);
    }

    // Neither the top or bottom items were a match so the
    // method must not exist in the file
    return 0;
}

OS_EXPORT HBufC* stringToDes(const char* str)
{
//    JELOG2(EUtils);
    HBufC* resultBuf = 0;
    try
    {
        if (str)
        {
            int len = strlen(str);
            resultBuf = HBufC::New(len + 1);
            TPtr ptr = resultBuf->Des();
            TPtr8 ptr8((TUint8 *)str, len);
            ptr8.SetLength(len);
            ptr.Copy(ptr8);
            ptr.ZeroTerminate();
        }
    }
    catch (...)
    {
        ELOG(EUtils, "stringToDes, exception");
    }
    return resultBuf;
}

OS_EXPORT wchar_t* desToWstring(TPtr16& aDes)
{
    return (wchar_t*) aDes.PtrZ();
}

OS_EXPORT TAppVersion wstringToAppVersion(const std::wstring& aVersionString)
{
    TAppVersion appVersion;
    char* temp = JavaCommonUtils::wstringToUtf8(aVersionString);
    std::string versionStr = temp;

    std::string::size_type idx = 0;
    idx = versionStr.find(".", idx);

    // 00.00.00 --> 00 00.00
    if (idx != std::string::npos)
    {
        versionStr.replace(idx, 1, " ");
    }

    idx = versionStr.find(".", idx + 1);

    // 00 00.00 --> 00 00 00
    if (idx != std::string::npos)
    {
        versionStr.replace(idx, 1, " ");
    }

    int major = 0;
    int minor = 0;
    int build = 0;
    int result = sscanf(
                     versionStr.c_str(), "%d %d %d", &major, &minor, &build);

    if (major > 0)
    {
        appVersion.iMajor = major;
    }

    if (minor > 0)
    {
        appVersion.iMinor = minor;
    }

    if (build > 0)
    {
        appVersion.iBuild = build;
    }
    delete[] temp;

    return appVersion;
}

OS_EXPORT HBufC* wstringToBuf(const std::wstring& aString)
{
    HBufC* stringBuf = HBufC::New(aString.size());
    if (stringBuf != 0)
    {
        TPtr16 namePtr(stringBuf->Des());
        namePtr.Append((const TUint16*)aString.c_str(), aString.size());
    }
    return stringBuf;
}

/**
 *
 */
OS_EXPORT java::util::Uid& TUidToUid(const TUid& aId,java::util::Uid& aOutUid)
{
//  JELOG2(EUtils);
    if (0 == aId.iUid)
        return aOutUid;

    std::wstringstream stream;
    stream.fill('0');
    stream << std::setw(8) << std::hex << (int)aId.iUid; // codescanner::leave

    std::wstring idAsStr;
    idAsStr.reserve(11);
    idAsStr.append(L"[");
    idAsStr.append(stream.str());
    idAsStr.append(L"]");
    Uid tmpUid(idAsStr);
    aOutUid = tmpUid;
    return aOutUid;
}

/**
 *
 */
OS_EXPORT TInt uidToTUid(const java::util::Uid& aUid,TUid& aOutId)
{
//  JELOG2(EUtils);
    long long tmpInt = 0;
    std::wstring uidAsStr = aUid.toString();
    if (0 == uidAsStr.size())
        return KErrArgument;

    JavaCommonUtils::trimWstring(uidAsStr,L'\n');
    JavaCommonUtils::trimWstring(uidAsStr,L'\t');
    JavaCommonUtils::trimWstring(uidAsStr,L' ');

    if ((10 != uidAsStr.size()) && (8 != uidAsStr.size()))
        return KErrArgument;

    if (('[' == uidAsStr.at(0)) && (']' == uidAsStr.at(uidAsStr.size()-1)))
    {
        if (10 != uidAsStr.size())
            return KErrArgument;
        std::wstring plainNumStr = uidAsStr.substr(1,uidAsStr.size()-2);
        std::wstringstream stream(plainNumStr);
        stream >> std::hex >> tmpInt; // codescanner::leave
        if ((false == stream.eof()) || (stream.fail()))
            return KErrArgument;
    }
    else
    {
        std::wstringstream stream(uidAsStr);
        stream >> std::hex >> tmpInt; // codescanner::leave
        if ((false == stream.eof()) || (stream.fail()))
            return KErrArgument;
    }

    if ((tmpInt > 0xEFFFFFFF) || (tmpInt < 0))
        return KErrArgument;
    aOutId.iUid = tmpInt;
    return KErrNone;
}

