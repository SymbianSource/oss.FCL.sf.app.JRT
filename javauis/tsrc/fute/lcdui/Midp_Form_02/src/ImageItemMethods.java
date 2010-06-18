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
 * This class is to test the various methods of ImageItem
 *
 */

public class ImageItemMethods extends Form implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet m = null;

    //the ImageItem methods as elements in the listMethodsStr
    private String listMethodsStr[] = {"SetLabel", "SetImage", "SetLayout"};

    //the ImageItem labels as Strings to setLabel in StringItem
    private String listSetLabelStr[] = {"Label", "NULL"};

    //the image strings
    private String listSetImageStr[] = {"Image", "NULL"};

    //the layout strings
    private String listSetLayoutStr[] = { "CENTER", "DEFAULT", "LEFT", "NEWLINE_AFTER",
                                          "NEWLINE_BEFORE", "RIGHT"
                                        };

    //the list to add listMethodsStr
    private List listMethods = null;

    //the list to add listSetLabelStr
    private List listSetLabel = null;

    //the list to add listSetImageStr
    private List listSetImage = null;

    //the list to add listSetLayoutStr
    private List listSetLayout = null;

    //ImageItem to test
    private ImageItem imgItem = null;

    //the command to go back to the Screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the commmand to invoke the StringItem method for testing
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    //the command to display the listMethods Screen
    private Command cmdMethods = new Command("Methods", Command.SCREEN, 1);

    //the command to set the StringItem
    private Command cmdSet = new Command("Set", Command.OK, 1);

    private Image smallImage = null;

    public ImageItemMethods(MIDlet m)
    {

        super(m.getClass().getName());
        this.m = m;
        try
        {
            smallImage  = Image.createImage("/Icon.png");
        }
        catch (IOException e)
        {
            System.out.println("Error loading image\n" + e);
        }
        imgItem = new ImageItem("ImageItem", smallImage, ImageItem.LAYOUT_DEFAULT, null);
        append(imgItem);
        addCommand(cmdBack);
        addCommand(cmdMethods);
        setCommandListener(this);
    }

    /**
     * This method creates the listMethods List and sets the
     * current screen as listMethods.
     *
     */
    private void addListMethods()
    {
        listMethods = new List("Methods", List.IMPLICIT, listMethodsStr, null);
        listMethods.addCommand(cmdBack);
        listMethods.addCommand(cmdOk);
        listMethods.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listMethods);
    }

    /**
     * This method creates the listSetLabel List and sets the
     * current screen as listSetLabel .
     *
     */
    private void addListSetLabel()
    {
        listSetLabel = new List("SetLabel", List.IMPLICIT, listSetLabelStr, null);
        listSetLabel.addCommand(cmdBack);
        listSetLabel.addCommand(cmdSet);
        listSetLabel.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetLabel);
    }

    /**
     * This method creates the listSetImage List and sets the
     * current screen as listSetImage.
     *
     */
    private void addListSetImage()
    {
        listSetImage = new List("SetImage", List.IMPLICIT, listSetImageStr, null);
        listSetImage.addCommand(cmdBack);
        listSetImage.addCommand(cmdSet);
        listSetImage.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetImage);
    }

    /**
     * This method creates the listSetLayout List and sets the
     * current screen as listSetLayout.
     *
     */
    private void addListSetLayout()
    {
        listSetLayout = new List("SetLayout", List.IMPLICIT, listSetLayoutStr, null);
        listSetLayout.addCommand(cmdBack);
        listSetLayout.addCommand(cmdSet);
        listSetLayout.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetLayout);
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
        {
            if (s == listMethods)
                Display.getDisplay(m).setCurrent(this);
            else if (s == listSetLabel || s == listSetImage || s == listSetLayout)
                Display.getDisplay(m).setCurrent(listMethods);
            else
                FormUpdateTests.show();
        }
        else if (c == cmdMethods)
        {
            addListMethods();
            imgItem.setImage(smallImage);
            imgItem.setLayout(ImageItem.LAYOUT_DEFAULT);
            imgItem.setLabel("ImageItem");
        }
        else if (c == cmdOk)
        {
            int index = listMethods.getSelectedIndex();
            String element = listMethods.getString(index);
            if (element.equals(listMethodsStr[0]))
                addListSetLabel();
            else if (element.equals(listMethodsStr[1]))
                addListSetImage();
            else if (element.equals(listMethodsStr[2]))
                addListSetLayout();
        }
        else if (c == cmdSet)
        {
            if (s == listSetImage)
            {
                int index = listSetImage.getSelectedIndex();
                String element = listSetImage.getString(index);
                if (element.equals(listSetImageStr[0]))
                {
                    try
                    {
                        smallImage  = Image.createImage("/Icon.png");
                    }
                    catch (IOException e)
                    {
                        System.out.println("Error loading image\n" + e);
                    }
                    imgItem.setImage(smallImage);
                }
                else if (element.equals(listSetImageStr[1]))
                    imgItem.setImage(null);
            }
            if (s == listSetLabel)
            {
                int index = listSetLabel.getSelectedIndex();
                String element = listSetLabel.getString(index);
                if (element.equals(listSetLabelStr[0]))
                    imgItem.setLabel("ImageItem label which should be too long to fit on one line");
                if (element.equals(listSetLabelStr[1]))
                    imgItem.setLabel(null);
            }
            if (s == listSetLayout)
            {
                int index = listSetLayout.getSelectedIndex();
                String element = listSetLayout.getString(index);
                if (element.equals(listSetLayoutStr[0]))
                {
                    imgItem.setLayout(ImageItem.LAYOUT_CENTER);
                    imgItem.setLabel("LAYOUT_CENTER");
                }
                if (element.equals(listSetLayoutStr[1]))
                {
                    imgItem.setLayout(ImageItem.LAYOUT_DEFAULT);
                    imgItem.setLabel("LAYOUT_DEFAULT");
                }
                if (element.equals(listSetLayoutStr[2]))
                {
                    imgItem.setLayout(ImageItem.LAYOUT_LEFT);
                    imgItem.setLabel("LAYOUT_LEFT");
                }
                if (element.equals(listSetLayoutStr[3]))
                {
                    imgItem.setLayout(ImageItem.LAYOUT_NEWLINE_AFTER);
                    imgItem.setLabel("LAYOUT_NEWLINE_AFTER");
                }
                if (element.equals(listSetLayoutStr[4]))
                {
                    imgItem.setLayout(ImageItem.LAYOUT_NEWLINE_BEFORE);
                    imgItem.setLabel("LAYOUT_NEWLINE_BEFORE");
                }
                if (element.equals(listSetLayoutStr[5]))
                {
                    imgItem.setLayout(ImageItem.LAYOUT_RIGHT);
                    imgItem.setLabel("LAYOUT_RIGHT");
                }
            }
            Display.getDisplay(m).setCurrent(this);
        }
    }
}
