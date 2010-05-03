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
* Description:  ?Description
*
*/

// Using debug macros.
#include <j2me/jdebug.h>

#include <coecntrl.h>
#include <lcdui.h>
//
#include "CMIDDisplayable.h"
#include "CMIDTicker.h"
#include "CMIDForm.h"
// needed for creating alert
#include "CMIDAlert.h"
#include "CMIDCanvas.h"
// needed for creating font
#include "CMIDFont.h"
#include "CMIDItem.h"
#include "CMIDControlItem.h"
// needed for creating image item
#include "CMIDImageItem.h"
// needed for creating string item
#include "CMIDStringItem.h"
// needed for creating date field item
#include "CMIDDateFieldItem.h"
// needed for creating gauge
#include "CMIDGaugeItem.h"
// needed for creating text box control
#include "CMIDTextBoxControl.h"
// needed for creating popup text box control
#include "CMIDTextBoxDialogControl.h"
// needed for creating text field item
#include "CMIDTextFieldItem.h"
// needed for creating utils
#include "CMIDUtils.h"
// for iUIManager member
#include "CMIDUIManager.h"
#include "CMIDCommand.h"
// needed for creating spacer
#include "CMIDSpacer.h"
// needed for creating custom item
#include "CMIDCustomItem.h"
#include "CMIDComponentFactory.h"
// needed for creating list
#include "CMIDList.h"
// needed for creating choice group
#include "CMIDChoiceGroupItem.h"
// needed for creating text editor
#include "CMIDTextEditor.h"
// needed for creating canvas graphics item
#include "CMIDCanvasGraphicsItem.h"
#include "CMIDCanvasGraphicsItemPainter.h"

CMIDComponentFactory::CMIDComponentFactory() : iUtils(NULL)
{
}

CMIDComponentFactory::~CMIDComponentFactory()
{
    displayableCreated = EFalse;
    delete iUIManager;
    if (iUtils)
    {
        iUtils->Dispose();
        iUtils = NULL;
    }
}

/**
 *
 * ComponentFactory implementation
 *
 */
void CMIDComponentFactory::ConstructL(MMIDEnv& aEnv)
{
    iEnv = &aEnv;
    displayableCreated = EFalse;
    iUIManager = CMIDUIManager::NewL(aEnv);
}

MMIDDisplayable* CMIDComponentFactory::CreateDisplayableL()
{
    return CMIDDisplayable::NewL(*iEnv,*iUIManager);
}

MMIDCanvas* CMIDComponentFactory::CreateCanvasL(
    MMIDDisplayable& aDisplayable,MMIDComponent::TType aCanvasType)
{
    CCoeControl& window = static_cast< CMIDDisplayable& >(aDisplayable).ContentWindow();
    displayableCreated = ETrue;
    return CMIDCanvas::NewL(*iEnv, window, aCanvasType);
}

MMIDAlert* CMIDComponentFactory::CreateAlertL(MMIDDisplayable& aDisplayable,MMIDAlert::TAlertType aType,const TDesC& aString,MMIDImage* aImage)
{
    if (!iUIManager->OpenMenuHandlerL()->GetDisplayable() && !displayableCreated)
    {
        iUIManager->OpenDefaultDisplayableL(ETrue);
    }
    return CMIDAlert::NewL(
               *iEnv, aType, aDisplayable, aString, aImage, GetUtils());
}

MMIDForm* CMIDComponentFactory::CreateFormL(MMIDDisplayable& aDisplayable)
{
    displayableCreated = ETrue;
    return CMIDForm::NewL(*iEnv,aDisplayable);
}

MMIDList* CMIDComponentFactory::CreateListL(TInt aType,MMIDDisplayable& aDisplayable,RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray)
{
    displayableCreated = ETrue;
    return CMIDList::NewL(*iEnv,aDisplayable,aType,aStringArray,aImageArray);
}

MMIDTextBox* CMIDComponentFactory::CreateTextBoxL(TInt aConstraints, TInt aMaxSize, const TDesC& aText, MMIDDisplayable& aDisplayable)
{
    if (iEnv->MidletAttributeContainsVal(LcduiMidletAttributes::KAttribUIEnhancement, LcduiMidletAttributeValues::KUIEnhFullScreenTextBox))
    {
        return CMIDTextBoxControl::NewL(aConstraints, aText, aMaxSize, &aDisplayable);
    }
    else
    {
        if (!iUIManager->OpenMenuHandlerL()->GetDisplayable() && !displayableCreated)
        {
            iUIManager->OpenDefaultDisplayableL(ETrue);
        }
        return CMIDTextBoxDialogControl::NewL(aConstraints, aText, aMaxSize, &aDisplayable);
    }
}

MMIDImageItem* CMIDComponentFactory::CreateImageItemL(
    const TDesC& aLabel, MMIDImage* aImage, MMIDItem::TLayout aLayout,
    MMIDItem::TAppearance aAppearance, const TDesC& aAltText)
{
    return CMIDImageItem::NewL(aLabel, aImage, aLayout, aAppearance, aAltText, iUIManager);
}

MMIDCustomItem* CMIDComponentFactory::CreateCustomItemL(const TDesC& aLabel)
{
    return CMIDCustomItem::NewL(*iEnv, aLabel, iUIManager);
}

MMIDStringItem* CMIDComponentFactory::CreateStringItemL(const TDesC& aLabel,const TDesC& aText,MMIDItem::TAppearance aAppearance)
{
    return CMIDStringItem::NewL(aLabel, aText, aAppearance, iUIManager);
}

