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
 * This class is to test the ticker
 * which is added and removed from different Screen objects
 *
 */

public class Test2 extends List implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet parent = null;

    //ScreenFormTests reference
    private ScreenTickerTests st = null;

    //the testTicker to test on different screens
    private Ticker testTicker = null;

    // The TextBox to which Ticker is added and removed
    private TextBox tb = null;

    //The List to which Ticker is added and removed
    private List list = null;

    //The Alert to which the Ticker is added and removed
    private Alert alert = null;

    //the form to which the Ticker is added and removed
    private Form form = null;

    //The TextField which is added to Form to test Form check
    private TextField tf = null;

    //The ImageItem which is added to Form to test Form check
    private ImageItem imgItem = null;

    //The DateField which is added to Form to test Form check
    private DateField df = null;

    //The Gauge which is added to Form to test Form check
    private Gauge g = null;

    //The ChoiceGroup which is added to Form to test Form check
    private ChoiceGroup cg = null;

    //list elements
    private String elements[] = {"element 1", "element 2", "element 3", "element 4", "element 5",
                                 "element 6", "element 7", "element 8"
                                };
    // this instance
    private Test2 instance = null;

    //List which contains ticker tests as elements
    private List Test2List = null;

    //String which is added to Test2List
    private final String addedToAlert = "Added to Alert";

    //String which is added to Test2List
    private final String addedToList = "Added to List";

    //String which is added to Test2List
    private final String addedToForm = "Added to Form";

    //String which is added to Test2List
    private final String addedToTextBox = "Added to TextBox";

    //String which is added to Test2List
    private final String removedFromAlert = "Removed from Alert";

    //String which is added to Test2List
    private final String removedFromList = "Removed from List";

    //String which is added to Test2List
    private final String removedFromForm = "Removed form Form";

    //String which is added to TextBox
    private final String removedFromTextBox = "Removed from TextBox";

    /*
     *  To indicate whether a Ticker is associated with the Alert. false
     *  indicates no Ticker is associated.
     */
    private static boolean alertFlag = false;

    /**
     *  To indicate whether a Ticker is associated with the Alert. false
     *  indicates no Ticker is associated.
     */
    private static boolean formFlag = false;

    /**
     *  To indicate whether a Ticker is associated with the List. false
     *  indicates no Ticker is associated.
     */
    private static boolean listFlag = false;

    /**
     *  To indicate whether a Ticker is associated with the TextBox. false
     *  indicates no Ticker is associated.
     */
    private static boolean textBoxFlag = false;

    //the smallImage
    private Image smallImage = null;

    //The set of images passed to the List constructor
    private Image[] imageElements = null;

    private Command cmdOk = new Command("OK", Command.OK, 1);

    //the command to go back to ScreenTickerTests screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    /**
     *
     * @param st
     */
    public Test2(ScreenTickerTests st)
    {
        //title of the List and the List type
        super("Test2", List.IMPLICIT);
        this.st = st;
        testTicker = new Ticker(" ");

        //get the parent MIDlet
        this.parent = ScreenTickerTests.getParent();

        if (instance == null)
        {
            instance = this;
        }

        append(addedToAlert, null);
        append(removedFromAlert, null);

        append(addedToList, null);
        append(removedFromList, null);

        append(addedToForm, null);
        append(removedFromForm, null);

        append(addedToTextBox, null);
        append(removedFromTextBox, null);

        //add command Ok to this list
        addCommand(cmdOk);

        //add command Back to this list
        addCommand(cmdBack);

        //set the listener
        setCommandListener(this);

        alert = new Alert("Ticker to Alert", null, null, AlertType.INFO);
        list = new List("Ticker to List", List.EXCLUSIVE, elements, createImage());
        list.addCommand(cmdBack);
        list.setCommandListener(this);

        tb = new TextBox("Ticke to TextBox", null, 100, TextField.ANY);
        tb.addCommand(cmdBack);
        tb.setCommandListener(this);

        form = new Form("Ticker to Form");
        tf = new TextField("TextField", null, 30, TextField.ANY);
        imgItem = new ImageItem("ImageItem", createImageItem(), ImageItem.LAYOUT_DEFAULT,
                                "error loading image");
        df = new DateField("DateField", DateField.DATE_TIME);
        g = new Gauge("Gauge", true, 10, 5);
        cg = new ChoiceGroup("ChoiceGroup", ChoiceGroup.MULTIPLE);
        cg.append("element 1", createImageItem());
        cg.append("element 2", null);
        cg.append("element 3", createImageItem());

        form.append(tf);
        form.append(imgItem);
        form.append(df);
        form.append(g);
        form.append(cg);
        form.addCommand(cmdBack);
        form.setCommandListener(this);
    }

    //this method creates image elements which is used by the list elements
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

    //this method creates a image which is used by the imageitem to test Form check
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
        int index = instance.getSelectedIndex();
        String element = instance.getString(index);
        if (c == List.SELECT_COMMAND || c == cmdOk)
        {
            if (element.equals(addedToAlert))
            {
                if (alertFlag == false)
                    alertFlag = true;
                testTicker.setString("Ticker to Alert");
                alert.setTicker(testTicker);
                Display.getDisplay(parent).setCurrent(alert);
            }
            else if (element.equals(removedFromAlert))
            {
                if (alertFlag == true)
                {
                    alertFlag = false;
                    alert.setTicker(null);
                }
                Display.getDisplay(parent).setCurrent(alert);
            }
            else if (element.equals(addedToList))
            {
                if (listFlag == false)
                    listFlag = true;
                testTicker.setString("Ticker to List");
                list.setTicker(testTicker);
                Display.getDisplay(parent).setCurrent(list);
            }
            else if (element.equals(removedFromList))
            {
                if (listFlag == true)
                {
                    listFlag = false;
                    list.setTicker(null);
                }
                Display.getDisplay(parent).setCurrent(list);
            }
            else if (element.equals(addedToTextBox))
            {
                if (textBoxFlag == false)
                    textBoxFlag = true;
                testTicker.setString("Ticker to TextBox");
                tb.setTicker(testTicker);
                Display.getDisplay(parent).setCurrent(tb);
            }
            else if (element.equals(removedFromTextBox))
            {
                if (textBoxFlag == true)
                {
                    textBoxFlag = false;
                    tb.setTicker(null);
                }
                Display.getDisplay(parent).setCurrent(tb);
            }
            else if (element.equals(addedToForm))
            {
                if (formFlag == false)
                {
                    formFlag = true;
                }
                testTicker.setString("Ticker to Form");
                form.setTicker(testTicker);
                Display.getDisplay(parent).setCurrent(form);
            }
            else if (element.equals(removedFromForm))
            {
                if (formFlag == true)
                {
                    formFlag = false;
                    form.setTicker(null);
                }
                Display.getDisplay(parent).setCurrent(form);
            }
        }
        if (c == cmdBack)
        {
            if (s == list || s == tb || s == form)
                Display.getDisplay(parent).setCurrent(this);
            else if (s == instance)
                Display.getDisplay(parent).setCurrent(st);
        }
    }
}

