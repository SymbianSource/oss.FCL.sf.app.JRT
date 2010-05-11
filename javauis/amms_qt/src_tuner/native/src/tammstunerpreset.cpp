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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include    "TAMMSTunerPreset.h"

#include    <s32mem.h>
#include <logger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::TAMMSTunerPreset
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TAMMSTunerPreset::TAMMSTunerPreset()
{
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::SetPresetName
// -----------------------------------------------------------------------------
//
void TAMMSTunerPreset::SetPresetName(const TDesC* aPresetName)
{
    iPresetName.Copy(*aPresetName);
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::SetPresetFrequency
// -----------------------------------------------------------------------------
//
void TAMMSTunerPreset::SetPresetFrequency(const TInt aPresetFrequency)
{
    iPresetFrequency = aPresetFrequency;
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::SetPresetStereoMode
// -----------------------------------------------------------------------------
//
void TAMMSTunerPreset::SetPresetStereoMode(const TInt aPresetStereoMode)
{
    iPresetStereoMode = aPresetStereoMode;
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::PresetName
// -----------------------------------------------------------------------------
//
TPtrC TAMMSTunerPreset::PresetName()
{
    return iPresetName;
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::PresetFrequency
// -----------------------------------------------------------------------------
//
TInt TAMMSTunerPreset::PresetFrequency()
{
    return iPresetFrequency;
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::PresetStereoMode
// -----------------------------------------------------------------------------
//
TInt TAMMSTunerPreset::PresetStereoMode()
{
    return iPresetStereoMode;
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::ExternalizeL
// Writes preset values to stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TAMMSTunerPreset::ExternalizeL(RWriteStream& aStream) const
{
    aStream<<iPresetName;
    aStream.WriteUint32L(iPresetFrequency);
    aStream.WriteUint32L(iPresetStereoMode);
}

// -----------------------------------------------------------------------------
// TAMMSTunerPreset::InternalizeL
// Reads preset values from stream.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void TAMMSTunerPreset::InternalizeL(RReadStream& aStream)
{
    aStream>>iPresetName;
    iPresetFrequency = aStream.ReadUint32L();
    iPresetStereoMode = aStream.ReadUint32L();
}

//  End of File
