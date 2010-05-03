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


#ifndef SWTTABLECOLUMN_H
#define SWTTABLECOLUMN_H


#include <e32base.h>
#include <coecobs.h>
#include "eswtwidgetsexpanded.h"


/**
 *  This class implements the native peer of TableColumn.
 *  @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtTableColumn)
        : public CBase
        , public MSwtTableColumn
        , public MCoeControlObserver
{

public:

    /**
     * Two-phased constructor.
     * @param aPeer     The Java object associated with this C++ object.
     * @param aParent   Reference to the parent table.
     * @param aStyle    The style of the column.
     * @param aIndex    The index of the new column in the table.
     */
    static CSwtTableColumn* NewL(TSwtPeer aPeer, MSwtTable& aParent, TInt aStyle, TInt aIndex);

    /**
     * Destructor.
     */
    virtual ~CSwtTableColumn();

    /**
     * Returns the Java peer object of an instance of this class.
     * @return The native peer object.
     */
    TSwtPeer JavaPeer() const;

// From MCoeControlObserver

    /**
     * For observing the column header buttons.
     */
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

// From MSwtTableColumn

    TSwtPeer        Dispose();
    TInt            Alignment() const;
    void            Pack();
    void            SetAlignment(TInt aAlignment);
    void            SetWidth(TInt aWidth);
    void            SetTextL(const TDesC& aText);
    const TDesC*    GetText() const;
    void            SetColumnImage(const MSwtImage* aImage);
    TInt            Width() const;
    TBool           GetResizable() const;
    void            SetResizable(TBool aResizable);
    TInt            GetStyle() const;

private:

    /**
     * The constructor.
     * @param aPeer     The Java peer object.
     * @param aParent   The parent table that contains this column.
     * @param aStyle    The style of the column.
     */
    CSwtTableColumn(TSwtPeer aPeer, MSwtTable& aParent, TInt aStyle);

    /**
     * 2nd phase constructor.
     */
    void ConstructL(TInt aIndex);

private: // data

    /**
     * The Java peer object
     */
    const TSwtPeer iPeer;

    /**
     * Parent table
     */
    MSwtTable& iParent;

    /**
     * The style of the column
     */
    TInt iStyle;

    /**
     * If the column is resizable.
     */
    TBool iResizable;
};

#endif // SWTTABLECOLUMN_H
