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
#include "swtpalettedata.h"
#include "javalocalref.h"
#include "utils.h"


// ======== MEMBER FUNCTIONS ========


/**
 * C++ constructor
 * Simply initialises the direct / indirect status.
 */
inline CSwtPaletteData::CSwtPaletteData(TBool aDirect)
        : iIsDirect(aDirect)
{
}

/**
 * Destructor
 */
CSwtPaletteData::~CSwtPaletteData()
{
    if (!iIsDirect)
        delete iData.iColors;
}

/**
 *
 */
void CSwtPaletteData::ConstructL(JNIEnv* aJniEnv, jobject aPaletteData, TBool& aFailed)
{
    // Get the class object
    RJavaLocalRef<jclass> clazz(aJniEnv, aJniEnv->GetObjectClass(aPaletteData));
    if (!clazz)
    {
        aFailed = ETrue;
        return;
    }
    CleanupClosePushL(clazz);

    iIsDirect = GetBooleanField(aJniEnv, aPaletteData, clazz, "isDirect", aFailed);

    if (iIsDirect)         // Direct image, read colour masks and shifts
    {
        iData.iDirect.iRedMask    = GetIntField(aJniEnv, aPaletteData, clazz, "redMask",    aFailed);
        iData.iDirect.iGreenMask  = GetIntField(aJniEnv, aPaletteData, clazz, "greenMask",  aFailed);
        iData.iDirect.iBlueMask   = GetIntField(aJniEnv, aPaletteData, clazz, "blueMask",   aFailed);
        iData.iDirect.iRedShift   = GetIntField(aJniEnv, aPaletteData, clazz, "redShift",   aFailed);
        iData.iDirect.iGreenShift = GetIntField(aJniEnv, aPaletteData, clazz, "greenShift", aFailed);
        iData.iDirect.iBlueShift  = GetIntField(aJniEnv, aPaletteData, clazz, "blueShift",  aFailed);
    }
    else                   // Paletted image, read the palette's colours
    {
        if (!aFailed)
        {
            jobject objField = GetObjectField(aJniEnv, aPaletteData, clazz, "colors", "[Lorg/eclipse/swt/graphics/RGB;", aFailed);
            RJavaLocalRef<jobjectArray> colors(aJniEnv, static_cast<jobjectArray>(objField));
            if (colors)
            {
                CleanupClosePushL(colors);

                // Allocate the palette object
                TInt count = aJniEnv->GetArrayLength(colors);
                iData.iColors = CPalette::NewL(count);

                // Read the colour values
                jclass rgbClass = aJniEnv->FindClass("org/eclipse/swt/graphics/RGB");
                if (rgbClass == NULL)
                    aFailed = ETrue;
                else
                {
                    for (TInt i=0; i<count; ++i)
                    {
                        jobject col = aJniEnv->GetObjectArrayElement(colors, i);
                        TInt red   = GetIntField(aJniEnv, col, rgbClass, "red",   aFailed);
                        TInt green = GetIntField(aJniEnv, col, rgbClass, "green", aFailed);
                        TInt blue  = GetIntField(aJniEnv, col, rgbClass, "blue",  aFailed);
                        iData.iColors->SetEntry(i, TRgb(red,green,blue));
                        aJniEnv->DeleteLocalRef(col);
                    }
                    aJniEnv->DeleteLocalRef(rgbClass);
                }

                CleanupStack::PopAndDestroy(); // colors
            }
        }
    }

    CleanupStack::PopAndDestroy(); // clazz
}

/**
 * Constructs a native copy of the provided Java palette object.
 */
CSwtPaletteData* CSwtPaletteData::NewL(JNIEnv* aJniEnv, jobject aPaletteData)
{
    ASSERT(aJniEnv!=NULL);
    ASSERT(aPaletteData!=NULL);

    TBool javaFailure = EFalse;

    CSwtPaletteData* self = new(ELeave) CSwtPaletteData(EFalse); // The value passed in is of no importance
    CleanupStack::PushL(self);
    self->ConstructL(aJniEnv, aPaletteData, javaFailure);
    CleanupStack::Pop(self);

    if (javaFailure)
    {
        delete self;
        self = NULL;
    }

    return self;
}

//
// Virtual methods from MSwtPaletteData
//

TBool CSwtPaletteData::IsDirect() const
{
    return iIsDirect;
}

const MSwtPaletteData::TDirectData& CSwtPaletteData::DirectData() const
{
    ASSERT(iIsDirect);
    return iData.iDirect;
}

const CPalette* CSwtPaletteData::IndirectData() const
{
    ASSERT(!iIsDirect);
    return iData.iColors;
}
