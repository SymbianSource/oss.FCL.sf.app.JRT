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
* Description:  This class provides container for message.
*
*/


#ifndef MIDPRUNTIMEARGUMENTS_H
#define MIDPRUNTIMEARGUMENTS_H

#include <string>

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

const char* const APP_UID_ARGUMENT          = "-appid";
const char* const PUSH_ARGUMENT             = "-push";
const char* const PREWARM_ARGUMENT          = "-prewarm";
const char* const AUTO_INVOCATION_ARGUMENT  = "-autoinvocation";
const char* const DEBUG_ARGUMENT            = "-debug";
const char* const EXTRA_ARGUMENTS           = "-extra_args";
const char* const BG_START_ARGUMENT         = "-background";

} // namespace runtime
} //end namespace java

#endif // MIDPRUNTIMEARGUMENTS_H
