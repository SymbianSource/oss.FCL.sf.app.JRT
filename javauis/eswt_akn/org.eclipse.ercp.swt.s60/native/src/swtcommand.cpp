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


#include "swtcommand.h"


// ======== MEMBER FUNCTIONS ========


inline CSwtCommand::CSwtCommand(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                const MSwtControl& aParentControl, const MSwtCommand* aParentCommand,
                                TInt aType, TInt aPriority)
        : iDisplay(aDisplay)
        , iPeer(aPeer)
        , iParentControl(aParentControl)
        , iParentCommand(aParentCommand)
        , iType(aType)
        , iPriority(aPriority)
        , iEnabled(ETrue)
{
}

CSwtCommand::~CSwtCommand()
{
    delete iText;
    delete iLongLabel;

    if (iImage != NULL)
        iImage->RemoveRef();

#ifdef _lint
    iParentCommand = NULL;
    iImage = NULL;
#endif
    //lint -esym(1540, CSwtCommand::iPeer)
}

void CSwtCommand::ConstructL()
{
    iParentControl.GetShell().InstallCba();
    
    // Notify Command arranger (must be done at the end of ConstructL,
    // when Command is created for sure)
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    ASSERT(commandArranger); // Mobile Extensions must have been loaded
    commandArranger->CommandAddedL(*this);
} //lint !e1762

CSwtCommand* CSwtCommand::NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                               const MSwtControl& aParentControl, const MSwtCommand* aParentCommand,
                               TInt aType, TInt aPriority)
{
    CSwtCommand* self = new(ELeave) CSwtCommand(aDisplay, aPeer,
            aParentControl, aParentCommand, aType, aPriority);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

TSwtPeer CSwtCommand::Dispose()
{
    // Notify Command arranger
    // Done here rather than in destructor because command may go through
    // destruction before being added to arranger
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    ASSERT(commandArranger); // Mobile Extensions must have been loaded
    commandArranger->CommandRemoved(*this);

    // Actual dispose
    TSwtPeer peer(JavaPeer());
    delete this;
    return peer;
}

TSwtPeer CSwtCommand::JavaPeer() const
{
    return iPeer;
} //lint !e1763

const MSwtControl& CSwtCommand::ParentControl() const
{
    return iParentControl;
}

const MSwtCommand* CSwtCommand::ParentCommand() const
{
    return iParentCommand;
}

void CSwtCommand::SetAcceleratorL(TInt aAccelerator)
{
    iAccelerator = aAccelerator;

    // Notify Command arranger
//  MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
//  ASSERT(commandArranger); // Mobile Extensions must have been loaded
}

void CSwtCommand::SetEnabled(TBool aEnabled)
{
    iEnabled = aEnabled;

    // Notify Command arranger
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    ASSERT(commandArranger); // Mobile Extensions must have been loaded
    commandArranger->CommandEnablingUpdated(*this);
}

void CSwtCommand::SetDefaultCommand(TBool aDefault)
{
    iDefault = aDefault;

    // Notify Command arranger
    if (aDefault)
    {
        MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
        ASSERT(commandArranger); // Mobile Extensions must have been loaded
        commandArranger->CommandSetAsDefault(*this);
    }
}

void CSwtCommand::SetTextL(const TDesC& aText)
{
    if (iText)
    {
        delete iText;
        iText = NULL;
    }

    iText = aText.AllocL();

    // Notify Command arranger
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    ASSERT(commandArranger); // Mobile Extensions must have been loaded
    commandArranger->CommandContentUpdated(*this);
}

void CSwtCommand::SetImageL(const MSwtImage* aImage)
{
    /* Here are ideas for CommandArranger
    if (aImage != NULL)
        {
        // Beware, ownership of CFbsBitmap is transferred
        // (at the end, so use cleanupstack in case it leaves)
        // => create a copy!
        aImage->Bitmap()
        aImage->MaskBitmap() // may it be null (in MSwtImage and in nativesymbiancommand)?
        }
    else
        {
        }
    */

    if (iImage != NULL)
        iImage->RemoveRef();
    iImage = aImage;
    if (iImage != NULL)
        iImage->AddRef();

    // Notify Command arranger
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    ASSERT(commandArranger); // Mobile Extensions must have been loaded
    commandArranger->CommandContentUpdated(*this);
}

void CSwtCommand::SetLongLabelL(const TDesC& aText)
{
    if (iLongLabel)
    {
        delete iLongLabel;
        iLongLabel = NULL;
    }

    iLongLabel = aText.AllocL();

    // Notify Command arranger
    MSwtCommandArranger* commandArranger = iDisplay.CommandArranger();
    ASSERT(commandArranger); // Mobile Extensions must have been loaded
    commandArranger->CommandContentUpdated(*this);
}

TInt CSwtCommand::Accelerator() const
{
    return iAccelerator;
}

TBool CSwtCommand::IsEnabled() const
{
    return iEnabled;
}

TBool CSwtCommand::IsDefaultCommand() const
{
    return iDefault;
}

const TDesC& CSwtCommand::Text() const
{
    if (iText != NULL)
    {
        return *iText;
    }
    else
    {
        return KNullDesC;
    }
}

const MSwtImage* CSwtCommand::Image() const
{
    return iImage;
}

const TDesC& CSwtCommand::LongLabel() const
{
    if (iLongLabel != NULL)
    {
        return *iLongLabel;
    }
    else
    {
        return KNullDesC;
    }
}

TInt CSwtCommand::Priority() const
{
    return iPriority;
}

TInt CSwtCommand::Type() const
{
    return iType;
}
