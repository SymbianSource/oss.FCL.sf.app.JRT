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


#ifndef SWTBROWSERVPNAPITEM_H
#define SWTBROWSERVPNAPITEM_H


class CVpnApItem;


/**
 * CSwtBrowserVpnApItem
 * @lib eswtapifacade.dll
 */
class CSwtBrowserVpnApItem
        : public CBase
{
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static  CSwtBrowserVpnApItem* NewL();

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserVpnApItem();
    CVpnApItem& VpnApItem();
    IMPORT_C TInt ReadUint(const TUint aColumn, TUint32& aValue);
    IMPORT_C TInt ReadVpnApUid(TUint32& aValue);
    IMPORT_C TInt ReadRealIapId(TUint32& aValue);

private:

    /**
     * Constructor.
     */
    CSwtBrowserVpnApItem();

    /**
     * Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // data

    /**
     * Pointer to VpnApItem
     * Own
     */
    CVpnApItem* iVpnApItem;
};

#endif // SWTBROWSERVPNAPITEM_H
