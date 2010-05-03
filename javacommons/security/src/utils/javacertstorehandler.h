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
#ifndef JAVACERTSTOREHANDLER_H
#define JAVACERTSTOREHANDLER_H

#include <vector>
#include <string>

namespace java
{
namespace security
{

class JavaCertStoreHandler
{
public:
    static void retrieveRootProtDomainInfo(const char rootHash[],
                                           std::string& protDomainName,
                                           std::string& protDomainCategory);
    static void retrieveRootContent(std::wstring& rootHash,
                                    long long* len,
                                    std::string& rootContent);
    static void retrieveRootsContents(std::vector<std::string>&);
};

} //end namespace security
} //end namespace java

#endif // JAVACERTSTOREHANDLER_H

