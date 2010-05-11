/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides container for message.
*
*/

#include <string>
#include <algorithm>
#include <fstream>
#include <list>

#include "javacoreui.h"
#include "javacoreuiparams.h"
#include "runtimestarterutils.h"

#include "dynamiclibloader.h"
#include "javacommonutils.h"
#include "javaoslayer.h"
#include "logger.h"

#ifdef __SYMBIAN32__
#include "javasymbianoslayer.h"
#else // __SYMBIAN32__
#include <signal.h>

#endif // __SYMBIAN32__

#include "runtimeexception.h"

#include "javastarter.h"
#include "javastarterimpl.h"


using namespace java::runtime;
using namespace java::util;
using namespace java::ui;
using namespace java;

const wchar_t* const NATIVE_RUNTIME_MAIN_CLASS  = L"com.nokia.mj.impl.rt.midp.Main";
const wchar_t* const MAIN_RUNTIME_MAIN_CLASS    = L"com.nokia.mj.impl.rt.main.Main";

const wchar_t* const ARG_CONF                   = L"-conf=";
const wchar_t* const CONF_CDC                   = L"cdc";
const wchar_t* const CONF_FOUNDATION            = L"foun";
const wchar_t* const CONF_CLDC                  = L"cldc";

const wchar_t* const ARG_PROFILE                = L"-profile=";
const wchar_t* const PROFILE_MAIN               = L"main";
const wchar_t* const PROFILE_STANDALONE_MIDLET  = L"standalonemidlet";
const wchar_t* const ARG_STANDALONE             = L"-standalone";

const wchar_t* const ARG_ORIENTATION            = L"-Dcom.nokia.startup.arg.orientation=";
const wchar_t* const ORIENTATION_PORTRAIT       = L"portrait";
const wchar_t* const ORIENTATION_LANDSCAPE      = L"landscape";

const wchar_t* const ARG_START_SCREEN           = L"-Dcom.nokia.startup.arg.startscreen=";
const wchar_t* const START_SCREEN_DEFAULT       = L"default";
const wchar_t* const START_SCREEN_NO            = L"no";

const wchar_t* const ARG_JAR                    = L"-jar";
const wchar_t* const ARG_JAD                    = L"-jad";
const wchar_t* const ARG_CP                     = L"-cp";
const wchar_t* const ARG_CLASSPATH              = L"-classpath";
const wchar_t* const ARG_MAIN                   = L"-main";
const wchar_t* const ARG_UID                    = L"-uid";
const wchar_t* const ARG_ROOT_PATH              = L"-rootpath";
const wchar_t* const ARG_HANDLE                 = L"-handle";
const wchar_t* const ARG_END                    = L"-interalArgEnd";
const wchar_t* const ARG_EMPTY                  = L"";



OS_EXPORT int java::start(int argc, const char** argv)
{
    JELOG2(EJavaRuntime);
    LOG(EJavaRuntime, EInfo, "java::start(1)");
    int result = -1;
    try
    {
        int i = 0;
        if (argc > 0 && argv[0][0] != '-')
        {
            // Check if the first argument is the name of the exe. If so, skip it.
            std::string firstArg(argv[0]);
            std::transform(firstArg.begin(), firstArg.end(),
                           firstArg.begin(), tolower);
            size_t pos = firstArg.rfind(".exe");
            bool endsWithExe = (pos != std::string::npos) &&
                               (pos == (firstArg.length() - 4));
            if (endsWithExe)
            {
                // It did end with .exe so ignoring the first argument.
                i = 1;
            }

        }

        std::list<std::wstring> args;
        for (; i < argc; ++i)
        {
            args.push_back(JavaCommonUtils::utf8ToWstring(argv[i]));
        }

        std::auto_ptr<JavaStarterImpl> javaStarter(new JavaStarterImpl(args));
        result = javaStarter->start();
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaRuntime, "java::start RuntimeException catched: %s ",
              e.toString().c_str());
    }

    catch (ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "java::start ExceptionBase catched: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime, "java::start Exception %s catched", e.what());
    }

    LOG1(EJavaRuntime, EInfo, "java::start with status %d", result);
    return result;
}

