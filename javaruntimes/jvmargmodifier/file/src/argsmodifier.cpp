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

#include <iterator>


#include "logger.h"

#include "argsmodifier.h"

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

JvmArgsModifier::JvmArgsModifier()
{
}

JvmArgsModifier::~JvmArgsModifier()
{
}

void JvmArgsModifier::combineArguments(std::list<std::wstring>& aOrig, const std::list<std::wstring>& aNew)
{
    // for now just append the new values
    std::copy(aNew.begin(),aNew.end(), std::back_inserter(aOrig));
}

} // end namespace runtime
} // end namespace java

