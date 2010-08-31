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
* Description:  This class is used for volume setting
*
*/


//  INCLUDE FILES

#include "cmmavolumecontrol.h"
#include "cmmaplayer.h"
#include <logger.h>

_LIT(KMMAVolumeErrorMsg, "Can't set volume level");

const TInt KMMAJavaSoundIndex = 0;
const TInt KMMAGlobalVolumeSoundIndex = 2;

void CMMAVolumeControl::StaticSetLevelL(CMMAVolumeControl* aVolumeControl,
                                        TInt aLevel)
{
    // Java level is the first
    aVolumeControl->SetVolumeLevelL(KMMAJavaSoundIndex, aLevel);
}

void CMMAVolumeControl::StaticGetLevelL(CMMAVolumeControl* aVolumeControl,
                                        TInt* aLevel)
{
    // Java level is the first
    aVolumeControl->GetVolumeLevelL(KMMAJavaSoundIndex, aLevel);
}


CMMAVolumeControl::CMMAVolumeControl(CMMAPlayer* aPlayer)
        : iPlayer(aPlayer), iLevel(KMMAVolumeMaxLevel)
{
}


CMMAVolumeControl::~CMMAVolumeControl()
{
    iLevels.Close();
}


void CMMAVolumeControl::ConstructBaseL()
{
    iPlayer->AddStateListenerL(this);

    // Add level for java, will set in StaticSetLevelL method.
    // In constructor iLevels array is empty and Java level will be the first,
    // KMMAJavaSoundIndex.
    AddLevelL();

    iLevel = CalculateLevel();

    // The default value is not yet known. Volume control may change the
    // volume of the controller before it has been retrieved so when the
    // state of the player is changed the Java volume is retrieved from
    // the controller.
    iLevels[ KMMAJavaSoundIndex ] = KErrNotFound;
}

const TDesC& CMMAVolumeControl::ClassName() const
{
    return KMMAVolumeControlName;
}

void CMMAVolumeControl::SetLevelL(TInt aLevel)
{
    // Level cannot be set to derived control if player is not prefetched.
    if (iPlayer->State() > CMMAPlayer::ERealized)
    {
        DoSetLevelL(aLevel);
    }
    iLevel = aLevel;
}

void CMMAVolumeControl::StateChanged(TInt aState)
{
    LOG1(EJavaMMAPI, EInfo, "CMMAVolumeControl::StateChanged - state %d", aState);
    // Set the volume if the player is prefetched
    if (aState == CMMAPlayer::EPrefetched)
    {
        TRAPD(error,
        {
            // Get the default value for the Java sound level
            if (iLevels[ KMMAJavaSoundIndex ] == KErrNotFound)
            {
                iLevels[ KMMAJavaSoundIndex ] = DoGetLevelL();
                iLevel = CalculateLevel();
            }
            SetLevelL(iLevel);

        });
        if (error != KErrNone)
        {
            iPlayer->PostStringEvent(CMMAPlayerEvent::EError,
                                     KMMAVolumeErrorMsg);
        }
    }

    // Level is already set for Global Volume so no need to set it again
    // Notify the initial global volume value to java
    // Notify only if STATE == CMMAPlayer::ERealized
    // Error ID AKUR-7G69Q5 GLOBAL VOLUME EVENT CR
    if (aState == CMMAPlayer::ERealized)
    {
        if ((iLevels.Count() - 1) == KMMAGlobalVolumeSoundIndex)
        {
            LOG(EJavaMMAPI, EInfo, "MMA::CMMAVolumeControl::StateChanged : Post GLOBAL VOL EVENT  ");
            if (iLevels[ KMMAGlobalVolumeSoundIndex ] != KErrNotFound)
            {
                LOG1(EJavaMMAPI, EInfo, "MMA::CMMAVolumeControl::StateChanged : Post complete Val = %d ",iLevels[ KMMAGlobalVolumeSoundIndex ]);
                iPlayer->PostLongEvent(CMMAPlayerEvent::ENOKIA_EXTERNAL_VOLUME_EVENT,
                                       iLevels[ KMMAGlobalVolumeSoundIndex ]);
            }
        }
    }
}

