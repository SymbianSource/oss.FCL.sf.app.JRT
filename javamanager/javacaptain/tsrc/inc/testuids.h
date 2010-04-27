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
#include "javauid.h"

using java::util::Uid;

// startup sequence : running_ind 1x after 0.5s
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 1x -> exits
#define uid_normal1 Uid(L"normal1")
//const Uid uid_normal1(L"normal1");
// As above but is always (re)-started to push mode
#define uid_push1 Uid(L"push1")
//const Uid uid_push1(L"push1");

// startup sequence : running_ind 2x after 0.5s (no delay in between)
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 1x -> exits
#define uid_normal2 Uid(L"normal2")
//const Uid uid_normal2(L"normal2");

// startup sequence : running_ind 2x after 0.5s (no delay in between)
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 2x (no delay) -> exits
#define uid_normal3 Uid(L"normal3")
//const Uid uid_normal3(L"normal3");

// startup sequence : running_ind 1x no delay
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind 1x -> exits
#define uid_normal4 Uid(L"normal4")
//const Uid uid_normal4(L"normal4");

// startup sequence : running_ind 1x
// runtime behaviour: stays running
// exit sequence    : sends nothing -> stays running
#define uid_wont_terminate1 Uid(L"wont_terminate1")
//const Uid uid_wont_terminate1(L"wont_terminate1");

// startup sequence : running_ind 1x
// runtime behaviour: stays running
// exit sequence    : sends terminate_ind but stays running
#define uid_wont_terminate2 Uid(L"wont_terminate2")
//const Uid uid_wont_terminate2(L"wont_terminate2");

// startup sequence : running_ind 0x
// runtime behaviour: exit immediately
// exit sequence    : sends terminate_ind 1x -> exits
#define uid_premature_exit1 Uid(L"premature_exit1")
//const Uid uid_premature_exit1(L"premature_exit1");

// startup sequence : running_ind 1x
// runtime behaviour: exit immediately
// exit sequence    : sends terminate_ind 1x -> exits
#define uid_premature_exit2 Uid(L"premature_exit2")
//const Uid uid_premature_exit2(L"premature_exit2");

// startup sequence : running_ind 1x
// runtime behaviour: exit immediately
// exit sequence    : sends nothing -> exits
#define uid_premature_exit3 Uid(L"premature_exit3")
//const Uid uid_premature_exit3(L"premature_exit3");

