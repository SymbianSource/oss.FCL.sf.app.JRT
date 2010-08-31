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
package t_uirobot.textbox;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import spede.SpedeRunner;
import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * This module test tests setInitialInputMode in TextBox. Most test
 * cases check their results automatically by comparing the content of the
 * text box with the expected content at the end of the test case. Some test
 * cases take screenshots for checking the result.<br>
 * <br>
 * This test can be run in the the normal developer build (Engineering English),
 * in the Japan variant or in the China variant. Testing Thai variant is not
 * fully implemented but it could be added by specifying the expected
 * result (THAI_adgj). The used variant is hard-coded in the initial value
 * of the "variant" variable, and need to be changed manually before testing,
 * see the declaration of the "variant" variable.<br>
 * <br>
 * Five cases are ignored.<br>
 * They are not "safe" to run due to the input language selection from the
 * edit options menu:<br>
 * The input language could be wrongly set up for the next (sub)cases.
 * The input language dialog remembers the last selected language.
 * Therefore, the language should be selected
 * by using indexes relative to e.g. English (now it is absolute).
 * However, this assumes that the starting language would be always English,
 * and it will lead into complications especially if the language list
 * will change in future.
 * The current index in the language list can't be easily found because
 * the current input language
 * could not be the same as the selected language from list.
 *  A solution would be too complicated and making assumptions.
 * However, the tests change again the input language at runtime
 * (by LCDUI API) and these tests would not be necessary.
 * They actually compare the result to e.g. Arabic, Cyrillic characters,
 * which tests that the language was changed accordingly, in fact.
 * This is tested by other subcases.
 *
 * Note: The references images differ from 3.0 to 3.1
 */
public class TextBoxInputModeUITest extends UITestBase
{

    private static final int LENGTH = 2000;
    /* Possible build variants */
    private static final int DEVELOPER_BUILD = 0; // Engineering English
    private static final int JAPAN_VARIANT = 1;
    private static final int CHINA_VARIANT = 2;
    private static final int THAI_VARIANT = 3;

    /* This variable defines which build variant is in use */
    private int variant = DEVELOPER_BUILD;
    // private int variant = JAPAN_VARIANT;
    // private int variant = CHINA_VARIANT;
    // private int variant = THAI_VARIANT;


    /* Expected results for diffrent input modes */
    private static final String ARABIC_adgj =
        "" + (char) 1576 + (char) 1575 + (char) 1587 + (char) 1583;
    private static final String HEBREW_adgj =
        "" + (char) 1491 + (char) 1488 + (char) 1502 + (char) 1497;
    private static final String GREEK_Adgj =
        "" + (char) 913  + (char) 948  + (char) 951  + (char) 954;
    private static final String GREEK_adgj =
        "" + (char) 945  + (char) 948  + (char) 951  + (char) 954;
    private static final String CYRILLIC_Adgj =
        "" + (char) 1040  + (char) 1076  + (char) 1080  + (char) 1084;
    private static final String CYRILLIC_adgj =
        "" + (char) 1072  + (char) 1076  + (char) 1080  + (char) 1084;
    private static final String CHINA_adgj =
        "" + (char) 38463 + (char) 24694 + (char) 21704 + (char) 25289;
    private static final String HONGKONG_adgj =
        "" + (char) 26159 + (char) 21542 + (char) 30340 + (char) 35441
        + (char) 28858 + (char) 20102 + (char) 20057 + (char) 28911;
    private static final String TAIWAN_adgj =
        "" + (char) 12553 + (char) 30340 + (char) 12557 + (char) 30340
        + (char) 12560 + (char) 30340 + (char) 12563 + (char) 30340;
    private static final String KANJI_adgj =
        "" + (char) 12363 + (char) 12373 + (char) 12383 + (char) 12394;
    private static final String KATAKANA_adgj =
        "" + (char) 12459 + (char) 12469 + (char) 12479 + (char) 12490;
    private static final String HALFWIDTH_KATAKANA_adgj =
        "" + (char) 65398 + (char) 65403 + (char) 65408 + (char) 65413;
    private static final String FULLWIDTH_Adgj =
        "" + (char) 65313 + (char) 65348 + (char) 65351 + (char) 65354;
    private static final String FULLWIDTH_2345 =
        "" + (char) 65298 + (char) 65299 + (char) 65300 + (char) 65301;
    private static final String THAI_adgj =
        "" + (char) 1 + (char) 1 + (char) 1 + (char) 1;  // TO BE DEFINED !!!

    // ==============================================================

