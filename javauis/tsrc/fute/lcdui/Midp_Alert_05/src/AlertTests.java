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
 */

public class AlertTests extends Form implements CommandListener
{

    //reference to parent MIDlet
    private Midp_Alert_05 parent = null;

    Alert newAlert = new Alert(null);

    Image image = null;
    String longString = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas" +
                        " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                        " urna, hendrerit id fringilla eu, feugiat blandit tellus. Curabitur" +
                        " scelerisque tempus ante, eu convallis ligula ultrices nec. Nullam" +
                        " vulputate aliquet ullamcorper. Lorem ipsum dolor sit amet," +
                        " consectetur adipiscing elit. Proin at odio vel urna luctus dignissim" +
                        " a quis urna. Lorem ipsum dolor sit amet, consectetur adipiscing elit." +
                        " Phasellus aliquet luctus eros vitae aliquam. Suspendisse ut magna a" +
                        " ipsum fermentum imperdiet. Maecenas eu nunc sed metus placerat" +
                        " adipiscing. Duis lacus lacus, vestibulum id vestibulum a, tempus at" +
                        " nulla. Proin sagittis nibh vitae dui interdum varius. Cras purus" +
                        " nulla, lobortis eu laoreet a, ultricies vitae sem. Integer aliquam" +
                        " ultricies metus. Nulla nec dignissim mi. Nullam risus augue, egestas" +
                        " vitae facilisis eu, adipiscing eu turpis. Nulla convallis semper" +
                        " lobortis. Nullam eget lacus nunc. Nam ipsum lectus, mattis in mollis" +
                        " eu, tristique nec nibh. Nam pellentesque euismod est, a laoreet turpis" +
                        " pulvinar bibendum. Sed malesuada lacus eu ante luctus ut faucibus odio" +
                        " tristique. Donec quis mi ante. Praesent a porta velit. Duis tincidunt" +
                        " velit eget sapien viverra pulvinar.";

    //image names
    private final static String[] ALERT_IMAGES_FILE_NAMES =
    {
        null,
        "/small.png",
        "/fractal.png",
        "/invalid.png"
    };

    private Gauge indicator = null;

