/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


package org.eclipse.swt.widgets.control;

import com.nokia.microedition.media.control.MMAGUIFactory;
import org.eclipse.swt.widgets.DCControl;

/**
 * This class is used to create DCControl instance with initDisplayMode()
 * method.
 *
 */
public class Factory implements MMAGUIFactory
{
    /**
     * Control created in initDisplayMode
     */
    private DCControl iDCControl;

    /**
     * Default constructor. This is needed enable dynamic class loading.
     */
    public Factory()
    {
    }

    /**
     * @see MMAGUIFactory
     */
    public Object initDisplayMode()
    {
        iDCControl = new DCControl();
        return iDCControl;
    }

    /**
     * @see MMAGUIFactory
     * @throw
     */
    public void setContentHandle(int aContentHandle)
    {
        iDCControl.setContentHandle(aContentHandle);
    }
}
