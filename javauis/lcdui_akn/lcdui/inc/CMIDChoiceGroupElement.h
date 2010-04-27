/*
* Copyright (c) 2003-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CMIDChoiceGroupElement encapsulates a ChoiceGroup
*                listbox entry (text, image, selection state and all)
*
*/


#ifndef CMIDCHOICEGROUPELEMENT_H
#define CMIDCHOICEGROUPELEMENT_H

//  INCLUDES
#include <e32base.h>  // e32def, e32std
// using CGulIcon API in several places related to choice group
// and for iIcon memeber
#include <gulicon.h>

//  CONSTANTS

//  MACROS

// Element flags
#define CGEF_SELECTED          0x01
#define CGEF_OWNSICON          0x02


//  DATA TYPES

//  EXTERNAL DATA STRUCTURES

//  FUNCTION PROTOTYPES

//  FORWARD DECLARATIONS

//  CLASS DEFINITIONS
NONSHARABLE_CLASS(CMIDChoiceGroupElement) : public CBase
{
public:
    static CMIDChoiceGroupElement* NewL();

    // Destructor
    virtual ~CMIDChoiceGroupElement();

    // Set element text
    void SetTextL(const TDesC& aText);

    // Get element text
    const TPtrC Text() const
    {
        return iText ? iText->Des() : TPtrC(KNullDesC);
    }

    // Get listbox line ("<i>\t<j>\tText"), where i is an index to
    // the selection icon and j to the element image (given as param)
    const TPtrC LboxText(TInt aImageIndex);

    // Set icon. If owned, an icon is deleted by the element
    void SetIcon(CGulIcon* aIcon, TBool aElementOwnsIcon = ETrue);

    // Set icon from bitmaps. If owned, an icon is deleted by the element
    void SetIconL(
        CFbsBitmap& aBitmap,
        CFbsBitmap* aMask = NULL,
        TBool aElementOwnsIcon = ETrue);

    // Get icon
    CGulIcon* Icon() const
    {
        return iIcon;
    }

    // Set element to be selected (preserve original flags)
    void SetSelected(TBool aSelected = ETrue)
    {
        aSelected ? (iFlags |= CGEF_SELECTED) : (iFlags &= ~CGEF_SELECTED);
    }

    // Element selection state query method
    TBool IsSelected() const
    {
        return (iFlags & CGEF_SELECTED);
    }

    // Get flags
    TInt Flags() const
    {
        return iFlags;
    }

protected:

private:
    CMIDChoiceGroupElement();
private:

    // Type flags
    TInt iFlags;

    // Element text
    HBufC* iText;

    // Listbox line
    HBufC* iLboxText;

    // Icon
    CGulIcon* iIcon;

};

#endif // CMIDCHOICEGROUPELEMENT_H

