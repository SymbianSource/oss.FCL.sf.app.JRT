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
* Description:  This class is used for playing sounds
*
*/

#ifndef CMMAEMCPLAYERBASE_H
#define CMMAEMCPLAYERBASE_H

//  INCLUDES
#include <mmf/common/mmfbase.h>
#include "cmmaplayer.h"
#include "e32base.h"
#include "f32file.h"
#include "StreamControl.h"
#include "Events.h"
#include "MMControlFactory.h"
#include "ControlObserver.h"
#include "DataBufferSource.h"
#include "FileSource.h"
#include "DescriptorSource.h"
#include "DataBuffer.h"
#include "SinkControl.h"
#include "badesca.h"
#include "MimeTypes.h"
#include "StreamControlObserver.h"
#include "SourceControlObserver.h"


using multimedia ::MStreamControl;
using multimedia ::MControlObserver;
using multimedia ::MDataBufferSource;
using multimedia ::MDescriptorSource;
using multimedia ::MDataBuffer;
using multimedia ::MControl;
using multimedia ::CMultimediaFactory;
using multimedia ::MSinkControl;
using multimedia ::MFileSource;
using multimedia::MSourceControlObserver;
using multimedia::MStreamControlObserver;

class CMMAEMCResolver;
//  CLASS DECLARATION
/**
*   This is base class for players those use EMC
*
*
*/

NONSHARABLE_CLASS(CMMAEMCPlayerBase): public CMMAPlayer,
        public MControlObserver
{
protected:
    //   C++ constructor
    CMMAEMCPlayerBase(CMMAEMCResolver* aResolver);
    void ConstructL();

    ~CMMAEMCPlayerBase();
public: // new methods
    /**
     * Getter for StreamControl
     */
    IMPORT_C MStreamControl * StreamControl();

    /**
    * Multimedia Factory getter
    */
    IMPORT_C CMultimediaFactory* MMFactory();

    /**
     * Check whether this player is playing from a file locator
     * @return ETrue if is a file player, EFalse otherwise
     */
    TBool IsFilePlayer();

public: // from CMMAPlayer
    void StartL();
    void StopL(TBool aPostEvent);
    void DeallocateL();
    void GetDuration(TInt64* aDuration);
    void SetMediaTimeL(TInt64* aTime);
    void GetMediaTime(TInt64* aMediaTime);
    void CloseL();

private: //New methods
    void CreateStreamL();
    void DetermineMimeTypeL(const TDesC& aFileName, TDes8& aMimeType);
    void CleanupSource();
    void AddDataSourceToStreamL();

public: // from MControlObserver
    void Event(MControl* aControl,
               TUint aEventType,
               TAny* aEventObject);

protected:

    CActiveSchedulerWait* iActiveSchedulerWait;

    /**
     * StreamControl infos, owned
     */
    MStreamControl* iMStreamControl;
    MDataBufferSource* iMDataBufferSource;
    MDescriptorSource* iMDescriptorSource;
    MFileSource* iMFileSource;
    MSinkControl* iMAudioSink;
    MStreamControl::TStreamState iPrevStreamControlState;
    enum TSourceType
    {
        EFILESOURCE,
        EDATABUFFERSOURCE,
        EDESCRIPTORSOURCE
    };
    TSourceType iSourceType;
    RPointerArray<MDataBuffer> iBuffers;
    RArray<TInt> iAvailable;
    TBool iIsEOFReached;
    TBool iAutoWriteBuffer;
    TInt iBytesReadFromFile;
    TInt iFileSizeInBytes;
    CMultimediaFactory* iFactory;

    HBufC8* iMimeType;
    HBufC8* iDescData;

    /**
     * Filename used for playing directly from file, owned
     */
    HBufC* iFileName;

    /**
    * Cached media time
    */
    TInt64 iMediaTime;

    /**
    * The time that will be sent with CMMAPlayerEvent::EStarted
    * (may be different from iMediaTime).
    */
    TInt64 iStartedEventTime;
};

#endif // CMMAEMCPLAYERBASE_H

// End of file
