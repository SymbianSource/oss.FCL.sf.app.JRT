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

import java.util.Vector;

import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Image;

/**
 * Storage of objects which have to be disposed manually.
 */
class DisposeStorage
{

    private Vector storage;

    DisposeStorage()
    {
        storage = new Vector();
    }

    /**
     * Add object which must be disposed.
     *
     * @param obj object to add.
     */
    public void addObject(Object obj)
    {
        storage.addElement(obj);
    }

    /**
     * Dispose all objects from garbage.
     */
    public void dispose()
    {
        Object tempObject = null;
        for(int i = 0; i < storage.size(); i++)
        {
            tempObject = storage.elementAt(i);
            if(tempObject instanceof Image)
            {
                ((Image) tempObject).dispose();
            }
            else if(tempObject instanceof Color)
            {
                ((Color) tempObject).dispose();
            }
        }
        storage.removeAllElements();
    }



}