    /**
     * Constructor.
     */
    public TextBoxInputModeUITest()
    {
        super();
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public TextBoxInputModeUITest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /***************************************************************************
     * To create the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods.
     *
     * @return test suite
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        // Ignored cases
        /*aSuite.addTest(new TextBoxInputModeSTest(
            "testinputModesWithArabicInputLanguage",
            new TestMethod(){ public void run(TestCase tc) {
                ((TextBoxInputModeSTest) tc)
                .testinputModesWithArabicInputLanguage();} }));

        aSuite.addTest(new TextBoxInputModeSTest(
            "testinputModesWithCyrillicInputLanguage",
            new TestMethod(){ public void run(TestCase tc) {
                ((TextBoxInputModeSTest) tc)
                .testinputModesWithCyrillicInputLanguage();} }));

        aSuite.addTest(new TextBoxInputModeSTest(
            "testinputModesWithDanskInputLanguage",
            new TestMethod(){ public void run(TestCase tc) {
                ((TextBoxInputModeSTest) tc)
                .testinputModesWithDanskInputLanguage();} }));

        aSuite.addTest(new TextBoxInputModeSTest(
            "testIS_TRADITIONAL_HANZI1()",
            new TestMethod(){ public void run(TestCase tc) {
                ((TextBoxInputModeSTest) tc)
                .testIS_TRADITIONAL_HANZI1();} }));

        aSuite.addTest(new TextBoxInputModeSTest(
            "testIS_TRADITIONAL_HANZI2",
            new TestMethod(){ public void run(TestCase tc) {
                ((TextBoxInputModeSTest) tc)
                .testIS_TRADITIONAL_HANZI2();} }));

        aSuite.addTest(new TextBoxSTest("textboxPasswordTest()",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxSTest) tc).textboxPasswordTest();
                    }
                }
        ));
        ));
        */

        aSuite.addTest(new TextBoxInputModeUITest("testDefaultInputMode",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testDefaultInputMode();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_LATIN_DIGITS",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testIS_LATIN_DIGITS();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_LATIN",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testIS_LATIN();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testMIDP_UPPERCASE_LATIN",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testMIDP_UPPERCASE_LATIN();
            }
        }
                                                 ));
        aSuite.addTest(new TextBoxInputModeUITest("testMIDP_LOWERCASE_LATIN",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testMIDP_LOWERCASE_LATIN();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_BASIC_LATIN",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_BASIC_LATIN();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_GREEK",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_GREEK();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_CYRILLIC",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_CYRILLIC();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_HEBREW",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_HEBREW();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_ARABIC()",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_ARABIC();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_DEVANAGARI",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_DEVANAGARI();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_ARMENIAN",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_ARMENIAN();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_THAI",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_THAI();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_HIRAGANA()",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_HIRAGANA();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUCB_KATAKANA",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUCB_KATAKANA();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_HALFWIDTH_KATAKANA",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testIS_HALFWIDTH_KATAKANA();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_KANJI()",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testIS_KANJI();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_FULLWIDTH_DIGITS",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testIS_FULLWIDTH_DIGITS();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_FULLWIDTH_LATIN",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testIS_FULLWIDTH_LATIN();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_SIMPLIFIED_HANZI()",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testIS_SIMPLIFIED_HANZI();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testIS_TRADITIONAL_HANZI",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testIS_TRADITIONAL_HANZI();
            }
        }
                                                 ));


        aSuite.addTest(new TextBoxInputModeUITest("testnull",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testnull();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testUNKNOWN",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testUNKNOWN();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testempty",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testempty();
            }
        }
                                                 ));


        aSuite.addTest(new TextBoxInputModeUITest("testinputModeChange",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModeChange();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModeAndMenuOpen",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModeAndMenuOpen();
            }
        }
                                                 ));

        /*aSuite.addTest(new TextBoxInputModeSTest("testinputModesInNumber",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeSTest) tc).testinputModesInNumber();
                    }
                }
        ));

        aSuite.addTest(new TextBoxInputModeSTest("testinputModesInDecimal",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeSTest) tc).testinputModesInDecimal();
                    }
                }
        ));*/

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInEmailaddr",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputModesInEmailaddr();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInEmailaddr2",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputModesInEmailaddr2();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInUrl",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModesInUrl();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInUrl2",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModesInUrl2();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInPassword",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModesInPassword();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInPassword2",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputModesInPassword2();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInAny()",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModesInAny();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInAny2",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModesInAny2();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInCapsSentence",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputModesInCapsSentence();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModesInCapsWord()",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModesInCapsWord();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest(
                           "testinputModesInCapsSentenceEmailaddr",
                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputModesInCapsSentenceEmailaddr();
            }
        }
                       ));

        aSuite.addTest(new TextBoxInputModeUITest(
                           "testinputModesInCapsSentenceUrl",
                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputModesInCapsSentenceUrl();
            }
        }
                       ));

        /*aSuite.addTest(new TextBoxInputModeSTest(
            "testinputModesAfterNumberMode()",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeSTest) tc)
                            .testinputModesAfterNumberMode();
                    }
                }
        ));

        aSuite.addTest(new TextBoxInputModeSTest(
            "testinputModesAfterNumberMode2",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeSTest) tc)
                            .testinputModesAfterNumberMode2();
                    }
                }
        )); */

        aSuite.addTest(new TextBoxInputModeUITest(
                           "testinputLatinAfterOtherModes",
                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputLatinAfterOtherModes();
            }
        }
                       ));

        aSuite.addTest(new TextBoxInputModeUITest(
                           "testinputLatinAfterOtherModes2",
                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testinputLatinAfterOtherModes2();
            }
        }
                       ));

        aSuite.addTest(new TextBoxInputModeUITest("testinputModeNullSetting",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc).testinputModeNullSetting();
            }
        }
                                                 ));

        aSuite.addTest(new TextBoxInputModeUITest(
                           "testcommaInLowerAndUpperCase",
                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxInputModeUITest) tc)
                .testcommaInLowerAndUpperCase();
            }
        }
                       )); //*/

        return aSuite;
    }

    // ==============================================================

    /**
     * Tests default input mode, i.e. setInitialInputMode not called. The
     * default input mode depends on the build variant in use.
     */
    private void testDefaultInputMode()
    {
        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "NONE", // no initial input mode setting
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_LATIN_DIGITS"). The method is called before
     * displaying the text box. Number input mode will be used.
     */
    private void testIS_LATIN_DIGITS()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "IS_LATIN_DIGITS",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        "2345");                        // expected content
    }

    /**
     * Tests setInitialInputMode("IS_LATIN").
     * The method is called before displaying the text box.
     * Latin text input mode will be used
     */
    private void testIS_LATIN()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "IS_LATIN",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        "Adgj");                        // expected content
    }

    /**
     * Tests setInitialInputMode("MIDP_UPPERCASE_LATIN").
     * The method is called before displaying the text box.
     * Uppercase latin text input mode will be used.
     */
    private void testMIDP_UPPERCASE_LATIN()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "MIDP_UPPERCASE_LATIN",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        "ADGJ");                        // expected content
    }

    /**
     * Tests setInitialInputMode("MIDP_LOWERCASE_LATIN").
     * The method is called before displaying the text box.
     * Lowercase latin text input mode will be used.
     */
    private void testMIDP_LOWERCASE_LATIN()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "MIDP_LOWERCASE_LATIN",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        "adgj");                        // expected content
    }

    /**.
     * Tests setInitialInputMode("UCB_BASIC_LATIN").
     * The method is called before displaying the text box.
     * Latin text input mode will be used (same as "IS_LATIN").
     */
    private void testUCB_BASIC_LATIN()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_BASIC_LATIN",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        "Adgj");                        // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_GREEK").
     * The method is called before displaying the text box.
     * Greek input mode will be used.
     */
    private void testUCB_GREEK()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_GREEK",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        GREEK_Adgj);                    // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_CYRILLIC").
     * The method is called before displaying the text box.
     * Cyrillic input mode will be used.
     */
    private void testUCB_CYRILLIC()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_CYRILLIC",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        CYRILLIC_Adgj);                 // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_HEBREW").
     * The method is called before displaying the text box.
     * Hebrew input mode will be used.
     */
    private void testUCB_HEBREW()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_HEBREW",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presess
                        HEBREW_adgj);                   // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_ARABIC").
     * The method is called before displaying the text box.
     * Arabic input mode will be used.
     */
    private void testUCB_ARABIC()
    {
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_ARABIC",
                        "",                             // initial content
                        new int[] {'2', '3', '4', '5'}, // key presses
                        ARABIC_adgj);                   // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_DEVANAGARI").
     * The method is called before displaying the text box.
     * Devanagari/Hindi input mode will be used.
     */
    private void testUCB_DEVANAGARI()
    {

        String expectedContent = "\u0905\u090f\u0915\u091a";
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_DEVANAGARI",
                        "",            // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);  // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_ARMENIAN").
     * The method is called before displaying the text box.
     * This input mode is not supported in 3.0, so default input mode
     * will be used.
     */
    private void testUCB_ARMENIAN()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_ARMENIAN",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("UCB_THAI").
     * The method is called before displaying the text box.
     * This input mode is supported only in Thai variant; other variants
     * will use the default input mode.
     */
    private void testUCB_THAI()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_THAI",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("UCB_HIRAGANA").
     * The method is called before displaying the text box.
     * This input mode is supported only in Japan variant; other variants
     * will use the default input mode.
     */
    private void testUCB_HIRAGANA()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_HIRAGANA",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("UCB_KATAKANA").
     * The method is called before displaying the text box.
     * This input mode is supported only in Japan variant; other variants
     * will use the default input mode.
     */
    private void testUCB_KATAKANA()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KATAKANA_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "UCB_KATAKANA",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_HALFWIDTH_KATAKANA").
     * The method is called before displaying the text box.
     * This input mode is supported only in Japan variant; other variants
     * will use the default input mode.
     */
    private void testIS_HALFWIDTH_KATAKANA()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = HALFWIDTH_KATAKANA_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "IS_HALFWIDTH_KATAKANA",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_KANJI").
     * The method is called before displaying the text box.
     * This input mode is supported only in Japan variant; other variants
     * will use the default input mode.
     */
    private void testIS_KANJI()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "IS_KANJI",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_FULLWIDTH_DIGITS").
     * The method is called before displaying the text box.
     * This input mode is supported only in Japan variant; other variants
     * will use the default input mode.
     */
    private void testIS_FULLWIDTH_DIGITS()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = FULLWIDTH_2345;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "IS_FULLWIDTH_DIGITS",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_FULLWIDTH_LATIN").
     * The method is called before displaying the text box.
     * This input mode is supported only in Japan variant; other variants
     * will use the default input mode.
     */
    private void testIS_FULLWIDTH_LATIN()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = FULLWIDTH_Adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "IS_FULLWIDTH_LATIN",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_SIMPLIFIED_HANZI").
     * The method is called before displaying the text box.
     * This input mode is supported only in China variant; other variants
     * will use the default input mode.
     */
    private void testIS_SIMPLIFIED_HANZI()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "IS_SIMPLIFIED_HANZI",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_TRADITIONAL_HANZI").
     * The method is called before displaying the text box.
     * This input mode is supported only in China variant and has effect only
     * when user's display/input language is either Traditional Hongkong or
     * Traditional Taiwan; otherwise the default input mode will be used.
     */
    private void testIS_TRADITIONAL_HANZI()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "IS_TRADITIONAL_HANZI",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);

    }

    /**
         * Tests setInitialInputMode(null).
         * The method is called before displaying the text box. This
         * default input mode will be used.
         */
    private void testnull()
    {
        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, null,
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("UNKNOWN").
     * The method is called before displaying the text box.
     * The default input mode will be used.
     */
    private void testUNKNOWN()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "UNKNOWN",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }

    /**
     * Tests setInitialInputMode("").
     * The method is called before displaying the text box.
     * The default input mode will be used.
     */
    private void testempty()
    {

        String expectedContent = "Adgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj;
        }
        doInputModeTest(getName(),
                        TextField.ANY, "",
                        "",                             // initial content
                        inputKeys(new int[] {'2', '3', '4', '5'}), // key presses
                        expectedContent);
    }


    /**
     * Tests changing the initial input mode when the TextBox is
     * already visible. Setting the initial input mode has no effect until
     * focus is changed temporarily elsewhere and then back to TextBox.
     */
    private void testinputModeChange()
    {

        String expectedContent = "Adgj2345";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;  // Clear-keys used in chinese input
            // will remove latin numbers
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj + "2345";
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj + "2345";
        }
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[] {"IS_LATIN_DIGITS"},
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), true,
                                // keys are pressed both before and after the temporary
                                // focus change => input mode before the focus change
                                // will be the same as the original input mode although
                                // setInitialInputMode has already been called
                                expectedContent);
    }

    /**
     * Tests that initial input mode is not restored when EditOptions menu
     * has obscured the editor. Setting the initial input mode has no effect
     * until focus is changed temporarily elsewhere and then back to TextBox.
     */
    private void testinputModeAndMenuOpen()
    {

        String expectedContent = "Adgj2345";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;  // Clear-keys used in chinese input
            // will remove latin numbers
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj + "2345";
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj + "2345";
        }
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[] {"IS_LATIN_DIGITS"},
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', Key.Edit, Key.CBA2, '4', '5'}),
                                true,
                                // keys are pressed both before and after the temporary
                                // focus change => input mode before the focus change
                                // will be the same as the original input mode although
                                // setInitialInputMode has already been called
                                expectedContent);
    }

    /**
     * Tests different sequential input modes in NUMERIC text box.
     * Focus is changed after each setting to make the input mode effective.
     * Setting the initial input mode has no effect in NUMERIC text box.
     */
    private void testinputModesInNumber()
    {
        doInputModeSequenceTest(getName(),
                                TextField.NUMERIC, new String[]
                                {
                                    "NONE", // starts from default input mode
                                    "IS_LATIN", "IS_LATIN_DIGITS", "IS_FULLWIDTH_DIGITS",
                                    "UCB_ARABIC", "UCB_GREEK", "IS_KANJI",
                                },
                                "",                              // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // key presses (per input mode)
                                "2345234523452345234523452345"); // expected content
    }

    /**
     * Tests different sequential input modes in DECIMAL text box.
     * Focus is changed after each setting to make the input mode effective.
     * Setting the initial input mode has no effect in DECIMAL text box.
     */
    private void testinputModesInDecimal()
    {
        doInputModeSequenceTest(getName(),
                                TextField.DECIMAL, new String[]
                                {
                                    "NONE", // starts from default input mode
                                    "IS_LATIN", "IS_LATIN_DIGITS", "IS_FULLWIDTH_DIGITS",
                                    "UCB_ARABIC", "UCB_GREEK", "IS_KANJI",
                                },
                                "",                              // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // key presses (per input mode)
                                "2345234523452345234523452345"); // expected content
    }

    /**
     * Tests different sequential input modes in EMAILADDR text box.
     * Focus is changed after each setting to make the input mode effective.
     * In all variants the default input mode is latin with lower case.
     */
    private void testinputModesInEmailaddr()
    {
        doInputModeSequenceTest(getName(),
                                TextField.EMAILADDR, new String[]
                                {
                                    "NONE", // starts from default input mode
                                    "IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
                                    "UCB_ARABIC", "UCB_GREEK", "IS_LATIN_DIGITS",
                                },
                                "",                             // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // key presses (per input mode)
                                "adgjadgjADGJadgj" + ARABIC_adgj + GREEK_adgj + "2345");
        // expected content
    }

    /**
     * Tests different sequential input modes in EMAILADDR text box.
     * Focus is changed after each setting to make the input mode effective.
     * Setting a Japanese or Chinese input mode will have effect in
     * the corresponding variants.
     */
    private void testinputModesInEmailaddr2()
    {
        String expectedContent = "23452345234523452345";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj + CHINA_adgj + CHINA_adgj + CHINA_adgj;
            // Clear-keys used in chinese input will remove latin numbers
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = "23452345" + FULLWIDTH_2345 + KANJI_adgj
                              + KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = "2345234523452345" + THAI_adgj;
        }
        doInputModeSequenceTest(getName(),
                                TextField.EMAILADDR, new String[]
                                {
                                    "IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI",
                                    "IS_FULLWIDTH_DIGITS", "IS_KANJI", "UCB_THAI",
                                },
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                // key presses (per input mode)
                                expectedContent);
    }

    /**
     * Tests different sequential input modes in URL text box.
     * Focus is changed after each setting to make the input mode effective.
     * In all variants the default input mode is latin with lower case.
     */
    private void testinputModesInUrl()
    {
        doInputModeSequenceTest(getName(),
                                TextField.URL, new String[]
                                {
                                    "NONE", // default input mode in ANY text box
                                    "IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
                                    "UCB_ARABIC", "UCB_GREEK", "IS_LATIN_DIGITS",
                                },
                                "",                             // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // key presses (per input mode)
                                "adgjadgjADGJadgj" + ARABIC_adgj + GREEK_adgj + "2345");
        // expected content
    }

    /**
     * Tests different sequential input modes in URL text box.
     * Focus is changed after each setting to make the input mode effective.
     * Setting a Japanese or Chinese input mode will have effect in
     * the corresponding variants.
     */
    private void testinputModesInUrl2()
    {
        String expectedContent = "23452345234523452345";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj + CHINA_adgj + CHINA_adgj + CHINA_adgj;
            // Clear-keys used in chinese input removes latin numbers
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = "23452345" + FULLWIDTH_2345 + KANJI_adgj
                              + KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = "2345234523452345" + THAI_adgj;
        }
        doInputModeSequenceTest(getName(),
                                TextField.URL, new String[]
                                {
                                    "IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI",
                                    "IS_FULLWIDTH_DIGITS", "IS_KANJI", "UCB_THAI",
                                },
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                // key presses (per input mode)
                                expectedContent);
    }

    /**
     * Tests different sequential input modes in PASSWORD text box.
     * Focus is changed after each setting to make the input mode effective.
     * In all variants the default input mode is latin with lower case.
     */
    private void testinputModesInPassword()
    {
        doInputModeSequenceTest(getName(),
                                TextField.PASSWORD, new String[]
                                {
                                    "NONE", // default input mode in ANY text box
                                    "IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
                                    "UCB_ARABIC", "UCB_GREEK", /*"IS_LATIN_DIGITS", */
                                },
                                "",                             // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // key presses (per input mode)
                                "adgjadgjADGJadgj" + ARABIC_adgj + GREEK_adgj/* + "2345"*/);
        // expected content
    }

    /**
     * Tests different sequential input modes in PASSWORD text box.
     * Focus is changed after each setting to make the input mode effective.
     * Setting a Japanese or Chinese input mode will have effect in
     * the corresponding variants.
     */
    private void testinputModesInPassword2()
    {
        String expectedContent = "23452345234523452345";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj + CHINA_adgj + CHINA_adgj + CHINA_adgj;
            // Clear-keys used in chinese input removes latin numbers
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = "23452345" + FULLWIDTH_2345 + KANJI_adgj
                              + KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = "2345234523452345" + THAI_adgj;
        }
        doInputModeSequenceTest(getName(),
                                TextField.PASSWORD, new String[]
                                {
                                    "IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI",
                                    "IS_FULLWIDTH_DIGITS", "IS_KANJI", "UCB_THAI",
                                },
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                // pressed keys (per input mode)
                                expectedContent);
    }

    /**
     * Tests different sequential input modes in ANY text box.
     * Focus is changed after each setting to make the input mode effective.
     */
    private void testinputModesInAny()
    {
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[]
                                {
                                    "UCB_GREEK", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
                                    "UCB_ARABIC", "IS_LATIN_DIGITS",
                                },
                                "",                             // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // pressed keys (per input mode)
                                GREEK_Adgj + "ADGJadgj" + ARABIC_adgj + "2345");
        // expected content
    }

    /**
     * Tests different sequential input modes in ANY text box.
     * Focus is changed after each setting to make the input mode effective.
     * Setting a Japanese or Chinese input mode will have effect in
     * the corresponding variants.
     */
    private void testinputModesInAny2()
    {
        String expectedContent = "Adgjadgjadgjadgjadgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj + CHINA_adgj + CHINA_adgj
                              + CHINA_adgj + CHINA_adgj;
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj + KANJI_adgj
                              + FULLWIDTH_2345 + KANJI_adgj + KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj + THAI_adgj + THAI_adgj
                              + THAI_adgj + THAI_adgj;
        }
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[]
                                {
                                    "NONE", // starts from default input mode
                                    "IS_SIMPLIFIED_HANZI", "IS_FULLWIDTH_DIGITS", "IS_KANJI",
                                    "UCB_THAI",
                                },
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                // key presses (per input mode)
                                expectedContent);
    }

    /**
     * Tests that MIDP_UPPERCASE_LATIN and MIDP_LOWERCASE LATIN input modes are
     * ignored if the INITIAL_CAPS_SENTENCE modifier is defined in ANY text box.
     * Focus is changed after each setting to make the input mode effective.
     */
    private void testinputModesInCapsSentence()
    {
        String expectedContent = "Adgjadgj"  + ARABIC_adgj + "2345";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj + CHINA_adgj;
            // Clear-keys used in chinese input removes latin etc. chars
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj + KANJI_adgj
                              + ARABIC_adgj + "2345";
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj + THAI_adgj + ARABIC_adgj + "2345";
        }
        doInputModeSequenceTest(getName(),
                                TextField.INITIAL_CAPS_SENTENCE, new String[]
                                {
                                    "MIDP_LOWERCASE_LATIN", "MIDP_UPPERCASE_LATIN",
                                    "UCB_ARABIC", "IS_LATIN_DIGITS",
                                },
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                // key presses (per input mode)
                                expectedContent);
    }

    /**
     * Tests that MIDP_UPPERCASE_LATIN and MIDP_LOWERCASE LATIN input modes are
     * ignored if the INITIAL_CAPS_WORD modifier is defined in ANY text box.
     * Focus is changed after each setting to make the input mode effective.
     */
    private void testinputModesInCapsWord()
    {
        String expectedContent = "Adgjadgj"  + ARABIC_adgj + "2345";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj + CHINA_adgj;
            // Clear-keys used in chinese input removes latin etc. chars
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj + KANJI_adgj + ARABIC_adgj + "2345";
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj + THAI_adgj + ARABIC_adgj + "2345";
        }
        doInputModeSequenceTest(getName(),
                                TextField.INITIAL_CAPS_WORD, new String[]
                                {
                                    "MIDP_LOWERCASE_LATIN", "MIDP_UPPERCASE_LATIN",
                                    "UCB_ARABIC", "IS_LATIN_DIGITS",
                                },
                                "",                            // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                // key presses (per input mode)
                                expectedContent);
    }

    /**
     * Tests that MIDP_LOWERCASE_LATIN and MIDP_UPPERCASE_LATIN input modes
     * are not ignored in EMAILADDR having the INITIAL_CAPS_SENTENCE modfier
     * because the modifier is meaningless in EMAILADDR.
     * Focus is changed after each setting to make the input mode effective.
     */
    private void testinputModesInCapsSentenceEmailaddr()
    {
        doInputModeSequenceTest(getName(),
                                TextField.EMAILADDR | TextField.INITIAL_CAPS_SENTENCE,
                                new String[]
                                {"IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN"},
                                "",                            // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // key presses (per input mode)
                                "adgjADGJadgj");               // expected content
    }

    /**
     * Tests that MIDP_LOWERCASE_LATIN and MIDP_UPPERCASE_LATIN input modes
     * are not ignored in URL having the INITIAL_CAPS_SENTENCE modfier
     * because the modifier is meaningless in URL.
     * Focus is changed after each setting to make the input mode effective.
     */
    private void testinputModesInCapsSentenceUrl()
    {
        doInputModeSequenceTest(getName(),
                                TextField.URL | TextField.INITIAL_CAPS_SENTENCE, new String[]
                                {"IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN"},
                                "",                             // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                // key presses (per input mode)
                                "adgjADGJadgj");               // expected content
    }

    /**
     * Tests different textual input modes after IS_LATIN_DIGITS.
     * Focus is changed after each setting to make the input mode effective.
     */
    /*private void testinputModesAfterNumberMode() {
        doInputModeSequenceTest(getName(),
                TextField.ANY, new String[]
                {"IS_LATIN_DIGITS", "MIDP_UPPERCASE_LATIN",
                 "IS_LATIN_DIGITS", "MIDP_LOWERCASE_LATIN",
                 "IS_LATIN_DIGITS", "UCB_ARABIC", },
                 "",                             // initial content
                 new int[]{'2', '3', '4', '5'}, false,
                 "2345ADGJ2345adgj2345" + ARABIC_adgj);
    }*/

    /**
     * Tests Chinese and Japanese input modes after IS_LATIN_DIGITS.
     * Focus is changed after each setting to make the input mode effective.
     * Setting a Japanese or Chinese input mode will have effect only in
     * the corresponding variants.
     */
    /*private void testinputModesAfterNumberMode2() {
        String expectedContent = "23452345234523452345234523452345";
        if (variant == CHINA_VARIANT) {
            expectedContent = CHINA_adgj;
                  // Clear-keys used in chinese input removes latin numbers
        }
        else if (variant == JAPAN_VARIANT) {
            expectedContent = "234523452345" + KANJI_adgj
                + "2345" + FULLWIDTH_2345 + "23452345";
        }
        else if (variant == THAI_VARIANT) {
            expectedContent = "2345234523452345234523452345" + THAI_adgj;
        }
        doInputModeSequenceTest(getName(),
                TextField.ANY, new String[]
                {"IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI",
                 "IS_LATIN_DIGITS", "IS_KANJI",
                 "IS_LATIN_DIGITS", "IS_FULLWIDTH_DIGITS",
                 "IS_LATIN_DIGITS", "UCB_THAI", },
                 "",                            // initial content
                 inputKeys(new int[]{'2', '3', '4', '5'}), false,
                                                // key presses (per input mode)
                 expectedContent);
    }*/

    /**
     * Tests latin input modes after other input modes.
     * Focus is changed after each setting to make the input mode effective.
     */
    private void testinputLatinAfterOtherModes()
    {
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[]
                                {
                                    "UCB_HEBREW", "IS_LATIN",
                                    "UCB_GREEK", "MIDP_UPPERCASE_LATIN",
                                    "UCB_ARABIC", "MIDP_LOWERCASE_LATIN",
                                    "UCB_CYRILLIC", "IS_LATIN",
                                },
                                "",                             // initial content
                                new int[] {'2', '3', '4', '5'}, false,
                                HEBREW_adgj + "adgj" + GREEK_adgj + "ADGJ"
                                + ARABIC_adgj + "adgj" + CYRILLIC_adgj + "adgj");
    }

    /**
     * Tests latin input modes after Chinese and Japanese input modes.
     * Focus is changed after each setting to make the input mode effective.
     * Setting a Japanese or Chinese input mode will have effect only in
     * the corresponding variants.
     */
    private void testinputLatinAfterOtherModes2()
    {
        String expectedContent = "Adgjadgjadgjadgjadgjadgjadgjadgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj;
            // Clear-keys used in chinese input removes latin chars
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = KANJI_adgj + "adgj" + KANJI_adgj + "adgj"
                              + FULLWIDTH_2345 + "adgjadgjadgj";
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = THAI_adgj + "adgjadgjadgjadgjadgj"
                              + THAI_adgj + "adgj";
        }
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[]
                                {
                                    "IS_SIMPLIFIED_HANZI", "MIDP_LOWERCASE_LATIN",
                                    "IS_KANJI", "MIDP_LOWERCASE_LATIN",
                                    "IS_FULLWIDTH_DIGITS", "MIDP_LOWERCASE_LATIN",
                                    "UCB_THAI", "MIDP_LOWERCASE_LATIN",
                                },
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                // key presses (per input mode)
                                expectedContent);
    }

    /**
     * Tests null input mode after different input modes in ANY text box.
     */
    private void testinputModeNullSetting()
    {
        // NOTE: upper cases should be corrected!
        String expectedContent = "2345adgj" + ARABIC_adgj + "adgj"
                                 + "adgjadgjadgjadgjadgjadgjadgjadgj";
        if(variant == CHINA_VARIANT)
        {
            expectedContent = CHINA_adgj + CHINA_adgj
                              + CHINA_adgj + CHINA_adgj + CHINA_adgj + CHINA_adgj
                              + CHINA_adgj + CHINA_adgj + CHINA_adgj + CHINA_adgj;
            // Clear-keys used in chinese input removes latin etc. chars
        }
        else if(variant == JAPAN_VARIANT)
        {
            expectedContent = "2345" + KANJI_adgj + ARABIC_adgj + KANJI_adgj
                              + KANJI_adgj + KANJI_adgj + HALFWIDTH_KATAKANA_adgj
                              + KANJI_adgj + FULLWIDTH_2345 + KANJI_adgj + KANJI_adgj
                              + KANJI_adgj;
        }
        else if(variant == THAI_VARIANT)
        {
            expectedContent = "2345A" + THAI_adgj + ARABIC_adgj + THAI_adgj
                              + THAI_adgj + THAI_adgj + THAI_adgj + THAI_adgj
                              + THAI_adgj + THAI_adgj + THAI_adgj + THAI_adgj;
        }
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[]
                                {
                                    "IS_LATIN_DIGITS", null,
                                    "UCB_ARABIC", null,
                                    "IS_SIMPLIFIED_HANZI", null,
                                    "IS_HALFWIDTH_KATAKANA", null,
                                    "IS_FULLWIDTH_DIGITS", null,
                                    "UCB_THAI", null,
                                },
                                "",                             // initial content
                                inputKeys(new int[] {'2', '3', '4', '5'}), false,
                                expectedContent);
    }

    /**
     * Tests comma and space in MIDP_LOWERCASE_LATIN and MIDP_UPPERCASE_LATIN
     * text box. Note: after a comma and space the input mode changes to
     * Text case (not sure whether this is correct behaviour)
     */
    private void testcommaInLowerAndUpperCase()
    {
        doInputModeSequenceTest(getName(),
                                TextField.ANY, new String[]
                                {"MIDP_LOWERCASE_LATIN", "MIDP_UPPERCASE_LATIN"},
                                "",                            // initial content
                                new int[] {'2', '3', '1', '0', '4', '5' }, false,
                                // key presses (per input mode)
                                "ad. GjAD. Gj");
    }



    /**
     * Private method for testing initial input mode. Creates TextBox and
     * sets the initial input mode before displaying it.
     * Generates the given key presses. At the end compares the content
     * of TextBox to the expected content, unless the expected content is null;
     * in that case takes a screenshot.
     *
     * @param testCaseName Name of the test case
     * @param constraints Combination of TextField.ANY,...
     * @param initialInputMode Intial input mode to be set, e.g "IS_LATIN";
     *      "NONE" means that setInitialInputMode is not called at all
     * @param initialContent Initial context of TextBox
     * @param pressedKeys Array of key presses to be generated
     * @param expectedContent Expected content of TextBox
     */
    private void doInputModeTest(String testCaseName,
                                 int constraints, String initialInputMode, String initialContent,
                                 int[] pressedKeys, String expectedContent)
    {

        boolean passed = true;
        SpedeRunner spede = (SpedeRunner) SpedeRunner.getInstance();

        try
        {
            // Create TextBox
            TextBox textBox = new TextBox(
                testCaseName, // title
                initialContent, LENGTH, constraints);

            // Set initial input mode before displaying TextBox
            if("NONE".equals(initialInputMode))
            {
                // initial input mode not set
            }
            else
            {
                textBox.setInitialInputMode(initialInputMode);
                block(KEYEVENT_DELAY);
            }
            setCurrent(textBox);

            // Do key presses
            if(pressedKeys != null)
            {
                for(int i = 0; i < pressedKeys.length; i++)
                {
                    key(pressedKeys[i]);
                }
            }

            // Check the result of key presses (i.e. the used input mode)
            passed = checkResult(testCaseName, textBox,
                                 expectedContent, expectedContent == null ? true : false);
        }
        catch(Exception exc)
        {
            spede.addToResultsList(testCaseName + " test failed: "
                                   + exc.toString());
            spede.addToResultsList(exc);
            passed = false;
        }
        assertTrue(testCaseName, passed);
    }

    /**
     * Private method for testing several initial input mode settings.
     * Creates TextBox and sets given initial input modes one after another
     * while TextBox is visible. The focus is changed to another TextBox
     * between each setting in order to make the setting effective.
     * Generates the given key presses after each setting.
     * At the end compares the text box content to the expected content,
     * unless the expected content is null; in that case takes a screenshot.
     *
     * @param testCaseName Name of the test case
     * @param constraints Combination of TextField.ANY,...
     * @param nextInputMode Array of intial input modes to be set. e.g.
     *      {"NONE", "IS_LATIN, "IS_LATIN_DIGITS"}
     * @param initialContent Initial context of the TextBox
     * @param pressedKeys Array of key presses to be generated
     * @param beforeAndAfter If true, key presses are done both before
     *     and after the setIntialInputMode call, otherwise only after it
     * @param expectedContent Expected content of TextBox
     */
    private void doInputModeSequenceTest(String testCaseName,
                                         int constraints, String[] nextInputModes, String initialContent,
                                         int[] pressedKeys, boolean beforeAndAfter,
                                         String expectedContent)
    {

        boolean passed = true;
        SpedeRunner spede = (SpedeRunner) SpedeRunner.getInstance();

        try
        {
            // Create TextBox
            TextBox textBox = new TextBox(testCaseName,
                                          initialContent, LENGTH, constraints);

            // Create another TextBox in order to be able to change focus
            TextBox other = new TextBox("Focus change",
                                        "Temporary text box", LENGTH, TextField.ANY);

            // Display the original TextBox
            Display.getDisplay(getMIDlet()).setCurrent(textBox);
            block(CHANGE_DISPLAYABLE_DELAY);

            for(int m = 0; m < nextInputModes.length; m++)
            {

                // Set initial input mode while the original TextBox is visible
                textBox.setInitialInputMode(nextInputModes[m]);

                if(beforeAndAfter)
                {
                    // Do key presses (input mode setting not yet effective)
                    if(pressedKeys != null)
                    {
                        for(int i = 0; i < pressedKeys.length; i++)
                        {
                            key(pressedKeys[i]);
                        }
                    }
                }

                // Display the other TextBox for a while and then the
                //  original TextBox => new editing session starts
                setCurrent(other);
                setCurrent(textBox);

                // Do key presses again (now input mode setting is effective)
                if(pressedKeys != null)
                {
                    for(int i = 0; i < pressedKeys.length; i++)
                    {
                        key(pressedKeys[i]);
                    }
                }

            }

            // Check the result of all key presses (i.e. the used input modes)
            passed = checkResult(testCaseName, textBox,
                                 expectedContent, expectedContent == null ? true : false);

        }
        catch(Exception exc)
        {
            spede.addToResultsList(testCaseName + " test failed: "
                                   + exc.toString());
            spede.addToResultsList(exc);
            passed = false;
        }
        assertTrue(testCaseName, passed);

    }

    /**
     * Checks whether the content of the given text box corresponds to
     * the expected content.
     *
     * @param testCaseName Name of the test case
     * @param textBox TextBox to be checked
     * @param expectedContent The expected content
     * @param takeScreenshot If true, screenshot is taken
     * @return result Returns whether test case passed or failed
     */
    private boolean checkResult(String testCaseName,
                                TextBox textBox, String expectedContent, boolean takeScreenshot)
    {

        boolean passed = true;

        // Check the expected content
        if(expectedContent != null)
        {
            if(!expectedContent.equals(textBox.getString()))
            {
                passed = false;
                print(testCaseName + " test failed: content: " + textBox.getString()
                      + ", expected: " + expectedContent);
            }
        }

        // Take the screenshot and compare it with the previous one
        if(takeScreenshot)
        {
            if(!takeScreenshot(testCaseName + ".gif"))
            {
                passed = false;
                print(testCaseName + " test "
                      + " failed: screenshot does not match");
            }
        }

        if(passed)
        {
            print(testCaseName + " test succeeded: " + textBox.getString());
        }
        return passed;
    }

    /**
     * Adds key presses needed for Chinese and Japanese input methods
     *
     * @param keys Key presses according to westen input
     * @return Key presses for the current variant
     */
    private int[] inputKeys(int[] keys)
    {

        int[] input = null;
        int k = 0;

        if(variant == CHINA_VARIANT)
        {
            input = new int[4 * keys.length];
            for(int i = 0; i < keys.length; i++)
            {
                input[k] = keys[i];
                k = k + 1;
                if(keys[i] >= '0' && keys[i] <= '9')
                {
                    input[k] = Key.Select;
                    input[k + 1] = Key.Select;
                    input[k + 2] = Key.Clear;
                    k = k + 3;
                }
            }
        }
        else if(variant == JAPAN_VARIANT)
        {
            input = new int[2 * keys.length];
            for(int i = 0; i < keys.length; i++)
            {
                input[k] = keys[i];
                k = k + 1;
                if(keys[i] >= '0' && keys[i] <= '9')
                {
                    input[k] = Key.Select;
                    k = k + 1;
                }
            }
        }
        else
        {
            input = keys;
            k = input.length;
        }

        // Set the array size correct
        if(k < input.length)
        {
            int[] temp = new int[k];
            System.arraycopy(input, 0, temp, 0, temp.length);
            input = temp;
        }
        return input;
    }

}

