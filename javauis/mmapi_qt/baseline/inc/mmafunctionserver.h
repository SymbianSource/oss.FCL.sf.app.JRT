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


#ifndef CMMAEVENTSOURCE_H
#define CMMAEVENTSOURCE_H

//  INCLUDES
#include "mmmaeventposter.h"
//#include <jutils.h>
#include "functionserver.h"
#include "legacyeventserverwrapper.h"
#include <logger.h>
// FORWARD DECLARATIONS
class CApaWindowGroupName;
class CMMAPlayer;
class MMMAPlayerInstanceObserver;
#ifdef RD_JAVA_VOLUME_CONTROL
class CMMAGlobalVolume;
#endif // RD_JAVA_VOLUME_CONTROL

//  CLASS DECLARATION
/**
*   This class provides CJavaEventSource services.
*
*
*/

class MMAFunctionServer : public java::util::FunctionServer,public LegacyEventServerWrapper,
        public MMMAEventPoster
{
public:
    /**
     * Default constructor.
     */
    MMAFunctionServer(JNIEnv& aJni, jobject aPeer);

    /**
     * Destructor.
     */
    ~MMAFunctionServer();

public: // From CJavaEventSource, construction
    /**
     * Normal construction, Invoked from base class.
     * Method is defined in the base class.
     */
    void ConstructL(JNIEnv& aJni,
                    jobject /*aPeer*/
                    );

    /**
     * Server side construction.
     */
    void ConstructSvrL();
    static TInt NewL(JNIEnv& aJni, jobject aPeer);
    
		void Dispose();
    
    TBool IsDisposed() const
    {
        return iDisposed;
    }    

public: // new methods
    /**
     * Adds object to cleanup. Added objects are deleted when event source
     * is deleted.
     *
     * @param aEventSource Event source to use.
     * @param aHandle Java handle to CBase object
     */
    IMPORT_C static void StaticAddObjectFromHandleL(MMAFunctionServer* aEventSource,
            TInt aHandle);

    /**
     * Adds player object to cleanup. Added objects are deleted when event
     * source is deleted. An object added to the event source must be
     * deleted with DisposePlayer method.
     *
     * @param aEventSource Event source to use.
     * @param aHandle Java handle to CBase object
     */
    void AddPlayerL(CMMAPlayer* aPlayer);

    /**
     * Removes player from array and deletes it.
     *
     * @param aPlayer Player to be deleted.
     */
    void DisposePlayer(CMMAPlayer* aPlayer);

    /**
     * Sets player instance observer.
     * @param aObserver Will be informed when player is created or deleted.
     */
    IMPORT_C void SetPlayerInstanceObserver(MMMAPlayerInstanceObserver* aObserver);

    /**
     * Return reference to the array containing all players.
     * @return All players
     */
    IMPORT_C RPointerArray< CMMAPlayer >& Players();

    /**
     * Realeases resources.
     */
    void Release();

    /**
     * Returns player pointer for java player handle if the native player
     * peer exists. Otherwise returns null.
     * @param aPlayerHandle Java side player handle integer
     * @param CMMAPlayer* player pointer
     */
    CMMAPlayer* FindPlayer(TInt aPlayerHandle);

    /**
     * Sets MMA global volume handler
     * @param aGlobalVolume Global volume handler. The onwership is
     *        transferred to this class
     */
#ifdef RD_JAVA_VOLUME_CONTROL
    void SetGlobalVolume(CMMAGlobalVolume* aGlobalVolume);
#endif // RD_JAVA_VOLUME_CONTROL

		 java::util::FunctionServer* getFunctionServer() const;
		 
		 JNIEnv* getValidJniEnv();
    	 jobject getPeer();
private:
    /**
     * @see StaticAddObjectFromHandleL
     * @param aHandle Java handle to CBase object
     */
    void AddObjectFromHandleL(TInt aHandle);

private: // from CJavaEventSource
    void FinalizeSvr();

public: // MMMAEventPoster
    void PostEvent(CMMAEvent* aEvent, TInt aPriority);

private:
    bool mVmAttached;
    java::util::FunctionServer* iServer;
private:
    /**
     * Objects added with StaticAddObjectFromHandleL. Will be destroyed
     * in server finalization.
     */
    RPointerArray< CBase > iObjects;

    // Owned array of players.
    RPointerArray< CMMAPlayer > iPlayers;

    /**
     * MMA needs ui framework. Window group must be hidden from the
     * user.
     */
    CApaWindowGroupName*    iWgName;

    /**
     * Used to check if session was connected in the constructor.
     */
    TBool iFbsSessionConnected;
    TBool iDisposed;

    // Not owned. Will be informed if it is set.
    MMMAPlayerInstanceObserver* iInstanceObserver;

    JavaVM* iJavaVM;

    // Global volume handler. Owned
#ifdef RD_JAVA_VOLUME_CONTROL
    CMMAGlobalVolume* iGlobalVolume;
#endif // RD_JAVA_VOLUME_CONTROL
};

#endif // CMMAEVENTSOURCE_H
