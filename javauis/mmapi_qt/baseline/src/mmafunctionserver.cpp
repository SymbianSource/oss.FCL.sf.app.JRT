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
#include <logger.h> // DEBUG
#include <coemain.h>
#include <eikenv.h>

#include "mmafunctionserver.h"
#include "cmmaevent.h"
#include "cmmaplayer.h"
#include "mmmaplayerinstanceobserver.h"
#ifdef RD_JAVA_VOLUME_CONTROL
#include "cmmaglobalvolume.h"
#endif // RD_JAVA_VOLUME_CONTROL

MMAFunctionServer::MMAFunctionServer(JNIEnv& aJni, jobject aPeer): java::util::FunctionServer("JavaMMAPIFunctionServer")
{
	LOG( EJavaMMAPI, EInfo, "++MMAFunctionServer::MMAFunctionServer");
    createServerToNewThread();
    attachToVm(aJni, aPeer);
    mVmAttached = true;
    iServer = reinterpret_cast<java::util::FunctionServer*>(this);
}

MMAFunctionServer::~MMAFunctionServer()
{
    LOG( EJavaMMAPI, EInfo, "MMA::MMAFunctionServer::~MMAFunctionServer()");
        if (mVmAttached)
        {
            detachFromVm();
        }
        stopServer();
#ifdef RD_JAVA_VOLUME_CONTROL
    delete iGlobalVolume;
#endif // RD_JAVA_VOLUME_CONTROL
}

TInt MMAFunctionServer::NewL(JNIEnv& aJni, jobject aPeer)
{
	
	MMAFunctionServer* self = 	new(ELeave) MMAFunctionServer(aJni,aPeer);
	self->ConstructL(aJni,aPeer);
	return reinterpret_cast<TInt>(self);
}
void MMAFunctionServer::ConstructL(JNIEnv& aJni,
                                 jobject aPeer
                                 )
{
    LOG( EJavaMMAPI, EInfo, "MMA::MMAFunctionServer::ConstructL +");
    aJni.GetJavaVM(&iJavaVM); // Get pointer to VM
    LOG( EJavaMMAPI, EInfo, "MMA::MMAFunctionServer::ConstructL -");
}

java::util::FunctionServer* MMAFunctionServer::getFunctionServer() const
{
	LOG( EJavaMMAPI, EInfo, "++getFunctionServer1");
    JELOG2(EJavaMMAPI);
    return iServer;
}
JNIEnv* MMAFunctionServer::getValidJniEnv()
    {
        JELOG2(EJavaLocation);
        return mJniEnv;
    }
jobject MMAFunctionServer::getPeer()
    {
        return mJavaPeerObject;
    }

void MMAFunctionServer::ConstructSvrL()
{
    // creating fbs session
    User::LeaveIfError(RFbsSession::Connect());
    iFbsSessionConnected = ETrue;
    LOG( EJavaMMAPI, EInfo, "MMA::MMAFunctionServer::ConstructSvrL ok");
}

EXPORT_C void MMAFunctionServer::StaticAddObjectFromHandleL(
    MMAFunctionServer* aEventSource,
    TInt aHandle)
{
    aEventSource->AddObjectFromHandleL(aHandle);
}

void MMAFunctionServer::AddPlayerL(CMMAPlayer* aPlayer)
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

void MMAFunctionServer::DisposePlayer(CMMAPlayer* aPlayer)
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

EXPORT_C void MMAFunctionServer::SetPlayerInstanceObserver(
    MMMAPlayerInstanceObserver* aObserver)
{
    iInstanceObserver = aObserver;
}

EXPORT_C RPointerArray< CMMAPlayer >& MMAFunctionServer::Players()
{
    return iPlayers;
}

void MMAFunctionServer::Release()
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

void MMAFunctionServer::AddObjectFromHandleL(TInt aHandle)
{
	User::LeaveIfError(iObjects.Append(reinterpret_cast< CBase* >(aHandle)));
}


void MMAFunctionServer::FinalizeSvr()
{
    LOG1( EJavaMMAPI, EInfo, "MMA::MMAFunctionServer::FinalizeSvr() object count = %d",
              iObjects.Count());
    LOG1( EJavaMMAPI, EInfo, "MMA::MMAFunctionServer::FinalizeSvr() player count = %d",
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
    LOG( EJavaMMAPI, EInfo, "MMA::MMAFunctionServer::FinalizeSvr() ok");
}


void MMAFunctionServer::PostEvent(CMMAEvent* aEvent, TInt /*aPriority*/)
{
   LOG( EJavaMMAPI, EInfo, "MMAFunctionServer::PostEvent");
   aEvent->Dispatch(*mJniEnv);
}

CMMAPlayer* MMAFunctionServer::FindPlayer(TInt aPlayerHandle)
{
    LOG( EJavaMMAPI, EInfo, "MMAFunctionServer::FindPlayer");
    CMMAPlayer* player = reinterpret_cast< CMMAPlayer* >(aPlayerHandle);
    TInt index = iPlayers.Find(player);
    if (index != KErrNotFound)
    {
        player = iPlayers[ index ];
        LOG( EJavaMMAPI, EInfo, "MMAFunctionServer::FindPlayer: player found");
    }
    else
    {
        LOG( EJavaMMAPI, EInfo, "MMAFunctionServer::FindPlayer: player not found");
        player = NULL;
    }
    return player;
}
#ifdef RD_JAVA_VOLUME_CONTROL
void MMAFunctionServer::SetGlobalVolume(CMMAGlobalVolume* aGlobalVolume)
{
    delete iGlobalVolume;
    iGlobalVolume = aGlobalVolume;
}
#endif // RD_JAVA_VOLUME_CONTROL

//  END OF FILE
