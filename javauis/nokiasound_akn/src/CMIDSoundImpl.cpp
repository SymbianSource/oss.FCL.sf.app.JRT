/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface and a base class for CMIDClip and CMIDTone.
*
*/


#include "CMIDSoundImpl.h"
#include "CMIDSoundEvent.h"
#include "com_nokia_mid_sound_Sound.h"

CMIDSoundImpl::~CMIDSoundImpl()
{

}

CMIDSoundImpl::CMIDSoundImpl(MMIDEventSender* aEventSender)
{
    iEventSender = aEventSender;
}

void CMIDSoundImpl::ConstructL()
{

}

void CMIDSoundImpl::Resume()
{
    if (iPlayed && (iState == EReadyToPlay))
    {
        Play(1);
    }
}
