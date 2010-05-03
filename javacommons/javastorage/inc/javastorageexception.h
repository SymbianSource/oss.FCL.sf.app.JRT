/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JavaStorageException
*
*/


#ifndef JAVASTORAGEEXCEPTION_H
#define JAVASTORAGEEXCEPTION_H

#include "exceptionbase.h"
#include "javaosheaders.h"

namespace java
{
namespace storage
{

/**
 * JavaStorage specific JavaStorageException. It is used to indicate
 * JavaStorage related errors.
 */
class JavaStorageException : public java::util::ExceptionBase
{
public:
    JavaStorageException(int aStatus,
                         const std::string& aMessage,
                         const std::string& aFile,
                         const std::string& aMethod,
                         int aLine)
    throw() : ExceptionBase(aMessage,
                                    aFile,
                                    aMethod,
                                    aLine), mStatus(aStatus) {}

    virtual ~JavaStorageException() throw() {}

    /**
     * Convert exception to string.
     *
     * @return exception as string.
     */
    OS_IMPORT std::string toString() const throw();

    JavaStorageException(const JavaStorageException& x) : ExceptionBase(x)
    {
        mStatus = x.mStatus;
    }
    int    mStatus;

private:
    // Not implemented.
    JavaStorageException& operator= (const JavaStorageException&);
};

enum StorageErrorCodes
{
    ECommsFailure = -100,
    EInvalidStorage,
    EInvalidConnection,
    EInternalError,
    EInvalidDataStructure,
    EGeneralFailure,
    EInvalidArgument
};


} // end namespace storage
} // end namespace java

#endif // JAVASTORAGEEXCEPTION_H
