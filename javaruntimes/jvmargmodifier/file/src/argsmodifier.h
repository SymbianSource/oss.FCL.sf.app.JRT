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
* Description:  JvmArgs modifier appends new args and replaces modified args
*
*/


#ifndef ARGSMODIFIER_H
#define ARGSMODIFIER_H

#include <string>
#include <list>

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

OS_NONSHARABLE_CLASS(JvmArgsModifier)
{
public:
    JvmArgsModifier();
    ~JvmArgsModifier();

    void combineArguments(std::list<std::wstring>& aOrig, const std::list<std::wstring>& aNew);
};

} // end namespace runtime
} // end namespace java


#endif // ARGSMODIFIER_H
