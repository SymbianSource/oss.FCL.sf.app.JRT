/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.test.java.eswt.textextension;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ercp.swt.mobile.TextExtension;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the textextension component
 * -
 */

public class TextExtensionTestMIDlet_01 extends TestBaseMIDlet
{
    /** */
    private TextExtension topLeftField  = null;
    private TextExtension topRightField = null;
    private TextExtension bottomLeftField   = null;
    private TextExtension bottomRightField  = null;

    /** Do menu commands */
    private Command cmdDoAppend     = null;
    private Command cmdDoInsert     = null;

    /** Input mode menu commands */
    private Command cmdModeReadOnly = null;
    private Command cmdModeEditable = null;

    /** Text Limit menu commands */
    private Command cmdLimitNONE    = null;
    private Command cmdLimitCustom  = null;

    /** Text adding menu commands */
    private Command cmdLong         = null;
    private Command cmdExtraLong    = null;
    private Command cmdOverExtraLong    = null;

    /** Text type menu commands */
    private Command cmdMULTI    = null;
    private Command cmdSINGLE   = null;

    /** Aligment menu commands */
    private Command cmdAligmentLEFT   = null;
    private Command cmdAligmentRIGHT  = null;
    private Command cmdAligmentCENTER = null;

    /** Wrapping menu command */
    private Command cmdWrapYES  = null;
    private Command cmdWrapNO   = null;

    /** Scrolling commands */
    private Command cmdScrollVertical   = null;
    private Command cmdScrollHorizontal = null;
    private Command cmdScrollBOTH       = null;
    private Command cmdScrollNONE       = null;

    /** Style menu commands */
    private Command cmdEmail  = null;
    private Command cmdURL = null;
    private Command cmdNonPred = null;
    private Command cmdLatinOnly = null;
    private Command cmdEmail_Non  = null;
    private Command cmdURL_Non = null;
    private Command cmdURL_LatinOnly_Non = null;
    private Command cmdEmail_LatinOnly = null;
    private Command cmdURL_LatinOnly = null;
    private Command cmdEmail_LatinOnly_Non = null;

    /** Case modifier commands */
    private Command cmdUPPERCASE = null;
    private Command cmdLOWERCASE = null;
    private Command cmdTEXTCASE = null;
    private Command cmdTITLECASE = null;


    private Command cmdBASIC_LATIN = null;
    private Command cmdGREEK = null;
    private Command cmdCYRILLIC = null;
    private Command cmdARMENIAN = null;
    private Command cmdHEBREW = null;
    private Command cmdARABIC = null;
    private Command cmdDEVANAGARI = null;
    private Command cmdBENGALI = null;
    private Command cmdTHAI = null;
    private Command cmdHIRAGANA = null;
    private Command cmdKATAKANA = null;
    private Command cmdHANGUL_SYLLABLES = null;
    private Command cmdFULLWIDTH_DIGITS = null;
    private Command cmdFULLWIDTH_LATIN = null;
    private Command cmdHALFWIDTH_KATAKANA = null;
    private Command cmdHANJA = null;
    private Command cmdKANJI = null;
    private Command cmdLATIN = null;
    private Command cmdLATIN_DIGITS = null;
    private Command cmdSIMPLIFIED_HANZI = null;
    private Command cmdTRADITIONAL_HANZI = null;

    private int extension;

    final private String BASIC_LATIN = "UCB_BASIC_LATIN";
    final private String GREEK = "UCB_GREEK";
    final private String CYRILLIC = "UCB_CYRILLIC";
    final private String ARMENIAN = "UCB_ARMENIAN";
    final private String HEBREW = "UCB_HEBREW";
    final private String ARABIC = "UCB_ARABIC";
    final private String DEVANAGARI = "UCB_DEVANAGARI";
    final private String BENGALI = "UCB_BENGALI";
    final private String THAI = "UCB_THAI";
    final private String HIRAGANA = "UCB_HIRAGANA";
    final private String KATAKANA = "UCB_KATAKANA";
    final private String HANGUL_SYLLABLES = "UCB_HANGUL_SYLLABLES";
    final private String FULLWIDTH_DIGITS = "IS_FULLWIDTH_DIGITS";
    final private String FULLWIDTH_LATIN = "IS_FULLWIDTH_LATIN";
    final private String HALFWIDTH_KATAKANA = "IS_HALFWIDTH_KATAKANA";
    final private String HANJA = "IS_HANJA";
    final private String KANJI = "IS_KANJI";
    final private String LATIN = "IS_LATIN";
    final private String LATIN_DIGITS = "IS_LATIN_DIGITS";
    final private String SIMPLIFIED_HANZI = "IS_SIMPLIFIED_HANZI";
    final private String TRADITIONAL_HANZI = "IS_TRADITIONAL_HANZI";

