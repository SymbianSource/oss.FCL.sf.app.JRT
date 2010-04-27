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
* Description:  RecordControl for AudioRecorder
*
*/


#ifndef CMMAAUDIORECORDCONTROL_H
#define CMMAAUDIORECORDCONTROL_H

//  INCLUDES
#include "cmmarecordcontrol.h"
#include "cmmaaudiorecorder.h"

//  CLASS DECLARATION
/**
*   RecordControl for AudioRecorder
*
*
*/

NONSHARABLE_CLASS(CMMAAudioRecordControl): public CMMARecordControl,
        public MMMFControllerEventMonitorObserver
{
public:
    static CMMAAudioRecordControl* NewL(CMMAAudioRecorder* aRecorder);
    ~CMMAAudioRecordControl();

protected:
    CMMAAudioRecordControl(CMMAAudioRecorder* aRecorder);

public: // From CMMARecordControl
    void InitializeL();
    void DoStartRecordL();
    void DoStopRecordL();
    void DoResetL();
    TInt SetRecordSizeLimitL(TInt aSize);

public: // From MMMFControllerEventMonitorObserver
    void HandleEvent(const TMMFEvent& aEvent);

private: // data
    CMMAAudioRecorder* iRecorder; // not owned
};


#endif // CMMAAUDIORECORDCONTROL_H
