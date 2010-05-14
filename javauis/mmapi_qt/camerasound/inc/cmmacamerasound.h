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
* Description:  This class is used to play sound from file.
*
*/


#ifndef CMMACAMERASOUND_H
#define CMMACAMERASOUND_H

//  INCLUDES
#include <mdaaudiosampleplayer.h>

// CLASS DECLARATION
/**
* This class is used to play sound from file.
*/
NONSHARABLE_CLASS(CMMACameraSound) : public CBase,
        public MMdaAudioPlayerCallback
{
public: // Constructor and destructor

    /**
    * Two-phased constructor.
    */
    static CMMACameraSound* NewLC();

    /**
    * Destructor.
    */
    virtual ~CMMACameraSound();


public: // New functions

    /**
     * Plays sound from a file when capturing a image.
     */
    static void PlayImageCaptureSoundL();

    /**
    * Plays sound from a file when video recording starts.
    */
    static void PlayVideoRecordSoundL();

protected: // New functions

    /**
    * Plays sound from a file.
    * @param aFileName - The full path name of the file
    *                    containing the audio data.
    */
    void PlaySoundL(const TDesC& aFileName);


public: // from MMdaAudioPlayerCallback

    void MapcInitComplete(TInt aError,
                          const TTimeIntervalMicroSeconds& /*aDuration*/);

    void MapcPlayComplete(TInt aError);


private: // Constructors

    /**
    * C++ default constructor.
    */
    CMMACameraSound();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL();

private: //Data

    CActiveSchedulerWait* iActiveSchedulerWait; // owned

    TInt iError;
};

#endif // CMMACAMERASOUND_H

// End of File
