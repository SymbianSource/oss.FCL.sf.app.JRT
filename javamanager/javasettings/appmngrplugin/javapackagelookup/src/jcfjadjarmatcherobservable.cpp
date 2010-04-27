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
* Description:  This class is used to notify observer
*
*/


#include "jcfjadjarmatcherobservable.h"

// ---------------------------------------------------------------------------
//    AddObserver
// -----------------------------------------------------------------------------
void MJcfJadJarMatcherObservable::AddObserver(MJcfJadJarMatcherObserver*  aObs)
// Definition. Adds an observer for this object.
{
    iObs = aObs;
}

// ---------------------------------------------------------------------------
//    NotifyObserver
// -----------------------------------------------------------------------------
void MJcfJadJarMatcherObservable::NotifyObserver(TInt aStatus)
// Definition. Method calls Update method of observer of this object.
{
    if (iObs)
    {
        iObs->Update(aStatus);
    }
}

