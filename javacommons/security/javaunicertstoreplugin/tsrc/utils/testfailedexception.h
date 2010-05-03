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


#ifndef TESTFAILEDEXCEPTION_H
#define TESTFAILEDEXCEPTION_H

#include <string>
#include <exception>

class TestFailedException : public std::exception
{
public:

    TestFailedException(const std::string& message, const std::string& file,
                        const std::string& method, int line);

    virtual ~TestFailedException() throw() {}

    std::string toString() const;

    TestFailedException(const TestFailedException& x)
    {
        mErrCode = x.mErrCode;
        mMessage = x.mMessage;
        mFile    = x.mFile;
        mMethod  = x.mMethod;
        mLine    = x.mLine;
    }

    int         mErrCode;
    std::string mMessage;
    std::string mFile;
    std::string mMethod;
    int         mLine;

private:
    TestFailedException& operator= (const TestFailedException&);
};

inline TestFailedException::TestFailedException(const std::string& message, const std::string& file,
        const std::string& method, int line)
        : mErrCode(0),mMessage(message), mFile(file), mMethod(method),mLine(line) {}

inline std::string TestFailedException::toString() const
{
    std::string str;
    if (0 != mErrCode)
    {
        char errCode[7];
        sprintf(errCode, "%d, ", mErrCode);
        str.append(errCode);
    }
    str.append(mMessage);

    str.append(" (file: ");
    str.append(mFile);

    str.append(", method: ");
    str.append(mMethod);

    str.append(", line: ");
    char line[7];
    sprintf(line, "%d", mLine);
    str.append(line);
    str.append(").");
    return str;
}

#endif // TESTFAILEDEXCEPTION_H

