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
* Description:  Handles Oreintation operations
 *
*/


// EXTERNAL INCLUDES
#include <e32std.h>

// INTERNAL INCLUDES
#include <corientation.h>
#include <sensrvsensorchannels.h>
#include <sensrvchanneldatatypes.h>
#include <sensrvmagneticnorthsensor.h>
#include <sensrvmagnetometersensor.h>
#include "javajniutils.h"

using namespace java::location;

#define ERROR -2
#define CALIBRATIONERROR -3

// ---------------------------------------------------------------------------
// COrientation::COrientation
// ---------------------------------------------------------------------------
//
COrientation::COrientation()
{
    iChannelFinder = NULL;
    iXYZChannel = NULL;
}

// ---------------------------------------------------------------------------
// COrientation::NewL
// ---------------------------------------------------------------------------
//
void COrientation::NewL(TInt* aHandle)
{
    //  CallMethodL(new(ELeave) COrientation(), aserver);
    COrientation* self = new(ELeave) COrientation();
    *aHandle = reinterpret_cast<TInt>(self);
}

// ---------------------------------------------------------------------------
// COrientation::~COrientation
// ---------------------------------------------------------------------------
//
COrientation::~COrientation()
{
    if (iXYZChannel)
    {
        delete iXYZChannel;
        iXYZChannel = NULL;
    }

    if (iChannelFinder)
    {
        delete iChannelFinder;
        iChannelFinder = NULL;
    }

    iChannelInfoList.Reset();
    iChannelInfoList.Close();
}

// ---------------------------------------------------------------------------
// COrientation::GetOrientation
// ---------------------------------------------------------------------------
//
TInt COrientation::GetOrientationL()
{
    mJni = mFunctionServer->getValidJniEnv();
    mPeer = mFunctionServer->getPeer();

    iOrientationClass =
        mJni->FindClass("javax/microedition/location/Orientation");

    //Get Method ID of Azimuth Data Callback
    mAzimuthDataMethod = mJni->GetStaticMethodID(iOrientationClass,
                         "AzimuthDataCallBack",
                         "(I)V");

    //Check if all the JNI inits have succeeded
    if (NULL == mAzimuthDataMethod)
    {
        return KErrGeneral;
    }

    TInt Err = KErrNone;
    TInt result = this->OpenChannel();

    if (result == KErrNone)
    {
        TInt CalibErr = CheckCalibration();

        if (CalibErr < KErrNone)
        {
            return CalibErr;
        }

        TRAP(Err, iXYZChannel->StartDataListeningL(this, 1, 1, 0));

        if (Err == KErrNotFound)
        {
            return Err;
        }
        else if (Err == KErrAlreadyExists)
        {
            return Err;
        }
    }

    return result;
}

// ---------------------------------------------------------------------------
// COrientation::OpenChannel
// ---------------------------------------------------------------------------
//
TInt COrientation::OpenChannel()
{
    TInt ChnlFindErr = 0;
    TRAP(ChnlFindErr, this->CreateChannelFinderL());

    if (ChnlFindErr < 0)
    {
        return ChnlFindErr;
    }

    iChannelInfoList.Reset();

    TInt FindErr = 0;
    TRAP(FindErr, iChannelFinder->FindChannelsL(iChannelInfoList,
            ichannelInfo));

    if (FindErr < KErrNone)
    {
        return FindErr;
    }

    else if (iChannelInfoList.Count() != 1)
    {
        return ERROR;
    }
    else
    {
        TInt SensChanlErr = 0;
        TRAP(SensChanlErr, this->CreateSensorChannelL());

        if (SensChanlErr < 0)
        {
            return SensChanlErr;
        }

        TInt err = 0;
        TRAP(err, iXYZChannel->OpenChannelL());

        if (err == KErrNotFound)
        {
            return err;
        }
        else if (err == KErrAlreadyExists)
        {
            return err;
        }
    }

    return KErrNone;
}

// ---------------------------------------------------------------------------
// COrientation::DataReceived
// Call Back Function
// ---------------------------------------------------------------------------
//
void COrientation::DataReceived(CSensrvChannel& aChannel, TInt /*aCount*/,
                                TInt /*aDataLost*/)
{
    TSensrvChannelInfo info = aChannel.GetChannelInfo();
    TInt err = KErrNone;

    // Get the Azimuth data
    if (info.iChannelType == KSensrvChannelTypeIdMagneticNorthData)
    {
        TPckgBuf<TSensrvMagneticNorthData> dataBuf;
        err = aChannel.GetData(dataBuf);

        if (err >= KErrNone)
        {
            TSensrvMagneticNorthData data = dataBuf();
            mAzimuth = data.iAngleFromMagneticNorth;
        }

        //Stop Data Listening and Close the channel.
        iXYZChannel->StopDataListening();
        iXYZChannel->CloseChannel();

        mJni = mFunctionServer->getValidJniEnv();

        (*mJni).CallStaticVoidMethod(iOrientationClass, mAzimuthDataMethod, err);
    }
}

// ---------------------------------------------------------------------------
// COrientation::DataError
// Call Back Function
// ---------------------------------------------------------------------------
//
void COrientation::DataError(CSensrvChannel& aChannel,
                             TSensrvErrorSeverity aError)
{
    TSensrvChannelInfo info = aChannel.GetChannelInfo();

    //Stop Data Listening and Close the channel.
    iXYZChannel->StopDataListening();
    iXYZChannel->CloseChannel();

    mJni = mFunctionServer->getValidJniEnv();

    if (info.iChannelType == KSensrvChannelTypeIdMagneticNorthData)
    {
        (*mJni).CallStaticVoidMethod(iOrientationClass,
                                     mAzimuthDataMethod, aError);
    }
}

// ---------------------------------------------------------------------------
// COrientation::GetDataListenerInterfaceL
// Call Back Function
// Description : Dummy function to allign with Listener
// ---------------------------------------------------------------------------
//
void COrientation::GetDataListenerInterfaceL(TUid /* aInterfaceUid */,
        TAny*& aInterface)

{
    aInterface = NULL;
}

// ---------------------------------------------------------------------------
// COrientation::CreateChannelFinderL
// ---------------------------------------------------------------------------
//
void COrientation::CreateChannelFinderL()
{
    // create channelfinder if it does not exist already
    if (!iChannelFinder)
    {
        iChannelFinder = CSensrvChannelFinder::NewL();
    }
}

// ---------------------------------------------------------------------------
// COrientation::CreateSensorChannelL
// ---------------------------------------------------------------------------
//
void COrientation::CreateSensorChannelL()
{
    if (iXYZChannel)
    {
        delete iXYZChannel;
        iXYZChannel = NULL;
    }

    iXYZChannel = CSensrvChannel::NewL(iChannelInfoList[0]);
}

// ---------------------------------------------------------------------------
// COrientation::CheckCalibration
// ---------------------------------------------------------------------------
//
TInt COrientation::CheckCalibration()
{
    TSensrvProperty property;
    TInt value(0);
    TInt maxValue(0);
    TInt LeaveError(KErrNone);

    TRAP(LeaveError, iXYZChannel->GetPropertyL(KSensrvPropCalibrationLevel,
            KSensrvItemIndexNone, property));

    if (LeaveError < KErrNone)
    {
        return LeaveError;
    }

    property.GetValue(value);
    property.GetMaxValue(maxValue);

    if (value != maxValue)
    {
        return CALIBRATIONERROR;
    }

    return KErrNone;
}
