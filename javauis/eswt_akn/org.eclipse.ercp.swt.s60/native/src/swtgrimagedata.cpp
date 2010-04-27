/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "swtgrimagedata.h"
#include "swtgrpalettedata.h"


// ======== MEMBER FUNCTIONS ========


/**
 * C++ constructor with initialisation data
 */
CSwtGrImageData::CSwtGrImageData(const TInfo& aInfo, CSwtGrPaletteData* aPalette,
                                 HBufC8* aPixelBuffer, HBufC8* aMaskBuffer, HBufC8* aAlphaBuffer)
        : iInfo(aInfo)
        , iPalette(aPalette)
        , iPixelBuffer(aPixelBuffer)
        , iMaskBuffer(aMaskBuffer)
        , iAlphaBuffer(aAlphaBuffer)
{
}


/**
 * Destructor
 */
CSwtGrImageData::~CSwtGrImageData()
{
    delete iPalette;
    delete iPixelBuffer;
    delete iMaskBuffer;
    delete iAlphaBuffer;
}


/**
 * Constructs an initialised image data
 *
 * Palette and buffer ownership is transferred.
 */
CSwtGrImageData* CSwtGrImageData::NewL(const TInfo& aInfo, CSwtGrPaletteData* aPalette,
                                       HBufC8* aPixelBuffer, HBufC8* aMaskBuffer, HBufC8* aAlphaBuffer)
{
    ASSERT(aPalette!=NULL);
    ASSERT(aPixelBuffer!=NULL);

    // Not using new(ELeave) so as to allow for cleanup when memory allocation fails
    CSwtGrImageData* self = new CSwtGrImageData(aInfo, aPalette,
            aPixelBuffer, aMaskBuffer, aAlphaBuffer);
    if (self == NULL)
    {
        delete aPalette;
        delete aPixelBuffer;
        delete aMaskBuffer;
        delete aAlphaBuffer;
        User::LeaveNoMemory();
    }

    return self;
}


//
// Virtual methods from MSwtImageData
//

const MSwtImageData::TInfo& CSwtGrImageData::Info() const
{
    return iInfo;
}


const MSwtPaletteData& CSwtGrImageData::Palette() const
{
    return *iPalette;
}


const HBufC8& CSwtGrImageData::PixelBuffer() const
{
    return *iPixelBuffer;
}


const HBufC8* CSwtGrImageData::MaskBuffer() const
{
    return iMaskBuffer;
}


const HBufC8* CSwtGrImageData::AlphaBuffer() const
{
    return iAlphaBuffer;
}
