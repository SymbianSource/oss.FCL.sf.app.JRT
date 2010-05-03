/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
 *
*/


#ifndef CORIENTATION_H
#define CORIENTATION_H

// EXTERNAL INCLUDES
#include <e32cmn.h>
#include <e32std.h>

// INTERNAL INCLUDES
#include "sensrvchannelinfo.h"
#include "sensrvdatalistener.h"
#include "sensrvchannelfinder.h"
#include "javajniutils.h"
#include "locationfunctionserver.h"

namespace java
{
namespace location
{

class COrientation: public CBase, public MSensrvDataListener
{
public:

    // Oreinataion data
    TInt mAzimuth;

    // Channel Info
    TSensrvChannelInfo ichannelInfo;

public:
    // Constructors and destructor

    /**
     * Constructor.
     */
    COrientation();

    /**
     * Destructor.
     */
    ~COrientation();

    /**
     * @return New instance from this class
     */
    static void NewL(TInt* aHandle);

public:
    /**
     * From MSensrvDataListener( Call backs)
     */

    void DataReceived(CSensrvChannel& aChannel, TInt aCount, TInt aDataLost);

    void DataError(CSensrvChannel& aChannel, TSensrvErrorSeverity aError);

    void GetDataListenerInterfaceL(TUid /* aInterfaceUid */, TAny*& aInterface);

public:
    // new method

    /**
     * Opens the Channel and Starts the Data Listening
     * @return Error number which indicates either sucess or failure
     */
    TInt GetOrientationL();

public:
    // Location Function Server
    LocationFunctionServer* mFunctionServer;

private:
    // new methods

    /**
     * Opens the the Channel
     */
    TInt OpenChannel();

    /**
     * Creates the Channel Finder
     */
    void CreateChannelFinderL();

    /**
     * Creates the Sensor Channel
     */
    void CreateSensorChannelL();

    /**
     * Checks the hardware caliberation
     */
    TInt CheckCalibration();

private:

    // JNI Environment Data
    JNIEnv* mJni;
    jobject mPeer;
    jclass iOrientationClass;

    // Azimuth Data Callback JNI Method ID
    jmethodID mAzimuthDataMethod;

    // sennor channel
    CSensrvChannel* iXYZChannel;

    // Channel Information List
    RSensrvChannelInfoList iChannelInfoList;

    // Channel Finder
    CSensrvChannelFinder* iChannelFinder;
};

}
}

#endif // CORIENTATION_H
