/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main program for the JavaInstaller process
*
*/


#include <unistd.h>
#include <string.h>
// next three includes are for semaphores
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef __SYMBIAN32__
#include "javauids.h"
#endif
#include "comms.h"
#include "commsclientendpoint.h"
#include "exceptionbase.h"
#include "javacommonutils.h"
#include "javaoslayer.h"
#include "jvmstarter.h"
#include "logger.h"
#include "runtimeexception.h"
#include "runtimestarterutils.h"

using namespace java::comms;
using namespace java::runtime;
using namespace java::util;

const wchar_t* const INSTALLER_MAIN_CLASS =
    L"com.nokia.mj.impl.installer.Installer";

int startJvm(int argc, char *argv[]);

#ifdef __WINS__
#include <e32ldr.h>
#endif //__WINS__

// random small int value
#define SEM_KEY 1823 // codescanner::magicnumbers
// same as Symbian KErrAlreadyExists
#define INSTALLER_ALREADY_RUNNING -11 // codescanner::magicnumbers
// semaphore operation error, same as Symbian KErrLocked
#define SEMAPHORE_ERROR -22 // codescanner::magicnumbers
// Ubuntu semaphore headers do not define these contants
#ifndef SEM_R
#define SEM_R S_IRUSR
#endif
#ifndef SEM_A
#define SEM_A S_IWUSR
#endif

#ifndef __SYMBIAN32__
extern "C"
#endif //__SYMBIAN32__

int dllMain(int argc, char *argv[])
{
    JELOG(EJavaRuntime, "INSTALLER main()");
    JavaOsLayer::startUpTrace("INSTALLER main() start", -1, -1);

#ifdef __SYMBIAN32__

    // Only processes with TrustedUI capability are allowed to start
    // Java Installer in S60 device (e.g. TCK runner, preinstaller,
    // appconverter, javacleaner, AppInstUI,javainstaller,
    // javapostupdater and SDK Debug Agent)

    TSecureId creatorSecId(User::CreatorSecureId());
    if (!User::CreatorHasCapability(ECapabilityTrustedUI))
    {
        ELOG1(EJavaRuntime,
              "INSTALLER main: Process %x is not allowed to launch Java Installer, since it is missing TrustedUI capability.",
              creatorSecId.iId);
        return KErrPermissionDenied;
    }

#endif

#ifdef __SYMBIAN32__
    _LIT(KJavaInstallerProcessName, "JavaInstaller");
    _LIT(KJavaInstallerThreadName, "JavaInstallerMainThread");
    // Rename installer process and main thread.
    TInt err = User::RenameProcess(KJavaInstallerProcessName);
    if (KErrNone != err)
    {
        ELOG1(EJavaRuntime,
              "INSTALLER main: cannot change process name to JavaInstaller, err %d", err);
    }
    err = User::RenameThread(KJavaInstallerThreadName);
    if (KErrNone != err)
    {
        ELOG1(EJavaRuntime,
              "INSTALLER main: cannot change main thread name to "
              "JavaInstallerMainThread, err %d", err);
    }
#endif //__SYMBIAN32__

    // Allow running only one instance of this process at same time.

    // Because of S60 semop implementation (semop waits even if
    // IPC_NOWAIT has been specified) -> use semaphore access control
    // to limit number of instances to one.

#ifdef __SYMBIAN32__
    int perm = 0x1FF; // all permissions to every process
#else
    // Create one System V / XSI semaphore accessible only by this process
    int perm = (SEM_R | SEM_A);
#endif //__SYMBIAN32__
    int semid = semget(SEM_KEY, 1, IPC_CREAT | perm);

    // Increase process instance count by one.
    // This increase is automatically undone when this process exits.
    sembuf st = {0, 1, SEM_UNDO | IPC_NOWAIT};
    int ret = semop(semid, &st, 1);
    if (-1 == ret)
    {
        ELOG(EJavaRuntime, "INSTALLER main: semop failed");
        JavaOsLayer::startUpTrace("INSTALLER main() end", -1, -1);
        return SEMAPHORE_ERROR;
    }

    // Get the current instance count
    ret = semctl(semid, 0, GETVAL);
    if (-1 == ret)
    {
        ELOG(EJavaRuntime, "INSTALLER main: semctl failed");
        JavaOsLayer::startUpTrace("INSTALLER main() end", -1, -1);
        return SEMAPHORE_ERROR;
    }
    else if (ret > 1)
    {
        WLOG(EJavaRuntime, "Installer already running. Exiting.");
        JavaOsLayer::startUpTrace("INSTALLER main() end", -1, -1);
        return INSTALLER_ALREADY_RUNNING;
    }

    int result = -1;
    try
    {
        // Start the VM.
        result = startJvm(argc, argv);
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaRuntime, "INSTALLER main() RuntimeException caught: %s ",
              e.toString().c_str());
    }
    catch (ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "INSTALLER main() ExceptionBase caught: %s ",
              e.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime, "INSTALLER main() Exception %s caught", e.what());
    }

    // Try to remove the semaphore, ignore possible error
    ret = semctl(semid, 0, IPC_RMID);
    if (-1 == ret)
    {
        ELOG1(EJavaRuntime, "INSTALLER main() Remove semaphore failed, err %d",
              errno);
    }

    LOG1(EJavaRuntime, EInfo, "INSTALLER main() exit = %d", result);
    JavaOsLayer::startUpTrace("INSTALLER main() end", -1, -1);
    return result;
}

