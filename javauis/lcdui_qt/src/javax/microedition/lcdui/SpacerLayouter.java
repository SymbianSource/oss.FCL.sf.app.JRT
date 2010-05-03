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
* Description: 
*
*/
package javax.microedition.lcdui;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

/**
 * Class responsible for correct layout of Spacer item.
 */
class SpacerLayouter extends ItemLayouter {

    /**
     * Constructor.
     *
     * @param dflp DefaultFormLayoutPolicy.
     */
    SpacerLayouter(DefaultFormLayoutPolicy dflp) {
        super(dflp);
    }

    /**
     * eSWT specific calls to implement getControl.
     *
     * @param parent for the control.
     * @param item Spacer item.
     */
    Control eswtGetControl(Composite parent, Item item) {
        Composite composite = new Composite(parent, SWT.NONE);
        composite.setSize(item.getMinimumWidth(), item.getMinimumHeight());
        return composite;
    }

    /**
     * Returns true if this eSWT control is suitable to be used for updating.
     *
     * @param item Item.
     * @param control eSWT control.
     * @return true if this control is suitable for update
     */
    boolean eswtIsSpecificControl(Item item, Control control) {
        return true;
    }

    /**
     * Updates the values of Spacer. (Not used because of Spacer specific use)
     *
     * @param item Item.
     * @param control eSWT control.
     * @param reason reason to update.
     */
    void eswtUpdateItem(Item item, Control control, int reason, Object param) {
        // no implementation needed.
    }

}
