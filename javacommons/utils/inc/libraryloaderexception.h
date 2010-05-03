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

#ifndef LIBRARYLOADEREXCEPTION_H
#define LIBRARYLOADEREXCEPTION_H

#include "exceptionbase.h"
#include "javaosheaders.h"

namespace java
{
namespace util
{

class OS_IMPORT LibraryLoaderException : public java::util::ExceptionBase
{

public:
    OS_IMPORT LibraryLoaderException(int aErrCode,
                                     const std::string& aMessage,
                                     const std::string& aFile,
                                     const std::string& aMethod,
                                     int aLine) throw();

    OS_IMPORT LibraryLoaderException(int aErrCode,
                                     int aStatusCode,
                                     const std::string& aMessage,
                                     const std::string& aFile,
                                     const std::string& aMethod,
                                     int aLine) throw();

    OS_IMPORT virtual ~LibraryLoaderException() throw();

    LibraryLoaderException(const LibraryLoaderException& x)
            : ExceptionBase(x) {}

private:

    //Not implemented.
    LibraryLoaderException& operator= (const LibraryLoaderException&);
};

}//end namespace util
}//end namespace java

#endif // LIBRARYLOADEREXCEPTION_H
