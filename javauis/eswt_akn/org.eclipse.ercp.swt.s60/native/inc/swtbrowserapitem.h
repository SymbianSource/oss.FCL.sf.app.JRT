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


#ifndef SWTBROWSERAPITEM_H
#define SWTBROWSERAPITEM_H

class CApAccessPointItem;


/**
 * CSwtBrowserApItem
 * @lib eswtapifacade.dll
 */

class CSwtBrowserApItem
        : public CBase
{
public:

    /**
     * Two-phased constructor.
     */
    IMPORT_C static  CSwtBrowserApItem* NewL();

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserApItem();

    IMPORT_C CApAccessPointItem& ApItem();


private:

    /**
     * Constructor.
     */
    CSwtBrowserApItem();

    /**
     * Symbian 2nd phase constructor is private.
     */
    void ConstructL();



private: // data

    /**
     * Pointer to CApAccessPointItem
     * Own
     */
    CApAccessPointItem* iApItem;

};

#endif // SWTBROWSERAPITEM_H

