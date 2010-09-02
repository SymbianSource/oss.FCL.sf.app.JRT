/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is meant for starting the J9 JVM.
*
*/

#include <string>
#include <fstream>


#include "j9starters60.h"

#include "jvmargsmodifier.h"

#include "logger.h"
#include "javaoslayer.h"
#include "dynamiclibloader.h"
#include "javacommonutils.h"

using namespace java::runtime;
using namespace java::util;

extern const wchar_t CLASS_PATH_SEPARATOR = L';';
extern const char PATH_SEPARATOR_FROM  = '/';
extern const char PATH_SEPARATOR_TO    = '\\';

const wchar_t* const ODC_PROPERTY = L"-Dcom.ibm.j9.ext.odcs=";


EXPORT_C JvmStarter*
JvmStarter::getJvmStarterInstance(const Configuration configuration,
                                  const std::wstring& indetifier)
{
    JELOG2(EJavaRuntime);
    return  new J9StarterS60(configuration, indetifier);
}

EXPORT_C JvmStarter*
JvmStarter::getJvmStarterInstance()
{
    JELOG2(EJavaRuntime);
    return new J9StarterS60();
}

J9StarterS60::J9StarterS60()
#ifdef __WINSCW__
    :
    mVariant(0)
#endif // __WINSCW__
{
    JELOG2(EJavaRuntime);
}

J9StarterS60::J9StarterS60(const Configuration configuration,
                           const std::wstring& indetifier)
#ifdef __WINSCW__
    :
    mVariant(0)
#endif // __WINSCW__
{
    JELOG2(EJavaRuntime);
    mConfiguration = configuration;
    mIdentifier = indetifier;
    setDefaultArguments();
}


J9StarterS60::~J9StarterS60()
{
    JELOG2(EJavaRuntime);
#ifdef __WINSCW__
    delete mVariant;
    mVariant = 0;
    mLibLoader.Close();
#endif // __WINSCW__
}

void J9StarterS60::setDefaultArguments()
{
    if (mConfiguration == CLDC)
    {
        mJvmArgs.push_back(L"-jcl:cldc11:nokiaextcldc");
        mJvmArgs.push_back(L"-Xfuture:cldc");
        mJvmArgs.push_back(L"-Dcom.nokia.jvm.port=j9.JvmPortCldc");
    }
    else if (mConfiguration == CDC)
    {
        mJvmArgs.push_back(L"-jcl:cdc11:nokiaextcdc");
        mJvmArgs.push_back(L"-Dcom.nokia.jvm.port=j9.JvmPortCdc");
    }
    else if (mConfiguration == FOUNDATION)
    {
        mJvmArgs.push_front(L"-jcl:foun11:nokiaextcdc");
        mJvmArgs.push_front(L"-Dcom.nokia.jvm.port=j9.JvmPortFoun");
    }
#ifdef RD_JAVA_UI_QT
    mJvmArgs.push_back(L"-Xmso80K"); // Native thread stack size.
    mJvmArgs.push_back(L"-Dcom.nokia.mj.impl.rt.ui="
                       L"com.nokia.mj.impl.rt.ui.qt.RuntimeUiQt");
#else // RD_JAVA_UI_QT
    mJvmArgs.push_back(L"-Xmso16k"); // Native thread stack size.
    mJvmArgs.push_back(L"-Dcom.nokia.mj.impl.rt.ui="
                       L"com.nokia.mj.impl.rt.ui.avkon.RuntimeUiAvkon");
#endif // RD_JAVA_UI_QT

    mJvmArgs.push_back(L"-Dfile.encoding=ISO-8859-1");
    mJvmArgs.push_back(L"-Dmicroedition.connection.pkgs="
                       L"com.nokia.mj.impl.gcf.protocol");
    mJvmArgs.push_back(L"-Dmicroedition.encoding=ISO-8859-1");
    mJvmArgs.push_back(L"-Xgc:finInc=32,compactMaxRelocationEntries=4096,"
                       L"finalizeMasterPriority=10,finalizeSlavePriority=10");
    mJvmArgs.push_back(L"-Xiss1K");   // Initial java thread stack size.
    mJvmArgs.push_back(L"-Xits8K");   // Buffer for class/method/field names
    // & signatures.
    mJvmArgs.push_back(L"-Xminf0.1"); // Min percentage of heap free after GC.
    mJvmArgs.push_back(L"-Xmaxf0.3"); // Max percentage of heap free after GC.
    mJvmArgs.push_back(L"-Xmine16K"); // Minimum size for heap expansion.
    mJvmArgs.push_back(L"-Xmns64K");  // Initial new space size. Keep this in sync with MemoryLogger.java
    mJvmArgs.push_back(L"-Xmos64K");  // Initial old space size. Keep this in sync with MemoryLogger.java

#ifdef __WINSCW__
    mJvmArgs.push_back(L"-Xmox16M");  // Maximum old space size.
    mJvmArgs.push_back(L"-Xmx16M");   // Memory maximum.
#else // __WINSCW__
    mJvmArgs.push_back(L"-Xmox64M");  // Maximum old space size.
    mJvmArgs.push_back(L"-Xmx64M");   // Memory maximum.
#endif  // __WINSCW__

    mJvmArgs.push_back(L"-Xmco16k");  // ROM class segment increment.
    mJvmArgs.push_back(L"-Xmr1k");    // Remembered set size.

    // Set the java home to point either to c:\\resource\\java\\jvm or
    // z:\\resource\\java\\jvm. Default is z drive if the dir doesn't
    // exist on the c drive
    std::wstring javaHome = L"-Djava.home=";
    struct stat fileStat;
    int err = lstat("c:\\resource\\java\\jvm\\bin", &fileStat);
    if (err == 0)
    {
        javaHome += L"c";
    }
    else
    {
        javaHome += L"z";
    }
    javaHome += L":\\resource\\java\\jvm";

    mJvmArgs.push_back(javaHome.c_str());

    // Not using extension dir to list platform odc files.
    setInternalOdcFiles();

}
void J9StarterS60::overrideOldHeapSize(int heapSize)
{
    JELOG2(EJavaRuntime);
    std::wstring oldSpace = L"-Xmos";
    oldSpace += JavaCommonUtils::intToWstring(heapSize);
    oldSpace += L"K";
    mJvmArgs.push_back(oldSpace);
}


