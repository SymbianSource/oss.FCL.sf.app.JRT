/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Java RemCon Observer Active Object
*
*/


#ifndef JAVAREMCONOBSERVER_H
#define JAVAREMCONOBSERVER_H

#include <e32base.h>
#include <remconcoreapi.h>
#include <remconcoreapitargetobserver.h>
#include <e32msgqueue.h>

enum TMethodType
{
    EMethodNone = 0,
    EMethodMrccatoCommand = 1,
    EMethodMrccatoPlay = 2,
};

/**
 * Class is active object which should be created if some API wants use
 * RemCon API for receiving events
 *
 * Java RunTime needs to listen to various events like volume key press,
 * media key press and so on. For this purpose, various modules
 * in Java middleware register with CRemConCoreApiTarget to get events
 * to the observer MRemConCoreApiTargetObserver.
 * For this to happen, a target session has to be opened to RemCon,
 * which is done via CRemConInterfaceSelector::OpenTargetL(). At present,
 * whichever module wants it (say MMAPI, LCDUI etc) goes ahead and registers
 * by calling CRemConInterfaceSelector::OpenTargetL(). However when someother
 * module also calls the same, the OpenTargetL() returns KErrInUse as in
 * Java all modules are in one single process and one process can have
 * only one target session open to RemCon. This means that only one of the
 * required modules will get the events needed.
 *
 * Class provides common callback API for events from RemCon
 *
 *  @lib javaremconobserver
 *  @since S60 v3.2
 */
class CJavaRemConObserver : public CActive
{
private:
    class TJavaRemConMessage
    {
    public:
        TRemConCoreApiPlaybackSpeed iSpeed;
        TRemConCoreApiButtonAction iButtonAct;
        TRemConCoreApiOperationId iOperationId;
        TInt iMethod;
    };

public:

    /**
    * Constructor
    *
    * @since S60 v3.2
    * @param aObserver Set observer, not owned
    */
    IMPORT_C static CJavaRemConObserver* NewL(
        MRemConCoreApiTargetObserver& iObserver);

    CJavaRemConObserver(MRemConCoreApiTargetObserver& iObserver);

    ~CJavaRemConObserver();

    void AddPlayMessage(TRemConCoreApiPlaybackSpeed aSpeed,
                        TRemConCoreApiButtonAction aButtonAct);

    void AddCommandMessage(TRemConCoreApiOperationId aOperationId,
                           TRemConCoreApiButtonAction aButtonAct);

protected:
    void ConstructL();

private:
    void IssueRequest();

    // From CActive
    void RunL();

    void DoCancel();


private:

    /**
    * Observer called in RunL
    */
    MRemConCoreApiTargetObserver& iObserver;

    /**
    * Message queue for storing events from RemCon
    */
    RMsgQueue<TJavaRemConMessage> iQueue;
};

#endif // JAVAREMCONOBSERVER_H