#ifdef __SYMBIAN32__
/**
 *  Start Java Captain process
 */
TInt startJavaCaptain()
{
    // Start 'systemams.exe' in S60 5.0 (it will start javacaptain) and
    // 'javacaptain.exe' in 9.2 and later

#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    _LIT(KJavaCaptainExe, "systemams.exe");
#else
    _LIT(KJavaCaptainExe, "javacaptain.exe");
#endif
    _LIT(KJavaCaptainArg, "");
    RProcess proc;
    int err = proc.Create(KJavaCaptainExe, KJavaCaptainArg);
    if (err == KErrNone)
    {
        proc.Resume();
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        LOG(EJavaRuntime, EInfo,
            "InstallerStarter: startJavaCaptain systemams.exe was started ok");
#else
        LOG(EJavaRuntime, EInfo,
            "InstallerStarter: startJavaCaptain javacaptain.exe was started ok");
#endif

        // Wait 3 seconds so that Java Captain has time to start
        User::After(3000000); // codescanner::userafter
    }
    else
    {
#ifdef RD_JAVA_S60_RELEASE_5_0_IAD
        ELOG1(EJavaRuntime,
              "InstallerStarter: startJavaCaptain start systemams.exe failed: %d", err);
#else
        ELOG1(EJavaRuntime,
              "InstallerStarter: startJavaCaptain start javacaptain.exe failed: %d", err);
#endif
    }

    proc.Close();
    return err;
}
#endif

