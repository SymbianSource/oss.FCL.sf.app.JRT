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
 * This class allows the user to select the Item to test
 *
 */

public class FormUpdateTests extends List implements CommandListener
{

    //the reference to the MIDlet to go to different screens
    private static Midp_Form_02 parent = null;

    //the this instance
    private static FormUpdateTests instance = null;

    //String to add as an element to this List
    private final String exCgStr = "Exclusive ChoiceGroup";

    //String to add as an element to this List
    private final String mulCgStr = "Multiple ChoiceGroup";

    //String to add as an element to this List
    private final String popCgStr = "Popup ChoiceGroup";

    //String to add as an element to this List
    private final String dfStr = "DateField";

    //String to add as an element to this List
    private final String iGStr = "Interactive Gauge";

    //String to add as an element to this List
    private final String nGStr = "NonInteractive Gauge";

    //String to add as an element to this List
    private final String siStr = "StringItem";

    //String to add as an element to this List
    private final String tfStr = "TextField";

    //String to add as an element to this List
    private final String imgStr = "ImageItem";

    //the ChoiceGroup to test the methods
    private ChoiceGroup cg = null;

    //the DateField to test the methods
    private DateField df = null;

    //the Gauge to test the methods
    private Gauge g = null;

    //the StringItem to test the methods
    private StringItem si = null;

    //the TextField to test the methods
    private TextField tf = null;

    //the boolean to select the type of the Gauge
    private boolean intGauge = true;

    //the boolean to select the type of the Gauge
    private boolean nonIntGauge = false;

    //the Command to invoke the item to test
    private Command cmdOk = new Command("Ok", Command.OK, 1);

    // The command to exit MIDlet.
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * This constructor appends the elements to this List
     *
     *@param MIDlet the reference to the MIDlet
     */
    public FormUpdateTests(Midp_Form_02 parent)
    {
        super(parent.getClass().getName(), List.EXCLUSIVE);
        this.parent = parent;

        addCommand(cmdOk);
        addCommand(cmdExit);
        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
        //append elements to this List
        instance.append(exCgStr, null);
        instance.append(mulCgStr, null);
        instance.append(popCgStr, null);
        instance.append(dfStr, null);
        instance.append(iGStr, null);
        instance.append(nGStr, null);
        instance.append(siStr, null);
        instance.append(tfStr, null);
        instance.append(imgStr, null);
        //show this Screen
        show();
    }

    /**
     *  Sets this to be the current screen
     *
     *@return    The Parent value
     */
    public static MIDlet getParent()
    {
        return parent;
    }

    /**
     *  Show this screen
     */
    public static void show()
    {
        Display.getDisplay(parent).setCurrent(instance);
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
        String item = instance.getString(index);
        if (c == cmdOk)
        {
            if (item.equals(exCgStr))
                Display.getDisplay(parent).setCurrent(new ChoiceGroupMethods(ChoiceGroup.EXCLUSIVE));
            if (item.equals(mulCgStr))
                Display.getDisplay(parent).setCurrent(new ChoiceGroupMethods(ChoiceGroup.MULTIPLE));
            if (item.equals(popCgStr))
                Display.getDisplay(parent).setCurrent(new ChoiceGroupMethods(ChoiceGroup.POPUP));
            if (item.equals(dfStr))
                Display.getDisplay(parent).setCurrent(new DateFieldMethods(parent));
            if (item.equals(iGStr))
                Display.getDisplay(parent).setCurrent(new GaugeMethods(parent, intGauge));
            if (item.equals(nGStr))
                Display.getDisplay(parent).setCurrent(new GaugeMethods(parent, nonIntGauge));
            if (item.equals(siStr))
                Display.getDisplay(parent).setCurrent(new StringItemMethods(parent));
            if (item.equals(tfStr))
                Display.getDisplay(parent).setCurrent(new TextFieldMethods(parent));
            if (item.equals(imgStr))
                Display.getDisplay(parent).setCurrent(new ImageItemMethods(parent));
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method

}
