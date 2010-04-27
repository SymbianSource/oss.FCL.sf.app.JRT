/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This header is snapshot callback
*
*/


#ifndef MMMASNAPSHOTREADYCALLBACK_H
#define MMMASNAPSHOTREADYCALLBACK_H

NONSHARABLE_CLASS(MMMASnapshotReadyCallback)
{
public:
    /**
     * Called when snapshot is ready. Error situations
     * must be checked separately.
     */
    virtual void SnapshotReady() = 0;

protected:
    virtual ~MMMASnapshotReadyCallback()
    {
    };
};

#endif // MMMASNAPSHOTREADYCALLBACK_H
