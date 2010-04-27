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
* Description:  Observable interface
*
*/


#ifndef JCFJADJARMATCHEROBSERVABLE_H
#define JCFJADJARMATCHEROBSERVABLE_H

#include <e32def.h>
#include "jcfjadjarmatcherobserver.h"

/**
* Public interface for observer registration.
*  Obserevers can be registered through this API
*
*  @lib JcfJadJarMatcher.lib
*  @since S60 2.0
*/
class MJcfJadJarMatcherObservable
{
public:
    /**
     * To add an observer for this object.
     *
     * Preconditions. The following conditions must be true prior to calling
     * this method; otherwise, its operation is undefined.
     * - None.
     *
     * Postconditions. The following condition is true immediately after
     * returning from this method.
     * - None.
     *
     * @param aObs - an observer to be added.
     */
    void AddObserver(MJcfJadJarMatcherObserver* aObs);

    /**
     * To notify observer of this object.
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
    void NotifyObserver(TInt aStatus);

private:
    /**
     * The observer.
     */
    MJcfJadJarMatcherObserver* iObs;
};

#endif // JCFJADJARMATCHEROBSERVABLE_H