    private Command cmdLongTitle = new Command("Set long title", Command.SCREEN, 1);
    private Command cmdShortTitle = new Command("Set short title", Command.SCREEN, 1);
    private Command cmdNullTitle = new Command("Set null title", Command.SCREEN, 1);
    private Command cmdVeryLongString = new Command("Set very long string", Command.SCREEN, 1);
    private Command cmdShortString = new Command("Set short string", Command.SCREEN, 1);
    private Command cmdLongString = new Command("Set long string", Command.SCREEN, 1);
    private Command cmdNullString = new Command("Set null string", Command.SCREEN, 1);
    private Command cmdSmallImage = new Command("Set small image", Command.SCREEN, 1);
    private Command cmdBigImage = new Command("Set big image", Command.SCREEN, 1);
    private Command cmdInvalidImage = new Command("Set invalid image", Command.SCREEN, 1);
    private Command cmdNullImage = new Command("Set null image", Command.SCREEN, 1);
    private Command cmdAlarm = new Command("Set Alarm", Command.SCREEN, 1);
    private Command cmdConfirmation = new Command("Set Confirmation", Command.SCREEN, 1);
    private Command cmdError = new Command("Set Error", Command.SCREEN, 1);
    private Command cmdInfo = new Command("Set Info", Command.SCREEN, 1);
    private Command cmdWarning = new Command("Set Warning", Command.SCREEN, 1);
    private Command cmdNullType = new Command("Set null type", Command.SCREEN, 1);
    private Command cmdValue = new Command("Increase value", Command.SCREEN, 1);
    private Command cmdAddIndicator = new Command("Add Indicator", Command.SCREEN, 1);
    private Command cmdRemoveIndicator = new Command("Remove Indicator", Command.SCREEN, 1);
    private Command cmdPlaySound = new Command("Play Sound", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * @param m the parent MIDlet of this class
     */
    public AlertTests(Midp_Alert_05 m)
    {
        super("AlertTests");
        this.parent = m;

        indicator = new Gauge(null, false,  10, 0);

        newAlert.setTitle("Alert Title");
        newAlert.addCommand(cmdShortString);
        newAlert.addCommand(cmdShortTitle);
        newAlert.addCommand(cmdLongTitle);
        newAlert.addCommand(cmdNullTitle);
        newAlert.addCommand(cmdAlarm);

        newAlert.addCommand(cmdLongString);
        newAlert.addCommand(cmdVeryLongString);
        newAlert.addCommand(cmdNullString);
        newAlert.addCommand(cmdSmallImage);
        newAlert.addCommand(cmdBigImage);
        newAlert.addCommand(cmdInvalidImage);
        newAlert.addCommand(cmdNullImage);
        newAlert.addCommand(cmdAlarm);
        newAlert.addCommand(cmdConfirmation);
        newAlert.addCommand(cmdError);
        newAlert.addCommand(cmdInfo);
        newAlert.addCommand(cmdWarning);
        newAlert.addCommand(cmdNullType);
        newAlert.addCommand(cmdAddIndicator);
        newAlert.addCommand(cmdRemoveIndicator);
        newAlert.addCommand(cmdValue);
        newAlert.addCommand(cmdPlaySound);
        newAlert.addCommand(cmdExit);
        newAlert.setCommandListener(this);

        Display.getDisplay(parent).setCurrent(newAlert);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        System.out.println("Checking commands...");
        System.out.println(s.toString());
        System.out.println(c.getLabel());
        if (c == cmdNullTitle)
        {
            newAlert.setTitle(null);
        }
        if (c == cmdShortTitle)
        {
            newAlert.setTitle("Short Title");
        }

        if (c == cmdLongTitle)
        {
            newAlert.setTitle("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas" +
                              " tempor nisl in quam aliquet eu posuere eros tempor.");
        }

        if (c == cmdShortString)
        {
            System.out.println("Short String Command");
            newAlert.setString("Short string");
        }
        if (c == cmdLongString)
        {
            newAlert.setString("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas" +
                               " tempor nisl in quam aliquet eu posuere eros tempor. Vivamus felis" +
                               " urna, hendrerit id fringilla eu, feugiat blandit tellus. Curabitur" +
                               " scelerisque tempus ante, eu convallis ligula ultrices nec.");
        }
        if (c == cmdVeryLongString)
        {
            newAlert.setString(longString);
        }
        if (c == cmdNullString)
        {
            newAlert.setString(null);
        }

        if (c == cmdAlarm)
        {
            System.out.println("Setting Alarm...");
            newAlert.setType(AlertType.ALARM);
        }
        if (c == cmdConfirmation)
        {
            newAlert.setType(AlertType.CONFIRMATION);
        }
        if (c == cmdError)
        {
            newAlert.setType(AlertType.ERROR);
        }
        if (c == cmdInfo)
        {
            newAlert.setType(AlertType.INFO);
        }
        if (c == cmdWarning)
        {
            newAlert.setType(AlertType.WARNING);
        }
        if (c == cmdNullType)
        {
            newAlert.setType(null);
        }

        if (c == cmdPlaySound)
        {
            if (newAlert.getType().playSound(Display.getDisplay(parent)))
                newAlert.setString("playSound() = true");
            else
                newAlert.setString("playSound() = false");
        }

        if (c == cmdSmallImage)
        {
            try
            {
                image = Image.createImage(ALERT_IMAGES_FILE_NAMES[1]);
            }
            catch (java.io.IOException e)
            {
            }
            newAlert.setImage(image);
        }
        if (c == cmdBigImage)
        {
            try
            {
                image = Image.createImage(ALERT_IMAGES_FILE_NAMES[2]);
            }
            catch (java.io.IOException e)
            {
            }
            newAlert.setImage(image);
        }
        if (c == cmdNullImage)
        {
            newAlert.setImage(null);
        }

        if (c == cmdInvalidImage)
        {
            try
            {
                image = Image.createImage(ALERT_IMAGES_FILE_NAMES[3]);
            }
            catch (java.io.IOException e)
            {
            }
            newAlert.setImage(image);
        }
        if (c == cmdAddIndicator)
            newAlert.setIndicator(indicator);
        if (c == cmdRemoveIndicator)
            newAlert.setIndicator(null);

        if (c == cmdValue)
        {
            indicator.setValue(indicator.getValue()+1);
        }
        if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
