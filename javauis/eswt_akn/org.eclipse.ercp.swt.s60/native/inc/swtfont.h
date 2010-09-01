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


#ifndef SWTFONT_H
#define SWTFONT_H


#include "eswtgraphics.h"


class CFont;


/**
 * CSwtFont
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtFont)
        : public CBase
        , public MSwtFont
{
public:
    /**
     * Returns a new created font using the specs provided by parameters.
     */
    static CSwtFont* NewL(const MSwtDevice& aDevice, const TDesC& aTypefaceName,
    const TInt aHeightInPoints, const TInt aStyle);
    /**
     * Returns a new created font using the specs provided by aFont.
     * The returned font might not match aFont. For instance if aFont
     * is an Avkon layout font whose size in points is 6.5 then the returned
     * font will be CFont whose size in points is 7 (Java API restriction).
     */
    static CSwtFont* NewL(const MSwtDevice& aDevice, const CFont& aFont);

protected:
    CSwtFont(const MSwtDevice& aDevice, CFont* aFont);
    virtual ~CSwtFont();
    void ConstructL(const CFont& aFont);
    void ConstructL(const TDesC& aTypefaceName, const TInt aHeight, const TInt aStyle);

// From MSwtFont
public:
    void   Dispose();
    const CFont& Font() const;
    TInt   DesignHeightInPoints() const;
    HBufC* GetNameL() const;
    TInt   Style() const;
    TInt   TextWidthInPixels(const TDesC& aText) const;

// Data
private:
    CFont* iFont;               // A handle to the used font. Has to be released when no longer needed.
    const  MSwtDevice& iDevice; // Reference to the device which provides the fonts.
    TBool  iIsFontToRelease;    // True if iFont should be released in the destructor.
    HBufC* iName;               // Name of the desired font set at the construction. Owned.
};



#endif // SWTFONT_H



