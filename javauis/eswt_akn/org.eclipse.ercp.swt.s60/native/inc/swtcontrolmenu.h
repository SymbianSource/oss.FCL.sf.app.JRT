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


#ifndef SWTCONTROLMENU_H
#define SWTCONTROLMENU_H


#include "swtmenu.h"


/**
 * CSwtControlMenu
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtControlMenu)
        : public CSwtMenu
{
// Own methods
public:
    /**
     * Two phase constructor.
     * @param aDisplay
     * @param aPeer
     * @param aStyle
     * @return CSwtControlMenu*
     */
    static CSwtControlMenu* NewL(MSwtDisplay& aDisplay,
    TSwtPeer aPeer, TInt aStyle);

protected:
    /**
     * First phase constructor.
     * @param aDisplay
     * @param aPeer
     * @param aStyle
     */
    CSwtControlMenu(MSwtDisplay& aDisplay,
                    TSwtPeer aPeer, TInt aStyle);

    /**
     * Destructor
     */
    virtual ~CSwtControlMenu();

public:
    /**
     * Command observer setter.
     * @param aObserver Observer.
     */
    void SetObserver(MEikCommandObserver* aObserver);

    /**
     * Command observer getter.
     * @return Observer.
     */
    MEikCommandObserver* Observer() const;

// From MSwtMenu
public:
    void ProcessCommandL(TInt aCommandId, TInt aItemIndex,
                         TBool aOfferToSubMenus = EFalse) const;

// Data
private:
    /**
     * Current command observer, can be NULL.
     * Not own.
     */
    MEikCommandObserver* iObserver;
};

#endif // SWTCONTROLMENU_H
