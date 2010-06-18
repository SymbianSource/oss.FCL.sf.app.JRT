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
 * This class is to create all the needed images to test Midp_ImageItem_01
 *
 */

public class Images
{

    /**
     *  The one and only instance of this class
     */
    private static Images instance = null;

    /**
     *  The big image which exceeds the width of the screen
     */
    public Image bigImageW = null;

    /**
     *  The big image which exceeds the height of the screen
     */
    public Image bigImageH = null;

    /**
     *  The big image which exceeds the height and width of the screen
     */
    public Image bigImage = null;

    /**
     *  The small jigsaw image
     */
    public Image smallJigsaw = null;

    /**
     *  The micro jigsaw image
     */
    public Image microJigsaw = null;

    /**
     *  The normal jigsaw image
     */
    public Image jigsaw = null;


    /**
     *  Constructor for the Images object
     */
    private Images()
    {
        try
        {
            bigImage = Image.createImage("/bigImage.png");
            bigImageW  = Image.createImage("/BigImageW.png");
            bigImageH  = Image.createImage("/BigImageH.png");
            smallJigsaw = Image.createImage("/jigsaw_small.png");
            microJigsaw = Image.createImage("/jigsaw_micro.png");
            jigsaw = Image.createImage("/jigsaw.png");
        }
        catch (Exception e)
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
            instance = new Images();
        return instance;
    }
    //end method
}

