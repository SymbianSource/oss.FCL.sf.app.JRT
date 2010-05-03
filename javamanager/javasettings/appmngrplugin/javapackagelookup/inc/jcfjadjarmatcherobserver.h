/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface
*
*/


#ifndef JCFJADJARMATCHEROBSERVER_H
#define JCFJADJARMATCHEROBSERVER_H

#include <e32def.h>

/**
* Public interface for observer notification.
*  Observers can be notified through this API
*
*  @lib JcfJadJarMatcher.lib
*  @since S60 2.0
*/
class MJcfJadJarMatcherObserver
{
public:
    /**
     * This method is called whenever observed object needs.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aStatus - a generic argument.
     */
    virtual void Update(TInt aStatus) = 0;
};

#endif // JCFJADJARMATCHEROBSERVER_H
