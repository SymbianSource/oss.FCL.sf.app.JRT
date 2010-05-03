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
* Description:
*
*/


#ifndef JAVASTARTER_H
#define JAVASTARTER_H

namespace java
{
#ifdef __SYMBIAN32__
#define OS_IMPORT IMPORT_C
#else
#define OS_IMPORT
#endif

OS_IMPORT int start(int argc, const char** argv); // UTF-8
OS_IMPORT int start(const char* fileName);        // UTF-8

} // end namespace java

#endif // JAVASTARTER_H