    final private int longtext = 149;
    final private int extralong = 999;
    final private int overextra = 1001;

    final private String thousand_chars = "123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_"+
                                          "123456789_123456789_123456789_123456789_123456789_123456789_123456789_";
    /**
     * show a messagebox with the thrown exception
     * @param the text to be displayed in the messagebox
     */
    protected void showException(String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText("Exception");
        mb.setMessage(msg);
        mb.open();
    }

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // set title
        this.shell.setText("TextExtension Test");

        // create and set the layout
        this.shell.setLayout(new FormLayout());

        // create menu

        // Do menu
        Command cmdDo = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdDo.setText("Do");

        this.cmdDoAppend = new Command(cmdDo, Command.GENERAL, 1);
        this.cmdDoAppend.setText("Append");
        this.cmdDoAppend.addSelectionListener(this);

        this.cmdDoInsert = new Command(cmdDo, Command.GENERAL, 1);
        this.cmdDoInsert.setText("Insert");
        this.cmdDoInsert.addSelectionListener(this);

        Command cmdStyle = new Command(this.shell, Command.COMMANDGROUP, 2);
        cmdStyle.setText("Style");

        this.cmdEmail = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdEmail.setText("EMAIL");
        this.cmdEmail.addSelectionListener(this);

        this.cmdURL = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdURL.setText("URL");
        this.cmdURL.addSelectionListener(this);

        this.cmdNonPred = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdNonPred.setText("NON_PREDICTIVE");
        this.cmdNonPred.addSelectionListener(this);

        this.cmdLatinOnly = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdLatinOnly.setText("LATIN_ONLY");
        this.cmdLatinOnly.addSelectionListener(this);

        this.cmdEmail_Non = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdEmail_Non.setText("EMAIL | NON_PREDICTIVE");
        this.cmdEmail_Non.addSelectionListener(this);

        this.cmdURL_Non = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdURL_Non.setText("URL | NON_PREDICTIVE");
        this.cmdURL_Non.addSelectionListener(this);

        this.cmdEmail_LatinOnly = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdEmail_LatinOnly.setText("EMAIL | LATIN_ONLY");
        this.cmdEmail_LatinOnly.addSelectionListener(this);

        this.cmdEmail_LatinOnly_Non = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdEmail_LatinOnly_Non.setText("EMAIL | LATIN_ONLY | NON_PREDICTIVE");
        this.cmdEmail_LatinOnly_Non.addSelectionListener(this);

        this.cmdURL_LatinOnly = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdURL_LatinOnly.setText("URL | LATIN_ONLY");
        this.cmdURL_LatinOnly.addSelectionListener(this);

        this.cmdURL_LatinOnly_Non = new Command(cmdStyle, Command.GENERAL, 1);
        this.cmdURL_LatinOnly_Non.setText("URL | LATIN_ONLY | NON_PREDICTIVE");
        this.cmdURL_LatinOnly_Non.addSelectionListener(this);

        // Setting Case menu
        Command cmdSetCase = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSetCase.setText("Set Case");

        this.cmdUPPERCASE = new Command(cmdSetCase, Command.GENERAL, 1);
        this.cmdUPPERCASE.setText("UPPERCASE");
        this.cmdUPPERCASE.addSelectionListener(this);

        this.cmdTEXTCASE = new Command(cmdSetCase, Command.GENERAL, 1);
        this.cmdTEXTCASE.setText("TEXTCASE");
        this.cmdTEXTCASE.addSelectionListener(this);

        this.cmdLOWERCASE = new Command(cmdSetCase, Command.GENERAL, 1);
        this.cmdLOWERCASE.setText("LOWERCASE");
        this.cmdLOWERCASE.addSelectionListener(this);

