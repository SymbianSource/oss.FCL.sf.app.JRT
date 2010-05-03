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
* Description:  This class provides CJavaEventSource services
*
*/


//  INCLUDE FILES
#include <jdebug.h> // DEBUG
#include <coemain.h>
#include <eikenv.h>

#include "cmmaeventsource.h"
#include "cmmaevent.h"
#include "cmmaplayer.h"
#include "mmmaplayerinstanceobserver.h"
#ifdef RD_JAVA_VOLUME_CONTROL
#include "cmmaglobalvolume.h"
#endif // RD_JAVA_VOLUME_CONTROL

CMMAEventSource::CMMAEventSource()
{
    iFbsSessionConnected = EFalse;
    iJavaVM = 0;
}

CMMAEventSource::~CMMAEventSource()
{
    DEBUG("MMA::CMMAEventSource::~CMMAEventSource()");
    if (iJavaVM)
        iJavaVM->DetachCurrentThread();
#ifdef RD_JAVA_VOLUME_CONTROL
    delete iGlobalVolume;
#endif // RD_JAVA_VOLUME_CONTROL
}

void CMMAEventSource::ConstructL(JNIEnv& aJni,
                                 jobject aPeer,
                                 TJavaEventServer aServer)
{
    DEBUG("MMA::CMMAEventSource::ConstructL +");
    CJavaEventSourceBase::ConstructL(aJni, aPeer, aServer);
    aJni.GetJavaVM(&iJavaVM); // Get pointer to VM
    DEBUG("MMA::CMMAEventSource::ConstructL -");
}

void CMMAEventSource::ConstructSvrL()
{
    // creating fbs session
    User::LeaveIfError(RFbsSession::Connect());
    iFbsSessionConnected = ETrue;
    DEBUG("MMA::CMMAEventSource::ConstructSvrL ok");
}

EXPORT_C void CMMAEventSource::StaticAddObjectFromHandleL(
    CMMAEventSource* aEventSource,
    TInt aHandle)
{
    aEventSource->AddObjectFromHandleL(aHandle);
}

void CMMAEventSource::AddPlayerL(CMMAPlayer* aPlayer)
{
    if (iInstanceObserver)
    {
        iInstanceObserver->AddPlayerNotifyL(aPlayer);
    }
#ifdef RD_JAVA_VOLUME_CONTROL
    if (iGlobalVolume)
    {
        iGlobalVolume->AddPlayerL(aPlayer);
    }
#endif // RD_JAVA_VOLUME_CONTROL
    TInt err = iPlayers.Append(aPlayer);

    if (err != KErrNone)
    {
        // There was no memory to add new player to array,
        // player must be removed from instance observer.
        if (iInstanceObserver)
        {
            iInstanceObserver->RemovePlayerNotify(aPlayer);
        }
#ifdef RD_JAVA_VOLUME_CONTROL
        if (iGlobalVolume)
        {
            iGlobalVolume->RemovePlayer(aPlayer);
        }
#endif // RD_JAVA_VOLUME_CONTROL
        User::Leave(err);
    }
}

void CMMAEventSource::DisposePlayer(CMMAPlayer* aPlayer)
{
    TInt index = iPlayers.Find(aPlayer);
    if (index != KErrNotFound)
    {
        if (iInstanceObserver)
        {
            iInstanceObserver->RemovePlayerNotify(aPlayer);
        }
#ifdef RD_JAVA_VOLUME_CONTROL
        if (iGlobalVolume)
        {
            iGlobalVolume->RemovePlayer(aPlayer);
        }
#endif // RD_JAVA_VOLUME_CONTROL
        delete iPlayers[ index ];

        // remove object from list
        iPlayers.Remove(index);
    }
}

EXPORT_C void CMMAEventSource::SetPlayerInstanceObserver(
    MMMAPlayerInstanceObserver* aObserver)
{
    iInstanceObserver = aObserver;
}

EXPORT_C RPointerArray< CMMAPlayer >& CMMAEventSource::Players()
{
    return iPlayers;
}

void CMMAEventSource::Release()
{
    // If player instance observer is added to event source, remove all
    // players from observer before deleting those.
    if (iInstanceObserver)
    {
        for (TInt i = 0; i < iPlayers.Count(); i++)
        {
            iInstanceObserver->RemovePlayerNotify(iPlayers[ i ]);
#ifdef RD_JAVA_VOLUME_CONTROL
            if (iGlobalVolume)
            {
                iGlobalVolume->RemovePlayer(iPlayers[ i ]);
            }
#endif // RD_JAVA_VOLUME_CONTROL
        }
    }

    iPlayers.ResetAndDestroy();
}

void CMMAEventSource::AddObjectFromHandleL(TInt aHandle)
{
    User::LeaveIfError(iObjects.Append(JavaUnhand< CBase >(aHandle)));
}


void CMMAEventSource::FinalizeSvr()
{
    DEBUG_INT("MMA::CMMAEventSource::FinalizeSvr() object count = %d",
              iObjects.Count());
    DEBUG_INT("MMA::CMMAEventSource::FinalizeSvr() player count = %d",
              iPlayers.Count());

    // After object array alements are deleted all external references to
    // player objects are removed and player array can be deleted.
    iObjects.ResetAndDestroy();
    iPlayers.ResetAndDestroy();

    if (iFbsSessionConnected)
    {
        RFbsSession::Disconnect();
        iFbsSessionConnected = EFalse;
    }
    DEBUG("MMA::CMMAEventSource::FinalizeSvr() ok");
}


void CMMAEventSource::PostEvent(CMMAEvent* aEvent, TInt aPriority)
{
    CJavaEventSource< CMMAEventSource >::PostEvent(aEvent, aPriority);
}

CMMAPlayer* CMMAEventSource::FindPlayer(TInt aPlayerHandle)
{
    DEBUG("CMMAEventSource::FindPlayer");
    CMMAPlayer* player = JavaUnhand< CMMAPlayer >(aPlayerHandle);
    TInt index = iPlayers.Find(player);
    if (index != KErrNotFound)
    {
        player = iPlayers[ index ];
        DEBUG("CMMAEventSource::FindPlayer: player found");
    }
    else
    {
        DEBUG("CMMAEventSource::FindPlayer: player not found");
        player = NULL;
    }
    return player;
}
#ifdef RD_JAVA_VOLUME_CONTROL
void CMMAEventSource::SetGlobalVolume(CMMAGlobalVolume* aGlobalVolume)
{
    delete iGlobalVolume;
    iGlobalVolume = aGlobalVolume;
}
#endif // RD_JAVA_VOLUME_CONTROL

//  END OF FILE
