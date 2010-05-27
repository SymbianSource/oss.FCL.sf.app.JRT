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
 * This class is to test the various methods of Gauge
 *
 */

public class GaugeMethods extends Form implements CommandListener
{

    private MIDlet m = null;

    private String listMethodsStr[] = {"SetLabel", "SetValue", "SetMaxValue"};

    private String listSetMaxValueStr[] = {"increase", "decrease"};

    private String listSetValueStr[] = {"0", "1/2", "1"};

    private String listSetLabelStr[] = {"Label", "NULL"};

    private String methodsScrn = "methods";

    private String mainScrn = "main";

    private List listMethods = null;

    private List listSetMaxValue = null;

    private List listSetValue = null;

    private List listSetLabel = null;

    private Gauge g = null;

    private Command cmdBack = new Command("Back", Command.BACK, 1);

    private Command cmdOk = new Command("Ok", Command.OK, 1);

    private Command cmdMethods = new Command("Methods", Command.SCREEN, 1);

    private Command cmdSet = new Command("Set", Command.OK, 1);

    public GaugeMethods(MIDlet m, boolean type)
    {
        super(m.getClass().getName());
        this.m = m;
        if (type)
            g = new Gauge("Interactive Gauge", type, 10, 5);
        else
            g = new Gauge("Non-interactive Gauge", type, 10, 5);
        append(g);
        addCommand(cmdBack);
        addCommand(cmdMethods);
        setCommandListener(this);
    }

    private void addListMethods()
    {
        listMethods = new List("Methods", List.IMPLICIT, listMethodsStr, null);
        listMethods.addCommand(cmdBack);
        listMethods.addCommand(cmdOk);
        listMethods.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listMethods);
    }

    private void addListSetMaxValue()
    {
        listSetMaxValue = new List("SetMaxValue", List.IMPLICIT, listSetMaxValueStr, null);
        listSetMaxValue.addCommand(cmdBack);
        listSetMaxValue.addCommand(cmdSet);
        listSetMaxValue.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetMaxValue);
    }

    private void addListSetValue()
    {
        listSetValue = new List("SetValue", List.IMPLICIT, listSetValueStr, null);
        listSetValue.addCommand(cmdBack);
        listSetValue.addCommand(cmdSet);
        listSetValue.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetValue);
    }

    private void addListSetLabel()
    {
        listSetLabel = new List("SetLabel", List.IMPLICIT, listSetLabelStr, null);
        listSetLabel.addCommand(cmdBack);
        listSetLabel.addCommand(cmdSet);
        listSetLabel.setCommandListener(this);
        Display.getDisplay(m).setCurrent(listSetLabel);
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
            else if (s == listSetMaxValue || s == listSetValue || s == listSetLabel)
                Display.getDisplay(m).setCurrent(listMethods);
            else
            {
                FormUpdateTests.show();
            }
        }
        else if (c == cmdMethods)
        {
            addListMethods();
        }
        else if (c == cmdSet)
        {
            if (s == listSetMaxValue)
            {
                int index = listSetMaxValue.getSelectedIndex();
                String element = listSetMaxValue.getString(index);
                if (element.equals(listSetMaxValueStr[0]))
                    g.setMaxValue(g.getMaxValue() + 5);
                else
                    g.setMaxValue(g.getMaxValue() - 5);
            }
            else if (s == listSetValue)
            {
                int index = listSetValue.getSelectedIndex();
                String element = listSetValue.getString(index);
                if (element.equals(listSetValueStr[0]))
                    g.setValue(0);
                else if (element.equals(listSetValueStr[1]))
                    g.setValue(g.getMaxValue()/2);
                else if (element.equals(listSetValueStr[2]))
                    g.setValue(1);
            }
            else if (s == listSetLabel)
            {
                int index = listSetLabel.getSelectedIndex();
                String element = listSetLabel.getString(index);
                if (element.equals(listSetLabelStr[0]))
                    g.setLabel("Gauge label which should be too long to fit on one line");
                else
                    g.setLabel(null);
            }
            Display.getDisplay(m).setCurrent(this);
        }
        else if (c == cmdOk || c == List.SELECT_COMMAND)
        {
            int index = listMethods.getSelectedIndex();
            String element = listMethods.getString(index);
            if (element.equals(listMethodsStr[0]))
                addListSetLabel();
            if (element.equals(listMethodsStr[1]))
                addListSetValue();
            if (element.equals(listMethodsStr[2]))
                addListSetMaxValue();
        }
    }
}






