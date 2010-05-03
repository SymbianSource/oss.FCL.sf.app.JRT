/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This interface is meant for used when starting the JVM.
*
*/


#ifndef RUNTIMEEXCEPTION_H
#define RUNTIMEEXCEPTION_H

#include "exceptionbase.h"
#include "javaosheaders.h"

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

class OS_IMPORT RuntimeException: public java::util::ExceptionBase
{

public:
    OS_IMPORT RuntimeException(const std::string& message, const std::string& file,
                               const std::string& method, int line) throw();
    OS_IMPORT virtual ~RuntimeException() throw();

    /**
     * Convert exception to string.
     *
     * @return exception as string.
     */
    OS_IMPORT std::string toString() const throw();

    RuntimeException(const RuntimeException& x) : ExceptionBase(x) {}

};

} //end namespace runtime
} //end namespace java
#endif // RUNTIMEEXCEPTION_H