        this.cmdTITLECASE = new Command(cmdSetCase, Command.GENERAL, 1);
        this.cmdTITLECASE.setText("TITLECASE");
        this.cmdTITLECASE.addSelectionListener(this);

        // Setting input mode menu
        Command cmdSetInput = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSetInput.setText("Set Input");

        this.cmdBASIC_LATIN = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdBASIC_LATIN.setText(this.BASIC_LATIN);
        this.cmdBASIC_LATIN.addSelectionListener(this);

        this.cmdGREEK = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdGREEK.setText(this.GREEK);
        this.cmdGREEK.addSelectionListener(this);

        this.cmdCYRILLIC = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdCYRILLIC.setText(this.CYRILLIC);
        this.cmdCYRILLIC.addSelectionListener(this);

        this.cmdARMENIAN = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdARMENIAN.setText(this.ARMENIAN);
        this.cmdARMENIAN.addSelectionListener(this);

        this.cmdHEBREW = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdHEBREW.setText(this.HEBREW);
        this.cmdHEBREW.addSelectionListener(this);

        this.cmdARABIC = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdARABIC.setText(this.ARABIC);
        this.cmdARABIC.addSelectionListener(this);

        this.cmdDEVANAGARI = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdDEVANAGARI.setText(this.DEVANAGARI);
        this.cmdDEVANAGARI.addSelectionListener(this);

        this.cmdBENGALI = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdBENGALI.setText(this.BENGALI);
        this.cmdBENGALI.addSelectionListener(this);

        this.cmdTHAI = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdTHAI.setText(this.THAI);
        this.cmdTHAI.addSelectionListener(this);

        this.cmdHIRAGANA= new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdHIRAGANA.setText(this.HIRAGANA);
        this.cmdHIRAGANA.addSelectionListener(this);

        this.cmdKATAKANA = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdKATAKANA.setText(this.KATAKANA);
        this.cmdKATAKANA.addSelectionListener(this);

        this.cmdHANGUL_SYLLABLES = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdHANGUL_SYLLABLES.setText(this.HANGUL_SYLLABLES);
        this.cmdHANGUL_SYLLABLES.addSelectionListener(this);

        this.cmdFULLWIDTH_LATIN = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdFULLWIDTH_LATIN.setText(this.FULLWIDTH_LATIN);
        this.cmdFULLWIDTH_LATIN.addSelectionListener(this);

        this.cmdFULLWIDTH_DIGITS = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdFULLWIDTH_DIGITS.setText(this.FULLWIDTH_DIGITS);
        this.cmdFULLWIDTH_DIGITS.addSelectionListener(this);

        this.cmdHALFWIDTH_KATAKANA= new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdHALFWIDTH_KATAKANA.setText(this.HALFWIDTH_KATAKANA);
        this.cmdHALFWIDTH_KATAKANA.addSelectionListener(this);

        this.cmdHANJA = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdHANJA.setText(this.HANJA);
        this.cmdHANJA.addSelectionListener(this);

        this.cmdKANJI = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdKANJI.setText(this.KANJI);
        this.cmdKANJI.addSelectionListener(this);

        this.cmdLATIN = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdLATIN.setText(this.LATIN);
        this.cmdLATIN.addSelectionListener(this);

        this.cmdLATIN_DIGITS = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdLATIN_DIGITS.setText(this.LATIN_DIGITS);
        this.cmdLATIN_DIGITS.addSelectionListener(this);

        this.cmdSIMPLIFIED_HANZI = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdSIMPLIFIED_HANZI.setText(this.SIMPLIFIED_HANZI);
        this.cmdSIMPLIFIED_HANZI.addSelectionListener(this);

        this.cmdTRADITIONAL_HANZI = new Command(cmdSetInput, Command.GENERAL, 1);
        this.cmdTRADITIONAL_HANZI.setText(this.TRADITIONAL_HANZI);
        this.cmdTRADITIONAL_HANZI.addSelectionListener(this);

        // Long text submenu
        Command cmdText = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdText.setText("Set Long Texts");

        this.cmdLong = new Command(cmdText, Command.GENERAL, 2);
        this.cmdLong.setText("149 chars");
        this.cmdLong.addSelectionListener(this);

