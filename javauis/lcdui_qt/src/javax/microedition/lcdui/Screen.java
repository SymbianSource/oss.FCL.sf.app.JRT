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

/**
 * Super class for all high level UI classes e.g Form, TextBox. Appearance and
 * interaction is defined in subclasses.
 */
public abstract class Screen extends Displayable {

    /**
     * Constructor.
     */
    Screen(String title) {
        super(title);
    }

}