OS_EXPORT int java::start(const char* fileName)
{
    JELOG2(EJavaRuntime);
    LOG(EJavaRuntime, EInfo, "java::start(2)");
    int result = -1;
    try
    {
        std::list<std::wstring> args;
        JavaStarterImpl::getArgsFromFile(fileName, args);
        std::auto_ptr<JavaStarterImpl> javaStarter(new JavaStarterImpl(args));
        result = javaStarter->start();
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaRuntime, "java::start(2) RuntimeException catched: %s ",
              e.toString().c_str());
    }

    catch (ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "java::start(2) ExceptionBase catched: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime, "java::start(2) Exception %s catched", e.what());
    }

    LOG1(EJavaRuntime, EInfo, "java::start(2) exited with status %d", result);

    return result;
}

JavaStarterImpl::JavaStarterImpl(const std::list<std::wstring>& args):
        mJvmStarter(0),
        mRuntimeStarterUtils(0),
        mOriginalArgs(args),
        mShudownOk(false),
        mIsMainApp(true),
        mConfiguration(JvmStarter::UNDEFINED)

{
    JELOG2(EJavaRuntime);
    mRuntimeStarterUtils = new RuntimeStarterUtils();
}

JavaStarterImpl::~JavaStarterImpl()
{
    JELOG2(EJavaRuntime);
    delete mJvmStarter;
    mJvmStarter = 0;

    delete mRuntimeStarterUtils;
    mRuntimeStarterUtils = 0;
}

int JavaStarterImpl::start()
{
    JELOG2(EJavaRuntime);

    // Start the thread supervisor for noticing crashes.
    mRuntimeStarterUtils = new RuntimeStarterUtils(); // codescanner::nonleavenew
    mRuntimeStarterUtils->startThreadSupervisor();

    // Parse args that starts with '-'.
    parseFlags();

    // Create JVM starter for selected runtime.
    createJvmStarter();

    // Solves the UID of the application and sets the root path.
    setUidAndRootPath();

    // Create the start screen and start it if needed.
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    CoreUi& coreUi = CoreUi::getUiInstance(coreUiLoader);
    if (mUiParams.getScreenMode() != NO_START_SCREEN)
    {
        LOG(EJavaRuntime, EInfo, "StartUI");
        coreUi.start(mAppUid, &mUiParams);
        LOG(EJavaRuntime, EInfo, "StartUI ok");
    }

    // Sets the -jar, -jad, -cp (or -classpath) args if were provided.
    // Also a main class is set if needed.
    handleJadJarCpArgs();

    // Adds a handle to this object in order to enable the callbacks
    // from runtime.
    addHandle();

    // Add the application arguments if provided.
    addApplicationArgs();

#ifndef RD_JAVA_UI_QT
    // In Java 2.x we are using legacy UI.
    mJvmStarter->appendSystemProperty(L"-Dcom.nokia.legacy.support=LegacySymbian");
#endif //RD_JAVA_HYBRID

    int result = mJvmStarter->startJvm();
    CoreUi::releaseUi(coreUiLoader);
    if (mMonitor.get())
    {
        mShudownOk = true;
        LOG(EJavaRuntime, EInfo, "Notifying.");
        mMonitor->notify();
    }

    LOG1(EJavaRuntime, EInfo, "Native exited with status %d", result);

    return result;

}

