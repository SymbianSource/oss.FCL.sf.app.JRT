/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


package com.nokia.microedition.m2g;

/**
 * Anyone implementing this interface will receive notifications
 * about changes in DOM.
 * <br>
 * Subject              = M2GDocument
 * Observer/listener    = M2GSVGCanvas
 * <br>
 * M2GSVGCanvas from M2GSVGAnimator implements this in order to be notified
 * about changes in DOM, so that SVGImage updates
 * (via the API, e.g. setTrait(), insertBefore()) will be rendered
 * while Animator is in pause state.
 * <br>
 * M2GSVGElement informs M2GDocument about changes in DOM
 * (e.g. adding/removing nodes, changing traits),
 * and then M2GDocument informs M2GSVGCanvas.
 * <br>
 * @since S60 3.2
 */
public interface M2GDOMChangeObserver
{
    /**
     * Notifies about any change in DOM
     */
    public void notifyDOMChange();
}
