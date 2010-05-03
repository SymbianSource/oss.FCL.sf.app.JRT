/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef MMMAPLAYERINSTANCEOBSERVER_H
#define MMMAPLAYERINSTANCEOBSERVER_H

//  FORWARD DECLARATIONS
class CMMAPlayer;


//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Interface used to inform external components about player instances.
-----------------------------------------------------------------------------
*/
NONSHARABLE_CLASS(MMMAPlayerInstanceObserver)
{
public:
    /**
     * Adds new player to observer.
     *
     * @param aPlayer Player instance to add.
     */
    virtual void AddPlayerNotifyL(CMMAPlayer* aPlayer) = 0;

    /**
     * Removes player from observer.
     *
     * @param aPlayer Player instance to remove.
     */
    virtual void RemovePlayerNotify(CMMAPlayer* aPlayer) = 0;

protected:
    /**
     * Deletion through this interface is not allowed.
     */
    virtual ~MMMAPlayerInstanceObserver() {};
};

#endif // MMMAPLAYERINSTANCEOBSERVER_H
