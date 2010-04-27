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
* Description: Converts Universal Emulator Interface (UEI) arguments to
*              JVM arguments
*
*/


#ifndef UEIARGSPARSER_H
#define UEIARGSPARSER_H

#include <string>
#include <vector>
#include "javaosheaders.h"

namespace java
{
namespace runtime
{

OS_NONSHARABLE_CLASS(UeiArgsParser)
{
public:
    UeiArgsParser() {};
    virtual ~UeiArgsParser() {};

    std::wstring convertUeiArgsToJvmArgs(const std::wstring& aUeiParameters);

protected:
    // protected so that unit tests can access these functions
    std::vector<std::wstring> getArgs(const std::wstring& aUeiParameters);

    bool isVerbose(const std::wstring& aUeiArg);
    bool isRundjwp(const std::wstring& aUeiArg);

    std::wstring convertVerbose(const std::wstring& aVerbose);
    std::wstring convertRundjwp(const std::wstring& aRunjdwp);

    bool hasOption(const std::wstring& aArg, const std::wstring& aOption);
    std::wstring trimTrailingSpaces(std::wstring& aString, const std::wstring& aWhitespaces);
};

} // end namespace runtime
} // end namespace java


#endif // UEIARGSPARSER_H
