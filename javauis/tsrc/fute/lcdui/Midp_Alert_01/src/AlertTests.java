/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * This is the class to test the Alert test cases:
 * 1.Alert title
 * 2.Default Alert texts and tones
 * 3.Alert images and
 * 4.Timed Alerts
 */

public class AlertTests extends Form implements CommandListener
{

    //reference to parent MIDlet
    private Midp_Alert_01 parent = null;

    //The TextField to get Alert title
    private TextField title = new TextField("Title:", "Alert Title", 500, TextField.ANY);

    //The TextField to get Alert text
    private TextField text = new TextField("Text:", "Alert Text", 500, TextField.ANY);

    //Alert time
    private TextField time = new TextField("Time:", "5000", 100, TextField.NUMERIC);

    /**
    *  These are the types of the alerts to show within the choice group
     */
    private final static String[] CHOOSABLE_ALERTS =
    {
        "NULL",
        "ALARM",
        "CONFIRMATION",
        "ERROR",
        "INFO",
        "WARNING"
    };
    //alert types
    private final static AlertType[] CHOOSEABLE_ALERT_TYPES =
    {
        null,
        AlertType.ALARM,
        AlertType.CONFIRMATION,
        AlertType.ERROR,
        AlertType.INFO,
        AlertType.WARNING
    };

    //These are the image names shown in the ChoiceGroup
    private final static String[] ALERT_IMAGES =
    {
        "Null",
        "Small Image",
        "Big Image",
        "Invalid Image"
    };
    //image names
    private final static String[] ALERT_IMAGES_FILE_NAMES =
    {
        null,
        "/small.png",
        "/fractal.png",
        "/invalid.png"
    };

    //the ChoiceGroup with Alert types
    private ChoiceGroup alertTypes = new ChoiceGroup("Alert Type:", ChoiceGroup.EXCLUSIVE,
            CHOOSABLE_ALERTS, null);
    //the ChoiceGroup with Alert images
    private ChoiceGroup alertImages = new ChoiceGroup("Alert Image:",
            ChoiceGroup.EXCLUSIVE, ALERT_IMAGES, null);
    //Command to show alert
    private Command cmdShowAlert = new Command("Show alert", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);
    private Alert newAlert;
    /**
     * @param m the parent MIDlet of this class
     */
    public AlertTests(Midp_Alert_01 m)
    {
        super("AlertTests");
        this.parent = m;
        newAlert = new Alert(null);
        append(title);
        append(text);
        append(time);
        append(alertTypes);
        append(alertImages);
        addCommand(cmdShowAlert);
        addCommand(cmdExit);
        setCommandListener(this);
        Display.getDisplay(parent).setCurrent(this);
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
            try
            {
                index = alertTypes.getSelectedIndex();
                element = alertTypes.getString(index);

                int imgIndex = alertImages.getSelectedIndex();
                if (ALERT_IMAGES_FILE_NAMES[imgIndex] != null)
                {
                    image = Image.createImage(ALERT_IMAGES_FILE_NAMES[imgIndex]);
                }
                newAlert.setTitle(title.getString());
                newAlert.setString(text.getString());
                newAlert.setImage(image);
                newAlert.setType(CHOOSEABLE_ALERT_TYPES[index]);

                if (time.size() != 0)
                    newAlert.setTimeout(Integer.parseInt(time.getString()));

                Display.getDisplay(parent).setCurrent(newAlert);
            }
            catch (java.io.IOException e)
            {
                System.out.println("commandAction: can't open Image: " +
                                   e.toString());
                Alert errorAlert = new Alert("Cannot open Image", e.toString(), null,
                                             AlertType.ERROR);
                errorAlert.setTimeout(Alert.FOREVER);
                Display.getDisplay(parent).setCurrent(errorAlert);
            }
            catch (IllegalArgumentException e)
            {
                System.out.println("Exception: " + e.toString());
                Alert errorAlert = new Alert("Exception", e.toString(), null,
                                             AlertType.ERROR);
                errorAlert.setTimeout(Alert.FOREVER);
                Display.getDisplay(parent).setCurrent(errorAlert);
            }
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}


