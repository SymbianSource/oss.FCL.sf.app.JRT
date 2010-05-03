/*******************************************************************************
 * Copyright (c) 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "swtcustombutton.h"


// ======== MEMBER FUNCTIONS ========

CSwtCustomButton* CSwtCustomButton::NewL(CGulIcon* aIcon,
        CGulIcon* aDimmedIcon,
        CGulIcon* aPressedIcon,
        CGulIcon* aHoverIcon,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags)
{
    CSwtCustomButton* self = NewLC(aIcon, aDimmedIcon, aPressedIcon, aHoverIcon,
                                   aText, aHelpText, aButtonFlags, aStateFlags);
    CleanupStack::Pop(self);
    return self;
}


CSwtCustomButton* CSwtCustomButton::NewLC(CGulIcon* aIcon,
        CGulIcon* aDimmedIcon,
        CGulIcon* aPressedIcon,
        CGulIcon* aHoverIcon,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags)
{
    CSwtCustomButton* self = new(ELeave) CSwtCustomButton(aButtonFlags);
    CleanupStack::PushL(self);
    self->ConstructL(aIcon, aDimmedIcon, aPressedIcon, aHoverIcon, aText,
                     aHelpText, aStateFlags);
    return self;
}


CSwtCustomButton* CSwtCustomButton::NewL(const TDesC& aFilePath,
        const TInt aBmpId,
        const TInt aMaskId,
        const TInt aDimmedBmpId,
        const TInt aDimmedMaskId,
        const TInt aPressedBmpId,
        const TInt aPressedMaskId,
        const TInt aHoverBmpId,
        const TInt aHoverMaskId,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags,
        const TAknsItemID& aId,
        const TAknsItemID& aDimmedId,
        const TAknsItemID& aPressedId,
        const TAknsItemID& aHoverId)
{
    CSwtCustomButton* self = NewLC(aFilePath, aBmpId, aMaskId, aDimmedBmpId,
                                   aDimmedMaskId, aPressedBmpId, aPressedMaskId, aHoverBmpId, aHoverMaskId,
                                   aText, aHelpText, aButtonFlags, aStateFlags, aId, aDimmedId, aPressedId,
                                   aHoverId);
    CleanupStack::Pop(self);
    return self;
}


EXPORT_C CSwtCustomButton* CSwtCustomButton::NewLC(const TDesC& aFilePath,
        const TInt aBmpId,
        const TInt aMaskId,
        const TInt aDimmedBmpId,
        const TInt aDimmedMaskId,
        const TInt aPressedBmpId,
        const TInt aPressedMaskId,
        const TInt aHoverBmpId,
        const TInt aHoverMaskId,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags,
        const TAknsItemID& aId,
        const TAknsItemID& aDimmedId,
        const TAknsItemID& aPressedId,
        const TAknsItemID& aHoverId)
{
    CSwtCustomButton* self = new(ELeave) CSwtCustomButton(aButtonFlags);
    CleanupStack::PushL(self);
    self->ConstructL(aFilePath, aBmpId, aMaskId, aDimmedBmpId, aDimmedMaskId,
                     aPressedBmpId, aPressedMaskId, aHoverBmpId, aHoverMaskId,
                     aText, aHelpText, aStateFlags, aId, aDimmedId, aPressedId, aHoverId);
    return self;
}


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSwtCustomButton::~CSwtCustomButton()
{
    MakeVisible(EFalse);
}


CSwtCustomButton::CSwtCustomButton(const TInt aFlags) : CAknButton(aFlags)
{
}
