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
 * This test class will test Ticker text:
 * 1.contains whitespaces
 * 2.one character
 * 3.longer than the line
 */

public class Test1 extends List implements CommandListener
{

    //the reference to the MIDlet
    private MIDlet parent = null;

    // The ticker which is added to the Form screen
    private ScreenTickerTests st = null;

    // Ticker to test
    private Ticker testTicker = null;

    //String to test white spaces
    private final String whiteSpaces = "Ticker with whitespaces";

    //String to test one character
    private final String oneChar = "Ticker with one char";

    //String to test the ticker text which is longer than line
    private final String longerThanLine = "Ticker longer than a line";

    //String to test newline character
    private final String newlineChar = "Ticker with newline char";

    private final String hebrew = "Ticker with hebrew";
    private char heb_data[] = {0x05D0, 0x05D1, 0x05D2, 0x05E0};
    private String hebrewChars = new String(heb_data);
    private final String arabic = "Ticker with arabic";
    private char arab_data[] = {0x0660, 0x0661, 0x0662, 0x0641};
    private String arabicChars = new String(arab_data);

    private Command cmdOk = new Command("OK", Command.OK, 1);

    private Command cmdBack = new Command("Back", Command.BACK, 1);

    /**
     *
     * @param st
     */
    public Test1(ScreenTickerTests st)
    {
        //title of the List and the List type
        super("Test1", List.IMPLICIT);
        this.st = st;

        //get the parent MIDlet
        this.parent = ScreenTickerTests.getParent();
        testTicker = new Ticker("Hello MIDP");

        append(whiteSpaces, null);
        append(oneChar, null);
        append(longerThanLine, null);
        append(newlineChar, null);
        append(hebrew, null);
        append(arabic, null);
        addCommand(cmdOk);
        addCommand(cmdBack);
        setCommandListener(this);
    }
    /**
     *  This method handles command invocations.
     *
     *@param  c  This is the command responsible for the event.
     *@param  s  Should be equal to this.
     */
    public void commandAction(Command c, Displayable s)
    {
        int index = getSelectedIndex();
        String element = getString(index);
        if (c == SELECT_COMMAND || c == cmdOk)
        {
            st.setTicker(testTicker);
            if (element.equals(whiteSpaces))
                testTicker.setString("  ");
            else if (element.equals(oneChar))
                testTicker.setString("N");
            else if (element.equals(longerThanLine))
                testTicker.setString("The length of the text is longer than one" +
                                     " line and should take at least two lines.");
            else if (element.equals(newlineChar))
                testTicker.setString("Newline char \n");
            else if (element.equals(hebrew))
                testTicker.setString(hebrewChars);
            else if (element.equals(arabic))
                testTicker.setString(arabicChars);

            ScreenTickerTests.show();
        }
        else if (c == cmdBack)
            Display.getDisplay(parent).setCurrent(st);
    }
}

