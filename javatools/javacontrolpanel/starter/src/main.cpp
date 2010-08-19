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
* Description:  Main program for the Java Manager process
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
const wchar_t* const JavaControlPanel_MAIN_CLASS = L"com.nokia.mj.impl.javacontrolpanel.JavaControlPanel";

int main(int argc, char *argv[])
{
    LOG(EUtils, EInfo, "JavaControlPanel main()");
    int result = -1;
    try
    {
        result = startJvm(argc, argv);
    }
    catch (RuntimeException& e)
    {
        ELOG1(EUtils, "JavaControlPanel main() RuntimeException caught: %s ",
              e.toString().c_str());
    }

    catch (ExceptionBase& e)
    {
        ELOG1(EUtils, "JavaControlPanel main() ExceptionBase caught: %s ",
              e.toString().c_str());
    }

    catch (std::exception& e)
    {
        ELOG1(EUtils, "JavaControlPanel main() Exception %s caught", e.what());
    }

    LOG1(EUtils, EInfo, "JavaControlPanel EXIT = %d", result);
    return result;
}

int startJvm(int argc, char *argv[])
{
    JELOG2(EUtils);

    // Create instance of RuntimeStarterUtils for thread supervisioning.
    std::auto_ptr<RuntimeStarterUtils> starterUtils(new RuntimeStarterUtils());
    starterUtils->startThreadSupervisor();

    // Create starter for starting the JVM
    std::auto_ptr<JvmStarter>
    jvm(JvmStarter::getJvmStarterInstance(JvmStarter::CLDC,
                                          L"JavaControlPanel"));

    // Set the debugging features available provided by the captain.
    // starterUtils->enableDevelopmentFeatures(*jvm.get());
    // jvm->enableThreadDumping();

    jvm->appendSystemProperty(L"-Dcom.nokia.rt.port=javacontrolpanel");
    jvm->setMainClass(JavaControlPanel_MAIN_CLASS);

    for (int i = 1; i < argc; ++i)
    {
        jvm->appendApplicationArgument(
            JavaCommonUtils::utf8ToWstring(argv[i]));
    }

    // Start the JVM.
    return jvm->startJvm();
}
