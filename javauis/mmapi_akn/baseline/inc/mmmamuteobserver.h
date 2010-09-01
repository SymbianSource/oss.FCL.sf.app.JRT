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
* Description:  Interface for mute observers
*
*/

#ifndef MMMAMUTEOBSERVER_H
#define MMMAMUTEOBSERVER_H

//  CLASS DECLARATION
/**
*   Interface for receiving mute notification from CMMAMuteNotifier.
*
*
*/

class MMMAMuteObserver
{
public:

    /**
     * Handles mute notification according to received parameter.
     * @param aMuted true if mute should be on, false if not
     */
    virtual void HandleMuteNotification(TBool aMuted) = 0;

};

#endif // MMMAMUTEOBSERVER_H
