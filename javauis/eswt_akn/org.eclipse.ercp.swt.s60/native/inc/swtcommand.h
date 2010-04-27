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


#ifndef SWTCOMMAND_H
#define SWTCOMMAND_H


#include "eswtmobileextensions.h"


/**
 * CSwtCommand
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCommand)
        : public CBase
        , public MSwtCommand
{
// Methods
public:
    static CSwtCommand* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    const MSwtControl& aParentControl, const MSwtCommand* aParentCommand,
    TInt aType, TInt aPriority);

private:
    inline CSwtCommand(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                       const MSwtControl& aParentControl, const MSwtCommand* aParentCommand,
                       TInt aType, TInt aPriority);

    ~CSwtCommand();
    void ConstructL();

// From MSwtCommand
public:
    TSwtPeer           Dispose();
    TSwtPeer           JavaPeer() const;
    const MSwtControl& ParentControl() const;
    const MSwtCommand* ParentCommand() const;
    void               SetAcceleratorL(TInt aAccelerator);
    void               SetEnabled(TBool aEnabled);
    void               SetDefaultCommand(TBool aDefault);
    void               SetTextL(const TDesC& aText);
    void               SetImageL(const MSwtImage* aImage);
    void               SetLongLabelL(const TDesC& aText);
    TInt               Accelerator() const;
    TBool              IsEnabled() const;
    TBool              IsDefaultCommand() const;
    const TDesC&       Text() const;
    const MSwtImage*   Image() const;
    const TDesC&       LongLabel() const;
    TInt               Priority() const;
    TInt               Type() const;

// Data
private:
    MSwtDisplay&       iDisplay;
    const TSwtPeer     iPeer;
    const MSwtControl& iParentControl; // The parent control
    const MSwtCommand* iParentCommand; // The parent command (of commandgroup type), not owned, may be NULL
    TInt               iType;
    TInt               iPriority;
    TInt               iAccelerator;
    TBool              iEnabled;
    TBool              iDefault;
    HBufC*             iText;
    HBufC*             iLongLabel;
    const MSwtImage*   iImage;         // Ref counted, may be NULL
};


#endif // SWTCOMMAND_H
