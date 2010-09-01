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


#ifndef SWTBROWSERAPUTILS_H
#define SWTBROWSERAPUTILS_H

class CApUtils;
class CCommsDatabase;

/**
 * CSwtBrowserApUtils
 * @lib eswtapifacade.dll
 */


class CSwtBrowserApUtils
        : public CBase
{
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static  CSwtBrowserApUtils* NewL(CCommsDatabase& aDb);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserApUtils();

    /**
     * Returns in a TBool whether the access point identified
     *
     * @param aUid the access point ID.
     * @return TBool indicating whether an access point with the
     * given ID exists or not.
    */
    IMPORT_C TBool WapApExistsL(TUint32 aUid);

    /**
     * Gets the Iap ID for a given access point ID
     *
     * @param aUid The UID of the access point
     * @return The Iap record ID.
     */
    IMPORT_C TUint32 IapIdFromWapIdL(TUint32 aUid);

    /**
     * Gets the WapId ID for a given Iap ID
     *
     * @param aIapId The Iap Id
     * @return The Wap ID.
     */
    IMPORT_C TUint32 WapIdFromIapIdL(TUint32 aIapId);

    /**
     * Gets bearer type for given WapId
     *
     * @param aWapId WapId ID
     * @return bearer type.
     */
    IMPORT_C TInt BearerTypeL(TUint32 aWapId);

private:

    /**
     * Constructor.
     */
    CSwtBrowserApUtils();

    /**
     * Symbian 2nd phase constructor is private.
     */
    void ConstructL(CCommsDatabase& aDb);



private: // data

    /**
     * Pointer to CApUtils
     * Own
     */
    CApUtils* iApUtils;

};

#endif // SWTBROWSERAPUTILS_H