        this.cmdExtraLong = new Command(cmdText, Command.GENERAL, 2);
        this.cmdExtraLong.setText("999 chars");
        this.cmdExtraLong.addSelectionListener(this);

        this.cmdOverExtraLong = new Command(cmdText, Command.GENERAL, 2);
        this.cmdOverExtraLong.setText("1001 chars");
        this.cmdOverExtraLong.addSelectionListener(this);

        // Settings menu
        Command cmdSettings = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdSettings.setText("Settings");

        // Type submenu
        Command cmdType = new Command(cmdSettings, Command.COMMANDGROUP, 4);
        cmdType.setText("Type");

        this.cmdMULTI = new Command(cmdType, Command.GENERAL, 2);
        this.cmdMULTI.setText("Multi");
        this.cmdMULTI.addSelectionListener(this);

        this.cmdSINGLE = new Command(cmdType, Command.GENERAL, 2);
        this.cmdSINGLE.setText("Single");
        this.cmdSINGLE.addSelectionListener(this);

        // Aligment submenu
        Command cmdAligment = new Command(cmdSettings, Command.COMMANDGROUP, 4);
        cmdAligment.setText("Aligment");

        this.cmdAligmentLEFT = new Command(cmdAligment, Command.GENERAL, 2);
        this.cmdAligmentLEFT.setText("LEFT");
        this.cmdAligmentLEFT.addSelectionListener(this);

        this.cmdAligmentCENTER = new Command(cmdAligment, Command.GENERAL, 1);
        this.cmdAligmentCENTER.setText("CENTER");
        this.cmdAligmentCENTER.addSelectionListener(this);

        this.cmdAligmentRIGHT = new Command(cmdAligment, Command.GENERAL, 0);
        this.cmdAligmentRIGHT.setText("RIGHT");
        this.cmdAligmentRIGHT.addSelectionListener(this);

        // Mode submenu
        Command cmdMode = new Command(cmdSettings, Command.COMMANDGROUP, 3);
        cmdMode.setText("Mode");

        this.cmdModeEditable = new Command(cmdMode, Command.GENERAL, 1);
        this.cmdModeEditable.setText("EDITABLE");
        this.cmdModeEditable.addSelectionListener(this);

        this.cmdModeReadOnly = new Command(cmdMode, Command.GENERAL, 0);
        this.cmdModeReadOnly.setText("READ_ONLY");
        this.cmdModeReadOnly.addSelectionListener(this);

        // Text Limit submenu
        Command cmdLimit = new Command(cmdSettings, Command.COMMANDGROUP, 2);
        cmdLimit.setText("Limit");

        this.cmdLimitCustom = new Command(cmdLimit, Command.GENERAL, 1);
        this.cmdLimitCustom.setText("CUSTOM");
        this.cmdLimitCustom.addSelectionListener(this);

        this.cmdLimitNONE = new Command(cmdLimit, Command.GENERAL, 0);
        this.cmdLimitNONE.setText("NONE");
        this.cmdLimitNONE.addSelectionListener(this);

        // Wrapping submenu
        Command cmdWrap = new Command(cmdSettings, Command.COMMANDGROUP, 1);
        cmdWrap.setText("Wrap");

        this.cmdWrapYES = new Command(cmdWrap, Command.GENERAL, 1);
        this.cmdWrapYES.setText("YES");
        this.cmdWrapYES.addSelectionListener(this);

        this.cmdWrapNO = new Command(cmdWrap, Command.GENERAL, 0);
        this.cmdWrapNO.setText("NO");
        this.cmdWrapNO.addSelectionListener(this);

        // scrolling submenu
        Command cmdScroll = new Command(cmdSettings, Command.COMMANDGROUP, 0);
        cmdScroll.setText("Scroll Bar");

        this.cmdScrollNONE = new Command(cmdScroll, Command.GENERAL, 3);
        this.cmdScrollNONE.setText("NONE");
        this.cmdScrollNONE.addSelectionListener(this);

        this.cmdScrollHorizontal = new Command(cmdScroll, Command.GENERAL, 2);
        this.cmdScrollHorizontal.setText("HORIZONTAL");
        this.cmdScrollHorizontal.addSelectionListener(this);

