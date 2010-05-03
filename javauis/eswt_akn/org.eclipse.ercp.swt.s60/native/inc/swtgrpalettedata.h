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


#ifndef SWTGRPALETTEDATA_H
#define SWTGRPALETTEDATA_H


#include "eswtgraphics.h"


/**
 * CSwtGrPaletteData
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtGrPaletteData)
        : public CBase
        , public MSwtPaletteData
{
// Methods
public:
    static CSwtGrPaletteData* NewL(TBool aDirect);
    ~CSwtGrPaletteData();
    void SetIndirectData(CPalette* aColors);
    void SetDirectData(const TDirectData& aData);
private:
    inline CSwtGrPaletteData(TBool aDirect);

// From MSwtPaletteData
public:
    TBool              IsDirect()     const;
    const TDirectData& DirectData()   const;
    const CPalette*    IndirectData() const;

// Data types
private:
    union TData
    {
        TDirectData iDirect; // The direct palette information
        CPalette*   iColors; // The indirect palette information
    };

// Data
private:
    TBool iIsDirect; // ETrue for a direct palette, EFalse otherwise.
    TData iData;     // The palette data
};


#endif // SWTGRPALETTEDATA_H
