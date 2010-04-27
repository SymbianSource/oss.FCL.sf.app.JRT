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


#ifndef SWTPALETTEDATA_H
#define SWTPALETTEDATA_H


#include <jni.h>
#include "eswtgraphics.h"


/**
 * CSwtPaletteData
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtPaletteData)
        : public CBase
        , public MSwtPaletteData
{
// Methods
public:
    static CSwtPaletteData* NewL(JNIEnv* aJniEnv, jobject aPaletteData);
    ~CSwtPaletteData();

private:
    inline CSwtPaletteData(TBool aDirect);
    void ConstructL(JNIEnv* aJniEnv, jobject aPaletteData, TBool& aFailed);

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


#endif // SWTPALETTEDATA_H
