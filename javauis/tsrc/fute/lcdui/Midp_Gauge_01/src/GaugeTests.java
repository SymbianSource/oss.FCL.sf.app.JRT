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
 * This class is to test both the interactive and non-interactive
 * gauges.
 *
 */

public class GaugeTests extends Form implements CommandListener
{

    //reference to the MIDlet to go to different screens
    private static Midp_Gauge_01 parent = null;

    //the ChoiceGroup to select gauge types i.e. interactive or non-interactive
    private ChoiceGroup cg = null;

    //the instance of this class
    private static GaugeTests instance = null;

    //elements of cg
    private String[] gTypes = {"Interactive", "Non-interactive", "CONTINUOUS_IDLE", "INCREMENTAL_IDLE", "CONTINUOUS_RUNNING", "INCREMENTAL_UPDATING" };

    //the TextField to enter initial value of the Gauge
    public TextField iValue = new TextField("initial Value:", "1", 3, TextField.NUMERIC);

    //the TextField to enter max value of the Gauge
    public TextField mValue = new TextField("maximum Value:", "10", 3, TextField.NUMERIC);

    //invokes to create the Gauge according to the gauge types
    private Command cmdOk = new Command("Ok", Command.OK, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    /**
     * This constructor creates ChoiceGroup "cg" and appends to this form.
     * It also appends TextFields iValue and mValue for the user to enter the
     * initial and max values of the Gauge to create.
     *
     * @param parent The parent MIDlet of this class
     */
    public GaugeTests(Midp_Gauge_01 parent)
    {

        //set the name of the form to be the name of MIDlet
        super(parent.getClass().getName());
        this.parent = parent;
        cg = new ChoiceGroup("Select Gauge type:", Choice.EXCLUSIVE, gTypes, null);
        append(cg);
        //append TextFields
        append(iValue);
        append(mValue);
        addCommand(cmdOk);
        addCommand(cmdExit);
        setCommandListener(this);
        if (instance == null)
        {
            instance = this;
        }
        //show this screen
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
     * get the string fron the iValue and convert it
     * to int.
     *
     *@return int  the iValue
     */
    public int getInitialValue()
    {
        String s = iValue.getString();
        int iValue = Integer.parseInt(s);
        return iValue;
    }

    /**
     * get the string fron the mValue and convert it
     * to int.
     *
     *@return int  the mValue
     */
    public int getMaxValue()
    {
        String s = mValue.getString();
        int mValue = Integer.parseInt(s);
        return mValue;
    }

    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        int index = cg.getSelectedIndex();
        String t = cg.getString(index);
        boolean type = true;
        if (t.equals(gTypes[0]))
        {
            type = true;
        }
        else
        {
            type = false;
        }
        if (c == cmdOk)
        {
            if (t.equals(gTypes[2]))
            {
                Display.getDisplay(parent).setCurrent(new GaugeTypes(type, this, t));
            }
            else if (t.equals(gTypes[3]))
            {
                Display.getDisplay(parent).setCurrent(new GaugeTypes(type, this, t));
            }
            else if (t.equals(gTypes[4]))
            {
                Display.getDisplay(parent).setCurrent(new GaugeTypes(type, this, t));
            }
            else if (t.equals(gTypes[5]))
            {
                Display.getDisplay(parent).setCurrent(new GaugeTypes(type, this, t));
            }
            else
            {
                Display.getDisplay(parent).setCurrent(new GaugeTypes(type, this, t));
            }
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
    //end method
}




