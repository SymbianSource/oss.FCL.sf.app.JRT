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


#ifndef SWTBROWSERVPNAPENGINE_H
#define SWTBROWSERVPNAPENGINE_H


class CVpnApEngine;
class CSwtBrowserVpnApItem;
class CCommsDatabase;


/**
 * CSwtBrowserVpnApEngine
 * @lib eswtapifacade.dll
 */
class CSwtBrowserVpnApEngine
        : public CBase
{
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static  CSwtBrowserVpnApEngine* NewL(CCommsDatabase& aDb);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserVpnApEngine();

    IMPORT_C TBool IsVpnApL(const TUint32 aUid);
    IMPORT_C void VpnDataL(TUint32 aUid, CSwtBrowserVpnApItem& aVpnApItem);

private:
    /**
     * Constructor.
     */
    CSwtBrowserVpnApEngine();

    /**
     * Symbian 2nd phase constructor is private.
     */
    void ConstructL(CCommsDatabase& aDb);

private: // data
    /**
     * Pointer to VpnApEngine
     * Own
     */
    CVpnApEngine* iVpnApEngine;
};

#endif // SWTBROWSERVPNAPENGINE_H
