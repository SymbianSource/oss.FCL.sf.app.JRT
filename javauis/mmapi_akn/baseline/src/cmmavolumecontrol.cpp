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
#include "cmmaplayer.h"
#include "cmmavolumecontrol.h"
#include <jdebug.h>
#include <MProfile.h>
#include <MProfileEngine.h>
#include <CProfileChangeNotifyHandler.h>

_LIT(KMMAVolumeErrorMsg, "Can't set volume level");

const TInt KMMAJavaSoundIndex = 0;
const TInt KMMAProfileSoundIndex = 1;
const TInt KMMAGlobalVolumeSoundIndex = 2;
const TInt KAudioEarpiece = 3;

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


EXPORT_C CMMAVolumeControl::CMMAVolumeControl(CMMAPlayer* aPlayer)
        : iPlayer(aPlayer), iLevel(KMMAVolumeMaxLevel)
{
}


EXPORT_C CMMAVolumeControl::~CMMAVolumeControl()
{
    delete iProfChangeNotifier;
    iLevels.Close();
}


EXPORT_C void CMMAVolumeControl::ConstructBaseL()
{
    iPlayer->AddStateListenerL(this);

    // Add level for javasound, will set in StaticSetLevelL method.
    SetJavaSoundVolumeLevelL();

    SetProfileSoundVolumeLevelL();

    // Sets level to 0 if profile isn't on
    iLevel = CalculateLevel();

    // The default value is not yet known. Volume control may change the
    // volume of the controller before it has been retrieved so when the
    // state of the player is changed the Java volume is retrieved from
    // the controller.
    iLevels[ KMMAJavaSoundIndex ] = KErrNotFound;
}

void CMMAVolumeControl::SetJavaSoundVolumeLevelL()
{
    // In constructor iLevels array is empty and Java level will be the first,
    // KMMAJavaSoundIndex.
    AddLevelL();
}

void CMMAVolumeControl::SetProfileSoundVolumeLevelL()
{
    // Add level for profile, level's index will be 1, KMMAProfileSoundIndex
    AddLevelL();
}

void CMMAVolumeControl::SetProfilesBasedSoundMutingL()
{
    //Get the current active profile id.
    MProfileEngine* lProfileEngine = CreateProfileEngineL();
    iProfileId =lProfileEngine->ActiveProfileId();
    lProfileEngine->Release();

    // if profile is silent or meeting and no headset or bluetooth device
    // connected,set profile volume level to 0, otherwise keep the original
    // value 100
    if ( (EProfileSilentId == iProfileId || EProfileMeetingId == iProfileId)&&
            iAudioOutputPreference != KAudioEarpiece )
    {
        iLevels[ KMMAProfileSoundIndex ] = 0;
    }

    // Gets notfication about profile changes
    // Notification is sent to MProfileChangeObserver::HandleActiveProfileEventL()
    iProfChangeNotifier = CProfileChangeNotifyHandler::NewL( this );
    // Sets level to 0 if profile isn't on
    iLevel = CalculateLevel();

}
EXPORT_C void CMMAVolumeControl::HandleActiveProfileEventL(
                                 TProfileEvent aProfileEvent, TInt aProfileId)
{
    switch (aProfileEvent)
    {
    case EProfileNewActiveProfile:
    {
        // New profile is activated
        // Update the volume level, if profile is changed to silent or meeting
        // then mute it, if no headset or bluetooth device is connected
        // else set it to max volume level
        TInt level = 0;
        iProfileId = aProfileId;

        if ( EProfileSilentId != aProfileId &&
                EProfileMeetingId != aProfileId)
        {

            level = KMMAVolumeMaxLevel;
        }
        else if (iAudioOutputPreference == KAudioEarpiece)
        {
            level = KMMAVolumeMaxLevel;
        }
        else
        {
            level = 0;
        }
        TRAPD( error, SetVolumeLevelL( KMMAProfileSoundIndex, level ) );
        if ( error != KErrNone )
        {
            iPlayer->PostStringEvent( CMMAPlayerEvent::EError,
                                      KMMAVolumeErrorMsg );
        }
        break;
    }
    default: // do nothing
        break;
    }
}