void JavaStarterImpl::parseFlags()
{
    JELOG2(EJavaRuntime);
    for (mOriginalArgsIter = mOriginalArgs.begin();
            (mOriginalArgsIter != mOriginalArgs.end() && mOriginalArgsIter->at(0) == '-');
            ++mOriginalArgsIter)
    {
        LOG1(EJavaRuntime, EInfo, "Handling flags %S",  mOriginalArgsIter->c_str());
        if (*mOriginalArgsIter == ARG_JAR)
        {
            storeNextArgument(mJarFile);
            LOG1(EJavaRuntime, EInfo, "Jar file is %S",  mJarFile.c_str());
        }
        else if (*mOriginalArgsIter == ARG_JAD)
        {
            storeNextArgument(mJadFile);
            LOG1(EJavaRuntime, EInfo, "Jad file is %S",  mJadFile.c_str());
        }
        else if (*mOriginalArgsIter == ARG_CP || *mOriginalArgsIter == ARG_CLASSPATH)
        {
            storeNextArgument(mClassPath);
            LOG1(EJavaRuntime, EInfo, "Classpath is %S",  mClassPath.c_str());
        }
        else if (mOriginalArgsIter->find(ARG_CONF) == 0)
        {
            setConfigration(getArgValue(*mOriginalArgsIter));
        }
        else if (mOriginalArgsIter->find(ARG_PROFILE) == 0)
        {
            setProfile(getArgValue(*mOriginalArgsIter));
        }
        else if (mOriginalArgsIter->find(ARG_ORIENTATION) == 0)
        {
            setOrientation(getArgValue(*mOriginalArgsIter));
        }
        else if (mOriginalArgsIter->find(ARG_START_SCREEN) == 0)
        {
            setStartScreen(getArgValue(*mOriginalArgsIter));
        }
        else
        {
            mFlagArgs.push_back(*mOriginalArgsIter);
        }
    }
}

void JavaStarterImpl::setConfigration(const std::wstring& conf)
{
    JELOG2(EJavaRuntime);

    if (conf == CONF_CLDC)
    {
        LOG(EJavaRuntime, EInfo, "Setting conf to CLDC");
        mConfiguration = JvmStarter::CLDC;
    }
    else if (conf == CONF_CDC)
    {
        LOG(EJavaRuntime, EInfo, "Setting conf to CDC");
        mConfiguration = JvmStarter::CDC;
    }
    else if (conf == CONF_FOUNDATION)
    {
        LOG(EJavaRuntime, EInfo, "Setting conf to FOUNDATION");
        mConfiguration = JvmStarter::FOUNDATION;
    }
    else
    {
        ELOG1(EJavaRuntime, "Unknown conf %S", conf.c_str());
    }
}

void JavaStarterImpl::setProfile(const std::wstring& profile)
{
    JELOG2(EJavaRuntime);

    if (profile == PROFILE_STANDALONE_MIDLET)
    {
        LOG(EJavaRuntime, EInfo, "Setting profile to be standalone MIDlet");
        mIsMainApp = false;
    }
    else if (profile == PROFILE_MAIN)
    {
        LOG(EJavaRuntime, EInfo, "Setting profile to be main");
        mIsMainApp = true;
    }
    else
    {
        ELOG1(EJavaRuntime, "Unknown profile %S", profile.c_str());
    }
}

void JavaStarterImpl::setOrientation(const std::wstring& orientation)
{
    JELOG2(EJavaRuntime);

    if (orientation == ORIENTATION_PORTRAIT)
    {
        LOG(EJavaRuntime, EInfo, "Setting orientation to portrait");
        mUiParams.setOrientation(PORTRAIT);
    }
    else if (orientation == ORIENTATION_LANDSCAPE)
    {
        LOG(EJavaRuntime, EInfo, "Setting orientation to landscape");
        mUiParams.setOrientation(LANDSCAPE);
    }
    else
    {
        ELOG1(EJavaRuntime, "Unknown orientation %S", orientation.c_str());
    }
}