void CMMAVolumeControl::RefreshVolume()
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAVolumeControl::RefreshVolume ++ ");
    TRAPD(error,
    {
        // Get the default value for the Java sound level
        if (iLevels[ KMMAJavaSoundIndex ] == KErrNotFound)
        {
            iLevels[ KMMAJavaSoundIndex ] = DoGetLevelL();
            iLevel = CalculateLevel();
        }
        SetLevelL(iLevel);

    });

    if (error != KErrNone)
    {
        iPlayer->PostStringEvent(CMMAPlayerEvent::EError,
                                 KMMAVolumeErrorMsg);
    }
    LOG(EJavaMMAPI, EInfo, "MMA::CMMAVolumeControl::RefreshVolume -- ");
}

void CMMAVolumeControl::RefreshControl()
{
    RefreshVolume();
}

EXPORT_C TInt CMMAVolumeControl::AddLevelL()
{
    User::LeaveIfError(iLevels.Append(KMMAVolumeMaxLevel));

    // New level is the last element
    return iLevels.Count() - 1;
}

EXPORT_C void CMMAVolumeControl::SetVolumeLevelL(TInt aLevelIndex,
        TInt aVolumeLevel)
{
    LOG2(EJavaMMAPI, EInfo, "CMMAVolumeControl::SetVolumeLevelL - setting index %d, level %d",
         aLevelIndex, aVolumeLevel);
    if (0 >= iLevels.Count() ||  iLevels.Count() > 3)
    {
        return ;
    }
    TInt oldVolumeLevel = iLevels[ aLevelIndex ];
    iLevels[ aLevelIndex ] = aVolumeLevel;
    SetLevelL(CalculateLevel());

    // send event if level is really changed
    if ((aLevelIndex == KMMAJavaSoundIndex) &&
            (aVolumeLevel != oldVolumeLevel))
    {
        iPlayer->PostObjectEvent(CMMAPlayerEvent::EVolumeChanged,
                                 iControlObject);
    }

    // send event if global volume level is really changed
    // Error ID AKUR-7G69Q5 GLOBAL VOLUME EVENT CR
    if ((aLevelIndex == KMMAGlobalVolumeSoundIndex) &&
            (aVolumeLevel != oldVolumeLevel))
    {

        iPlayer->PostLongEvent(CMMAPlayerEvent::ENOKIA_EXTERNAL_VOLUME_EVENT,
                               iLevels[ aLevelIndex ]);
    }
}

void CMMAVolumeControl::GetVolumeLevelL(TInt aLevelIndex,
                                        TInt* aVolumeLevel)
{
    LOG1(EJavaMMAPI, EInfo, "CMMAVolumeControl::GetVolumeLevelL - level index %d", aLevelIndex);

    // Return max volume if the default Java volume level is not yet known
    if (aLevelIndex == KMMAJavaSoundIndex &&
            iLevels[ KMMAJavaSoundIndex ] == KErrNotFound)
    {
        *aVolumeLevel = KMMAVolumeMaxLevel;
        return;
    }

    *aVolumeLevel = iLevels[ aLevelIndex ];

    LOG1(EJavaMMAPI, EInfo, "CMMAVolumeControl::GetVolumeLevelL - level %d", *aVolumeLevel);
}

TInt CMMAVolumeControl::CalculateLevel()
{
    TInt levelCount = iLevels.Count();

    // 64 bit integer must be used to have more than 4 levels.
    TInt64 level = KMMAVolumeMaxLevel;
    TInt64 sum = 1;

    // All levels in the array are percentage of the max volume (0..100).
    // Actual sound level will be multiplying all levels.
    for (TInt i = 0; i < levelCount; i++)
    {
        // If the level is not known it is expected to be max volume
        level = (iLevels[ i ] == KErrNotFound ?
                 level * KMMAVolumeMaxLevel :
                 level * iLevels[ i ]);
        sum *= KMMAVolumeMaxLevel;
    }
    return level / sum;
}
//  END OF FILE
