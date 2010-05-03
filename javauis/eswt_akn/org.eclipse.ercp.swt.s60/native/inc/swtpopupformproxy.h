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


#ifndef SWTPOPUPFORMPROXY_H
#define SWTPOPUPFORMPROXY_H


#include <AknUtils.h>


class CSwtPopupFormProxyPrivate;


/**
 * CSwtPopupFormProxy
 * @lib eswtapifacade.dll
 */
class CSwtPopupFormProxy
        : public CBase
{
// Own functions
public:
    IMPORT_C static CSwtPopupFormProxy* NewL(const TDesC& aText, const TDesC& aTitle,
            const TInt aHeadResourceId, const TInt aSpaceResourceId, const TInt aDlgResourceId,
            const TInt aBtnResourceId, const TInt aAniResourceId, const TInt aStyle,
            const TInt aMsgBoxMenuBarResourceId,  const TInt aSwtCmdYes,
            const TInt aSwtCmdNo, const TInt aTimeOut = 0,
            const CEikImage* aImage = NULL, const TInt aImageResourceId = 0);
    virtual ~CSwtPopupFormProxy();
    IMPORT_C TInt RunL();
    IMPORT_C void OfferResourceChangeL(const TInt aType, const TInt aAnimation);

// Own functions
private:
    CSwtPopupFormProxy();
    void ConstructL(const TDesC& aText, const TDesC& aTitle,
                    const TInt aHeadResourceId, const TInt aSpaceResourceId, const TInt aDlgResourceId,
                    const TInt aBtnResourceId, const TInt aAniResourceId, const TInt aStyle,
                    const TInt aMsgBoxMenuBarResourceId, const TInt aSwtCmdYes,
                    const TInt aSwtCmdNo, const TInt aTimeOut,
                    const CEikImage* aImage, const TInt aImageResourceId);

// Data
private:
    CSwtPopupFormProxyPrivate* iPopupForm; // own
    CSwtPopupFormProxyPrivate* iPopupFormNoDelete; // not own
};

#endif // SWTPOPUPFORMPROXY_H
