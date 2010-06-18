/*
* Copyright (c) 2003-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 * import midp classes.
 */
import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

/**
 * This is the main class for the empty Form.
 */

public class EmptyForm extends Form
{

    /**
     *  The constructor creates and displays the Form.
     *
     *@param  parent  is the parent midlet.
     */
    public EmptyForm(MIDlet parent)
    {
        super(parent.getClass().getName());
        Display.getDisplay(parent).setCurrent(this);
    }
}
