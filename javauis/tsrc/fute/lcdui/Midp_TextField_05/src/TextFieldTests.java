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
 * This class is to test the below TextField test case:
 * -setInitialInputMode(String characterSubset): Initial mode set to
 *
 */

public class TextFieldTests extends Form implements CommandListener
{

    //parent MIDlet
    private static Midp_TextField_05 parent = null;

    //TextFields to test
    private TextField tf1 = null;

    private TextField tf2 = null;

    private TextField tf3 = null;

    private TextField tf4 = null;

    private TextField tf5 = null;

    private TextField tf6 = null;

    Command cmdNonLatin = new Command("Non-Latin", Command.SCREEN, 1);
    Command cmdIndian = new Command("Indian", Command.SCREEN, 1);
    Command cmdJapanese = new Command("Japanese", Command.SCREEN, 1);
    Command cmdKorean = new Command("Korean", Command.SCREEN, 1);
    Command cmdChinese = new Command("Chinese", Command.SCREEN, 1);
    Command cmdLatin = new Command("Latin", Command.SCREEN, 1);
    private Command cmdExit = new Command("Exit", Command.EXIT, 1);

    //maximum size which is specified while creating TextField
    private final int MAX_SIZE = 100;

    /**
     *
     * @param parent The parent MIDlet of this class
     */
    public TextFieldTests(Midp_TextField_05 parent)
    {
        //set the title of the form as the name of the MIDlet
        super(parent.getClass().getName());
        addCommand(cmdNonLatin);
        addCommand(cmdIndian);
        addCommand(cmdJapanese);
        addCommand(cmdKorean);
        addCommand(cmdChinese);
        addCommand(cmdLatin);
        addCommand(cmdExit);

        setCommandListener(this);
        this.parent = parent;
        tf1 = new TextField("MIDP_UPPERCASE_LATIN", null, MAX_SIZE, TextField.ANY);
        tf1.setInitialInputMode("MIDP_UPPERCASE_LATIN");

        tf2 = new TextField("MIDP_LOWERCASE_LATIN", null, MAX_SIZE, TextField.ANY);
        tf2.setInitialInputMode("MIDP_LOWERCASE_LATIN");

        tf3 = new TextField("IS_LATIN", null, MAX_SIZE, TextField.ANY);
        tf3.setInitialInputMode("IS_LATIN");

        tf4 = new TextField("UCB_BASIC_LATIN", null, MAX_SIZE, TextField.ANY);
        tf4.setInitialInputMode("UCB_BASIC_LATIN");

        tf5 = new TextField(null, null, MAX_SIZE, TextField.ANY);

        tf6 = new TextField(null, null, MAX_SIZE, TextField.ANY);

        append(tf1);
        append(tf2);
        append(tf3);
        append(tf4);

        Display.getDisplay(parent).setCurrent(this);
    }
    public void clearFields()
    {
        deleteAll();
        tf1.setInitialInputMode(null);
        tf2.setInitialInputMode(null);
        tf1.setString(null);
        tf2.setString(null);
        tf3.setString(null);
        tf4.setString(null);
        tf5.setString(null);
        tf6.setString(null);
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdLatin)
        {
            clearFields();

            tf1.setLabel("MIDP_UPPERCASE_LATIN");
            tf1.setInitialInputMode("MIDP_UPPERCASE_LATIN");

            tf2.setLabel("MIDP_LOWERCASE_LATIN");
            tf2.setInitialInputMode("MIDP_LOWERCASE_LATIN");

            tf3.setLabel("IS_LATIN");
            tf3.setInitialInputMode("IS_LATIN");

            tf4.setLabel("UCB_BASIC_LATIN");
            tf4.setInitialInputMode("UCB_BASIC_LATIN");

            append(tf1);
            append(tf2);
            append(tf3);
            append(tf4);
        }
        else if (c == cmdNonLatin)
        {
            clearFields();
            tf1.setLabel("UCB_GREEK");
            tf1.setInitialInputMode("UCB_GREEK");

            tf2.setLabel("UCB_CYRILLIC");
            tf2.setInitialInputMode("UCB_CYRILLIC");

            tf3.setLabel("UCB_ARMENIAN");
            tf3.setInitialInputMode("UCB_ARMENIAN");

            tf4.setLabel("UCB_HEBREW");
            tf4.setInitialInputMode("UCB_HEBREW");

            tf5.setLabel("UCB_ARABIC");
            tf5.setInitialInputMode("UCB_ARABIC");

            tf6.setLabel("UCB_THAI");
            tf6.setInitialInputMode("UCB_THAI");
            append(tf1);
            append(tf2);
            append(tf3);
            append(tf4);
            append(tf5);
            append(tf6);
        }
        else if (c == cmdIndian)
        {
            clearFields();
            tf1.setLabel("UCB_DEVANAGARI");
            tf1.setInitialInputMode("UCB_DEVANAGARI");

            tf2.setLabel("UCB_BENGALI");
            tf2.setInitialInputMode("UCB_BENGALI");
            append(tf1);
            append(tf2);
        }
        else if (c == cmdJapanese)
        {
            clearFields();
            tf1.setLabel("UCB_HIRAGANA");
            tf1.setInitialInputMode("UCB_HIRAGANA");

            tf2.setLabel("UCB_KATAKANA");
            tf2.setInitialInputMode("UCB_KATAKANA");

            tf3.setLabel("IS_FULLWIDTH_DIGITS");
            tf3.setInitialInputMode("IS_FULLWIDTH_DIGITS");

            tf4.setLabel("IS_FULLWIDTH_LATIN");
            tf4.setInitialInputMode("IS_FULLWIDTH_LATIN");

            tf5.setLabel("IS_HALFWIDTH_KATAKANA");
            tf5.setInitialInputMode("IS_HALFWIDTH_KATAKANA");

            tf6.setLabel("IS_KANJI");
            tf6.setInitialInputMode("IS_KANJI");

            append(tf1);
            append(tf2);
            append(tf3);
            append(tf4);
            append(tf5);
            append(tf6);
        }
        else if (c == cmdKorean)
        {
            clearFields();
            tf1.setLabel("IS_HANJA");
            tf1.setInitialInputMode("IS_HANJA");

            tf2.setLabel("UCB_HANGUL_SYLLABLES");
            tf2.setInitialInputMode("UCB_HANGUL_SYLLABLES");
            append(tf1);
            append(tf2);
        }
        else if (c == cmdChinese)
        {
            clearFields();
            tf1.setLabel("IS_SIMPLIFIED_HANZI");
            tf1.setInitialInputMode("IS_SIMPLIFIED_HANZI");

            tf2.setLabel("IS_TRADITIONAL_HANZI");
            tf2.setInitialInputMode("IS_TRADITIONAL_HANZI");
            append(tf1);
            append(tf2);
        }
        else if (c == cmdExit)
        {
            parent.destroyApp(false);
            parent.notifyDestroyed();
        }
    }
}
