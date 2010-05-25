/*
* Copyright (c) 2001-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * This class is to create all the images needed for this MIDlet
 *
 */

public class Images
{
    /**
     *  The one and only instance of this class
     */
    private static Images instance = null;

    /**
     *  The big image
     */
    public Image bigImage = null;

    /**
     *  The small image which fits to the layout
     */
    public Image smallImage = null;

    /**
     *  The invalid image
     */
    public Image invalidImage = null;

    /**
     *  Constructor for the Images object
     */
    private Images()
    {
        try
        {
            bigImage  = Image.createImage("/bigImage.png");
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

    /**
     *  Tries to load an image that is invalid
     *
     *@return                  The Invalid value
     *@exception  IOException  Description of Exception
     */
    public Image getInvalid()
    throws IOException
    {
        Image invalid = Image.createImage("/invalid.png");
        return invalid;
    }
    //end method

}