void J9StarterS60::overrideMaxHeapSize(int heapSize)
{
    JELOG2(EJavaRuntime);
    std::wstring heapAsString(JavaCommonUtils::intToWstring(heapSize));
    doOverideHeap(L"-Xmox", heapAsString);
    doOverideHeap(L"-Xmx", heapAsString);
}

void J9StarterS60::doOverideHeap(const std::wstring& arg, const std::wstring& size)
{
    JELOG2(EJavaRuntime);
    std::wstring maxHeapArg(arg);
    maxHeapArg += size;
    maxHeapArg += L"K";
    mJvmArgs.push_back(maxHeapArg);
}

void J9StarterS60::overrideNewHeapSize(int heapSize)
{
    JELOG2(EJavaRuntime);
    std::wstring newSpace = L"-Xmns";
    newSpace += JavaCommonUtils::intToWstring(heapSize);
    newSpace += L"K";
    mJvmArgs.push_back(newSpace);
}

void J9StarterS60::overrideNativeStackSize(int stackSize)
{
    JELOG2(EJavaRuntime);
    std::wstring stackSizeStr = L"-Xmso";
    stackSizeStr += JavaCommonUtils::intToWstring(stackSize);
    stackSizeStr += L"K";
    mJvmArgs.push_back(stackSizeStr);
}

void J9StarterS60::overrideJavaStackSize(int stackSize)
{
    JELOG2(EJavaRuntime);
    std::wstring stackSizeStr = L"-Xiss";
    stackSizeStr += JavaCommonUtils::intToWstring(stackSize);
    stackSizeStr += L"K";
    mJvmArgs.push_back(stackSizeStr);
}

