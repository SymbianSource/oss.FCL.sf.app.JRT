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
* Description:  This file defines global constants.
*
*/


#ifndef AMMSCONSTANTS_H
#define AMMSCONSTANTS_H

/**
*
*  This file defines global constants.
*
*
*  @since 3.0
*/

const TInt KAMMSVectorComponents = 3;    // Components in a vector
const TInt KAMMSTwoVectorComponents = 6; // Components in two vectors

// Component indexes in vectors
enum TVectorIndex
{
    EComponentX = 0,
    EComponentY = 1,
    EComponentZ = 2,
    EAzimuth = 0,
    EElevation = 1,
    ERadius = 2
};

enum TAMMSControlTypes
{
    EAMMSBaseControl = 0,
    EAMMSSpectatorControl,
    EAMMSSoundSource3DControl
};

_LIT(KAMMSGlobalVolume, "VolumeGlobal");

#endif // AMMSCONSTANTS_H


