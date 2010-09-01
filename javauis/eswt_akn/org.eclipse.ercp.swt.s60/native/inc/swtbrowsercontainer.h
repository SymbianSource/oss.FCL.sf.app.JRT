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


#ifndef SWTBROWSERCONTAINER_H
#define SWTBROWSERCONTAINER_H


#include <coecntrl.h>


class CSwtBrowser;


/**
 * CSwtBrowserContainer
 * The container for browser control
 * Browser control requires its container being a window-owing control,
 * and browser control must have the exact same size as its container does.
 * Otherwise there would be coordination problem,
 * e.g. the cursor and scrollbar is not in accord with content of displayed page.
 * CSwtBrowser can not meet this requirement because it can not be a window-owing control,
 * and it can have border and scrollbar area. That is why we have this control there.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserContainer)
        : public CCoeControl
{
public:
    /**
     * Two-phased constructor.
     * @param aParent
     */
    static CSwtBrowserContainer* NewL(CSwtBrowser& aApiProvider);

    /**
     * Decstructor.
     */
    ~CSwtBrowserContainer();

private:
    /**
     * C++ default constructor.
     * @param aApiProvider Api provider.
     */
    CSwtBrowserContainer(CSwtBrowser& aApiProvider);

    /**
     * Symbian 2nd phase constructor.
     * @param aParent Parent control
     */
    void ConstructL(CCoeControl* aParent);

protected:
// From CCoeControl
    void FocusChanged(TDrawNow aDrawNow);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    void SizeChanged();
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt /*aIndex*/) const;

private: // Data
    CSwtBrowser& iApiProvider;
};

#endif // SWTBROWSERCONTAINER_H