int startJvm(int argc, char *argv[])
{
    // Create instance of RuntimeStarterUtils for thread supervisioning.
    std::auto_ptr<RuntimeStarterUtils> starterUtils(new RuntimeStarterUtils()); // codescanner::nonleavenew
    starterUtils->startThreadSupervisor();

    // Create Comms endpoint for JavaCaptain.
    std::auto_ptr<CommsClientEndpoint> comms(
        new CommsClientEndpoint(L"InstallerJavaCaptain"));
    int result = comms->connect(IPC_ADDRESS_JAVA_CAPTAIN_C);

#ifdef __SYMBIAN32__
    if (KErrNotFound == result)
    {
        ELOG(EJavaRuntime,
             "InstallerStarter: Java Captain was not running. Trying to restart it.");

        // Java Captain is not running, try to start it
        result = startJavaCaptain();
        if (KErrNone == result)
        {
            result = comms->connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
        }
    }
#endif

    if (result != 0)
    {
        ELOG1(EJavaRuntime,
              "InstallerStarter: Connection to JavaCaptain failed: Reason = %d",
              result);
    }

    // Another Comms Endpoint for TCK Runner or preinstaller
    std::auto_ptr<CommsClientEndpoint> commsPoll;

    // If installer is started in 'poll' mode, connect also to TCK Runner,
    // preinstaller or javaappconverter. Command line is
    // 'javainstaller poll -address=(tck|preinstall|convert) [options]'
    const int minArgCount = 3;
    if (argc >= minArgCount)
    {
        if (!strcmp(argv[1], "poll")) // codescanner::accessArrayElementWithoutCheck2
        {
            int connected = 0;
            // next argument must be -address=(tck|preinstaller|convert)
            char* val = strpbrk(argv[2], "="); // codescanner::accessArrayElementWithoutCheck2
            if (val != 0)
            {
                val++;
                int commsAddress = 0;
                if (!strcmp(val, "tck"))
                {
                    commsAddress = IPC_ADDRESS_JAVA_TCK_RUNNER_C;
                }
                else if (!strcmp(val, "preinstall"))
                {
                    commsAddress = IPC_ADDRESS_JAVA_PREINSTALLER_C;
                }
                else if (!strcmp(val, "convert"))
                {
                    commsAddress = IPC_ADDRESS_JAVA_APPCONVERTER_C;
                }
                else if (!strcmp(val, "debugapi"))
                {
                    commsAddress = IPC_ADDRESS_JAVA_DEBUGAPI_C;
                }
                else
                {
                    ELOG1(EJavaRuntime, "InstallerStarter: unknown comms "
                          "poll address %s", val);
                }

                if (0 != commsAddress)
                {
                    commsPoll.reset(new CommsClientEndpoint(L"InstallerCommsPoll")); // codescanner::nonleavenew
                    result = commsPoll->connect(commsAddress);
                    if (0 != result)
                    {
                        ELOG3(EJavaRuntime,
                              "InstallerStarter: Connection to %s (address %d) failed: Reason = %d",
                              val, commsAddress, result);
                    }
                    else
                    {
                        connected = 1;
                    }
                }
            }
            else
            {
                ELOG1(EJavaRuntime, "InstallerStarter: No valid -address argument. Arg is %s",
                      argv[2]); // codescanner::accessArrayElementWithoutCheck2
            }

            // Installer cannot be started in 'poll' mode unless connection is ok
            if (!connected)
            {
                ELOG(EJavaRuntime,
                     "InstallerStarter: Installer cannot be started in poll mode because connection to initiator fails.");
                std::string errorStr("Installer cannot start in poll mode");
                throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
            }
        }
    }

    // Create starter for starting the JVM
    std::auto_ptr<JvmStarter>
    jvm(JvmStarter::getJvmStarterInstance(JvmStarter::CDC,
                                          L"Installer"));

    // Set the debugging features available provided by the captain.
    starterUtils->enableDevelopmentFeatures(*jvm.get());

    // Set the old heap size to 512K
    const int oldHeapSize = 512;
    jvm->overrideOldHeapSize(oldHeapSize);

    jvm->enableThreadDumping();

    jvm->setMainClass(INSTALLER_MAIN_CLASS);

    jvm->appendSystemProperty(L"-Dcom.nokia.rt.port=installer");

    for (int i = 1; i < argc; ++i)
    {
        jvm->appendApplicationArgument(
            JavaCommonUtils::utf8ToWstring(argv[i])); // codescanner::accessArrayElementWithoutCheck2
    }

#ifdef RD_JAVA_INSTALLERUI_ENABLED
#ifdef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    jvm->appendSystemProperty(
        L"-Dcom.nokia.mj.impl.installer.ui="
        L"com.nokia.mj.impl.installer.ui.eswt2.InstallerUiEswt");
    // Replace RuntimeUi with installer specific implementation.
    jvm->appendSystemProperty(
        L"-Dcom.nokia.mj.impl.rt.ui="
        L"com.nokia.mj.impl.installer.ui.eswt2.InstallerRuntimeUi");
#ifdef __SYMBIAN32__
#ifndef __WINS__
    jvm->appendSystemProperty(L"-Dcom.nokia.mj.impl.installer.ui.disableui=1");
#endif //__WINS__
#endif // __SYMBIAN32__
#else // RD_JAVA_S60_RELEASE_10_1_ONWARDS
    jvm->appendSystemProperty(
        L"-Dcom.nokia.mj.impl.installer.ui="
        L"com.nokia.mj.impl.installer.ui.eswt.InstallerUiEswt");
#endif // RD_JAVA_S60_RELEASE_10_1_ONWARDS
#endif // RD_JAVA_INSTALLERUI_ENABLED

    std::wstring extendedBootClassPath;
    // This call is platform dependent.
    starterUtils->getExtBootClassPath(extendedBootClassPath);
    if (extendedBootClassPath.length() > 0)
    {
        // Provide the bootclasspath also as a system propery
        // to be used for security extensions.
        std::wstring addOnList(L"-Dcom.nokia.mj.addon.list=");
        addOnList += extendedBootClassPath;
        jvm->appendSystemProperty(addOnList);
    }

    // Start the JVM.
    return jvm->startJvm();
}

#ifdef __SYMBIAN32__
#include "javasymbianoslayer.h"

EXPORT_C FuncPtr findDllMethod(const char* funcName)
{
    FuncPtr ptr = 0;
    if (strcmp(funcName, "dllMain") == 0)
    {
        ptr = (FuncPtr)dllMain;
    }
    return ptr;
}
#endif //__SYMBIAN32__
