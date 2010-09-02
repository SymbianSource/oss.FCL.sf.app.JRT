/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test util
*
*/

#include <sstream>

#define CHECKWITHNOTE(check, note) \
{\
    std::stringstream stream;\
    stream << note;\
    if (!(check))\
    {\
        ELOG1(EJavaRuntime, "TEST FAILURE: %s", stream.str().c_str());\
        FAIL(stream.str().c_str());\
    }\
    else\
    {\
        CHECK(true) /* Just to increase the check count*/ \
    }\
}
