/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CJavaApparcUtil definition.
*
*/


#ifndef JAVAAPPARCUTIL_H
#define JAVAAPPARCUTIL_H

#include <e32base.h>

namespace java
{
namespace backup
{
/**
 * Common utilities for Apparc registration.
 */
class CJavaApparcUtil : public CBase
{

public:

    /**
     * Creates a reasonably unique executable name, since the UI Framework
     * uses this as a key to look up the application opaque data.
     *
     * This name is not actually used to run the application, but it must
     * look reasonably like an acceptable file name.
     *
     * @param aAppUid Uid of the application
     * @param aAppName On return, it contains an unique executable name.
     * Maximum length of this descriptor should be >= 20 or the function
     * panics with USER 11.
     */
    static void inline AppName(const TUid aAppUid, TDes& aAppName);

};

} //namespace backup
} //namespace java

#include "javaapparcutil.inl"

#endif // JAVAAPPARCUTIL_H

// End of File
