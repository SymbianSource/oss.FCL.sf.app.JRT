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
import java.util.*;
import javax.microedition.lcdui.List;

/**
 * This class is to test the Ticker test cases:
 * 1.Ticker must not be interrupted when it changes from one Screen
 * object to another Screen object
 * 2.Old Ticker is hidden and stopped when added to different Screen objects.
 *
 */

public class Test3 extends Form implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet parent = null;

    //ScreenFormTests reference
    private ScreenTickerTests st = null;

    //the testTicker1 to test on different screens
    private Ticker testTicker1 = null;

    //the testTicker2 to test on different screens
    private Ticker testTicker2 = null;

    // The TextBox to which Ticker is added
    private TextBox tb = null;

    //The List to which Ticker is added
    private List list = null;

    //The Alert to which the Ticker is added
    private Alert alert = null;

    //The TextField which is added to Form
    private TextField tf = null;

    //The ImageItem which is added to Form
    private ImageItem imgItem = null;

    //The DateField which is added to Form
    private DateField df = null;

    //The Gauge which is added to Form
    private Gauge g = null;

    //The ChoiceGroup which is added to Form
    private ChoiceGroup cg = null;

    //list elements
    private String elements[] = {"element 1", "element 2", "element 3", "element 4", "element 5",
                                 "element 6", "element 7", "element 8"
                                };

    // this instance
    private Test3 instance = null;

    private Image smallImage = null;

    //The set of images passed to the List constructor
    private Image[] imageElements = null;


    private Command cmdShowS = new Command("ShowScreens", Command.SCREEN, 1);

    private Command cmdReplaceT = new Command("Replace Ticker", Command.SCREEN, 1);

    //a timer which schedules all the tasks in this MIDlet
    private Timer timer = null;

    private ShowAlertTT showAlertTT = null;

    private static boolean replaceTicker = false;

    /**
     *
     * @param st
     */
    public Test3(ScreenTickerTests st)
    {
        //title of the Form
        super("Test Form");
        this.st = st;

        timer = new Timer();

        testTicker1 = new Ticker("1st Ticker");

        //Ticker2 will be shown when cmdReplaceT is pressed
        testTicker2 = new Ticker("2nd Ticker");

        //get the parent MIDlet
        this.parent = ScreenTickerTests.getParent();

        if (instance == null)
        {
            instance = this;
        }
        alert = new Alert("Ticker to Alert", null, null, AlertType.ALARM);
        alert.setTicker(testTicker1);

        list = new List("Ticker to List", List.EXCLUSIVE, elements, createImage());
        list.setTicker(testTicker1);

        tb = new TextBox("Ticker to TextBox", null, 100, TextField.ANY);
        tb.setTicker(testTicker1);

        tf = new TextField("TextField", null, 30, TextField.ANY);
        imgItem = new ImageItem("ImageItem", createImageItem(), ImageItem.LAYOUT_DEFAULT,
                                "error loading image");
        df = new DateField("DateField", DateField.DATE_TIME);
        g = new Gauge("Gauge", true, 10, 5);
        cg = new ChoiceGroup("ChoiceGroup", ChoiceGroup.MULTIPLE);
        cg.append("element 1", createImageItem());
        cg.append("element 2", null);
        cg.append("element 3", createImageItem());

        instance.append(tf);
        instance.append(imgItem);
        instance.append(df);
        instance.append(g);
        instance.append(cg);

        instance.setTicker(testTicker1);

        //add Commands
        addCommand(cmdReplaceT);
        addCommand(cmdShowS);

        //set the listener
        setCommandListener(this);
    }

    /**
     * This method creates image elements which is used by the list elements
     */

    private Image[] createImage()
    {
        try
        {
            smallImage = Image.createImage("/Icon.png");
            imageElements = new Image[]
            {smallImage, null, smallImage, null, null, smallImage,
             smallImage, null
            };
        }
        catch (java.io.IOException e)
        {
            System.out.println("Error loading image\n" + e);
        }
        return imageElements;
    }

    /**
     * This method creates a image which is used by the imageitem to test Form check
     */
    private Image createImageItem()
    {
        try
        {
            smallImage = Image.createImage("/Icon.png");
        }
        catch (java.io.IOException e)
        {
            System.out.println("Error loading image\n" + e);
        }
        return smallImage;
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {

        if (c == cmdShowS)
        {
            showAlertTT = new ShowAlertTT();
            timer.schedule(showAlertTT, 100);
        }
        else if (c == cmdReplaceT)
        {
            replaceTicker = true;
            instance.setTicker(testTicker2);

        }
    }

    /**
     *
     */
    private class ShowAlertTT extends TimerTask
    {
        /**
         *  The action to be performed by this timer task.
         */
        public void run()
        {
            Display.getDisplay(parent).setCurrent(alert);
            if (replaceTicker == true)
                alert.setTicker(testTicker2);
            ShowFormTT showFormTT = new ShowFormTT();
            timer.schedule(showFormTT, 4000);
            cancel();
        }
    }

    private class ShowFormTT extends TimerTask
    {

        /**
         *  The action to be performed by this timer task.
         */
        public void run()
        {
            Display.getDisplay(parent).setCurrent(instance);
            if (replaceTicker == true)
                instance.setTicker(testTicker2);
            ShowListTT showListTT = new ShowListTT();
            timer.schedule(showListTT, 4000);
            cancel();
        }
    }

    private class ShowListTT extends TimerTask
    {

        /**
         *  The action to be performed by this timer task.
         */
        public void run()
        {
            Display.getDisplay(parent).setCurrent(list);
            if (replaceTicker == true)
                list.setTicker(testTicker2);
            ShowTextBoxTT showTextBoxTT = new ShowTextBoxTT();
            timer.schedule(showTextBoxTT, 4000);
            cancel();
        }
    }

    private class ShowTextBoxTT extends TimerTask
    {

        /**
         *  The action to be performed by this timer task.
         */
        public void run()
        {
            Display.getDisplay(parent).setCurrent(tb);
            if (replaceTicker == true)
                tb.setTicker(testTicker2);
            ShowInitFormTT showInitFormTT = new ShowInitFormTT();
            timer.schedule(showInitFormTT, 7000);
            cancel();
        }
    }

    private class ShowInitFormTT extends TimerTask
    {

        /**
         *  The action to be performed by this timer task.
         */
        public void run()
        {
            Display.getDisplay(parent).setCurrent(st);
            cancel();
        }
    }
}

