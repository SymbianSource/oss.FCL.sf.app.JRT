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


#ifndef SWTCLIPBOARD_H
#define SWTCLIPBOARD_H


#include <e32base.h>
#include "eswtwidgetsexpanded.h"


class CClipboard;


const TUid KSwtWidgetsExpandedUid = {0x101FD68A};


/**
 * CSwtClipboard
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtClipboard)
        : public CBase
        , public MSwtClipboard
{
public:
    /**
     * The 1st and 2nd phase constructor wrapper
     * @param aPeer The peer to its java counterpart
     * @return CSwtClipboard*  A pointer to the  created CSwtBroser object
     */
    static CSwtClipboard* NewL(TSwtPeer aPeer);

    /**
     * Descontructor.
     */
    virtual ~CSwtClipboard();

// From MSwtClipboard
    TSwtPeer Dispose();
    void     ClearL();
    void     GetAvailableTypesL(RArray<TInt>& aTypes) const;
    HBufC8*  GetByteContentsL(TInt aDataId) const;
    HBufC*   GetTextContentsL() const;
    void     SetByteContentL(const TDesC8& aPtr, TInt aJavaType);
    void     SetTextContentL(const TDesC& aPtr);
    TAny*    GetContentsL(TInt aTypeId) const;

private:
    /**
    * Contructor.
    * @param aPeer The peer to its java counterpart
    */
    CSwtClipboard(TSwtPeer aPeer);

    /**
    * Second phase constructor
    */
    void ConstructL();

    /**
     * Convert java data type to the corresponding Symbian TUid
     * @since S60 v3.0
     * @param aJavaType Java type
     * @return Symbian TUid
     */
    TUid ConvertJavaToNativeType(TInt aJavaType) const;

private: // Data
    /**
    * The java peer object
    */
    const TSwtPeer iPeer;

    CEikonEnv* iCoeEnv;
};


#endif // SWTCLIPBOARD_H