void JavaStarterImpl::setStartScreen(const std::wstring& startscreen)
{
    JELOG2(EJavaRuntime);

    if (startscreen == START_SCREEN_DEFAULT)
    {
        LOG(EJavaRuntime, EInfo, "Setting default start screen");
        mUiParams.setScreenMode(DEFAULT_START_SCREEN);
    }
    else if (startscreen == START_SCREEN_NO)
    {
        LOG(EJavaRuntime, EInfo, "Setting default start screen");
        mUiParams.setScreenMode(NO_START_SCREEN);
    }
    else
    {
        mUiParams.setScreenMode(USER_DEFINED_SCREEN);
        mUiParams.setImagePath(startscreen);
        LOG1(EJavaRuntime, EInfo, "Setting start screen from file %S", startscreen.c_str());
    }
}

void JavaStarterImpl::createJvmStarter()
{
    JELOG2(EJavaRuntime);
    if (mIsMainApp)
    {
        // We are starting main app.
        if (mConfiguration == JvmStarter::UNDEFINED)
        {
            // If the user has not explicitely set the conf, CDC is default.
            mConfiguration = JvmStarter::CDC;
        }
        mJvmStarter = JvmStarter::getJvmStarterInstance(mConfiguration,
                      PROFILE_MAIN);
        mJvmStarter->appendSystemProperty(L"-Dcom.nokia.rt.port=main");
        mJvmStarter->setMainClass(MAIN_RUNTIME_MAIN_CLASS);
    }
    else
    {
        // We are starting standalone MIDlet.
        if (mConfiguration == JvmStarter::UNDEFINED)
        {
            // If the user has not explicitely set the conf, CLDC is default.
            mConfiguration = JvmStarter::CLDC;
        }
        mJvmStarter = JvmStarter::getJvmStarterInstance(mConfiguration,
                      PROFILE_STANDALONE_MIDLET);
        mJvmStarter->appendSystemProperty(L"-Dcom.nokia.rt.port=midp");
        mJvmStarter->setMainClass(NATIVE_RUNTIME_MAIN_CLASS);
        mJvmStarter->appendApplicationArgument(ARG_STANDALONE);
        mJvmStarter->appendApplicationArgument(ARG_EMPTY);
    }
}

void JavaStarterImpl::handleJadJarCpArgs()
{
    JELOG2(EJavaRuntime);
    if (mJarFile.length() > 0)
    {
        // The -jar argument was provided. Appending it to classpath and
        // passing that as an argument to the runtime.
        mJvmStarter->appendClassPath(mJarFile);
        mJvmStarter->appendApplicationArgument(ARG_JAR);
        mJvmStarter->appendApplicationArgument(mJarFile);
    }
    else
    {
        // The -jar argument was not provided. We assume that the next argument
        // is the main class of the application in case of main app.
        // passing that as an argument to the runtime.
        if (mIsMainApp && mOriginalArgsIter != mOriginalArgs.end())
        {
            mJvmStarter->appendApplicationArgument(ARG_MAIN);
            mJvmStarter->appendApplicationArgument(*mOriginalArgsIter);
            ++mOriginalArgsIter;
        }
    }

    if (mJadFile.length() > 0)
    {
        mJvmStarter->appendApplicationArgument(ARG_JAD);
        mJvmStarter->appendApplicationArgument(mJadFile);
    }

    if (mClassPath.length() > 0)
    {
        mJvmStarter->appendClassPath(mClassPath);
    }
}


void JavaStarterImpl::setUidAndRootPath()
{
    JELOG2(EJavaRuntime);
#ifdef __SYMBIAN32__
    TUidToUid(RProcess().Type()[2], mAppUid);
    LOG1(EJavaRuntime, EInfo, "PROCESS UID %S",  mAppUid.toString().c_str());
#else // __SYMBIAN32__
//    mAppUid = Uid(L"java_standalone");

#endif // __SYMBIAN32__

    mJvmStarter->appendApplicationArgument(ARG_UID);
    mJvmStarter->appendApplicationArgument(mAppUid.toString());

    std::wstring rooPath;
#ifdef __SYMBIAN32__
    Uid uid;
    TUidToUid(RProcess().SecureId(), uid);
    rooPath += L"c:\\private\\";
    rooPath += uid.toString().substr(1,8);
    rooPath += L"\\";
#else // __SYMBIAN32__
    rooPath += L"./";
#endif // __SYMBIAN32__
    mJvmStarter->appendApplicationArgument(ARG_ROOT_PATH);
    mJvmStarter->appendApplicationArgument(rooPath);
}

