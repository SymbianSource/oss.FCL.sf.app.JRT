/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  General functions
*
*/

#ifndef M2GUTILS_H
#define M2GUTILS_H

// INCLUDE FILES
#include <gdi.h> // DisplayMode, Draw, BitBlt
#include <fbs.h> // CFbsBitmap
#include "M2GGeneral.h"
#include <QPixmap>
#include <jni.h>
M2G_NS_START

// CONSTANTS

// DATA TYPES

// MACROS

// FORWARD DECLARATIONS
class CFbsBitmapDevice;
class CFbsBitGc;

// FUNCTION PROTOTYPES

// CLASS DECLARATION


//For UI Thread Execution.
#include "CSynchronization.h"

#define M2G_DO_LOCK CSynchronization::InstanceL()->Lock();


//TODO Have to Raise Exception in case we find any error.
#define M2G_DO_UNLOCK(aEnv) {\
                    TInt errorCode = CSynchronization::InstanceL()->GetErrorCode();\
                    CSynchronization::InstanceL()->Unlock();\
                    }\

/**
 * @class M2GBitmapUtils
 * @brief Static methods for alpha blending and bitmaps
 */

class M2GBitmapUtils
{
public: // STATIC METHODS
    /**
     * Blits two bitmap
     * @since Series S60 3.0
     * @param aTarget Target bitmap.
     * @param aSource Source bitmap.
     * @param aPoint Position for the top left corner of the bitmap.
     * @param aRect  Rectangle defining the piece of the bitmap to be drawn.
     * @param aSourceMask Mask.
     * @return M2G_OK if ok
     */
    static TInt BitBlt(CFbsBitmap& aTarget,
                       const CFbsBitmap& aSource,
                       const TPoint& aPoint,
                       const TRect* aRect = NULL,
                       const CFbsBitmap* aSourceMask = NULL);

    static TInt BitBlt(CBitmapContext& aTargetContext,
                       const CFbsBitmap& aSource,
                       const TPoint& aPoint,
                       const TRect* aRect,
                       const CFbsBitmap* aSourceMask,
                       /*MSwtClient* aClientHandle,*/
                       TBool aUseNativeClear = EFalse);
    
    static TInt BitQBlt(QImage& aTargetQimage,
                                const QImage& aSourceQimage,
                                const TPoint& aPoint,
                                const TRect* aRect,
                                const CFbsBitmap* aSourceMask);
    
    /**
     * Checks if two bitmap are equal.
     * @since Series S60 3.0
     * @param aLhs Left side bitmap
     * @param aRhs Right side bitmap
     * @return ETrue if bitmaps are equal.
     */
    inline static TBool Equals(const CFbsBitmap& aLhs, const CFbsBitmap& aRhs)
    {
        return (aLhs.SizeInPixels() == aRhs.SizeInPixels());
    }

    /**
     * Checks if a bitmap has same values as given values.
     * @since Series S60 3.0
     * @param aBitmap Bitmap
     * @param aSz Size in pixels.
     * @param aMode Display mode.
     * @return ETrue if bitmaps are equal.
     */
    inline static TBool Equals(
        const CFbsBitmap& aBitmap,
        const TSize& aSz,
        const TDisplayMode* aMode)
    {
        return (((aBitmap.SizeInPixels() == aSz) &&
                 ((aMode == NULL) || (aBitmap.DisplayMode() == *aMode))
                ) ? ETrue : EFalse);
    }

    /**
     * Return byte per pixel
     * @since Series S60 3.1
     * @param aBitmap
     * @param aRhs Right side bitmap
     * @return ETrue if bitmaps are equal.
     */
    inline static TInt BytesPerPixel(const CFbsBitmap& aBitmap)
    {
        switch (aBitmap.DisplayMode())
        {
        case EGray256 :
        case EColor256 :
            return 1;
        case EColor64K :
            return 2;
        case EColor16M :
        case EColor16MU :
        case EColor16MA :
            return 4;
        default :
            return 0;
        }
    }

};


//-----------------------------------------------------------------------------
// Creating integer 'handles' from C++ objects for referencing them inside Java
// The shift garauntees a positive integer, so object creation native methods
// can overload the return value to be a handle or an error code
//
// Unhanding the integer requires the destination type to be known, so is
// implemented as a template function, it should be invoked as
//
//      CXyz* xyz=JavaUnhand(aHandle);
//
const TInt KJavaHandleShift=2;

inline TInt JavaMakeHandle(const TAny* aObject)
{
    return reinterpret_cast<TUint>(aObject)>>KJavaHandleShift;
}

template <typename T>
inline T* JavaUnhand(TInt aHandle)
{
    return reinterpret_cast<T*>(aHandle<<KJavaHandleShift);
}

//----------------------------------------------------------------------------
/* Takes an EPOC string and returns a Java JNI string */
inline jstring CreateJavaString(JNIEnv& aJni, const TDesC16& aString)
{
    const jchar* stringPtr = aString.Ptr();
    const jsize stringLength = aString.Length();
    jstring jniString = aJni.NewString(stringPtr, stringLength);
    return jniString;
}



/**
 * @class TM2GRenderRect
 * @brief Class for handling rendering rectangle
 */