EXPORT_C const TDesC& CMMAVolumeControl::ClassName() const
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

EXPORT_C void CMMAVolumeControl::StateChanged(TInt aState)
{
    DEBUG_INT("CMMAVolumeControl::StateChanged - state %d", aState);
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
            if ((iLevels[ KMMAGlobalVolumeSoundIndex ] != KErrNotFound) && 
            (iLevels[ KMMAGlobalVolumeSoundIndex] != iInitialGlobalVolumeLevel))
            {
                DEBUG("MMA::CMMAVolumeControl::StateChanged : Post GLOBAL VOL EVENT  ");
                DEBUG_INT("MMA::CMMAVolumeControl::StateChanged : Post complete Val = %d ",
                                       iLevels[ KMMAGlobalVolumeSoundIndex ]);
                iPlayer->PostLongEvent(CMMAPlayerEvent::ENOKIA_EXTERNAL_VOLUME_EVENT,
                                       iLevels[ KMMAGlobalVolumeSoundIndex ]);
            }
        }
    }
}

EXPORT_C void CMMAVolumeControl::RefreshVolume()
{
    DEBUG("MMA::CMMAVolumeControl::RefreshVolume ++ ");
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
    DEBUG("MMA::CMMAVolumeControl::RefreshVolume -- ");
}

EXPORT_C void CMMAVolumeControl::RefreshControl()
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
    DEBUG_INT2("CMMAVolumeControl::SetVolumeLevelL - setting index %d, level %d",
               aLevelIndex, aVolumeLevel);
    if (0 >= iLevels.Count() ||  iLevels.Count() > 4)
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

void CMMAVolumeControl::InitializeGlobalVolumeLevel(TInt aGlobalVolumeLevel)
{
    iInitialGlobalVolumeLevel = aGlobalVolumeLevel;
}
void CMMAVolumeControl::GetVolumeLevelL(TInt aLevelIndex,
                                        TInt* aVolumeLevel)
{
    DEBUG_INT("CMMAVolumeControl::GetVolumeLevelL - level index %d", aLevelIndex);

    // Return max volume if the default Java volume level is not yet known
    if (aLevelIndex == KMMAJavaSoundIndex &&
            iLevels[ KMMAJavaSoundIndex ] == KErrNotFound)
    {
        *aVolumeLevel = KMMAVolumeMaxLevel;
        return;
    }

    *aVolumeLevel = iLevels[ aLevelIndex ];

    DEBUG_INT("CMMAVolumeControl::GetVolumeLevelL - level %d", *aVolumeLevel);
}

EXPORT_C void CMMAVolumeControl::SetAudioOutputPreferenceL(
    TInt aRoutingPreference)
{
    iAudioOutputPreference = aRoutingPreference;
    // If audio o/p preference is set to private then set the profile sound
    // to max value else if profile is in silent or meeting set the profile
    // sound to 0   
    if ( iAudioOutputPreference == KAudioEarpiece)
    {
       SetVolumeLevelL(KMMAProfileSoundIndex, KMMAVolumeMaxLevel);
    }
    else if (EProfileSilentId == iProfileId || EProfileMeetingId == iProfileId)
    {
       SetVolumeLevelL(KMMAProfileSoundIndex, 0);
    }
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
        DEBUG_INT2("CMMAVolumeControl::CalculateLevel value at iLevels[ %d ] is %d",i,iLevels[i]);
        // If the level is not known it is expected to be max volume
        level = (iLevels[ i ] == KErrNotFound ?
                 level * KMMAVolumeMaxLevel :
                 level * iLevels[ i ]);
        sum *= KMMAVolumeMaxLevel;
    }
    return level / sum;
}
//  END OF FILE