void J9StarterS60::setInternalOdcFiles()
{
    JELOG2(EJavaRuntime);

    int pathType = BOOT_CLASSPATH_MIDP;
    if (mIdentifier == L"Installer")
    {
        pathType = BOOT_CLASSPATH_INSTALLER;
    }
    else if (mIdentifier == L"TCK_runner")
    {
        pathType = BOOT_CLASSPATH_TCKRUNNER;
    }
    else if (mIdentifier == L"JavaControlPanel")
    {
        pathType = BOOT_CLASSPATH_JAVACONTROLPANEL;
    }

    std::list<std::wstring> odcFiles;
    std::list<std::wstring> bcpEntities;
    JavaOsLayer::bootClassPath(odcFiles, bcpEntities, pathType);

    // Collect all the odc files into a comma separated list.
    for (std::list<std::wstring>::const_iterator iter = odcFiles.begin();
            iter != odcFiles.end();
            ++iter)
    {
        appendOdcFile(*iter);
    }

    // Add the rest of the files into bootclasspath.
    for (std::list<std::wstring>::const_iterator iter = bcpEntities.begin();
            iter != bcpEntities.end();
            ++iter)
    {
        appendBootClassPath(*iter);
    }
}

void J9StarterS60::appendOdcFile(const std::wstring& odcFile)
{
    JELOG2(EJavaRuntime);
    if (mOdcExtDirProperty.length() == 0)
    {
        mOdcExtDirProperty = ODC_PROPERTY;
    }
    else
    {
        mOdcExtDirProperty.append(L",");
    }
    mOdcExtDirProperty.append(odcFile);
}

#ifdef __WINSCW__
void J9StarterS60::getJ9VariantL()
{
    // In emulator solve which variant should be loaded:
    // J9.dll, j9_2.dll or j9_3.dll.
    JELOG2(EJavaRuntime);
    _LIT(KJ9_1, "j9");
    _LIT(KJ9_2, "j9_1");

    // Do this only once.
    if (mVariant == 0)
    {
        mVariant = new std::string(); // codescanner::nonleavenew
        int st = mLibLoader.Load(KJ9_1);
        if (st != KErrNone)
        {
            LOG1(EJavaRuntime, EInfo, "Could not load j9.dll (err=%d)."
                 " Trying j9_1.dll", st);
            //
            *mVariant = "_1";
            st = mLibLoader.Load(KJ9_2);
            if (st != KErrNone)
            {
                LOG1(EJavaRuntime, EInfo, "Could not load j9_1.dll (err=%d)."
                     " Trying j9_2.dll", st);
                // Variant 3 is the last one we got
                *mVariant = "_2";
            }
        }
    }
}
#endif // __WINSCW__




typedef int (*J9mainFunc)(int argc, const char *argv[], char *envp[],
                          TThreadId uiThreadId, TInt windowGroupId);

int J9StarterS60::startJvm()
{
    JELOG2(EJavaRuntime);
#ifdef __WINSCW__
#ifdef _DEBUG
    // The VM has WSD. In order to be able to run more than one VM instances
    // in the emulator we might need to load j9_2 or j9_3 dll.
    TRAP_IGNORE(getJ9VariantL());
    // Set the JNI checks for WINSCW DEBUG.
    std::wstring jnicheck(L"-Xrunjnichk");
    if (*mVariant == "_1")
    {
        jnicheck += L"_1";
    }
    else if (*mVariant == "_2")
    {
        jnicheck += L"_2";
    }
    jnicheck += L":all,nonfatal,noadvice,nowarn";
    mJvmArgs.push_back(jnicheck);
#endif // _DEBUG
#endif // __WINSCW__


    // Set mJvmArgs container to contain all the JVM args and set mAppAndArgs
    // to contain the main class and the arguments.
    completeArgumentContainers();

    // Give arguments to modifyJvmArguments for modification. Args
    // are modified if the default empty implementation has been overridden
    // by eclipsing the modifyJvmArguments dll.
    modifyJvmArguments(mIdentifier, mJvmArgs, mAppAndArgs);

    // Allocate space for the raw JVM args.
    int rawJvmArgumentCount = mJvmArgs.size() + mAppAndArgs.size() + 1;
    ScopedCharPointerArray rawJvmArgs(rawJvmArgumentCount);

    int ind = 0;

    // Just putting something to the first argument. Needs to dublicate so that
    // the deletion succeeds.
    rawJvmArgs.get()[ind++] = strdup("c:\\");  // codescanner::driveletters

    LOG(EJavaRuntime, EInfo, "VM args:");
    LOG1(EJavaRuntime, EInfo, " %s",rawJvmArgs.get()[ind-1]);

    // Adding the JVM args.
    for (JvmArgs_t::iterator jvmArgsIter = mJvmArgs.begin();
            jvmArgsIter!= mJvmArgs.end();
            ++jvmArgsIter)
    {
        rawJvmArgs.get()[ind++] = JavaCommonUtils::wstringToUtf8(*jvmArgsIter);
        LOG1(EJavaRuntime, EInfo, " %s",rawJvmArgs.get()[ind-1]);
    }

    // Adding the Application main class and its args.
    for (JvmArgs_t::iterator appAndArgsIter = mAppAndArgs.begin();
            appAndArgsIter!= mAppAndArgs.end();
            ++appAndArgsIter)
    {
        rawJvmArgs.get()[ind++] = JavaCommonUtils::wstringToUtf8(*appAndArgsIter);
        LOG1(EJavaRuntime, EInfo, " %s",rawJvmArgs.get()[ind-1]);
    }

    // Clear all the strings & lists.
    clear();
    return startJvm(rawJvmArgumentCount, rawJvmArgs.get());
}

