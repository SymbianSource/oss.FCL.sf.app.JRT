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


#ifndef EXCEPTIONBASE_H
#define EXCEPTIONBASE_H

#include <string>
#include <exception>

#include "javaosheaders.h"

namespace java
{

namespace util
{

class OS_IMPORT ExceptionBase: public std::exception
{
public:

    OS_IMPORT ExceptionBase(const std::string& message, const std::string& file,
                            const std::string& method, int line) throw();

    OS_IMPORT ExceptionBase(int aErrCode,const std::string& message, const std::string& file,
                            const std::string& method, int line) throw();

    OS_IMPORT ExceptionBase(int aErrCode, int aStatusCode,
                            const std::string& message, const std::string& file,
                            const std::string& method, int line) throw();

    OS_IMPORT ExceptionBase(const std::wstring& message, const std::string& file,
                            const std::string& method, int line) throw();

    OS_IMPORT ExceptionBase(int aErrCode,const std::wstring& message, const std::string& file,
                            const std::string& method, int line) throw();

    OS_IMPORT ExceptionBase(int aErrCode, int aStatusCode,
                            const std::wstring& message, const std::string& file,
                            const std::string& method, int line) throw();

    OS_IMPORT virtual ~ExceptionBase() throw();
    OS_IMPORT virtual std::string toString() const;

    OS_IMPORT ExceptionBase(const ExceptionBase& x);

    OS_IMPORT virtual const char* what() const throw();

public:
    int         mErrCode;
    int         mStatusCode;
    std::string mMessage;
    std::string mFile;
    std::string mMethod;
    int         mLine;

private:
    ExceptionBase& operator= (const ExceptionBase&);

};

} //end namespace runtime
} //end namespace java

#endif // EXCEPTIONBASE_H
