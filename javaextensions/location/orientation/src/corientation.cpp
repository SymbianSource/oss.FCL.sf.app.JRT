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
void COrientation::GetOrientationL()
{
      
    iOrientationClass =
        (mFunctionServer->getValidJniEnv())->FindClass(
                                  "javax/microedition/location/Orientation");

    //Get Method ID of Azimuth Data Callback
    mAzimuthDataMethod = (mFunctionServer->getValidJniEnv())->GetStaticMethodID(
                                                      iOrientationClass,
                                                          "AzimuthDataCallBack",
                                                              "(I)V");

    //Check if all the JNI inits have succeeded
    if (NULL == mAzimuthDataMethod)
    {
        User::Leave( KErrGeneral);
    }

    OpenChannelL();

    CheckCalibrationL();

    iXYZChannel->StartDataListeningL(this, 1, 1, 0);

}

// ---------------------------------------------------------------------------
// COrientation::OpenChannel
// ---------------------------------------------------------------------------
//
void COrientation::OpenChannelL() 
{
    CreateChannelFinderL();

    iChannelInfoList.Reset();

    iChannelFinder->FindChannelsL(iChannelInfoList,
                                  ichannelInfo);
   
    if (iChannelInfoList.Count() != 1)
    {
        User::Leave( ERROR);
    }
    else
    {
        
        CreateSensorChannelL();

        iXYZChannel->OpenChannelL();

    }
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

        (mFunctionServer->getValidJniEnv())->CallStaticVoidMethod(
                                   iOrientationClass, mAzimuthDataMethod, err);
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

    if (info.iChannelType == KSensrvChannelTypeIdMagneticNorthData)
    {
        (mFunctionServer->getValidJniEnv())->CallStaticVoidMethod(
                                               iOrientationClass,
                                                   mAzimuthDataMethod,
                                                       aError);
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
void COrientation::CheckCalibrationL()
{
    TSensrvProperty property;
    TInt value(0);
    TInt maxValue(0);

    iXYZChannel->GetPropertyL(KSensrvPropCalibrationLevel,
                              KSensrvItemIndexNone, property);

    property.GetValue(value);
    property.GetMaxValue(maxValue);

    if (value != maxValue)
    {
        User::Leave( CALIBRATIONERROR);
    }

}
