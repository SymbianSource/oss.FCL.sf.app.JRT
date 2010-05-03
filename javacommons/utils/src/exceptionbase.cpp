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

#include <sstream>
#include "exceptionbase.h"

using namespace java::util;

OS_EXPORT std::string ExceptionBase::toString() const
{
    std::ostringstream extendedMessage;

    extendedMessage << mMessage;

    if (0 != mErrCode)
    {
        extendedMessage << ": error code [" << mErrCode << "], ";
    }

    if (0 != mStatusCode)
    {
        extendedMessage << "status code [" << mStatusCode << "], ";
    }

    extendedMessage << " (file: " << mFile;
    extendedMessage << ", method: " << mMethod;
    extendedMessage << ", line: " << mLine;
    extendedMessage << ").";

    return extendedMessage.str();
}

OS_EXPORT ExceptionBase::ExceptionBase(const ExceptionBase& x) : std::exception(x)
{
    mErrCode    = x.mErrCode;
    mMessage    = x.mMessage;
    mStatusCode = x.mStatusCode;
    mFile       = x.mFile;
    mMethod     = x.mMethod;
    mLine       = x.mLine;
}

OS_EXPORT const char* ExceptionBase::what() const throw()
{
    return toString().c_str();
}

OS_EXPORT ExceptionBase::ExceptionBase(const std::string& message,
                                       const std::string& file,
                                       const std::string& method,
                                       int line) throw() :
        mErrCode(0),
        mStatusCode(0),
        mMessage(message),
        mFile(file),
        mMethod(method),
        mLine(line)
{
}

OS_EXPORT ExceptionBase::ExceptionBase(int aErrCode,
                                       const std::string& message,
                                       const std::string& file,
                                       const std::string& method,
                                       int line) throw() :
        mErrCode(aErrCode),
        mStatusCode(0),
        mMessage(message),
        mFile(file),
        mMethod(method),
        mLine(line)
{
}

OS_EXPORT ExceptionBase::ExceptionBase(int aErrCode,
                                       int aStatusCode,
                                       const std::string& message,
                                       const std::string& file,
                                       const std::string& method,
                                       int line) throw() :
        mErrCode(aErrCode),
        mStatusCode(aStatusCode),
        mMessage(message),
        mFile(file),
        mMethod(method),
        mLine(line)
{
}

OS_EXPORT ExceptionBase::ExceptionBase(const std::wstring& message,
                                       const std::string& file,
                                       const std::string& method,
                                       int line) throw() :
        mErrCode(0),
        mStatusCode(0),
        mFile(file),
        mMethod(method),
        mLine(line)
{
    mMessage.assign(message.begin(),message.end());
}

OS_EXPORT ExceptionBase::ExceptionBase(int aErrCode,
                                       const std::wstring& message,
                                       const std::string& file,
                                       const std::string& method,
                                       int line) throw() :
        mErrCode(aErrCode),
        mStatusCode(0),
        mFile(file),
        mMethod(method),
        mLine(line)
{
    mMessage.assign(message.begin(),message.end());
}

OS_EXPORT ExceptionBase::ExceptionBase(int aErrCode,
                                       int aStatusCode,
                                       const std::wstring& message,
                                       const std::string& file,
                                       const std::string& method,
                                       int line) throw() :
        mErrCode(aErrCode),
        mStatusCode(aStatusCode),
        mFile(file), mMethod(method),
        mLine(line)
{
    mMessage.assign(message.begin(),message.end());
}

OS_EXPORT ExceptionBase::~ExceptionBase() throw()
{

}

