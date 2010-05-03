/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "runtimeexception.h"

using namespace java::runtime;

OS_EXPORT RuntimeException::RuntimeException(const std::string& message,
        const std::string& file,
        const std::string& method,
        int line) throw() :
        ExceptionBase(message, file,
                      method, line)
{
}

OS_EXPORT RuntimeException::~RuntimeException() throw()
{
}

OS_EXPORT std::string RuntimeException::toString() const throw()
{
    return ExceptionBase::toString();
}
