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
package com.nokia.openlcdui.mt_uirobot.textfield;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * RUN AS A FIRST/ONLY TEST. If something is in the clipboard, the results may
 * be incorrect.
 *
 *
 * This module test tests setInitialInputMode in TextField. Most test cases
 * check their results automatically by comparing the content of the text field
 * with the expected content at the end of the test case. Some test cases take
 * screenshots for checking the result.
 *
 * Currently this test can be run only in the the normal developer build
 * (Engineering English). If you want to run this test in the Japan, China or
 * Thai variant, you need to modify the test cases and expected results similar
 * to the test cases in textbox.TextBoxSetInitialInputModeTest.
 *
 * Three cases are ignored. They are not "safe" to run due to the input language
 * selection from the edit options menu: the input language could be wrongly set
 * up for the next (sub)cases. The input language dialog remembers the last
 * selected language. Therefore, the language should be selected by using
 * indexes relative to e.g. English (now it is absolute). However, this assumes
 * that the starting language would be always English, and it will lead into
 * complications especially if the language list will change in future. The
 * current index in the language list can't be easily found because the current
 * input language could not be the same as the selected language from list. A
 * solution would be too complicated and making assumptions. However, the tests
 * change again the input language at runtime (by LCDUI API) and these tests
 * would not be necessary. They compare the result to e.g. Arabic, Cyrillic
 * characters, which tests that the language was changed accordingly, in fact.
 * This is tested by other subcases.
 *
 * The case testinsertLineFeed was also ignored on 2006-11-20 due the above
 * reason. Note: The references images differ from 3.0 to 3.1, and 3.2
 */

public class TextFieldSetInitialInputModeTest extends UITestBase {

    /* Expected results for diffrent input modes */
    private static final String ARABIC_adgj = "" + (char) 1576 + (char) 1575
            + (char) 1587 + (char) 1583;
    private static final String ARABIC_ad_dot = "" + (char) 1576 + (char) 1575
            + ". ";
    private static final String HEBREW_adgj = "" + (char) 1491 + (char) 1488
            + (char) 1502 + (char) 1497;
    private static final String GREEK_Adgj = "" + (char) 913 + (char) 948
            + (char) 951 + (char) 954;
    private static final String GREEK_adgj = "" + (char) 945 + (char) 948
            + (char) 951 + (char) 954;
    private static final String GREEK_Ad_dot = "" + (char) 913 + (char) 948
            + ". ";
    private static final String CYRILLIC_Adgj = "" + (char) 1040 + (char) 1076
            + (char) 1080 + (char) 1084;
    private static final String CYRILLIC_adgj = "" + (char) 1072 + (char) 1076
            + (char) 1080 + (char) 1084;

    // ==============================================================
    private final int normalSize = 2000;

    /**
	 * Default constructor.
	 */
	public TextFieldSetInitialInputModeTest() {
	}

