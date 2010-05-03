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
* Description:  Reads JVM and application args from file
*
*/


#ifndef JVMARGSFILEREADER_H
#define JVMARGSFILEREADER_H

#include <string>
#include <list>

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

OS_NONSHARABLE_CLASS(JvmArgsFileReader)
{
public:
    JvmArgsFileReader(const std::wstring& aIdentifier);
    ~JvmArgsFileReader();

    const std::list<std::wstring>& getJvmArguments();
    const std::list<std::wstring>& getApplicationArguments();

private:
    void readArgs(const std::wstring& aFilename, std::list<std::wstring>& aArgs);
    std::string makeFullFilename(const std::wstring& aDrive, const std::wstring& aDir, const std::wstring& aFilename);

    std::list<std::wstring> mJvmArgs;
    std::list<std::wstring> mApplicationArgs;
    const std::wstring mIdentifier;
};

} // end namespace runtime
} // end namespace java


#endif // JVMARGSFILEREADER_H
