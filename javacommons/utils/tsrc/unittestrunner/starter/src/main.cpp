/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main program for the Unit test process
*
*/

#include <memory>
#include <unistd.h>

#include "logger.h"
#include "exceptionbase.h"
#include "runtimeexception.h"
#include "javacommonutils.h"
#include "runtimestarterutils.h"
#include "jvmstarter.h"

using namespace java::runtime;
using namespace java::util;

int startJvm(int argc, char *argv[]);
const wchar_t* const UNIT_TEST_RUNNER_MAIN_CLASS = L"com.nokia.mj.impl.rt.test.UnitTestRunner";
void findMidlet();

int main(int argc, char *argv[])
{
    LOG(EJavaRuntime, EInfo, "UNIT TEST RUNNER main()");

    int result = -1;
    try
    {
        result = startJvm(argc, argv);
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaRuntime, "UNIT TEST RUNNER main() RuntimeException caught: %s ",
              e.toString().c_str());
    }

    catch (ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "UNIT TEST RUNNER main() ExceptionBase caught: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime, "UNIT TEST RUNNER main() Exception %s caught", e.what());
    }

    LOG1(EJavaRuntime, EInfo, "UNIT TEST RUNNER EXIT = %d", result);
    return result;
}

int startJvm(int argc, char *argv[])
{
    JELOG2(EJavaRuntime);

    // Create instance of RuntimeStarterUtils for thread supervisioning.
    std::auto_ptr<RuntimeStarterUtils> starterUtils(new RuntimeStarterUtils());
    starterUtils->startThreadSupervisor();
    
    JvmStarter::Configuration config = JvmStarter::CLDC;
    int argInd = 1;
    if (argc > 2)
    {
        if (strncmp(argv[1], "-conf", 5) == 0)
        {
            argInd = 2;
            if (strcmp(argv[1]+5, "=cdc") == 0)
            {
                config = JvmStarter::CDC;
            }
            else if (strcmp(argv[1]+5, "=foundation") == 0)
            {
                config = JvmStarter::FOUNDATION;
            }
        }
    }
    // Create starter for starting the JVM
    std::auto_ptr<JvmStarter>
    jvm(JvmStarter::getJvmStarterInstance(config,
                                          L"Unit_test_runner"));

    // starterUtils->enableDevelopmentFeatures(*jvm.get());
    jvm->enableThreadDumping();

    jvm->appendSystemProperty(L"-Dcom.nokia.rt.port=unittestrunner");

    jvm->setMainClass(UNIT_TEST_RUNNER_MAIN_CLASS);

    for (; argInd < argc; ++argInd)
    {
        jvm->appendApplicationArgument(
            JavaCommonUtils::utf8ToWstring(argv[argInd]));
    }

    // Start the JVM.
    return jvm->startJvm();
}
