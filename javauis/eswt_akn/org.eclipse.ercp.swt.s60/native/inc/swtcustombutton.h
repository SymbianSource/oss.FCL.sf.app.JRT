/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

#ifndef SWTCUSTOMBUTTON_H
#define SWTCUSTOMBUTTON_H


#include <aknbutton.h>


/**
 * CSwtCustomButton
 *
 * This class is meant for customising CAknButton by overriding.
 *
 * @lib eswtapifacade
 */
NONSHARABLE_CLASS(CSwtCustomButton) : public CAknButton
{
    friend class CAknButton;
// Own methods
public:
    /**
     * Two-phased constructors. See CAknButton for details.
     */
    static CSwtCustomButton* NewL(
        CGulIcon* aIcon,
        CGulIcon* aDimmedIcon,
        CGulIcon* aPressedIcon,
        CGulIcon* aHoverIcon,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aButtonFlags,
        const TInt aStateFlags);

    static CSwtCustomButton* NewLC(
        CGulIcon* aIcon,
        CGulIcon* aDimmedIcon,
        CGulIcon* aPressedIcon,
        CGulIcon* aHoverIcon,
        const TDesC& aText,
        const TDesC& aHelpText,
        const TInt aFlags,
        const TInt aStateFlags);

    static CSwtCustomButton* NewL(
        const TDesC& aFilePath,
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
        const TAknsItemID& aId = KAknsIIDNone,
        const TAknsItemID& aDimmedId = KAknsIIDNone,
        const TAknsItemID& aPressedId = KAknsIIDNone,
        const TAknsItemID& aHoverId = KAknsIIDNone);

    static CSwtCustomButton* NewLC(
        const TDesC& aFilePath,
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
        const TAknsItemID& aId = KAknsIIDNone,
        const TAknsItemID& aDimmedId = KAknsIIDNone,
        const TAknsItemID& aPressedId = KAknsIIDNone,
        const TAknsItemID& aHoverId = KAknsIIDNone);

public:
    virtual ~CSwtCustomButton();

// Functions from base class
public:
    CSwtCustomButton(const TInt aFlags);
};

#endif // SWTCUSTOMBUTTON_H
