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
* Description:  This class is meant for starting the JVM.
*
*/


#include <string>

#include "jvmstarterimpl.h"

#include "logger.h"
#include "exception"
#include "runtimeexception.h"
#include "javaoslayer.h"

using namespace java::runtime;
using namespace java::util;

// Platform dependents
extern const wchar_t CLASS_PATH_SEPARATOR;
extern const char PATH_SEPARATOR_FROM;
extern const char PATH_SEPARATOR_TO;

JvmStarterImpl::JvmStarterImpl(): mJitDisabled(false),
        mTheadDumpEnabled(false)
{
    JELOG2(EJavaRuntime);

    DriveId driveId = JavaOsLayer::getMidpDrive();

    // Define JAVA_BIN_ROOT system property
    std::wstring javaBinRoot(L"-DJAVA_BIN_ROOT=");
    std::string binRoot(driveId.getDriveLetter());
    JavaOsLayer::getBinRoot(binRoot, true);
    javaBinRoot.append(binRoot.begin(), binRoot.end());
    mJvmArgs.push_front(javaBinRoot);

    // Define JAVA_RES_ROOT system property
    std::wstring javaResRoot(L"-DJAVA_RES_ROOT=");
    std::string resourceRoot(driveId.getDriveLetter());
    JavaOsLayer::getResRoot(resourceRoot, true);
    javaResRoot.append(resourceRoot.begin(), resourceRoot.end());
    mJvmArgs.push_front(javaResRoot);
}

JvmStarterImpl::~JvmStarterImpl()
{
    JELOG2(EJavaRuntime);
}

void JvmStarterImpl::appendClassPath(const std::wstring& file)
{
    JELOG2(EJavaRuntime);
    appendAndConvertClassPath(file, mClassPath);
}

void JvmStarterImpl::appendBootClassPath(const std::wstring& file)
{
    JELOG2(EJavaRuntime);
    appendAndConvertClassPath(file, mBootClassPathAppend);
}

void JvmStarterImpl::prependBootClassPath(const std::wstring& file)
{
    JELOG2(EJavaRuntime);
    appendAndConvertClassPath(file, mBootClassPathPrepend);
}

void JvmStarterImpl::appendExtensionPath(const std::wstring& extensionPath)
{
    JELOG2(EJavaRuntime);
    appendAndConvertClassPath(extensionPath, mExtensionPath);
}

void JvmStarterImpl::setMainClass(const std::wstring& mainClass)
{
    JELOG2(EJavaRuntime);
    mMainClass = mainClass;
}

void JvmStarterImpl::appendApplicationArgument(const std::wstring& argument)
{
    JELOG2(EJavaRuntime);
    mAppAndArgs.push_back(argument);
}

void JvmStarterImpl::appendSystemProperty(const std::wstring& systemproperty)
{
    JELOG2(EJavaRuntime);
    mJvmArgs.push_back(systemproperty);
}

void JvmStarterImpl::appendRawJvmArgument(const std::wstring& argument)
{
    JELOG2(EJavaRuntime);
    mJvmArgs.push_back(argument);
}

void JvmStarterImpl::disableJit()
{
    JELOG2(EJavaRuntime);
    mJitDisabled = true;
}

void JvmStarterImpl::enableThreadDumping()
{
    JELOG2(EJavaRuntime);
    mTheadDumpEnabled = true;
}

void JvmStarterImpl::appendAndConvertClassPath(const std::wstring& source,
        std::wstring& destination)
{
    JELOG2(EJavaRuntime);
    if (destination.length() > 0)
    {
        // Appending
        destination += CLASS_PATH_SEPARATOR;
    }
    destination += source;
    std::replace(destination.begin(), destination.end(),
                 PATH_SEPARATOR_FROM, PATH_SEPARATOR_TO);
}

void JvmStarterImpl::clear()
{
    JELOG2(EJavaRuntime);
    mIdentifier.clear();
    mClassPath.clear();
    mBootClassPathAppend.clear();
    mBootClassPathPrepend.clear();
    mExtensionPath.clear();
    mMainClass.clear();
    mJvmArgs.clear();
    mAppAndArgs.clear();
}

