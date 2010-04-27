/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: An interface for the toolkits for implementing
*              a child.
*
*/

#ifndef COREUIAPPUICHILD_H
#define COREUIAPPUICHILD_H

#include <aknappui.h>

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

/**
 * CoreAppUiChild is an interface that the toolkit must implement in order
 * to receive the window server events. The CoreUi implements the CAknAppUi
 * which will receive all the events from the window server. The toolkit is
 * able to receive the window server events propagated by the CoreUI by
 * setting it as a child by using CoreUiAvkonAppUi::setLcduiChild or
 * CoreUiAvkonAppUi::setEswtChild method.
 */
NONSHARABLE_CLASS(CoreAppUiChild)
{
public:
    /**
     * CoreUiAvkon default constructor.
     */
    CoreAppUiChild() {}

    /**
     * CoreUiAvkon destructor.
     */
    virtual ~CoreAppUiChild() {}

    /**
     * @see CEikAppUi::HandleCommandL.
     */
    virtual void HandleCommandL(TInt aCommand) = 0;

    /**
     * @see CEikAppUi::HandleForegroundEventL.
     */
    virtual void HandleForegroundEventL(TBool aForeground) = 0;

    /**
     * @see CEikAppUi::HandleSwitchOnEventL.
     */
    virtual void HandleSwitchOnEventL(CCoeControl* aDestination) = 0;

    /**
     * @see CEikAppUi::HandleResourceChangeL.
     */
    virtual void HandleResourceChangeL(TInt aType) = 0;

    /**
     * @see CEikAppUi::HandleApplicationSpecificEventL.
     */
    virtual void HandleApplicationSpecificEventL(TInt aType,const TWsEvent& aEvent) = 0;

    /**
     * @see CEikAppUi::HandleWsEventL.
     * The only difference to the original method is that the toolkit returns
     * a boolean info to the CoreUi whether it should call the
     * CAknAppUi::HandleWsEventL or not.
     * @return false when CAknAppUi::HandleWsEventL should be called, true
     * if it should not be called.
     */
    virtual TBool HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination) = 0;
};

} // end namespace ui
} // end namespace java

#endif // COREUIAPPUICHILD_H
