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
* Description:  A default empty implementation for JvmArgs modifier.
*
*/


#include "jvmargsmodifier.h"

#include "logger.h"

OS_EXPORT
void java::runtime::modifyJvmArguments(const std::wstring& /*identifier*/,
                                       std::list<std::wstring>& /*jvmArgs*/,
                                       std::list<std::wstring>& /*applicationAndArgs*/)
{
    JELOG2(EJavaRuntime);
    // This is intentionally an empty implementation.

    /*
        std::list<std::wstring>::iterator jvmArgsIterator;

        LOG(EJavaRuntime, EInfo, "JVM Args");
        for(jvmArgsIterator=jvmArgs.begin(); jvmArgsIterator!= jvmArgs.end();
            ++jvmArgsIterator)
        {
            LOG1(EJavaRuntime, EInfo, " '%S'", jvmArgsIterator->c_str());
        }

        std::list<std::wstring>::iterator applicationAndArgsIterator;
        LOG(EJavaRuntime, EInfo, "JVM Args");
        for(applicationAndArgsIterator=applicationAndArgs.begin();
            applicationAndArgsIterator!= applicationAndArgs.end();
            ++applicationAndArgsIterator)
        {
            LOG1(EJavaRuntime, EInfo, " '%S'", applicationAndArgsIterator->c_str());
        }
    */
}

