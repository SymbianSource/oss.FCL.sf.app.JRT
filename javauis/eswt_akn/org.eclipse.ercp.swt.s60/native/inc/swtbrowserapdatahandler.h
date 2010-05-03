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



#ifndef SWTBROWSERAPDATAHANDLER_H
#define SWTBROWSERAPDATAHANDLER_H

class CApDataHandler;


/**
 * CSwtBrowserApDataHandler
 * @lib eswtapifacade.dll
 */


class CSwtBrowserApDataHandler
        : public CBase
{
public:

    /**
     * Two-phased constructor.
     *
     * @param aDb a reference to a CCommsDataBase object.
     * @return The constructed CApDataHandler.
     */
    IMPORT_C static CSwtBrowserApDataHandler* NewL(CCommsDatabase& aDb);

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserApDataHandler();


    /**
     * Reads up one access point's data from the database.
     *
     * @param aUid The Uid of the access point to be read
     * @param aApItem A reference to a CApAccessPointItem.
     * @return void.
     */
    IMPORT_C void AccessPointDataL(TUint32 aUid,
                                   CApAccessPointItem& aApItem);

private:

    /**
     * Constructor.
     */
    CSwtBrowserApDataHandler();

    /**
     * Symbian 2nd phase constructor is private.
     * @param aDb a reference to a CCommsDataBase object.
     * @return void.
     */
    void ConstructL(CCommsDatabase& aDb);



private: // data

    /**
     * Pointer to ApDataHandler
     * Own
     */
    CApDataHandler* iApDataHandler;
};

#endif // SWTBROWSERAPDATAHANDLER_H