int J9StarterS60::startJvm(int argc, char** argv)
{
    JELOG2(EJavaRuntime);
    std::string j9DllName = "j9";

    // The VM has WSD. In order to be able to run more than one VM instances
    // in the emulator we might need to load j9_2 or j9_3 dll.
#ifdef __WINSCW__
    TRAP_IGNORE(getJ9VariantL());
    j9DllName += *mVariant;
#endif // __WINSCW__

    // Loading the main dll of the J9 VM.
    DynamicLibLoader jvmLibLoader(j9DllName.c_str(), false);

    // Looking for the method j9main.
    J9mainFunc j9main = (J9mainFunc)(jvmLibLoader.getFunction("j9main"));

    JavaOsLayer::startUpTrace("Starting VM()", -1, -1);
    LOG(EJavaRuntime, EInfo, "j9main()");

    // Call j9main.
    int status = j9main(argc, (const char**)argv, 0, 0, 0);
    LOG1(EJavaRuntime, EInfo, "j9main() returned. st = %d", status);
    return status;
}

void J9StarterS60::enableThreadDumpL()
{
    RLibrary debugLib;
    CleanupClosePushL(debugLib);
    // If the javathreaddumper.dll exists in the system, then a
    // thread dumper will be started.
    _LIT(KThreadDumpper, "javathreaddumper");

    if (debugLib.Load(KThreadDumpper) == KErrNone)
    {
        mJvmArgs.push_back(L"-Xdump");
        mJvmArgs.push_back(L"-Xrunjavathreaddumper");
    }
    CleanupStack::PopAndDestroy(&debugLib);
}

void J9StarterS60::completeArgumentContainers()
{
    JELOG2(EJavaRuntime);

    // Define Dump arguments.
    if (mTheadDumpEnabled)
    {
        TRAP_IGNORE(enableThreadDumpL());
    }

    // Disable JIT, if requested.
    if (mJitDisabled)
    {
        mJvmArgs.push_back(L"-Xint");
    }

    // Add the classpath.
    if (mClassPath.length() > 0)
    {
        mJvmArgs.push_front(mClassPath);
        mJvmArgs.push_front(L"-classpath");
    }

    // Add the extension classpath.
    std::wstring ecp(L"-Dcom.ibm.j9.ext.dirs=");
    if (mExtensionPath.length() > 0)
    {
        ecp += mExtensionPath;
    }
    mJvmArgs.push_front(ecp);

    // Add the prepending boot classpath if set.
    if (mBootClassPathPrepend.length() > 0)
    {
        std::wstring bcpp(L"-Xbootclasspath/p:");
        bcpp += mBootClassPathPrepend;
        mJvmArgs.push_front(bcpp);
    }

    // Add the appending boot classpath if set.
    if (mBootClassPathAppend.length() > 0)
    {
        std::wstring bcpa(L"-Xbootclasspath/a:");
        bcpa += mBootClassPathAppend;
        mJvmArgs.push_front(bcpa);
    }

    // Add the defined odc files.
    if (mOdcExtDirProperty.length() > 0)
    {
        appendSystemProperty(mOdcExtDirProperty);
    }

    // Add the main class.
    mAppAndArgs.push_front(mMainClass);
}
