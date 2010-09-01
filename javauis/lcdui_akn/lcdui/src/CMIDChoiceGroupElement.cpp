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



//  INCLUDES
#include <fbs.h>
#include <bitdev.h>
#include <gdi.h>

#include "CMIDChoiceGroupElement.h"
#include "CMIDUtils.h"

const TInt KSpaceForNumber    = 6;
const TInt KSpaceForTabulator = 1;

CMIDChoiceGroupElement* CMIDChoiceGroupElement::NewL()
{
    return new(ELeave)CMIDChoiceGroupElement();
}

CMIDChoiceGroupElement::CMIDChoiceGroupElement()
{
}


// Destructor
CMIDChoiceGroupElement::~CMIDChoiceGroupElement()
{
    delete iText;

    if (Flags() & CGEF_OWNSICON)
    {
        delete iIcon;
    }
}


// Set element text
void CMIDChoiceGroupElement::SetTextL(const TDesC& aText)
{
    // Get rid of old text, set to NULL in case allocation fails
    if (iText)
    {
        delete iText;
        iText = NULL;
    }

    // Make a copy of the string
    iText = aText.AllocL();
}


// Retrieve text for the listbox. This means that the image indeces
// are also added into the string and separated by tabs
const TPtrC CMIDChoiceGroupElement::LboxText(TInt aImageIndex)
{
    // Construct a new line with the correct image index
    _LIT(KLboxLineFormat, "%d\t%d\t%S");
    delete iLboxText;

    // Reserve space for number + tab + number + tab + element text
    TInt len = 2 * KSpaceForNumber + 2 * KSpaceForTabulator + (iText ? iText->Length() : 0);
    iLboxText = HBufC::New(len);

    if (!iLboxText)
    {
        return TPtrC(KNullDesC);
    }

    // Write the string into the buffer
    TPtr ptr = iLboxText->Des();
    ptr.Format(
        KLboxLineFormat,
        IsSelected() ? 0 : 1,
        aImageIndex,
        iText);

    return ptr;
}


// Set icon. If owned, an icon is deleted by the element
void CMIDChoiceGroupElement::SetIcon(
    CGulIcon* aIcon,
    TBool aElementOwnsIcon /*= ETrue */)
{
    // Get rid of old icon, if any and if owned
    if ((iIcon) && (Flags() & CGEF_OWNSICON))
    {
        delete iIcon;
    }

    iIcon = aIcon;

    if (aElementOwnsIcon)
    {
        iFlags |= CGEF_OWNSICON;
    }
    else
    {
        iFlags &= ~CGEF_OWNSICON;
    }
}


// Set icon from bitmaps. If owned, an icon is deleted by the element
// We expect that the bitmaps have been created for us and are in the correct format
void CMIDChoiceGroupElement::SetIconL(
    CFbsBitmap& aBitmap,
    CFbsBitmap* aMask /* = NULL */,
    TBool aElementOwnsIcon /* = ETrue */)
{
    CGulIcon* icon = CGulIcon::NewL(&aBitmap, aMask);
    icon->SetBitmapsOwnedExternally(!aElementOwnsIcon);

    SetIcon(icon, aElementOwnsIcon);
}

// End of File
