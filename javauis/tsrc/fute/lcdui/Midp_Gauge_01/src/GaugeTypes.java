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
 * This class is to test the inital and maximum values of
 * interactive and non-interactive gauges.
 *
 */

public class GaugeTypes extends Form implements CommandListener
{

    //reference to the parent MIDlet
    private MIDlet parent = null;

    //reference to the GaugeTests
    private GaugeTests gt = null;

    //the Gauge to test
    private Gauge g = null;

    private String[] gTypes = {"Interactive", "Non-interactive", "CONTINUOUS_IDLE", "INCREMENTAL_IDLE", "CONTINUOUS_RUNNING", "INCREMENTAL_UPDATING" };

    //command to go back to the GaugeTests screen
    private Command cmdBack = new Command("Back", Command.BACK, 1);

    //the command to get the value of Gaguge "g"
    private Command cmdGetValue = new Command("getValue", Command.SCREEN, 1);

    //the command to get the max value of Gauge "g"
    private Command cmdGetMaxValue = new Command("getMaxValue", Command.SCREEN, 1);
    private Command cmdOk = new Command("Ok", Command.SCREEN, 1);

    /**
     *
     *@parm type the boolean to select the type of the Gauge
     *@param gt The reference to the GaugeTests object
     */
    public GaugeTypes(boolean type, GaugeTests gt, String value)
    {

        super(" ");
        this.gt = gt;
        if (type)
        {
            g = new Gauge("Interactive", type, gt.getMaxValue(), gt.getInitialValue());
        }
        else
        {
            if (value.equals(gTypes[1]))
                g = new Gauge("Non-interactive", type, gt.getMaxValue(), gt.getInitialValue());
            else if (value.equals(gTypes[2]))
                g = new Gauge("CONTINUOUS_IDLE", type, Gauge.INDEFINITE, Gauge.CONTINUOUS_IDLE);
            else if (value.equals(gTypes[3]))
                g = new Gauge("INCREMENTAL_IDLE", type, Gauge.INDEFINITE, Gauge.INCREMENTAL_IDLE);
            else if (value.equals(gTypes[4]))
                g = new Gauge("CONTINUOUS_RUNNING", type, Gauge.INDEFINITE, Gauge.CONTINUOUS_RUNNING);
            else if (value.equals(gTypes[5]))
                g = new IndicatorRunnable();
        }

        append(g);
        addCommand(cmdBack);
        addCommand(cmdGetMaxValue);
        addCommand(cmdGetValue);
        addCommand(cmdOk);
        g.setDefaultCommand(cmdGetValue);
        setCommandListener(this);

        parent = gt.getParent();
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
            GaugeTests.show();
        else if (c == cmdGetValue)
            showAlert(g.getValue());
        else if (c == cmdGetMaxValue)
            showAlert(g.getMaxValue());
        else if (c == cmdOk)
        {
            Alert testAlert = null;
            try
            {
                testAlert = new Alert("Container");
                testAlert.setIndicator(g);
                g.setLabel("Test");
            }
            catch (IllegalStateException ex)
            {
                //Test is passed
                Alert iseAlert = new Alert("Illegal State",ex.toString(),null, AlertType.ERROR);
                Display.getDisplay(parent).setCurrent(iseAlert);

            }
            catch (Exception ex)
            {
                //test is failed
                Alert eAlert = new Alert("Wrong Exception","Wrong Exception was thrown: "+ex.toString(),null, AlertType.ERROR);
                Display.getDisplay(parent).setCurrent(eAlert);
            }
            finally
            {
                testAlert.setIndicator(null);
            }
        }
    }

    /**
     *  shows the alert with the value
     *
     *@param value the value
     */
    private void showAlert(int value)
    {
        Alert alert = new Alert("Value", "Value is: " + value, null, null);
        alert.setTimeout(Alert.FOREVER);
        Display.getDisplay(parent).setCurrent(alert);
    }
}
