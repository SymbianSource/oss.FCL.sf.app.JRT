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


#include <gdi.h>
#include "swtgrpalettedata.h"


// ======== MEMBER FUNCTIONS ========


/**
 * C++ constructor
 * Simply initialises the direct / indirect status.
 */
inline CSwtGrPaletteData::CSwtGrPaletteData(TBool aDirect)
        : iIsDirect(aDirect)
{
}

/**
 * Destructor
 */
CSwtGrPaletteData::~CSwtGrPaletteData()
{
    if (!iIsDirect)
        delete iData.iColors;
}

/**
 * Constructs an empty palette.
 * A direct palette is initialised to all zeros; an indirect palette is
 * initialised with <code>NULL</code> data.
 */
CSwtGrPaletteData* CSwtGrPaletteData::NewL(TBool aDirect)
{
    return new(ELeave) CSwtGrPaletteData(aDirect);
}

/**
 * Sets the data of an indirect palette
 * @param aColors The new data to use, ownership is transferred.
 */
void CSwtGrPaletteData::SetIndirectData(CPalette* aColors)
{
    ASSERT(!iIsDirect);

    if (aColors != iData.iColors)
    {
        delete iData.iColors;
        iData.iColors = aColors;
    }
}

/**
 * Sets the data of a direct palette
 */
void CSwtGrPaletteData::SetDirectData(const TDirectData& aData)
{
    ASSERT(iIsDirect);
    iData.iDirect = aData;
}


//
// Virtual methods from MSwtPaletteData
//

TBool CSwtGrPaletteData::IsDirect() const
{
    return iIsDirect;
}

const MSwtPaletteData::TDirectData& CSwtGrPaletteData::DirectData() const
{
    ASSERT(iIsDirect);
    return iData.iDirect;
}

const CPalette* CSwtGrPaletteData::IndirectData() const
{
    ASSERT(!iIsDirect);
    return iData.iColors;
}
