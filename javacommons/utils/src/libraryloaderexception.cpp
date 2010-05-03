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
* Description:
*
*/

#include "libraryloaderexception.h"

using namespace java::util;

OS_EXPORT LibraryLoaderException::LibraryLoaderException(int aErrCode,
        const std::string& aMessage,
        const std::string& aFile,
        const std::string& aMethod,
        int aLine) throw()
        : ExceptionBase(aErrCode,
                        aMessage,
                        aFile,
                        aMethod,
                        aLine)
{

}

OS_EXPORT LibraryLoaderException::LibraryLoaderException(int aErrCode,
        int aStatusCode,
        const std::string& aMessage,
        const std::string& aFile,
        const std::string& aMethod,
        int aLine) throw()
        : ExceptionBase(aErrCode,
                        aStatusCode,
                        aMessage,
                        aFile,
                        aMethod,
                        aLine)
{

}

OS_EXPORT LibraryLoaderException::~LibraryLoaderException() throw()
{

}