MMIDDateField* CMIDComponentFactory::CreateDateFieldL(
    const TDesC& aLabel, MMIDDateField::TFieldType aFieldType)
{
    return CMIDDateFieldItem::NewL(aLabel, aFieldType, iUIManager);
}

MMIDGauge* CMIDComponentFactory::CreateGaugeL(
    const TDesC& aLabel, TBool aInteractive, TInt aMaxValue, TInt aInitialValue)
{
    if (aInteractive)
    {
        return CMIDInteractiveGauge::NewL(aLabel, aMaxValue, aInitialValue, iUIManager);
    }
    else
    {
        return CMIDNonInteractiveGauge::NewL(aLabel, aMaxValue, aInitialValue , iUIManager);
    }
}

MMIDTextField* CMIDComponentFactory::CreateTextFieldL(
    const TDesC& aLabel, const TDesC& aText, TInt aConstraints, TInt aMaxSize)
{
    if (!iUIManager->OpenMenuHandlerL()->GetDisplayable())
    {
        iUIManager->OpenDefaultDisplayableL(EFalse);
    }
    return CMIDTextFieldItem::NewL(aLabel, aText, aConstraints, aMaxSize, iUIManager);
}

MMIDChoiceGroup* CMIDComponentFactory::CreateChoiceGroupL(
    const TDesC& aLabel, TInt aType, RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray)
{
    return CMIDChoiceGroupItem::NewL(
               *iEnv, aLabel, aType, aStringArray, aImageArray, iUIManager);
}

MMIDSpacer* CMIDComponentFactory::CreateSpacerL(const TSize& aMinimumSize)
{
    return CMIDSpacer::NewL(aMinimumSize, iUIManager);
}

MMIDFont* CMIDComponentFactory::CreateFontL(
    TUint aStyle, TUint aSize, TInt aFace, TBool aIsFreeSizeFont)
{
    return CMIDFont::NewL(aStyle, aSize, aFace, iUIManager, aIsFreeSizeFont);
}

MMIDTicker* CMIDComponentFactory::CreateTickerL(const TDesC& aText)
{
    return CMIDTicker::NewL(aText);
}

MMIDCommand* CMIDComponentFactory::CreateCommandL(
    const TDesC& aShortLabel, const TDesC& aLongLabel,
    MMIDCommand::TCommandType aCommandType, TInt aPriority, TInt aCommandID)
{
    return CMIDCommand::NewL(aShortLabel,aLongLabel,aCommandType,aPriority,aCommandID);
}

MMIDUtils* CMIDComponentFactory::CreateUtilsL()
{
    return CMIDUtils::NewL(*iEnv, iUIManager);
}

// ---------------------------------------------------------------------------
// CMIDComponentFactory::CreateTextEditorL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
MMIDTextEditor* CMIDComponentFactory::CreateTextEditorL(
    TInt aMaxSize,
    TInt aWidth,
    TInt aHeight,
    TBool aHeightInRows)
{
    DEBUG("CMIDComponentFactory::CreateTextEditorL +");

    CMIDTextEditor::TCtorParams params;

    params.iMaxSize = aMaxSize;
    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iHeightInRows = aHeightInRows;
    params.iUtils = GetUtils();

    MMIDTextEditor* editor = CMIDTextEditor::NewL(params);

    DEBUG("CMIDComponentFactory::CreateTextEditorL -");

    return editor;
}

// ---------------------------------------------------------------------------
// CMIDComponentFactory::CreateCanvasGraphicsItemL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
MMIDCanvasGraphicsItem* CMIDComponentFactory::CreateCanvasGraphicsItemL(
    MMIDCanvasGraphicsItemPainter* aItemPainter)
{
    DEBUG("CMIDComponentFactory::CreateCanvasGraphicsItemL +");

    CMIDCanvasGraphicsItem::TCtorParams params;

    params.iPainterHandle =
        static_cast<CMIDCanvasGraphicsItemPainter*>(aItemPainter);
    params.iUtils = GetUtils();

    MMIDCanvasGraphicsItem* item = CMIDCanvasGraphicsItem::NewL(params);

    DEBUG("CMIDComponentFactory::CreateCanvasGraphicsItemL -");

    return item;
}

// ---------------------------------------------------------------------------
// CMIDComponentFactory::CreateCanvasGraphicsItemPainterL
// (other items are commented in the header file)
// ---------------------------------------------------------------------------
//
MMIDCanvasGraphicsItemPainter*
CMIDComponentFactory::CreateCanvasGraphicsItemPainterL(
    TInt aWidth,
    TInt aHeight)
{
    DEBUG("CMIDComponentFactory::CreateCanvasGraphicsItemPainterL +");

    CMIDCanvasGraphicsItemPainter::TCtorParams params;

    params.iWidth = aWidth;
    params.iHeight = aHeight;
    params.iEnv = iEnv;

    MMIDCanvasGraphicsItemPainter* itemPainter =
        CMIDCanvasGraphicsItemPainter::NewL(params);

    DEBUG("CMIDComponentFactory::CreateCanvasGraphicsItemPainterL -");

    return itemPainter;
}

void CMIDComponentFactory::Dispose()
{
    delete this;
}

CMIDUtils* CMIDComponentFactory::GetUtils()
{
    // If iUtils is not initialized, this method will initialized it.
    if (!iUtils)
    {

        TRAPD(err, iUtils = static_cast<CMIDUtils*>(CreateUtilsL()));
        if (err != KErrNone)
        {
            DEBUG_INT("CMIDComponentFactory::GetUtils - error %d", err);
            iUtils = NULL;
        }

    }
    // Return shared instance of CMIDUtils: iUtils.
    return iUtils;
}

// End of File
