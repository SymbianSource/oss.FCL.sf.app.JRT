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
 * This is the class to test modal alerts
 */

public class Test1 extends Form implements CommandListener
{

    //the parent MIDlet
    private MIDlet parent = null;

    //the Alert title
    private TextField title = new TextField("Title:", "Alert Title", 500, TextField.ANY);

    //Alert images to be added to alertImages ChoiceGroup
    private final static String[] ALERT_IMAGES = {"Null", "Small Image"};

    //image file names
    private final static String[] ALERT_IMAGES_FILE_NAMES = {null, "/small.png"};

    //Multiple ChoiceGroup with "FOREVER" element
    private ChoiceGroup forever = null;

    //Alert text for modal Alert
    private String text = "Alert should be modal";

    //the ChoiceGroup with image names
    private ChoiceGroup alertImages = new ChoiceGroup("Alert Image:",
            ChoiceGroup.EXCLUSIVE, ALERT_IMAGES, null);

    //the Command to show the Alert
    private Command cmdShowAlert = new Command("Show alert", Command.SCREEN, 1);

    /**
     *
     */
    public Test1()
    {
        super("AlertTests");
        forever = new ChoiceGroup(null, ChoiceGroup.MULTIPLE);
        forever.append("FOREVER", null);
        append(title);
        append(forever);
        append(alertImages);
        addCommand(cmdShowAlert);
        setCommandListener(this);
        parent = AlertTests.getParent();
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdShowAlert)
        {
            Image image = null;
            int index= 0;
            String element = null;
            int type = 0;

            try
            {
                int imgIndex = alertImages.getSelectedIndex();
                if (ALERT_IMAGES_FILE_NAMES[imgIndex] != null)
                {
                    image = Image.createImage(ALERT_IMAGES_FILE_NAMES[imgIndex]);
                }
                String alertText = "This is a modal alert with very long text blah blah blah blah blah blah" +
                                   "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                                   "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                                   "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                                   "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                                   "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah" +
                                   "blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah blah";

                //the alert should become modal
                Alert newAlert = new Alert(title.getString(), alertText, image, AlertType.INFO);
                if (forever.isSelected(0))
                {
                    newAlert.setString(text);
                    newAlert.setTimeout(Alert.FOREVER);
                }
                Display.getDisplay(parent).setCurrent(newAlert);
            }
            catch (java.io.IOException e)
            {
                System.out.println("commandAction: can't open Image: " +
                                   e.toString());
                Alert errorAlert = new Alert("ERROR", "Can't open Image: " + e.toString(), null,
                                             AlertType.ERROR);
                errorAlert.setTimeout(Alert.FOREVER);
                Display.getDisplay(parent).setCurrent(errorAlert);
            }
        }
    }
}
