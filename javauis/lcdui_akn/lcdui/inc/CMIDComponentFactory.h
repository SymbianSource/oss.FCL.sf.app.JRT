/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CMIDCOMPONENTFACTORY_H
#define CMIDCOMPONENTFACTORY_H

#include <e32def.h>
#include <lcdui.h>
#include <coecntrl.h>

class CMIDUIManager;
class CMIDUtils;

/**
 *
 * Concrete implementation for constructing native peers to LCDUI objects
 *
 */

NONSHARABLE_CLASS(CMIDComponentFactory) : public CBase, public MMIDComponentFactory
{
public:
    CMIDComponentFactory();
    void ConstructL(MMIDEnv& aEnv);
    //
    // Classes derived from Displayable
    //
    MMIDDisplayable* CreateDisplayableL();

    MMIDCanvas* CreateCanvasL(MMIDDisplayable& aDisplayable, MMIDComponent::TType aCanvasType);

    MMIDAlert* CreateAlertL(MMIDDisplayable& aDisplayable,MMIDAlert::TAlertType aType,const TDesC& aString,MMIDImage* aImage);
    MMIDForm* CreateFormL(MMIDDisplayable& aDisplayable);

    MMIDList* CreateListL(TInt aType,MMIDDisplayable& aDisplayable,RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray);
    MMIDTextBox* CreateTextBoxL(TInt aConstraints,TInt aMaxSize,const TDesC& aText,MMIDDisplayable& aDisplayable);
    //
    // Classes derived from Item
    //
    MMIDImageItem* CreateImageItemL(const TDesC& aLabel,MMIDImage* aImage,MMIDItem::TLayout aLayout,MMIDItem::TAppearance aAppearance,const TDesC& aAltText);
    MMIDCustomItem* CreateCustomItemL(const TDesC& aLabel);
    MMIDStringItem* CreateStringItemL(const TDesC& aLabel,const TDesC& aText,MMIDItem::TAppearance aAppearance);
    MMIDDateField* CreateDateFieldL(const TDesC& aLabel,MMIDDateField::TFieldType aFieldType);
    MMIDGauge* CreateGaugeL(const TDesC& aLabel,TBool aInteractive,TInt aMaxValue,TInt aInitialValue);
    MMIDTextField* CreateTextFieldL(const TDesC& aLabel,const TDesC& aText,TInt aConstraints,TInt aMaxSize);

    MMIDChoiceGroup* CreateChoiceGroupL(const TDesC& aLabel,TInt aType,RArray<TPtrC>& aStringArray, RArray<MMIDImage*>& aImageArray);
    MMIDSpacer* CreateSpacerL(const TSize& aMinimumSize);
    //
    // Other UI classes
    //
    MMIDFont* CreateFontL(TUint aStyle,TUint aSize,TInt aFace,TBool aIsFreeSizeFont);
    MMIDTicker* CreateTickerL(const TDesC& aText);
    MMIDCommand* CreateCommandL(const TDesC& aShortLabel,const TDesC& aLongLabel,MMIDCommand::TCommandType aCommandType,TInt aPriority,TInt aCommandID);
    //
    // Other methods
    //
    MMIDUtils* CreateUtilsL();

    /**
     * Creates a new text editor component. Note that text editor is current
     * a custom component and not a standard LCDUI UI component.
     *
     * The implementation is part of nokialcdui component and s60lcdui plug-in.
     *
     * The ownership is transferred to the caller.
     *
     * @param aMaxSize The maximum size of the editor's content.
     * @param aWidth The width of the editor in pixels.
     * @param aHeight The height of the editor in pixels.
     * @param aHeightInRows Defines whether the height of the editor should be
     *        represented as in rows or in pixels.
     * @since S60 5.0
     */
    MMIDTextEditor* CreateTextEditorL(
        TInt aMaxSize,
        TInt aWidth,
        TInt aHeight,
        TBool aHeightInRows);

    /**
     * Creates a new canvas graphics item component. Note that canvas
     * graphics item is currently a custom component and not a standard
     * LCDUI UI component.
     *
     * The implementation is part of nokialcdui component and s60lcdui plug-in.
     *
     * The ownership is transferred to the caller.
     *
     * @param aItemPainter Pointer to its painter control.
     * @since S60 5.0
     */
    MMIDCanvasGraphicsItem* CreateCanvasGraphicsItemL(
        MMIDCanvasGraphicsItemPainter* aItemPainter);

    /**
     * Creates a new canvas graphics item painter control.
     *
     * The implementation is part of nokialcdui component and s60lcdui plug-in.
     *
     * The ownership is transferred to the caller.
     *
     * @param aWidth The width of the editor in pixels.
     * @param aHeight The height of the editor in pixels.
     * @since S60 5.0
     */
    MMIDCanvasGraphicsItemPainter* CreateCanvasGraphicsItemPainterL(
        TInt aWidth,
        TInt aHeight);

    void Dispose();
private:
    ~CMIDComponentFactory();

    /**
     * Get iUtils instance. If iUtils is NULL, it creates new one.
     * The ownership is not transferred.
     *
     * @return Shared instance of CMIDUtils.
     * @since S60 5.0
     */
    CMIDUtils* GetUtils();
private:
    MMIDEnv* iEnv;
    CMIDUIManager* iUIManager;
    TBool displayableCreated;

    // Shared instance of CMIDUtils. Other components uses this instance.
    // Those components gets it by GetUtils method. Owned
    CMIDUtils* iUtils;
};

#endif // CMIDCOMPONENTFACTORY_H