	/**
	 * Constructor.
	 *
	 * @param sTestName
	 *            name of testcase.
	 * @param rTestMethod
	 *            test method.
	 */
	public TextFieldSetInitialInputModeTest(String sTestName) {
		super(sTestName);
	}

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = TextFieldSetInitialInputModeTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new TextFieldSetInitialInputModeTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testDefaultInputMode");
        methodNames.addElement("testIS_LATIN_DIGITS");
        methodNames.addElement("testIS_LATIN_DIGITS2");
        methodNames.addElement("testIS_LATIN");
        methodNames.addElement("testMIDP_UPPERCASE_LATIN");
        methodNames.addElement("testMIDP_LOWERCASE_LATIN");
        methodNames.addElement("testUCB_BASIC_LATIN");
        methodNames.addElement("testUCB_GREEK");
        methodNames.addElement("testUCB_CYRILLIC");
        methodNames.addElement("testUCB_HEBREW");
        methodNames.addElement("testUCB_ARABIC");
        methodNames.addElement("testUCB_DEVANAGARI");
        methodNames.addElement("testUCB_ARMENIAN");
        methodNames.addElement("testUCB_THAI");
        methodNames.addElement("testUCB_HIRAGANA");
        methodNames.addElement("testUCB_KATAKANA");
        methodNames.addElement("testIS_HALFWIDTH_KATAKANA");
        methodNames.addElement("testIS_KANJI");
        methodNames.addElement("testIS_FULLWIDTH_DIGITS");
        methodNames.addElement("testIS_FULLWIDTH_LATIN");
        methodNames.addElement("testIS_SIMPLIFIED_HANZI");
        methodNames.addElement("testIS_TRADITIONAL_HANZI");
        methodNames.addElement("testnull");
        methodNames.addElement("testUNKNOWN");
        methodNames.addElement("testempty");
        methodNames.addElement("testinputModeChange");
        methodNames.addElement("testinputModeAndMenuOpen");
        methodNames.addElement("testinputModesInEmailaddr");
        methodNames.addElement("testinputModesInEmailaddr2");
        methodNames.addElement("testinputModesInUrl");
        methodNames.addElement("testinputModesInUrl2");
        methodNames.addElement("testinputModesInPassword");
        methodNames.addElement("testinputModesInPassword2");
        methodNames.addElement("testinputModesInAny");
        methodNames.addElement("testinputModesInAny2");
        methodNames.addElement("testinputModesInCapsSentence");
        methodNames.addElement("testinputModesInCapsWord");
        methodNames.addElement("testinputModesInCapsSentenceEmailaddr");
        methodNames.addElement("testinputModesInCapsSentenceUrl");
        methodNames.addElement("testinputLatinAfterOtherModes");
        methodNames.addElement("testinputLatinAfterOtherModes2");
        methodNames.addElement("testinputModeNullSetting");
        methodNames.addElement("testcommaInLowerAndUpperCase");
        methodNames.addElement("testUppercaseBeforeMakingFormCurrent");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testDefaultInputMode")) testDefaultInputMode();
        else if (getName().equals("testIS_LATIN_DIGITS")) testIS_LATIN_DIGITS();
        else if (getName().equals("testIS_LATIN_DIGITS2")) testIS_LATIN_DIGITS2();
        else if (getName().equals("testIS_LATIN")) testIS_LATIN();
        else if (getName().equals("testMIDP_UPPERCASE_LATIN")) testMIDP_UPPERCASE_LATIN();
        else if (getName().equals("testMIDP_LOWERCASE_LATIN")) testMIDP_LOWERCASE_LATIN();
        else if (getName().equals("testUCB_BASIC_LATIN")) testUCB_BASIC_LATIN();
        else if (getName().equals("testUCB_GREEK")) testUCB_GREEK();
        else if (getName().equals("testUCB_CYRILLIC")) testUCB_CYRILLIC();
        else if (getName().equals("testUCB_HEBREW")) testUCB_HEBREW();
        else if (getName().equals("testUCB_ARABIC")) testUCB_ARABIC();
        else if (getName().equals("testUCB_DEVANAGARI")) testUCB_DEVANAGARI();
        else if (getName().equals("testUCB_ARMENIAN")) testUCB_ARMENIAN();
        else if (getName().equals("testUCB_THAI")) testUCB_THAI();
        else if (getName().equals("testUCB_HIRAGANA")) testUCB_HIRAGANA();
        else if (getName().equals("testUCB_KATAKANA")) testUCB_KATAKANA();
        else if (getName().equals("testIS_HALFWIDTH_KATAKANA")) testIS_HALFWIDTH_KATAKANA();
        else if (getName().equals("testIS_KANJI")) testIS_KANJI();
        else if (getName().equals("testIS_FULLWIDTH_DIGITS")) testIS_FULLWIDTH_DIGITS();
        else if (getName().equals("testIS_FULLWIDTH_LATIN")) testIS_FULLWIDTH_LATIN();
        else if (getName().equals("testIS_SIMPLIFIED_HANZI")) testIS_SIMPLIFIED_HANZI();
        else if (getName().equals("testIS_TRADITIONAL_HANZI")) testIS_TRADITIONAL_HANZI();
        else if (getName().equals("testnull")) testnull();
        else if (getName().equals("testUNKNOWN")) testUNKNOWN();
        else if (getName().equals("testempty")) testempty();
        else if (getName().equals("testinputModeChange")) testinputModeChange();
        else if (getName().equals("testinputModeAndMenuOpen")) testinputModeAndMenuOpen();
        else if (getName().equals("testinputModesInEmailaddr")) testinputModesInEmailaddr();
        else if (getName().equals("testinputModesInEmailaddr2")) testinputModesInEmailaddr2();
        else if (getName().equals("testinputModesInUrl")) testinputModesInUrl();
        else if (getName().equals("testinputModesInUrl2")) testinputModesInUrl2();
        else if (getName().equals("testinputModesInPassword")) testinputModesInPassword();
        else if (getName().equals("testinputModesInPassword2")) testinputModesInPassword2();
        else if (getName().equals("testinputModesInAny")) testinputModesInAny();
        else if (getName().equals("testinputModesInAny")) testinputModesInAny2();
        else if (getName().equals("testinputModesInCapsSentence")) testinputModesInCapsSentence();
        else if (getName().equals("testinputModesInCapsWord")) testinputModesInCapsWord();
        else if (getName().equals("testinputModesInCapsSentenceEmailaddr")) testinputModesInCapsSentenceEmailaddr();
        else if (getName().equals("testinputModesInCapsSentenceUrl")) testinputModesInCapsSentenceUrl();
        else if (getName().equals("testinputLatinAfterOtherModes")) testinputLatinAfterOtherModes();
        else if (getName().equals("testinputLatinAfterOtherModes2")) testinputLatinAfterOtherModes2();
        else if (getName().equals("testinputModeNullSetting")) testinputModeNullSetting();
        else if (getName().equals("testcommaInLowerAndUpperCase")) testcommaInLowerAndUpperCase();
        else if (getName().equals("testUppercaseBeforeMakingFormCurrent")) testUppercaseBeforeMakingFormCurrent();
        else super.runTest();
    }


    // ==============================================================

    /**
     * Tests default input mode, i.e. setInitialInputMode not called. The
     * default input mode depends on the build variant in use.
     */
    private void testDefaultInputMode() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "NONE", // no initial input
                // mode setting
                "", // initial content
                new int[] { '2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_LATIN_DIGITS"). The method is called before
     * displaying the text field. Number input mode will be used.
     */
    private void testIS_LATIN_DIGITS() {
        doInputModeTest(getName(), TextField.ANY, "IS_LATIN_DIGITS", "",
        // initial
                // content
                new int[] { '2', '3', '4', '5', }, // key presses
                "2345"); // expected content
    }

    /**
     * Tests setInitialInputMode("IS_LATIN_DIGITS"). The method is called after
     * appending the text field into the form Number input mode will be used.
     */
    private void testIS_LATIN_DIGITS2() {
        doInputModeTest2(getName(), TextField.ANY, "IS_LATIN_DIGITS", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                "2345"); // expected content
    }

    /**
     * Tests setInitialInputMode("IS_LATIN"). The method is called before
     * displaying the text field. Latin text input mode will be used
     */
    private void testIS_LATIN() {
        doInputModeTest(getName(), TextField.ANY, "IS_LATIN", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', }, // key presses
                "Adgj"); // expected content
    }

    /**
     * Tests setInitialInputMode("MIDP_UPPERCASE_LATIN"). The method is called
     * before displaying the text field. Uppercase latin text input mode will be
     * used.
     */
    private void testMIDP_UPPERCASE_LATIN() {
        doInputModeTest(getName(), TextField.ANY, "MIDP_UPPERCASE_LATIN", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                "ADGJ"); // expected content
    }

    /**
     * Tests setInitialInputMode("MIDP_LOWERCASE_LATIN"). The method is called
     * before displaying the text field. Lowercase latin text input mode will be
     * used.
     */
    private void testMIDP_LOWERCASE_LATIN() {
        doInputModeTest(getName(), TextField.ANY, "MIDP_LOWERCASE_LATIN", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                "adgj"); // expected content
    }

    /**
     * . Tests setInitialInputMode("UCB_BASIC_LATIN"). The method is called
     * before displaying the text field. Latin text input mode will be used
     * (same as "IS_LATIN").
     */
    private void testUCB_BASIC_LATIN() {
        doInputModeTest(getName(), TextField.ANY, "UCB_BASIC_LATIN", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                "Adgj"); // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_GREEK"). The method is called before
     * displaying the text field. Greek input mode will be used.
     */
    private void testUCB_GREEK() {
        doInputModeTest(getName(), TextField.ANY, "UCB_GREEK", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', }, // key presses
                GREEK_Adgj); // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_CYRILLIC"). The method is called before
     * displaying the text field. Cyrillic input mode will be used.
     */
    private void testUCB_CYRILLIC() {
        doInputModeTest(getName(), TextField.ANY, "UCB_CYRILLIC", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                CYRILLIC_Adgj); // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_HEBREW"). The method is called before
     * displaying the text field. Hebrew input mode will be used.
     */
    private void testUCB_HEBREW() {
        doInputModeTest(getName(), TextField.ANY, "UCB_HEBREW", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', }, // key presess
                HEBREW_adgj); // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_ARABIC"). The method is called before
     * displaying the text field. Arabic input mode will be used.
     */
    private void testUCB_ARABIC() {
        doInputModeTest(getName(), TextField.ANY, "UCB_ARABIC", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', }, // key presses
                ARABIC_adgj); // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_DEVANAGARI"). The method is called before
     * displaying the text field. Devanagari/Hindi input mode will be used.
     */
    private void testUCB_DEVANAGARI() {

        String expectedContent = "\u0905\u090f\u0915\u091a";
        doInputModeTest(getName(), TextField.ANY, "UCB_DEVANAGARI", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent); // expected content
    }

    /**
     * Tests setInitialInputMode("UCB_ARMENIAN"). The method is called before
     * displaying the text field. This input mode is not supported in 3.0, so
     * default input mode will be used.
     */
    private void testUCB_ARMENIAN() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "UCB_ARMENIAN", "", // initial
                                                                      // content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("UCB_THAI"). The method is called before
     * displaying the text field. This input mode is supported only in Thai
     * variant; other variants will use the default input mode.
     */
    private void testUCB_THAI() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "UCB_THAI", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("UCB_HIRAGANA"). The method is called before
     * displaying the text field. This input mode is supported only in Japan
     * variant; other variants will use the default input mode.
     */
    private void testUCB_HIRAGANA() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "UCB_HIRAGANA", "", // initial
                                                                      // content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("UCB_KATAKANA"). The method is called before
     * displaying the text field. This input mode is supported only in Japan
     * variant; other variants will use the default input mode.
     */
    private void testUCB_KATAKANA() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "UCB_KATAKANA", "", // initial
                                                                      // content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_HALFWIDTH_KATAKANA"). The method is called
     * before displaying the text field. This input mode is supported only in
     * Japan variant; other variants will use the default input mode.
     */
    private void testIS_HALFWIDTH_KATAKANA() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "IS_HALFWIDTH_KATAKANA", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_KANJI"). The method is called before
     * displaying the text field. This input mode is supported only in Japan
     * variant; other variants will use the default input mode.
     */
    private void testIS_KANJI() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "IS_KANJI", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5',}, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_FULLWIDTH_DIGITS"). The method is called
     * before displaying the text field. This input mode is supported only in
     * Japan variant; other variants will use the default input mode.
     */
    private void testIS_FULLWIDTH_DIGITS() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "IS_FULLWIDTH_DIGITS", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_FULLWIDTH_LATIN"). The method is called
     * before displaying the text field. This input mode is supported only in
     * Japan variant; other variants will use the default input mode.
     */
    private void testIS_FULLWIDTH_LATIN() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "IS_FULLWIDTH_LATIN", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_SIMPLIFIED_HANZI"). The method is called
     * before displaying the text field. This input mode is supported only in
     * China variant; other variants will use the default input mode.
     */
    private void testIS_SIMPLIFIED_HANZI() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "IS_SIMPLIFIED_HANZI", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("IS_TRADITIONAL_HANZI"). The method is called
     * before displaying the text field. This input mode is supported only in
     * China variant and has effect only when user's display/input language is
     * either Traditional Hongkong or Traditional Taiwan; otherwise the default
     * input mode will be used.
     */
    private void testIS_TRADITIONAL_HANZI() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "IS_TRADITIONAL_HANZI", "",
                // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);

    }

    /**
     * Tests setInitialInputMode(null). The method is called before displaying
     * the text field. This default input mode will be used.
     */
    private void testnull() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, null, "", // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode("UNKNOWN"). The method is called before
     * displaying the text field. The default input mode will be used.
     */
    private void testUNKNOWN() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "UNKNOWN", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests setInitialInputMode(""). The method is called before displaying the
     * text field. The default input mode will be used.
     */
    private void testempty() {

        String expectedContent = "Adgj";
        doInputModeTest(getName(), TextField.ANY, "", "", // initial content
                new int[] {'2', '3', '4', '5', }, // key presses
                expectedContent);
    }

    /**
     * Tests changing the initial input mode when the TextField is already
     * visible. Setting the initial input mode has no effect until focus is
     * changed temporarily elsewhere and then back to TextField.
     */
    private void testinputModeChange() {

        String expectedContent = "Adgj2345";
        doInputModeSequenceTest(getName(), TextField.ANY,
                new String[] {"IS_LATIN_DIGITS" }, "", // initial content
                new int[] {'2', '3', '4', '5', }, true,
                // keys are pressed both before and after the temporary
                // focus change => input mode before the focus change
                // will be the same as the original input mode although
                // setInitialInputMode has already been called
                expectedContent);
    }

    /**
     * Tests that initial input mode is not restored when EditOptions menu has
     * obscured the editor. Setting the initial input mode has no effect until
     * focus is changed temporarily elsewhere and then back to TextField.
     */
    private void testinputModeAndMenuOpen() {

        String expectedContent = "Adgj2345";
        doInputModeSequenceTest(getName(), TextField.ANY,
                new String[] {"IS_LATIN_DIGITS" }, "", // initial content
                new int[] {'2', '3', Key.Edit, Key.CBA2, '4', '5' ,}, true,
                // keys are pressed both before and after the temporary
                // focus change => input mode before the focus change
                // will be the same as the original input mode although
                // setInitialInputMode has already been called
                expectedContent);
    }

    /**
     * Tests different sequential input modes in NUMERIC text field. Focus is
     * changed after each setting to make the input mode effective. Setting the
     * initial input mode has no effect in NUMERIC text field.
     */
    private void testinputModesInNumber() {
        doInputModeSequenceTest(getName(), TextField.NUMERIC, new String[] {
            "NONE", // starts from default input mode
            "IS_LATIN", "IS_LATIN_DIGITS", "IS_FULLWIDTH_DIGITS",
            "UCB_ARABIC", "UCB_GREEK", "IS_KANJI", }, "", // initial
                                                                // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                "2345234523452345234523452345"); // expected content
    }

    /**
     * Tests different sequential input modes in DECIMAL text field. Focus is
     * changed after each setting to make the input mode effective. Setting the
     * initial input mode has no effect in DECIMAL text field.
     */
    private void testinputModesInDecimal() {
        doInputModeSequenceTest(getName(), TextField.DECIMAL, new String[] {
            "NONE", // starts from default input mode
            "IS_LATIN", "IS_LATIN_DIGITS", "IS_FULLWIDTH_DIGITS",
            "UCB_ARABIC", "UCB_GREEK", "IS_KANJI", }, "", // initial
                // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                "2345234523452345234523452345"); // expected content
    }

    /**
     * Tests different sequential input modes in EMAILADDR text field. Focus is
     * changed after each setting to make the input mode effective. In all
     * variants the default input mode is latin with lower case.
     */
    private void testinputModesInEmailaddr() {
        doInputModeSequenceTest(getName(), TextField.EMAILADDR, new String[] {
            "NONE", // starts from default input mode
            "IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
            "UCB_ARABIC", "UCB_GREEK", "IS_LATIN_DIGITS", }, "",
        // initial content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                "adgjadgjADGJadgj" + ARABIC_adgj + GREEK_adgj + "2345");
        // expected content
    }

    /**
     * Tests different sequential input modes in EMAILADDR text field. Focus is
     * changed after each setting to make the input mode effective. Setting a
     * Japanese or Chinese input mode will have effect in the corresponding
     * variants.
     */
    private void testinputModesInEmailaddr2() {
        String expectedContent = "23452345234523452345";
        doInputModeSequenceTest(getName(), TextField.EMAILADDR, new String[] {
            "IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI",
            "IS_FULLWIDTH_DIGITS", "IS_KANJI", "UCB_THAI", }, "", // initial
                                                                 // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                expectedContent);
    }

    /**
     * Tests different sequential input modes in URL text field. Focus is
     * changed after each setting to make the input mode effective. In all
     * variants the default input mode is latin with lower case.
     */
    private void testinputModesInUrl() {
        doInputModeSequenceTest(getName(), TextField.URL, new String[] {
            "NONE", // default input mode in ANY text field
            "IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
            "UCB_ARABIC", "UCB_GREEK", "IS_LATIN_DIGITS", }, "", // initial
                                                                 // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                "adgjadgjADGJadgj" + ARABIC_adgj + GREEK_adgj + "2345");
        // expected content
    }

    /**
     * Tests different sequential input modes in URL text field. Focus is
     * changed after each setting to make the input mode effective. Setting a
     * Japanese or Chinese input mode will have effect in the corresponding
     * variants.
     */
    private void testinputModesInUrl2() {
        String expectedContent = "23452345234523452345";
        doInputModeSequenceTest(getName(), TextField.URL, new String[] {
            "IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI",
            "IS_FULLWIDTH_DIGITS", "IS_KANJI", "UCB_THAI", }, "", // initial
                                                                 // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                expectedContent);
    }

    /**
     * Tests different sequential input modes in PASSWORD text field. Focus is
     * changed after each setting to make the input mode effective. In all
     * variants the default input mode is latin with lower case.
     */
    private void testinputModesInPassword() {
        doInputModeSequenceTest(getName(), TextField.PASSWORD, new String[] {
            "NONE", // default input mode in ANY text field
            "IS_LATIN", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
            "UCB_ARABIC", "UCB_GREEK", "IS_LATIN_DIGITS", }, "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                "adgjadgjADGJadgj" + ARABIC_adgj + GREEK_adgj + "2345");
        // expected content
    }

    /**
     * Tests different sequential input modes in PASSWORD text field. Focus is
     * changed after each setting to make the input mode effective. Setting a
     * Japanese or Chinese input mode will have effect in the corresponding
     * variants.
     */
    private void testinputModesInPassword2() {
        String expectedContent = "23452345234523452345";
        doInputModeSequenceTest(getName(), TextField.PASSWORD, new String[] {
            "IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI",
            "IS_FULLWIDTH_DIGITS", "IS_KANJI", "UCB_THAI", }, "", // initial
                                                                  // content
                new int[] {'2', '3', '4', '5', }, false,
                // pressed keys (per input mode)
                expectedContent);
    }

    /**
     * Tests different sequential input modes in ANY text field. Focus is
     * changed after each setting to make the input mode effective.
     */
    private void testinputModesInAny() {
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "UCB_GREEK", "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN",
            "UCB_ARABIC", "IS_LATIN_DIGITS" , }, "", // initial content
                new int[] {'2', '3', '4', '5', }, false,
                // pressed keys (per input mode)
                GREEK_Adgj + "ADGJadgj" + ARABIC_adgj + "2345");
        // expected content
    }

    /**
     * Tests different sequential input modes in ANY text field. Focus is
     * changed after each setting to make the input mode effective. Setting a
     * Japanese or Chinese input mode will have effect in the corresponding
     * variants.
     */
    private void testinputModesInAny2() {
        String expectedContent = "Adgjadgjadgjadgjadgj";
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "NONE", // starts from default input mode
            "IS_SIMPLIFIED_HANZI", "IS_FULLWIDTH_DIGITS", "IS_KANJI",
            "UCB_THAI", }, "", // initial content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                expectedContent);
    }

    /**
     * Tests that MIDP_UPPERCASE_LATIN and MIDP_LOWERCASE LATIN input modes are
     * ignored if the INITIAL_CAPS_SENTENCE modifier is defined in ANY text
     * field. Focus is changed after each setting to make the input mode
     * effective.
     */
    private void testinputModesInCapsSentence() {
        String expectedContent = "Adgjadgj" + ARABIC_adgj + "2345";
        doInputModeSequenceTest(getName(), TextField.INITIAL_CAPS_SENTENCE,
                new String[] {"MIDP_LOWERCASE_LATIN", "MIDP_UPPERCASE_LATIN",
                    "UCB_ARABIC", "IS_LATIN_DIGITS", }, "", // initial
                // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                expectedContent);
    }

    /**
     * Tests that MIDP_UPPERCASE_LATIN and MIDP_LOWERCASE LATIN input modes are
     * ignored if the INITIAL_CAPS_WORD modifier is defined in ANY text field.
     * Focus is changed after each setting to make the input mode effective.
     */
    private void testinputModesInCapsWord() {
        String expectedContent = "Adgjadgj" + ARABIC_adgj + "2345";
        doInputModeSequenceTest(getName(), TextField.INITIAL_CAPS_WORD,
                new String[] {"MIDP_LOWERCASE_LATIN", "MIDP_UPPERCASE_LATIN",
                    "UCB_ARABIC", "IS_LATIN_DIGITS", }, "", // initial
                // content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                expectedContent);
    }

    /**
     * Tests that MIDP_LOWERCASE_LATIN and MIDP_UPPERCASE_LATIN input modes are
     * not ignored in EMAILADDR having the INITIAL_CAPS_SENTENCE modfier because
     * the modifier is meaningless in EMAILADDR. Focus is changed after each
     * setting to make the input mode effective.
     */
    private void testinputModesInCapsSentenceEmailaddr() {
        doInputModeSequenceTest(getName(), TextField.EMAILADDR
                | TextField.INITIAL_CAPS_SENTENCE, new String[] {"IS_LATIN",
                    "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN", }, "",
                // initial content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                "adgjADGJadgj"); // expected content
    }

    /**
     * Tests that MIDP_LOWERCASE_LATIN and MIDP_UPPERCASE_LATIN input modes are
     * not ignored in URL having the INITIAL_CAPS_SENTENCE modfier because the
     * modifier is meaningless in URL. Focus is changed after each setting to
     * make the input mode effective.
     */
    private void testinputModesInCapsSentenceUrl() {
        doInputModeSequenceTest(getName(), TextField.URL
                | TextField.INITIAL_CAPS_SENTENCE, new String[] {"IS_LATIN",
                    "MIDP_UPPERCASE_LATIN", "MIDP_LOWERCASE_LATIN", }, "",
                    // initial content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                "adgjADGJadgj"); // expected content
    }

    /**
     * Tests different textual input modes after IS_LATIN_DIGITS. Focus is
     * changed after each setting to make the input mode effective.
     */
    private void testinputModesAfterNumberMode() {
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "IS_LATIN_DIGITS", "MIDP_UPPERCASE_LATIN", "IS_LATIN_DIGITS",
            "MIDP_LOWERCASE_LATIN", "IS_LATIN_DIGITS", "UCB_ARABIC", }, "",
                // initial content
                new int[] {'2', '3', '4', '5', }, false, "2345ADGJ2345adgj2345"
                        + ARABIC_adgj);
    }

    /**
     * Tests Chinese and Japanese input modes after IS_LATIN_DIGITS. Focus is
     * changed after each setting to make the input mode effective. Setting a
     * Japanese or Chinese input mode will have effect only in the corresponding
     * variants.
     */
    private void testinputModesAfterNumberMode2() {
        String expectedContent = "23452345234523452345234523452345";
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "IS_LATIN_DIGITS", "IS_SIMPLIFIED_HANZI", "IS_LATIN_DIGITS",
            "IS_KANJI", "IS_LATIN_DIGITS", "IS_FULLWIDTH_DIGITS",
            "IS_LATIN_DIGITS", "UCB_THAI", }, "", // initial content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                expectedContent);
    }

    /**
     * Tests latin input modes after other input modes. Focus is changed after
     * each setting to make the input mode effective.
     */
    private void testinputLatinAfterOtherModes() {
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "UCB_HEBREW", "IS_LATIN", "UCB_GREEK", "MIDP_UPPERCASE_LATIN",
            "UCB_ARABIC", "MIDP_LOWERCASE_LATIN", "UCB_CYRILLIC",
            "IS_LATIN", }, "", // initial content
                new int[] {'2', '3', '4', '5', }, false, HEBREW_adgj + "adgj"
                        + GREEK_adgj + "ADGJ" + ARABIC_adgj + "adgj"
                        + CYRILLIC_adgj + "adgj");
    }

    /**
     * Tests latin input modes after Chinese and Japanese input modes. Focus is
     * changed after each setting to make the input mode effective. Setting a
     * Japanese or Chinese input mode will have effect only in the corresponding
     * variants.
     */
    private void testinputLatinAfterOtherModes2() {
        String expectedContent = "Adgjadgjadgjadgjadgjadgjadgjadgj";
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "IS_SIMPLIFIED_HANZI", "MIDP_LOWERCASE_LATIN", "IS_KANJI",
            "MIDP_LOWERCASE_LATIN", "IS_FULLWIDTH_DIGITS",
            "MIDP_LOWERCASE_LATIN", "UCB_THAI", "MIDP_LOWERCASE_LATIN", },
                "", // initial content
                new int[] {'2', '3', '4', '5', }, false,
                // key presses (per input mode)
                expectedContent);
    }

    /**
     * Tests null input mode after different input modes in ANY text field.
     */
    private void testinputModeNullSetting() {
        String expectedContent = "2345adgj" + ARABIC_adgj + "adgj"
                + "adgjadgjadgjadgjadgjadgjadgjadgj";
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "IS_LATIN_DIGITS", null, "UCB_ARABIC", null,
            "IS_SIMPLIFIED_HANZI", null, "IS_HALFWIDTH_KATAKANA", null,
            "IS_FULLWIDTH_DIGITS", null, "UCB_THAI", null, }, "", // initial
                // content
                new int[] {'2', '3', '4', '5', }, false, expectedContent);
    }

    /**
     * Tests comma and space in MIDP_LOWERCASE_LATIN and MIDP_UPPERCASE_LATIN
     * text field. Note: after a comma and space the input mode changes to Text
     * case (not sure whether this is correct behaviour)
     */
    private void testcommaInLowerAndUpperCase() {
        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "MIDP_LOWERCASE_LATIN", "MIDP_UPPERCASE_LATIN", }, "", // initial
                // content
                new int[] {'2', '3', '1', '0', '4', '5', }, false,
                // key presses (per input mode)
                "ad. GjAD. Gj");
    }

    /**
     * Tests cyrillic/latin/arabic input modes with Arabic input language.
     * cyrillic => intial input language got hard-coded default (Russian) latin =>
     * initial input language got from display language arabic => initial input
     * langauge got from global input language
     */
    private void testinputModesWithArabicInputLanguage() {

        changeInputLanguage(4, 1); // => Arabic input language

        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "UCB_CYRILLIC", "MIDP_LOWERCASE_LATIN", "UCB_ARABIC", }, "",
                // initial content
                new int[] {'2', '3', '4', '5', }, false, CYRILLIC_Adgj + "adgj"
                        + ARABIC_adgj);

        changeInputLanguage(5, 10); // => back to English input language
    }

    /**
     * Tests cyrillic/latin/arabic input modes with Cyrillic input language.
     * cyrillic => intial input language got from global input language latin =>
     * initial input language got from display language arabic => initial input
     * langauge got from hard-coded default (Arabic)
     */
    private void testinputModesWithCyrillicInputLanguage() {

        changeInputLanguage(4, 2); // => Cyrillic input language

        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "UCB_CYRILLIC", "MIDP_LOWERCASE_LATIN", "UCB_ARABIC", }, "",
                // initial content
                new int[] {'2', '3', '4', '5', }, false, CYRILLIC_Adgj + "adgj"
                        + ARABIC_adgj);

        changeInputLanguage(4, 10); // => back to English input language
    }

    /**
     * Tests cyrillic/latin/arabic input modes with Dansk input language.
     * cyrillic => intial input language got from hard-coded default (Russian)
     * latin => initial input language got from global input language arabic =>
     * initial input langauge got from hard-coded default (Arabic)
     */
    private void testinputModesWithDanskInputLanguage() {

        changeInputLanguage(4, 8); // => Dansk input language

        doInputModeSequenceTest(getName(), TextField.ANY, new String[] {
            "UCB_CYRILLIC", "MIDP_LOWERCASE_LATIN", "UCB_ARABIC", }, "",
            // initial content
                new int[] {'2', '3', '4', '5', }, false, CYRILLIC_Adgj + "adgj"
                        + ARABIC_adgj);

        changeInputLanguage(4, 10); // => back to English input language
    }

    /**
     * Set the initial input mode to MIDP_UPPERCASE_LATIN after adding the
     * TextField to the form, and then making the form current. The input text
     * must be uppercase.
     */
    private void testUppercaseBeforeMakingFormCurrent() {
        boolean passed = true;
        int[] pressedKeys = new int[] {'2', '3', '4', '5', };
        String expectedContent = "ADGJ";

        TextField textField = null;
        Form form = null;

        try {
            // Create a form and a text field, and append text field to form
			form = new Form(getName());

			textField = new TextField("Text field", "", normalSize, TextField.ANY);
			form.append(textField);

            // set the initial input mode before the form is current
            textField.setInitialInputMode("MIDP_UPPERCASE_LATIN");
            block(KEYEVENT_DELAY);

            // Display the form
            setCurrent(form);

            // Do key presses
            if (pressedKeys != null) {
                for (int i = 0; i < pressedKeys.length; i++) {
                    key(pressedKeys[i]);
                }
            }
            block(KEYEVENT_DELAY);

            // Check the result of key presses (i.e. the used input mode)
            passed = textField.getString().equals(expectedContent);

        }
        catch (Exception exc) {
            print(getName() + " test failed: " + exc.toString());
			print(exc);
            passed = false;
        }

        assertTrue(getName() + " test " + "content: " + textField.getString()
                + ", expected: " + expectedContent, passed);
    }

    /**
     * Private method for testing initial input mode. Creates TextField and sets
     * the initial input mode after constructing the textField. Generates the
     * given key presses. At the end compares the content of TextField to the
     * expected content, unless the expected content is null; in that case takes
     * a screenshot.
     *
     * @param testCaseName
     *            Name of the test case
     * @param constraints
     *            Combination of TextField.ANY,...
     * @param initialInputMode
     *            Intial input mode to be set, e.g "IS_LATIN"; "NONE" means that
     *            setInitialInputMode is not called at all
     * @param initialContent
     *            Initial context of TextField
     * @param pressedKeys
     *            Array of key presses to be generated
     * @param expectedContent
     *            Expected content of TextField
     */
    private void doInputModeTest(String testCaseName, int constraints,
            String initialInputMode, String initialContent, int[] pressedKeys,
            String expectedContent) {

        boolean passed = true;

        try {
            // Create a form with two text fields
            Form form = new Form(testCaseName);
			TextField textField = new TextField("Text field", initialContent,
					normalSize, constraints);
			TextField otherField = new TextField("Other field", "", normalSize,
					TextField.ANY);

            // Set initial input mode before displaying TextField
            if ("NONE".equals(initialInputMode)) {
                // initial input mode not set
            }
            else {
                textField.setInitialInputMode(initialInputMode);
            }
            form.append(textField);
            form.append(otherField);
            setCurrent(form);
            block(CHANGE_DISPLAYABLE_DELAY);
            block(CHANGE_DISPLAYABLE_DELAY);

            // Do key presses
            if (pressedKeys != null) {
                for (int i = 0; i < pressedKeys.length; i++) {
                    key(pressedKeys[i]);
                }
            }
            block(KEYEVENT_DELAY);

            // Check the result of key presses (i.e. the used input mode)
            passed = checkResult(testCaseName, textField, expectedContent,
                    expectedContent == null ? true : false);

        }
        catch (Exception exc) {
            print(testCaseName + " test failed: " + exc.toString());
			print(exc);
			passed = false;
        }

        assertTrue(testCaseName, passed);
    }

    /**
	 * Private method for testing initial input mode. Creates TextField and sets
	 * the initial input mode before displaying it. Generates the given key
	 * presses. At the end compares the content of TextField to the expected
	 * content, unless the expected content is null; in that case takes a
	 * screenshot.
	 *
	 * @param testCaseName
	 *            Name of the test case
	 * @param constraints
	 *            Combination of TextField.ANY,...
	 * @param initialInputMode
	 *            Intial input mode to be set, e.g "IS_LATIN"; "NONE" means that
	 *            setInitialInputMode is not called at all
	 * @param initialContent
	 *            Initial context of TextField
	 * @param pressedKeys
	 *            Array of key presses to be generated
	 * @param expectedContent
	 *            Expected content of TextField
	 */
    private void doInputModeTest2(String testCaseName, int constraints,
            String initialInputMode, String initialContent, int[] pressedKeys,
            String expectedContent) {

        boolean passed = true;

        try {
            // Create a form with two text fields
			Form form = new Form(testCaseName);
			TextField textField = new TextField("Text field", initialContent,
					normalSize, constraints);
			TextField otherField = new TextField("Other field", "", normalSize,
					TextField.ANY);
			form.append(textField);
			form.append(otherField);

            // Set initial input mode before displaying TextField
            if ("NONE".equals(initialInputMode)) {
                // initial input mode not set
            }
            else {
                textField.setInitialInputMode(initialInputMode);
            }
            setCurrent(form);
            block(CHANGE_DISPLAYABLE_DELAY);

            // Do key presses
            if (pressedKeys != null) {
                for (int i = 0; i < pressedKeys.length; i++) {
                    key(pressedKeys[i]);
                }
            }
            block(KEYEVENT_DELAY);

            // Check the result of key presses (i.e. the used input mode)
            passed = checkResult(testCaseName, textField, expectedContent,
                    expectedContent == null ? true : false);

        }
        catch (Exception exc) {
            print(testCaseName + " test failed: " + exc.toString());
			print(exc);
			passed = false;
        }

        assertTrue(testCaseName, passed);
    }

    /**
     * Private method for testing several initial input mode settings. Creates a
     * form with two TextFields and sets given initial input modes one after
     * another while TextField is visible. The focus is changed to the other
     * TextField of the form between each setting in order to make the setting
     * effective. Generates the given key presses after each setting. At the end
     * compares the text field content to the expected content, unless the
     * expected content is null; in that case takes a screenshot.
     *
     * @param testCaseName
     *            Name of the test case
     * @param constraints
     *            Combination of TextField.ANY,...
     * @param nextInputModes
     *            Array of intial input modes to be set. e.g. {"NONE",
     *            "IS_LATIN, "IS_LATIN_DIGITS"}
     * @param initialContent
     *            Initial context of the TextField
     * @param pressedKeys
     *            Array of key presses to be generated
     * @param beforeAndAfter
     *            If true, key presses are done both before and after the
     *            setIntialInputMode call, otherwise only after it
     * @param expectedContent
     *            Expected content of TextField
     */
    private void doInputModeSequenceTest(String testCaseName, int constraints,
            String[] nextInputModes, String initialContent, int[] pressedKeys,
            boolean beforeAndAfter, String expectedContent) {

        boolean passed = true;

        try {
            // Create a form with two text fields
            Form form = new Form(testCaseName);
            TextField textField = new TextField("Text field", initialContent,
                    normalSize, constraints);
            form.append(textField);
            TextField otherField = new TextField("Other field", "", normalSize,
                    TextField.ANY);
            form.append(otherField);

            // Display the form
            setCurrent(form);
            block(CHANGE_DISPLAYABLE_DELAY);

            for (int m = 0; m < nextInputModes.length; m++) {

                // Set initial input mode while the original TextField is
                // visible
                textField.setInitialInputMode(nextInputModes[m]);

                if (beforeAndAfter) {
                    // Do key presses (input mode setting not yet effective)
                    if (pressedKeys != null) {
                        for (int i = 0; i < pressedKeys.length; i++) {
                            key(pressedKeys[i]);
                        }
                    }
                    block(KEYEVENT_DELAY);
                }

                // Move focus the other text field and back
                // => new editing session starts
                block(KEYEVENT_DELAY);
                key(Key.DownArrow);
                key(Key.UpArrow);

                // Do key presses again (now input mode setting is effective)
                if (pressedKeys != null) {
                    for (int i = 0; i < pressedKeys.length; i++) {
                        key(pressedKeys[i]);
                    }
                }
                block(KEYEVENT_DELAY);
            }

            // Check the result of all key presses (i.e. the used input modes)
            passed = checkResult(testCaseName, textField, expectedContent,
                    expectedContent == null ? true : false);

        }
        catch (Exception exc) {
            print(testCaseName + " test failed: " + exc.toString());
			print(exc);
			passed = false;
        }

        assertTrue(testCaseName, passed);
    }

    /**
     * Checks whether the content of the given text field corresponds to the
     * expected content.
     *
     * @param testCaseName
     *            Name of the test case
     * @param textBox
     *            TextField to be checked
     * @param expectedContent
     *            The expected content
     * @param takeScreenshot
     *            If true, screenshot is taken
     * @return result Returns whether test case passed or failed
     */
    private boolean checkResult(String testCaseName, TextField textBox,
			String expectedContent, boolean takeScreenshot) {

		boolean passed = true;

		// Check the expected content
		if (expectedContent != null) {
			if (!expectedContent.equals(textBox.getString())) {
				passed = false;
				print(testCaseName + " test "
						+ " failed: content: " + textBox.getString()
						+ ", expected: " + expectedContent);
			}
		}

		// Take the screenshot and compare it with the previous one
		if (takeScreenshot) {
			if (!takeScreenshot(testCaseName + ".gif")) {
				passed = false;
				print(testCaseName + " test "
						+ " failed: screenshot does not match");

				print("If failure is caused by different "
						+ "special characters which are show in the cell at "
						+ "the top of the Edit Menu, please IGNORE the result"
						+ " and treat the test as PASSED!");
			}
		}

		if (passed) {
			print(testCaseName + " test succeeded: " + textBox.getString());
		}
		return passed;
	}

    /**
	 * Private method for changing the input language
	 *
	 * @param positionInEditOptions
	 *            Position of "Input language" menu item
	 * @param positionInLanguageList
	 *            Position of the desired language
	 */
    private void changeInputLanguage(int positionInEditOptions,
            int positionInLanguageList) {

        // Create a temporary TextBox in order to be able to set
        // the global input language
        TextBox other = new TextBox("Input language setting",
                "Temporary text box", normalSize, TextField.ANY);

        // Display the text box
        setCurrent(other);

        // Open EditOptions menu and select Input language menu item
        key(Key.Edit);

        for (int i = 0; i < positionInEditOptions; i++) {
            key(Key.DownArrow);
        }
        block(KEYEVENT_DELAY);

        key(Key.Select);

        // Select the desired input language in the language list
        for (int i = 1; i < positionInLanguageList; i++) {
            key(Key.DownArrow);
        }
        block(KEYEVENT_DELAY);

        // input language selected
        key(Key.Select);
        block(KEYEVENT_DELAY);
    }

}