        this.cmdScrollVertical = new Command(cmdScroll, Command.GENERAL, 1);
        this.cmdScrollVertical.setText("VERTICAL");
        this.cmdScrollVertical.addSelectionListener(this);

        this.cmdScrollBOTH = new Command(cmdScroll, Command.GENERAL, 0);
        this.cmdScrollBOTH.setText("BOTH");
        this.cmdScrollBOTH.addSelectionListener(this);

        this.extension = -1;

        // create field Labels
        Label tllabel = new Label(this.shell, SWT.NONE);
        tllabel.setText("UPPERCASE");

        FormData tlldata = new FormData();
        tlldata.left = new FormAttachment(0, 0);
        tlldata.top = new FormAttachment(0, 0);
        tllabel.setLayoutData(tlldata);

        Label trlabel = new Label(this.shell, SWT.NONE);
        trlabel.setText("LOWERCASE");

        FormData trdata = new FormData();
        trdata.left = new FormAttachment(50, 0);
        trdata.top = new FormAttachment(0, 0);
        trlabel.setLayoutData(trdata);

        // create Text fields
        // top left
        this.topLeftField = new TextExtension(this.shell, SWT.BORDER | SWT.SINGLE);
        this.topLeftField.setInitialInputMode(TextExtension.UPPERCASE,null);

        // create and set the layout data
        FormData textTLdata = new FormData();
        textTLdata.left = new FormAttachment(tllabel, 0, SWT.LEFT);
        textTLdata.top = new FormAttachment(tllabel, 5, SWT.BOTTOM);
        textTLdata.right = new FormAttachment(40, 0);
        this.topLeftField.setLayoutData(textTLdata);

        // top right
        this.topRightField = new TextExtension(this.shell, SWT.BORDER | SWT.SINGLE);
        this.topRightField.setInitialInputMode(TextExtension.LOWERCASE,null);
        // create and set the layout data
        FormData textTRdata = new FormData();
        textTRdata.left = new FormAttachment(trlabel, 0, SWT.LEFT);
        textTRdata.top = new FormAttachment(trlabel, 5, SWT.BOTTOM);
        textTRdata.right = new FormAttachment(90, 0);
        this.topRightField.setLayoutData(textTRdata);

        // create field Labels
        Label bllabel = new Label(this.shell, SWT.NONE);
        bllabel.setText("TEXTCASE");

        FormData bldata = new FormData();
        bldata.left = new FormAttachment(0, 0);
        bldata.top = new FormAttachment(50, 0);
        bllabel.setLayoutData(bldata);

        Label mlabel = new Label(this.shell, SWT.NONE);
        mlabel.setText("TITLECASE");

        FormData mldata = new FormData();
        mldata.left = new FormAttachment(50, 0);
        mldata.top = new FormAttachment(50, 0);
        mlabel.setLayoutData(mldata);

        // bottom left
        this.bottomLeftField = new TextExtension(this.shell, SWT.BORDER | SWT.SINGLE);
        this.bottomLeftField.setInitialInputMode(TextExtension.TEXTCASE,null);
        // create and set the layout data
        FormData textBLdata = new FormData();
        textBLdata.left = new FormAttachment(bllabel, 0, SWT.LEFT);
        textBLdata.top = new FormAttachment(bllabel, 5, SWT.BOTTOM);
        textBLdata.right = new FormAttachment(40, 0);
        this.bottomLeftField.setLayoutData(textBLdata);

