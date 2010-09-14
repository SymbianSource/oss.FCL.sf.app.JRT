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
* Description:  Observer class for Publish and Subscribe handling
*
*/


#ifndef MMMMACALLSTATEOBSERVER_H
#define MMMMACALLSTATEOBSERVER_H

/**
 *  Abstract interface to the PS key watcher.
 *
 *  @lib 
 */
NONSHARABLE_CLASS(MMMACallStateObserver)
    {
public:

    virtual ~MMMACallStateObserver() {};

    /**
    * Handle Call state event
    *
    * @since 
    * @param aUid The UID that identifies the property category
    * @param aKey The property sub-key
    */
    virtual void HandleCallStateEventL(TUid aUid, TInt aKey) = 0;
    };


#endif // M_MMMAPSKEYOBSERVER_H
