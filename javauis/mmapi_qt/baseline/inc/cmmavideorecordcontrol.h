/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is a RecordControl.
*
*/


#ifndef CMMAVIDEORECORDCONTROL_H
#define CMMAVIDEORECORDCONTROL_H

// EXTERNAL INCLUDES
#include <videorecorder.h>

//  INTERNAL INCLUDES
#include "cmmarecordcontrol.h"
#include "tmmaparametervalidator.h"

// FORWARD DECLARATIONS
class CMMAPlayer;
class CMMAPlayerProperties;
class CMMAMMFResolver;

//  CLASS DECLARATION
/**
*   This class implements RecordControl interface for camera player.
*
*
*/
NONSHARABLE_CLASS(CMMAVideoRecordControl): public CMMARecordControl,
        public MVideoRecorderUtilityObserver
{

public:
    /**
     * Creates new CMMAVideoRecordControl.
     *
     * @param aPlayer Player that controls the camera.
     * @param aProperties Properties given in java player creation.
     * @param aCameraHandle Handle to a camera, which is used as a source.
     */
    static CMMAVideoRecordControl* NewL(CMMAPlayer* aPlayer,
    CMMAMMFResolver* aResolver,
    const TMMAVideoSettings& aVideoSettings,
    CMMAAudioSettings* aAudioSettings,
    TInt aCameraHandle);
protected:
    CMMAVideoRecordControl(CMMAPlayer* aPlayer,
                           TInt aCameraHandle);
    ~CMMAVideoRecordControl();

    /**
     * Initializes this control.
     * @param aProperties Properties given in java player creation.
     */
    void ConstructL(RMMFControllerImplInfoArray& aRMMFControllerInfos,
                    const TMMAVideoSettings& aVideoSettings,
                    CMMAAudioSettings* aAudioSettings);

public: // From CMMARecordControl
    void InitializeL();
    void DoStartRecordL();
    void DoStopRecordL();
    void DoResetL();
    TInt SetRecordSizeLimitL(TInt aSize);

public: // From MVideoRecorderUtilityObserver
    /**
     * Notification to the client that the opening of the video clip has completed,
     * successfully, or otherwise
     *
     * @param   "aError"
     *          The status of the video recorder after initialisation.
     *          This is either KErrNone if the open has completed successfully,
     *          or one of the system wide error codes
     */
    void MvruoOpenComplete(TInt aError);

    /**
     * Notification that video recorder is ready to begin recording. This callback
     * is generated in response to a call to Prepare.
     *
     * @param   "aError"
     *          This is either KErrNone if the video recorder has been prepared for
     *          recording successfully, or one of the system wide error codes
     */
    void MvruoPrepareComplete(TInt aError);

    /**
     * Notification that video recording has completed. This is not called if
     * recording is explicitly stopped by calling Stop.
     *
     * @param   "aError"
     *          This is either KErrNone if recording was completed successfully,
     *          or one of the system wide error codes
     */
    void MvruoRecordComplete(TInt aError);

    /**
     * General event notification from controller. These events are specified by
     * the supplier of the controller
     *
     * @param   "aEvent"
     *          The event sent by the controller
     */
    void MvruoEvent(const TMMFEvent& aEvent);

private: // New methods

    /**
     * Actually sets record size limit
     * Will set local member to KNoUserDefinedRecordSize (-1)
     * @param aSize New record size limit
     */
    void DoSetRecordSizeLimitL(TInt aSize);

private: // Data
    TInt iCameraHandle; // camera that is used as an image source

    // Actual recording utility, owned
    CVideoRecorderUtility* iRecorder;

    CActiveSchedulerWait* iActiveSchedulerWait;

    /**
     * Global error code, used for getting error codes from callbacks
     */
    TInt iError;

    /**
     * Record size limit, will be KNoUserDefinedRecordSize (-1) when
     * user has not set limits
     */
    TInt iRecordSizeLimit;

    TMMAVideoSettings iVideoSettings; // Video settings

};


#endif // CMMAVIDEORECORDCONTROL_H
