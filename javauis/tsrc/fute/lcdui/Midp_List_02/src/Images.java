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

import java.io.*;
/**
 * This class is to create all the images needed to
 * test List.
 *
 */

public class Images
{

    /**
     *  The one and only instance of this class
     */
    private static Images instance = null;

    /**
     *  The small image which fits to the layout
     */
    public Image smallImage = null;

    /**
     *  Constructor for the Images object
     */
    private Images()
    {
        try
        {
            smallImage = Image.createImage("/Icon.png");
        }
        catch (IOException e)
        {
            System.out.println("Error loading image\n" + e);
        }
    }
    //end method

    /**
     *  Returns a reference to the one and only instance of this class
     *
     *@return    The Instance value
     */
    public static Images getInstance()
    {
        if (instance == null)
        {
            instance = new Images();
        }
        return instance;
    }
}
