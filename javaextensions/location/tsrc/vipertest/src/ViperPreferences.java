/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
import javax.microedition.lcdui.*;
import javax.microedition.location.*;

class ViperPreferences extends Form implements CommandListener
{
    int iInterval = -1;

    int iTimeout = 10;

    int iMaxAge = -1;

    int iCoordFormat = 0;

    private Command iOkCmd = new Command("OK", Command.OK, 1);

    private Display iDisplay;

    private Displayable iPreviousScreen;

    private TextField iIntervalField = new TextField("Interval",
            "" + iInterval, 5, TextField.NUMERIC);

    private TextField iTimeoutField = new TextField("Timeout", "" + iTimeout,
            5, TextField.NUMERIC);

    private TextField iMaxAgeField = new TextField("Max age", "" + iMaxAge, 5,
            TextField.NUMERIC);

    // private ChoiceGroup iCoordFormatChoice =
    // new ChoiceGroup("Coordinate display format",
    // Choice.POPUP,
    // new String[] {"DD",
    // "DD MM",
    // "DD MM SS"},
    // null);

    ViperPreferences(Display aDisplay, Displayable aPreviousScreen)
    {
        super("Preferences");
        iDisplay = aDisplay;
        iPreviousScreen = aPreviousScreen;
        addCommand(iOkCmd);
        setCommandListener(this);
        append(iIntervalField);
        append(iTimeoutField);
        append(iMaxAgeField);
        // append(iCoordFormatChoice);
    }

    public void commandAction(Command c, Displayable d)
    {
        if (c == iOkCmd)
        {
            iInterval = Integer.parseInt(iIntervalField.getString());
            iTimeout = Integer.parseInt(iTimeoutField.getString());
            iMaxAge = Integer.parseInt(iMaxAgeField.getString());
            // iCoordFormat = 0;
            // switch (iCoordFormatChoice.getSelectedIndex())
            // {
            // case 1:
            // iCoordFormat = Coordinates.DD_MM;
            // break;
            // case 2:
            // iCoordFormat = Coordinates.DD_MM_SS;
            //                break;
            //            }
            iDisplay.setCurrent(iPreviousScreen);
        }
    }
}