void JavaStarterImpl::addHandle()
{
    JELOG2(EJavaRuntime);
    // Provide access to this object by Java peer via delivering a pointer
    // to this object.
    mJvmStarter->appendApplicationArgument(ARG_HANDLE);
    MidpStarterInternalSupport* internalSupport = this;
    int handle = reinterpret_cast<int>(internalSupport);
    mJvmStarter->appendApplicationArgument(JavaCommonUtils::intToWstring(handle));
}

void JavaStarterImpl::addApplicationArgs()
{
    JELOG2(EJavaRuntime);
    // Adding end mark of the runtime arguments.
    mJvmStarter->appendApplicationArgument(ARG_END);

    // What is left in the argument list is considered to be application arguments.

    for (; mOriginalArgsIter != mOriginalArgs.end(); ++mOriginalArgsIter)
    {
        LOG1(EJavaRuntime, EInfo, "Adding APP args %S", mOriginalArgsIter->c_str());
        mJvmStarter->appendApplicationArgument(*mOriginalArgsIter);
    }
}

void JavaStarterImpl::storeNextArgument(std::wstring& arg)
{
    ++mOriginalArgsIter;
    if (mOriginalArgsIter != mOriginalArgs.end())
    {
        arg = *mOriginalArgsIter;
    }
}

std::wstring JavaStarterImpl::getArgValue(const std::wstring& arg)
{
    JELOG2(EJavaRuntime);
    return arg.substr(arg.find(L"=") + 1);
}


void JavaStarterImpl::getArgsFromFile(const char* file, std::list<std::wstring>& args)
{
    std::ifstream argsFile;
    try
    {
        argsFile.open(file, std::ifstream::in);

        std::string line;

        while (std::getline(argsFile, line))
        {
            size_t endln = line.find_last_not_of("\r\n");
            if (endln != std::string::npos)
            {
                line.erase(endln+1);
            }
            args.push_back(JavaCommonUtils::utf8ToWstring(line.c_str()));
        }
    }
    catch (std::exception& e)
    {
        LOG2(EJavaRuntime, EInfo, "Not able to read from file %s. Error %s",
             file, e.what());
    }
    argsFile.close();
}

void* JavaStarterImpl::ensureExit(void* ptr)
{
    JELOG2(EUtils);
#ifdef __SYMBIAN32__
    RThread().SetPriority(EPriorityMore);
#endif // __SYMBIAN32__
    JavaStarterImpl* starter = reinterpret_cast<JavaStarterImpl*>(ptr);
    LOG(EJavaRuntime, EInfo, "Starting to wait for the shutdown.");
    starter->mMonitor->wait(1200); // 1,2 seconds
    LOG(EJavaRuntime, EInfo, "woke up from monitor.");
    if (!starter->mShudownOk)
    {
        LOG(EJavaRuntime, EInfo, "Killing process.");
#ifdef __SYMBIAN32__
        RProcess().Kill(0);
#else
        kill(getpid(), SIGTERM);
#endif // __SYMBIAN32__
    }
    LOG(EJavaRuntime, EInfo, "Not killing process.");
    return 0;
}

void JavaStarterImpl::closeRuntimeInd()
{
    JELOG2(EJavaRuntime);
    LOG(EJavaRuntime, EInfo, "Starter got close indication from JVM");
    pthread_t tid;
    mMonitor.reset(Monitor::createMonitor());
    pthread_create(&tid, 0, ensureExit, this);
}

void JavaStarterImpl::setUids(const java::util::Uid& /*uid*/,
                              const java::util::Uid& /*suiteUid*/)
{
    JELOG2(EJavaRuntime);
    // Not applicable in this case.
}
