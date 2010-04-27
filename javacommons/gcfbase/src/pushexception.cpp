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
* Description:
*
*/

#include "pushexception.h"

using namespace java::push;

/**
 *
 */
OS_EXPORT PushException::PushException(int aStatus,const std::string& aMessage,
                                       const std::string& aFile,
                                       const std::string& aMethod, int aLine)throw()
        : ExceptionBase(aStatus,aMessage,aFile,aMethod,aLine)
{
}

/**
 *
 */
OS_EXPORT PushException::PushException(int aStatus,const std::wstring& aMessage,
                                       const std::string& aFile,
                                       const std::string& aMethod, int aLine)throw()
        : ExceptionBase(aStatus,aMessage,aFile,aMethod,aLine)
{
}

/**
 *
 */
OS_EXPORT PushException::~PushException() throw()
{
}

/**
 *
 */
OS_EXPORT PushException::PushException(const PushException& x)
        : ExceptionBase(x)
{
}