class TM2GRenderRect : public TRect
{
private:
    // NOTE ELength should be always the last one and indexing
    // should start from 0
    enum TArrayIndexes
    {
        EAnchorX = 0,
        EAnchorY = 1,
        EClipX = 2,
        EClipY = 3,
        EClipW = 4,
        EClipH = 5,
        ELength
    };

public: // METHODS
    /**
     * Ctor
     * @since Series S60 3.0
     * @param aAnchorX X anchor
     * @param aAnchorY Y anchor
     * @param aClipX Clip x
     * @param aClipY Clip y
     * @param aClipWidth Clip width
     * @param aClipHeight Clip height
     */
    TM2GRenderRect(
        TInt aAnchorX, TInt aAnchorY,
        TInt aClipX, TInt aClipY,
        TInt aClipW, TInt aClipH);

    /**
     * Ctor
     * @since Series S60 3.0
     * @param aDimensions Dimensions. @see TArrayIndexes.
     * @param aLength Array length
     */
    TM2GRenderRect(TInt* aDimensions, TInt aLength);

    /**
     * Copy ctor
     * @since Series S60 3.0
     * @param aRd
     */
    TM2GRenderRect(const TM2GRenderRect& aRd);

    /**
     * Assignment operator
     * @since Series S60 3.0
     * @param aRd
     * @return TM2GRenderRect
     */
    TM2GRenderRect& operator=(const TM2GRenderRect& aRd);

    /**
     * Dtor
     * @since Series S60 3.0
     */
    virtual ~TM2GRenderRect();

    /**
     * Returns x anchor
     * @since Series S60 3.0
     * @return X anchor
     */
    inline TInt GetAnchorX()
    {
        return iAnchorX;
    }

    /**
     * Returns y anchor
     * @since Series S60 3.0
     * @return Y anchor
     */
    inline TInt GetAnchorY()
    {
        return iAnchorY;
    }

    /**
     * Returns clip height
     * @since Series S60 3.0
     * @return Clip height
     */
    inline TInt GetClipH()
    {
        return Height();
    }

    /**
     * Returns x clip
     * @since Series S60 3.0
     * @return X clip
     */
    inline TInt GetClipX()
    {
        return iTl.iX;
    }

    /**
     * Returns y clip
     * @since Series S60 3.0
     * @return Y clip
     */
    inline TInt GetClipY()
    {
        return iTl.iY;
    }

    /**
     * Returns clip width
     * @since Series S60 3.0
     * @return Clip width
     */
    inline TInt GetClipW()
    {
        return Width();
    }

    /**
     * Sets x anchor
     * @since Series S60 3.0
     * @param aX X anchor
     */
    inline void SetAnchorX(TInt aX)
    {
        iAnchorX = aX;
    }

    /**
     * Sets y anchor
     * @since Series S60 3.0
     * @param aY Y anchor
     */
    inline void SetAnchorY(TInt aY)
    {
        iAnchorY = aY;
    }

    /**
     * Sets clip height
     * @since Series S60 3.0
     * @param aH Clip height
     */
    inline void SetClipH(TInt aH)
    {
        SetHeight(aH);
    }

    /**
     * Sets x clip
     * @since Series S60 3.0
     * @param aX X clip
     */
    inline void SetClipX(TInt aX)
    {
        iTl.iX = aX;
    }

    /**
     * Sets y clip
     * @since Series S60 3.0
     * @param aY Y clip
     */
    inline void SetClipY(TInt aY)
    {
        iTl.iY = aY;
    }

    /**
     * Sets clip width
     * @since Series S60 3.0
     * @param aW Clip width
     */
    inline void SetClipW(TInt aW)
    {
        SetWidth(aW);
    }

public: // STATIC METHODS
    /**
     * Gets region size
     * @since Series S60 3.0
     * @param aRect Render dimensions
     * @param aSz Source surface size
     * @return Region size to be paint
     */
    static TSize GetRegionSizeInPixels(
        TM2GRenderRect& aRect,
        const TSize& aSz);

public: // VARIABLES
    TInt iAnchorX;
    TInt iAnchorY;
};


/**
 * @class TM2GBitmapLock
 * @brief Bitmap autolocker
 */
class TM2GBitmapLock
{
public: // METHODS
    /**
     * Ctor
     * @param aBitmap Bitmap
     * @param aLock If ETrue then bitmap is locked by ctor.
     */
    TM2GBitmapLock(const CFbsBitmap* aBitmap, TBool aLock = ETrue);

    /**
     * Dtor
     * @since Series S60 3.0
     */
    virtual ~TM2GBitmapLock();

    /**
     * Locks bitmap heap
     * @since Series S60 3.0
     */
    void Lock();

    /**
     * Unlocks bitmap heap
     * @since Series S60 3.0
     */
    void Unlock();

protected: // METHODS
    /**
     * Copy ctor
     * @since Series S60 3.0
     * @param aRd
     */
    TM2GBitmapLock(const TM2GBitmapLock& aRd);

    /**
     * Assignment operator
     * @since Series S60 3.0
     * @param aRd
     * @return TM2GBitmapLock
     */
    TM2GBitmapLock& operator=(const TM2GBitmapLock& aRd);

public: // VARIABLES
    const CFbsBitmap* iBitmap;
    TBool iIsLocked;
};
class TSWTBitBlt/*: public MSwtFunctor*/
{
public:
    TSWTBitBlt(CBitmapContext& aTargetContext,
               const TPoint& aPoint,
               const CFbsBitmap* aBitmap,
               const TRect* aSourceRect,
               const CFbsBitmap* aMaskBitmap,
               TBool aUseNativeClear);

    void operator()() const;
    CBitmapContext& iTargetContext;
    const TPoint iPoint;
    const CFbsBitmap* iBitmap;
    const TRect* iRect;
    const CFbsBitmap* iMaskBitmap;
    TBool iUseNativeClear;
};
M2G_NS_END


#endif // M2GUTILS_H
