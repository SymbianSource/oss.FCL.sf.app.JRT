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
* Description:  Utility to get the preset and data
*
*/


#ifndef CAMMSEMCBASEMMFDEVSOUND_H
#define CAMMSEMCBASEMMFDEVSOUND_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <mmf/server/sounddevice.h>
#include <mmf/common/mmfbase.h>


// CLASS DECLARATION

/**
 *  Group for effect controls
 *
 *  @since 5.1
 */
NONSHARABLE_CLASS(CAMMSEMCBaseMMFDevSound)
        :public CBase, public MDevSoundObserver
{
public:  // destructor
    /**
    * Destructor.
    */
    virtual ~CAMMSEMCBaseMMFDevSound();

public:  // From MDevSoundObserver

    // empty implementation for callback methods from MDevSoundObserver
    virtual void InitializeComplete(TInt aError);
    virtual void BufferToBeFilled(CMMFBuffer* /*aBuffer*/) {}
    virtual void PlayError(TInt /*aError*/) {}

    virtual void ToneFinished(TInt /*aError*/) {}
    virtual void BufferToBeEmptied(CMMFBuffer* /*aBuffer*/) {}
    virtual void RecordError(TInt /*aError*/) {}
    virtual void ConvertError(TInt /*aError*/) {}
    virtual void DeviceMessage(TUid /*aMessageType*/,
                               const TDesC8& /*aMsg*/) {}
    virtual void SendEventToClient(const TMMFEvent& /*aEvent*/) {}


protected:
    virtual TInt CreateAndInitializeDevSoundL();

    /**
     * Deletes utilities
     * Does nothing if the utilities have already been deleted.
     */
    virtual void DeleteDevSound();

public:
    /**
     * c++ constructor.
     */
    CAMMSEMCBaseMMFDevSound();
    /**
     * 2nd phase constructor.
     */

    void ConstructL();

protected:

    CMMFDevSound* iMMFDevSound;


private:
    // Used to wait for Initializing the iMMFDevSound.
    CActiveSchedulerWait* iActiveSchedulerWait; // Owned.

};

#endif // CAMMSEMCBASEMMFDEVSOUND_H


