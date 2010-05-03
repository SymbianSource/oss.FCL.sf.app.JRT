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
* Description:  Main program for the TCK Runner process
*
*/

#include <memory>
#include <unistd.h>
/*
#ifdef __WINS__
#include <e32ldr.h>
#endif //__WINS__
*/
#include "logger.h"
#include "exceptionbase.h"
#include "runtimeexception.h"
#include "javacommonutils.h"
#include "runtimestarterutils.h"
#include "jvmstarter.h"

using namespace java::runtime;
using namespace java::util;

int startJvm(int argc, char *argv[]);
const wchar_t* const TCK_RUNNER_MAIN_CLASS = L"com.nokia.mj.impl.tckrunner.TckRunnerMain";

int main(int argc, char *argv[])
{
    LOG(ETckRunner, EInfo, "TCK RUNNER main()");
    /*
    #ifdef __WINS__
        RLoader loader;
        TInt status = loader.Connect();
        if (status == KErrNone)
        {
            loader.CancelLazyDllUnload();
            loader.Close();
        }
    #endif //__WINS__
    */
    int result = -1;
    try
    {
        result = startJvm(argc, argv);
    }
    catch (RuntimeException& e)
    {
        ELOG1(ETckRunner, "TCK RUNNER main() RuntimeException caught: %s ",
              e.toString().c_str());
    }

    catch (ExceptionBase& e)
    {
        ELOG1(ETckRunner, "TCK RUNNER main() ExceptionBase caught: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {
        ELOG1(ETckRunner, "TCK RUNNER main() Exception %s caught", e.what());
    }

    LOG1(ETckRunner, EInfo, "TCK RUNNER EXIT = %d", result);
    return result;
}

int startJvm(int argc, char *argv[])
{
    JELOG2(ETckRunner);

    // Create instance of RuntimeStarterUtils for thread supervisioning.
    std::auto_ptr<RuntimeStarterUtils> starterUtils(new RuntimeStarterUtils());
    starterUtils->startThreadSupervisor();

    // Create starter for starting the JVM
    std::auto_ptr<JvmStarter>
    jvm(JvmStarter::getJvmStarterInstance(JvmStarter::CLDC,
                                          L"TCK_runner"));

    // Set the debugging features available provided by the captain.
    // starterUtils->enableDevelopmentFeatures(*jvm.get());
    // jvm->enableThreadDumping();

    jvm->appendSystemProperty(L"-Dcom.nokia.rt.port=tckrunner");

    jvm->setMainClass(TCK_RUNNER_MAIN_CLASS);

    for (int i = 1; i < argc; ++i)
    {
        jvm->appendApplicationArgument(
            JavaCommonUtils::utf8ToWstring(argv[i]));
    }

    // Start the JVM.
    return jvm->startJvm();
}