        // top right
        this.bottomRightField = new TextExtension(this.shell, SWT.BORDER | SWT.SINGLE);
        this.bottomRightField.setInitialInputMode(TextExtension.TITLECASE,null);
        // create and set the layout data
        FormData mdata = new FormData();
        mdata.left = new FormAttachment(mlabel, 0, SWT.LEFT);
        mdata.top = new FormAttachment(mlabel, 5, SWT.BOTTOM);
        mdata.right = new FormAttachment(90, 0);
        this.bottomRightField.setLayoutData(mdata);

    }

    private void changeExtension(int style)
    {

        this.topLeftField = this.replaceField(this.topLeftField, this.topLeftField.getStyle(), style);
        this.topRightField = this.replaceField(this.topRightField, this.topRightField.getStyle(), style);
        this.bottomLeftField = this.replaceField(this.bottomLeftField, this.bottomLeftField.getStyle(), style);
        this.bottomRightField = this.replaceField(this.bottomRightField, this.bottomRightField.getStyle(), style);

        // update the layout and redraw
        this.shell.layout();
        this.shell.update();
    }

    private void changeCase(int style)
    {

        this.topLeftField.setInitialInputMode(style,null);
        this.topRightField.setInitialInputMode(style,null);
        this.bottomLeftField.setInitialInputMode(style,null);
        this.bottomRightField.setInitialInputMode(style,null);

        // update the layout and redraw
        this.shell.layout();
        this.shell.update();
    }
    private void changeInput(String style)
    {

        this.topLeftField.setInitialInputMode(TextExtension.UPPERCASE,style);
        this.topRightField.setInitialInputMode(TextExtension.LOWERCASE,style);
        this.bottomLeftField.setInitialInputMode(TextExtension.TEXTCASE,style);
        this.bottomRightField.setInitialInputMode(TextExtension.TITLECASE,style);
        this.shell.setText(style);
        // update the layout and redraw
        this.shell.layout();
        this.shell.update();
    }
    /**
     * Change the style of the field if needed.
     * The styles defined by mask parameter will be cleaned up (unset)
     * before new style (style parameter) will be applied.
     * @param field A Text control
     * @param mask Set of styles to be cleaned
     * @param change A style constant to be set
     */
    private void changeStyle(TextExtension field, int mask, int change)
    {
        if (field == null || field.isDisposed()) return;

        int style = field.getStyle();
        if ((style & change) != change)  // need to update the style
        {
            // update the style
            style &= ~mask;
            style ^= change;

            field = this.replaceField(field, style,this.extension);

            // update the layout and redraw
            this.shell.layout();
            this.shell.update();
        }
    }

    /**
     * Set long texts.
     * @param nro, number of inserted char.
     */
    private void setLongTexts(int nro)
    {

        this.topLeftField.setText(this.thousand_chars.substring(0,nro));
        this.topRightField.setText(this.thousand_chars.substring(0,nro));
        this.bottomLeftField.setText(this.thousand_chars.substring(0,nro));
        this.bottomRightField.setText(this.thousand_chars.substring(0,nro));

        // update the layout and redraw
        this.shell.layout();
        this.shell.update();
    }

    /**
     * Set the wrap on or off.
     * @param wrapOn if true, wrapping will be switched on and off otherwise.
     */
    private void setWrap(TextExtension field, boolean wrapOn)
    {
        int style = field.getStyle();
        style &= ~SWT.WRAP;

        if (wrapOn)
        {
            style |= SWT.WRAP;
        }

        field = this.replaceField(field, style,this.extension);

        this.shell.layout();
        this.shell.update();
    }

    /**
     * Set the scrollbar(s) for specified field.
     * @param field a Text field
     * @param type type of the scrollbar(s) to be set
     */
    private void setScrollBar(TextExtension field, int type)
    {
        // clean up the scroll bars
        int style = field.getStyle();
        style &= ~SWT.V_SCROLL;
        style &= ~SWT.H_SCROLL;
        // set new scrollbar(s)
        if (type != SWT.NONE)
        {
            style |= type;
        }

        // replace the field
        field = this.replaceField(field, style,this.extension);

        // update the shell
        this.shell.layout();
        this.shell.update();
    }

    /**
     * Replace specified field with new one.
     * Creates new field with defined style and copies some parameters along
     * with data from the specified field.
     * @param field a field containing data
     * @return created field
     */
    private TextExtension replaceField(TextExtension field, int style,int extensionstyle)
    {
        try
        {
            // store current state of the field
            String text = field.getText();
            Point selection = field.getSelection();
            FormData data = (FormData) field.getLayoutData();
            TextExtension nfield = null;
            this.extension = extensionstyle;
            // create new field
            if (this.extension == -1)
            {
                nfield = new TextExtension(this.shell, style);
            }
            else
            {
                nfield = new TextExtension(this.shell, style, this.extension);
            }
            nfield.setText(text);
            nfield.setSelection(selection);
            nfield.setLayoutData(data);
            nfield.setFocus();

            // dispose old field
            field.dispose();

            return nfield;
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Text could not be changed: "+ex.toString());
            return field;
        }
    }

    /*
     *  (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent e)
    {
        if (!(e.widget instanceof Command)) return;

        TextExtension field = this.getFocusedField();

        // Do commands handling
        if (e.widget == this.cmdDoAppend)
        {
            // prompt for a text
            QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
            dlg.setPromptText("Input some text:", "default");
            dlg.setMaximum(15);
            String text = dlg.open();
            if (text != null)
            {
                field.append(text);
            }
            return;
        }
        if (e.widget == this.cmdDoInsert)
        {
            // prompt for a text
            QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
            dlg.setPromptText("Input some text:", "default");
            dlg.setMaximum(15);
            String text = dlg.open();
            if (text != null)
            {
                field.insert(text);
            }
            return;
        }

        // Settings commands handling
        // aligment
        if (e.widget == this.cmdAligmentLEFT)
        {
            int mask = SWT.NONE | SWT.CENTER | SWT.RIGHT;
            this.changeStyle(field, mask, SWT.LEFT);
            return;
        }
        if (e.widget == this.cmdAligmentCENTER)
        {
            int mask = SWT.NONE | SWT.LEFT | SWT.RIGHT;
            this.changeStyle(field, mask, SWT.CENTER);
            return;
        }
        if (e.widget == this.cmdAligmentRIGHT)
        {
            int mask = SWT.NONE | SWT.LEFT | SWT.CENTER;
            this.changeStyle(field, mask, SWT.RIGHT);
            return;
        }
        // type
        if (e.widget == this.cmdMULTI)
        {
            int mask = SWT.SINGLE;
            this.changeStyle(field, mask, SWT.MULTI);
            return;
        }
        if (e.widget == this.cmdSINGLE)
        {
            int mask = SWT.MULTI;
            this.changeStyle(field, mask, SWT.SINGLE);
            return;
        }
        // mode
        if (e.widget == this.cmdModeEditable)
        {
            field.setEditable(true);
            return;
        }
        if (e.widget == this.cmdModeReadOnly)
        {
            field.setEditable(false);
            return;
        }
        // text limit
        if (e.widget == this.cmdLimitCustom)
        {
            QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.NUMERIC);
            dlg.setPromptText("Input a number:", "10");
            dlg.setMaximum(4);
            String res = dlg.open();
            if (res != null && !("".equals(res)))
            {
                field.setTextLimit(Integer.parseInt(res));
            }
            return;
        }
        if (e.widget == this.cmdLimitNONE)
        {
            field.setTextLimit(Text.LIMIT);
            return;
        }
        // wrapping
        if (e.widget == this.cmdWrapYES)
        {
            this.setWrap(this.getFocusedField(), true);
            return;
        }
        if (e.widget == this.cmdWrapNO)
        {
            this.setWrap(this.getFocusedField(), false);
            return;
        }
        // adding long texts
        if (e.widget == this.cmdLong)
        {
            this.setLongTexts(this.longtext);
            return;
        }
        if (e.widget == this.cmdExtraLong)
        {
            this.setLongTexts(this.extralong);
            return;
        }
        if (e.widget == this.cmdOverExtraLong)
        {
            this.setLongTexts(this.overextra);
            return;
        }
        // scrolling
        if (e.widget == this.cmdScrollNONE)
        {
            this.setScrollBar(field, SWT.NONE);
            return;
        }
        if (e.widget == this.cmdScrollVertical)
        {
            this.setScrollBar(field, SWT.V_SCROLL);
            return;
        }
        if (e.widget == this.cmdScrollBOTH)
        {
            this.setScrollBar(field, SWT.H_SCROLL | SWT.V_SCROLL);
            return;
        }
        if (e.widget == this.cmdScrollHorizontal)
        {
            this.setScrollBar(field, SWT.H_SCROLL);
            return;
        }
        if (e.widget == this.cmdEmail)
        {
            this.shell.setText("EMAILADDRESS - TextExtension Test");
            this.changeExtension(TextExtension.EMAILADDRESS);
            return;
        }
        if (e.widget == this.cmdURL)
        {
            this.shell.setText("URL - TextExtension Test");
            this.changeExtension(TextExtension.URL);
            return;
        }
        if (e.widget == this.cmdNonPred)
        {
            this.shell.setText("NonPred - TextExtension Test");
            this.changeExtension(TextExtension.NON_PREDICTIVE);
            return;
        }
        if (e.widget == this.cmdLatinOnly)
        {
            this.shell.setText("LatinOnly - TextExtension Test");
            this.changeExtension(TextExtension.LATIN_INPUT_ONLY);
            return;
        }
        if (e.widget == this.cmdEmail_Non)
        {
            this.shell.setText("Email_Non - TextExtension Test");
            this.changeExtension(TextExtension.EMAILADDRESS | TextExtension.NON_PREDICTIVE);
            return;
        }
        if (e.widget == this.cmdURL_Non)
        {
            this.shell.setText("URL_Non - TextExtension Test");
            this.changeExtension(TextExtension.URL | TextExtension.NON_PREDICTIVE);
            return;
        }
        if (e.widget == this.cmdEmail_LatinOnly_Non)
        {
            this.shell.setText("Email_LatinOnly_Non - TextExtension Test");
            this.changeExtension(TextExtension.EMAILADDRESS | TextExtension.NON_PREDICTIVE | TextExtension.LATIN_INPUT_ONLY);
            return;
        }
        if (e.widget == this.cmdEmail_LatinOnly)
        {
            this.shell.setText("Email_LatinOnly - TextExtension Test");
            this.changeExtension(TextExtension.EMAILADDRESS | TextExtension.LATIN_INPUT_ONLY);
            return;
        }
        if (e.widget == this.cmdURL_LatinOnly_Non)
        {
            this.shell.setText("URL_LatinOnly_Non - TextExtension Test");
            this.changeExtension(TextExtension.URL | TextExtension.NON_PREDICTIVE | TextExtension.LATIN_INPUT_ONLY);
            return;
        }
        if (e.widget == this.cmdURL_LatinOnly)
        {
            this.shell.setText("URL_LatinOnly - TextExtension Test");
            this.changeExtension(TextExtension.URL | TextExtension.LATIN_INPUT_ONLY);
            return;
        }
        if (e.widget == this.cmdUPPERCASE)
        {
            this.changeCase(TextExtension.UPPERCASE);
            return;
        }
        if (e.widget == this.cmdLOWERCASE)
        {
            this.changeCase(TextExtension.LOWERCASE);
            return;
        }
        if (e.widget == this.cmdTEXTCASE)
        {
            this.changeCase(TextExtension.TEXTCASE);
            return;
        }
        if (e.widget == this.cmdTITLECASE)
        {
            this.changeCase(TextExtension.TITLECASE);
            return;
        }
        if (e.widget == this.cmdBASIC_LATIN || e.widget == this.cmdGREEK ||
                e.widget == this.cmdCYRILLIC || e.widget == this.cmdARABIC ||
                e.widget == this.cmdBENGALI || e.widget == this.cmdARMENIAN ||
                e.widget == this.cmdTHAI || e.widget == this.cmdHEBREW ||
                e.widget == this.cmdDEVANAGARI || e.widget == this.cmdFULLWIDTH_DIGITS ||
                e.widget == this.cmdFULLWIDTH_LATIN || e.widget == this.cmdLATIN ||
                e.widget == this.cmdHALFWIDTH_KATAKANA || e.widget == this.cmdHANGUL_SYLLABLES ||
                e.widget == this.cmdHANJA || e.widget == this.cmdHIRAGANA ||
                e.widget == this.cmdKANJI || e.widget == this.cmdKATAKANA ||
                e.widget == this.cmdLATIN_DIGITS || e.widget == this.cmdSIMPLIFIED_HANZI ||
                e.widget == this.cmdTRADITIONAL_HANZI)
        {

            this.changeInput(((Command)(e.widget)).getText());

            return;
        }

        super.widgetSelected(e);
    }

    /**
     * Return currently focused Text control.
     * @return currently selected Text field, null otherwise.
     */
    private TextExtension getFocusedField()
    {
        Control control = this.shell.getDisplay().getFocusControl();
        if (control instanceof TextExtension)
            return (TextExtension) control;
        else
            return null;
    }

}
