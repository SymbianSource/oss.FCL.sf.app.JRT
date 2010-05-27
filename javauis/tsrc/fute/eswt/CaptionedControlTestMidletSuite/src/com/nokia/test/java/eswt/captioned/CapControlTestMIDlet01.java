/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.test.java.eswt.captioned;

import java.util.Date;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.TreeItem;
import org.eclipse.ercp.swt.mobile.CaptionedControl;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.DateEditor;
import org.eclipse.ercp.swt.mobile.HyperLink;
import org.eclipse.ercp.swt.mobile.ListBox;
import org.eclipse.ercp.swt.mobile.ListBoxItem;
import org.eclipse.ercp.swt.mobile.ListView;
import org.eclipse.ercp.swt.mobile.SortedList;


import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * A midlet for the CaptionedControl test case set.
 */
public class CapControlTestMIDlet01 extends TestBaseMIDlet
{

    /** Images */
    private Image imageSMALL    = null;
    private Image imageBIG      = null;

    /** Setting fields */
    private Composite settingFields = null;
    private Text textField          = null;
    private Text trailField         = null;
    private Label imageField        = null;
    private Label controlField      = null;

    /** */
    private Control control         = null;
    private CaptionedControl field  = null;

    /** Commands */
    private Command cmdSetTextDEFINE    = null;
    private Command cmdSetTextNULL      = null;
    private Command cmdSetTrailDEFINE   = null;
    private Command cmdSetTrailNULL     = null;
    private Command cmdSetImageNULL     = null;
    private Command cmdSetImageSMALL    = null;
    private Command cmdSetImageBIG      = null;

    private Command cmdControlLABEL     = null;
    private Command cmdControlHLINK     = null;
    private Command cmdControlTEXT      = null;
    private Command cmdControlCTEXT     = null;
    private Command cmdControlDATE      = null;
    private Command cmdControlBUTTON    = null;
    private Command cmdControlSLIDER    = null;
    private Command cmdControlPBAR      = null;

    private Command cmdExample = null;
    private Command cmdCombo    = null;
    private Command cmdButtons = null;
    private Command cmdDates = null;
    private Command cmdLists    = null;
    private Command cmdListView     = null;
    private Command cmdSortedLists  = null;
    private Command cmdTree     = null;
    private Command cmdTable    = null;
    private Command cmdClear    = null;
    private Command cmdCreate   = null;
    private Command cmdUpdate   = null;

    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#init()
     */
    protected void init()
    {
        // set title & layout
        this.shell.setText("Captioned Control Test");
        this.shell.setLayout(new GridLayout());

        // create setting fields composite
        this.settingFields = new Composite(this.shell, SWT.NONE);
        this.settingFields.setLayout(new GridLayout(2, false));
        GridData data = new GridData();
        data.grabExcessHorizontalSpace = true;
        data.horizontalAlignment = SWT.FILL;
        this.settingFields.setLayoutData(data);

        // load images
        this.imageBIG = new Image(
            this.display, this.getClass().getResourceAsStream("/big.png"));
        this.imageSMALL = new Image(
            this.display, this.getClass().getResourceAsStream("/small.png"));

        // create commands
        Command cmdSet = new Command(this.shell, Command.COMMANDGROUP, 2);
        cmdSet.setText("Set");

        // text submennu
        Command cmdText = new Command(cmdSet, Command.COMMANDGROUP, 3);
        cmdText.setText("Text");

        this.cmdSetTextDEFINE = new Command(cmdText, Command.GENERAL, 1);
        this.cmdSetTextDEFINE.setText("define");
        this.cmdSetTextDEFINE.addSelectionListener(this);

        this.cmdSetTextNULL = new Command(cmdText, Command.GENERAL, 0);
        this.cmdSetTextNULL.setText("null");
        this.cmdSetTextNULL.addSelectionListener(this);

        // trailing text submenu
        Command cmdTrail = new Command(cmdSet, Command.COMMANDGROUP, 2);
        cmdTrail.setText("Trailing text");

        this.cmdSetTrailDEFINE = new Command(cmdTrail, Command.GENERAL, 1);
        this.cmdSetTrailDEFINE.setText("define");
        this.cmdSetTrailDEFINE.addSelectionListener(this);

        this.cmdSetTrailNULL = new Command(cmdTrail, Command.GENERAL, 0);
        this.cmdSetTrailNULL.setText("null");
        this.cmdSetTrailNULL.addSelectionListener(this);

        // image submenu
        Command cmdImage = new Command(cmdSet, Command.COMMANDGROUP, 1);
        cmdImage.setText("Image");

        this.cmdSetImageSMALL = new Command(cmdImage, Command.GENERAL, 2);
        this.cmdSetImageSMALL.setText("small");
        this.cmdSetImageSMALL.addSelectionListener(this);

        this.cmdSetImageBIG = new Command(cmdImage, Command.GENERAL, 1);
        this.cmdSetImageBIG.setText("big");
        this.cmdSetImageBIG.addSelectionListener(this);

        this.cmdSetImageNULL = new Command(cmdImage, Command.GENERAL, 0);
        this.cmdSetImageNULL.setText("null");
        this.cmdSetImageNULL.addSelectionListener(this);

        // controls submenu
        Command cmdControl = new Command(cmdSet, Command.COMMANDGROUP, 0);
        cmdControl.setText("Control");
        this.cmdControlLABEL = new Command(cmdControl, Command.GENERAL, 7);
        this.cmdControlLABEL.setText("Label");
        this.cmdControlLABEL.addSelectionListener(this);

        this.cmdControlHLINK = new Command(cmdControl, Command.GENERAL, 6);
        this.cmdControlHLINK.setText("HyperLink");
        this.cmdControlHLINK.addSelectionListener(this);

        this.cmdControlTEXT = new Command(cmdControl, Command.GENERAL, 5);
        this.cmdControlTEXT.setText("Text");
        this.cmdControlTEXT.addSelectionListener(this);

        this.cmdControlCTEXT = new Command(cmdControl, Command.GENERAL, 4);
        this.cmdControlCTEXT.setText("ConstrainedText");
        this.cmdControlCTEXT.addSelectionListener(this);

        this.cmdControlDATE = new Command(cmdControl, Command.GENERAL, 3);
        this.cmdControlDATE.setText("DateEditor");
        this.cmdControlDATE.addSelectionListener(this);

        this.cmdControlBUTTON = new Command(cmdControl, Command.GENERAL, 2);
        this.cmdControlBUTTON.setText("Button");
        this.cmdControlBUTTON.addSelectionListener(this);

        this.cmdControlSLIDER = new Command(cmdControl, Command.GENERAL, 1);
        this.cmdControlSLIDER.setText("Slider");
        this.cmdControlSLIDER.addSelectionListener(this);

        this.cmdControlPBAR = new Command(cmdControl, Command.GENERAL, 0);
        this.cmdControlPBAR.setText("ProgressBar");
        this.cmdControlPBAR.addSelectionListener(this);

        // Clear command
        this.cmdClear = new Command(this.shell, Command.GENERAL, 1);
        this.cmdClear.setText("Clear");
        this.cmdClear.addSelectionListener(this);


        // examples submenu
        Command cmdExampleGroup = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdExampleGroup.setText("Example");
        // Create Example commands
        this.cmdExample = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdExample.setText("Text");
        this.cmdExample.addSelectionListener(this);

        this.cmdCombo = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdCombo.setText("Combo");
        this.cmdCombo.addSelectionListener(this);

        this.cmdButtons = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdButtons.setText("Buttons");
        this.cmdButtons.addSelectionListener(this);

        this.cmdDates = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdDates.setText("DateEditor");
        this.cmdDates.addSelectionListener(this);

        this.cmdLists = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdLists.setText("Lists");
        this.cmdLists.addSelectionListener(this);

        this.cmdListView = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdListView.setText("ListView");
        this.cmdListView.addSelectionListener(this);

        this.cmdSortedLists = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdSortedLists.setText("SortedList");
        this.cmdSortedLists.addSelectionListener(this);

        this.cmdTable = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdTable.setText("Table");
        this.cmdTable.addSelectionListener(this);

        this.cmdTree = new Command(cmdExampleGroup, Command.GENERAL, 0);
        this.cmdTree.setText("Tree");
        this.cmdTree.addSelectionListener(this);

        // Create & Change commands
        this.cmdCreate = new Command(this.shell, Command.GENERAL, 0);
        this.cmdCreate.setText("Create");
        this.cmdCreate.addSelectionListener(this);

        this.cmdUpdate = new Command(this.shell, Command.GENERAL, 0);
        this.cmdUpdate.setText("Update");
        this.cmdUpdate.addSelectionListener(this);
        this.cmdUpdate.setEnabled(false);
    }


    /* (non-Javadoc)
     * @see com.nokia.test.java.eswt.midlet.TestBaseMIDlet#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        // text commands
        if (event.widget == this.cmdSetTextDEFINE)
        {
            if (this.textField == null || this.textField.isDisposed())
            {
                // create new Text field
                this.textField = this.createTextField("Text: ");
            }
            if (!this.textField.isEnabled())
            {
                // enable the field and clear its content
                this.textField.setEnabled(true);
                this.textField.setText("");
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdSetTextNULL)
        {
            if (this.textField == null || this.textField.isDisposed())
            {
                // create new Text field
                this.textField = this.createTextField("Text: ");
            }
            // disable the field
            this.textField.setText("null");
            this.textField.setEnabled(false);
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // trailing text commands
        if (event.widget == this.cmdSetTrailDEFINE)
        {
            if (this.trailField == null || this.trailField.isDisposed())
            {
                // create new Text field
                this.trailField = this.createTextField("Trailing text: ");
            }
            if (!this.trailField.isEnabled())
            {
                // enable the field and clear its content
                this.trailField.setEnabled(true);
                this.trailField.setText("");
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdSetTrailNULL)
        {
            if (this.trailField == null || this.trailField.isDisposed())
            {
                // create new Text field
                this.trailField = this.createTextField("Trailing text: ");
            }
            // disable the field
            this.trailField.setText("null");
            this.trailField.setEnabled(false);
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // image commands
        if (event.widget == this.cmdSetImageSMALL)
        {
            if (this.imageField == null || this.imageField.isDisposed())
            {
                this.imageField = this.createInfoLabel("Image: ", "SMALL");
            }
            else
            {
                this.imageField.setText("SMALL");
                this.imageField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdSetImageBIG)
        {
            if (this.imageField == null || this.imageField.isDisposed())
            {
                this.imageField = this.createInfoLabel("Image: ", "BIG");
            }
            else
            {
                this.imageField.setText("BIG");
                this.imageField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdSetImageNULL)
        {
            if (this.imageField == null || this.imageField.isDisposed())
            {
                this.imageField = this.createInfoLabel("Image: ", "null");
            }
            else
            {
                this.imageField.setText("null");
                this.imageField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // control commands
        if (event.widget == this.cmdControlLABEL)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField = this.createInfoLabel("Control: ", "Label");
            }
            else
            {
                this.controlField.setText("Label");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdControlHLINK)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField =
                    this.createInfoLabel("Control: ", "HyperLink");
            }
            else
            {
                this.controlField.setText("HyperLink");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdControlTEXT)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField = this.createInfoLabel("Control: ", "Text");
            }
            else
            {
                this.controlField.setText("Text");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdControlCTEXT)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField =
                    this.createInfoLabel("Control: ", "ConstrainedText");
            }
            else
            {
                this.controlField.setText("ConstrainedText");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdControlDATE)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField =
                    this.createInfoLabel("Control: ", "DateEditor");
            }
            else
            {
                this.controlField.setText("DateEditor");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdControlSLIDER)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField =
                    this.createInfoLabel("Control: ", "Slider");
            }
            else
            {
                this.controlField.setText("Slider");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdControlPBAR)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField =
                    this.createInfoLabel("Control: ", "ProgressBar");
            }
            else
            {
                this.controlField.setText("ProgressBar");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdControlBUTTON)
        {
            if (this.controlField == null || this.controlField.isDisposed())
            {
                this.controlField =
                    this.createInfoLabel("Control: ", "Button");
            }
            else
            {
                this.controlField.setText("Button");
                this.controlField.pack();
            }
            //update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Create command
        if (event.widget == this.cmdCreate)
        {
            // create CaptionedControl
            this.create();
            this.cmdUpdate.setEnabled(true);
            this.cmdCreate.setEnabled(false);
            return;
        }
        // Example command
        if (event.widget == this.cmdExample)
        {
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);
            // create CaptionedControl
            this.control = new Text(this.field, SWT.BORDER);
            Command cmd1 = new Command(this.control, Command.GENERAL, 0);
            cmd1.setText("Item 1");
            cmd1.addSelectionListener(this);
            Command cmd2 = new Command(this.control, Command.GENERAL, 0);
            cmd2.setText("Item 2");
            cmd2.addSelectionListener(this);
            Command cmd3 = new Command(this.control, Command.GENERAL, 0);
            cmd3.setText("Item 3");
            cmd3.addSelectionListener(this);
            this.field.setText("Text");
            this.field.setTrailingText("?!?!");
            this.field.setImage(this.imageSMALL);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Buttons command
        if (event.widget == this.cmdButtons)
        {
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);
            // add buttons to captionedcontrol
            Button check1 = new Button(this.field, SWT.CHECK);
            check1.setText("option 1");
            Button check2 = new Button(this.field, SWT.CHECK);
            check2.setText("option 2");
            Button check3 = new Button(this.field, SWT.CHECK);
            check3.setText("option 3");

            this.field.setText("Select option");
            this.field.setTrailingText("?!?!");
            this.field.setImage(this.imageSMALL);
            CaptionedControl radioButtons = new CaptionedControl(this.shell, SWT.NONE);
            radioButtons.setLayoutData(data);
            // add buttons to captionedcontrol
            Button radio = new Button(radioButtons, SWT.RADIO);
            radio.setText("Good option");
            Button radio2 = new Button(radioButtons, SWT.RADIO);
            radio2.setText("Better option");
            Button radio3 = new Button(radioButtons, SWT.RADIO);
            radio3.setText("The ultimate option!!!");
            radioButtons.setText("Select next option");
            radioButtons.setTrailingText("?!?!");
            radioButtons.setImage(this.imageSMALL);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Combo command
        if (event.widget == this.cmdCombo)
        {
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);
            // create Combo in capcontrol
            Combo combo1 = new Combo(this.field, SWT.BORDER);
            for (int i = 0; i < 10; i++)
            {
                combo1.add("Very very long item in combo "+i);
            }
            this.field.setText("Combo with long items");
            this.field.setTrailingText("?!?!");
            this.field.setImage(this.imageSMALL);

            CaptionedControl capCombo = new CaptionedControl(this.shell, SWT.NONE);
            capCombo.setLayoutData(data);

            Combo combo2 = new Combo(capCombo, SWT.BORDER);
            for (int i = 0; i < 10; i++)
            {
                combo2.add("combo item "+i);
            }
            capCombo.setText("Combo");
            capCombo.setTrailingText("?!?!");
            capCombo.setImage(this.imageSMALL);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Combo command
        if (event.widget == this.cmdDates)
        {
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.FILL, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);
            // create Combo in capcontrol
            DateEditor date = new DateEditor(this.field, SWT.BORDER, DateEditor.DATE);

            this.field.setText("DATE");
            this.field.setTrailingText("today");


            CaptionedControl capTime = new CaptionedControl(this.shell, SWT.NONE);
            capTime.setLayoutData(data);

            DateEditor time = new DateEditor(capTime, SWT.BORDER, DateEditor.TIME);

            capTime.setText("TIME");
            capTime.setTrailingText("is now");

            CaptionedControl capDateTime = new CaptionedControl(this.shell, SWT.NONE);
            capDateTime.setLayoutData(data);

            DateEditor datetime = new DateEditor(capDateTime, SWT.BORDER, DateEditor.DATE_TIME);

            capDateTime.setText("DATE_TIME");
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Table command
        if (event.widget == this.cmdTable)
        {
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);
            // create table in capcontrol
            Table table1 = this.createTable(SWT.SINGLE, 3, 2, true, this.field);

            this.field.setText("Single Table");
            this.field.setTrailingText("?!?!");
            this.field.setImage(this.imageSMALL);

            CaptionedControl capTable = new CaptionedControl(this.shell, SWT.NONE);
            capTable.setLayoutData(data);

            // create table in capcontrol
            Table table2 = this.createTable(SWT.MULTI, 3, 2, true, capTable);

            capTable.setText("Multi Table");
            capTable.setTrailingText("?!?!");
            capTable.setImage(this.imageSMALL);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Tree command
        if (event.widget == this.cmdTree)
        {
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);
            // create table in capcontrol
            Tree tree1 = this.createTree(SWT.SINGLE, 3, 2, this.field);

            this.field.setText("Single Tree");
            this.field.setTrailingText("?!?!");
            this.field.setImage(this.imageSMALL);

            CaptionedControl capTree = new CaptionedControl(this.shell, SWT.NONE);
            capTree.setLayoutData(data);

            // create table in capcontrol
            Tree tree2 = this.createTree(SWT.MULTI, 3, 2, capTree);

            capTree.setText("Multi Tree");
            capTree.setTrailingText("?!?!");
            capTree.setImage(this.imageSMALL);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }

        // Lists command
        if (event.widget == this.cmdLists)
        {
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);
            // create table in capcontrol
            List list = new List(this.field,SWT.SINGLE);
            for (int i = 0; i < 5; i++)
            {
                list.add("Item "+i);
            }
            this.field.setText("Single List");
            this.field.setTrailingText("?!?!");
            this.field.setImage(this.imageSMALL);

            CaptionedControl capListBox = new CaptionedControl(this.shell, SWT.NONE);
            capListBox.setLayoutData(data);

            // create listBox in capcontrol
            ListBox listBox = new ListBox(capListBox,SWT.MULTI,ListBox.LB_STYLE_2LINE_ITEM);
            ListBoxItem[] items = new ListBoxItem[5];

            for (int i = 0; i < items.length; i++)
            {
                items[i] = new ListBoxItem("detail "+(i+1),null,"head "+(i+1),null);
            }
            listBox.setDataModel(items);
            capListBox.setText("Multi ListBox");
            capListBox.setTrailingText("?!?!");
            capListBox.setImage(this.imageSMALL);

            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdListView)
        {
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;

            CaptionedControl capListview = new CaptionedControl(this.shell, SWT.NONE);
            capListview.setLayoutData(data);
            // create listBox in capcontrol
            ListView listView = new ListView(capListview,SWT.MULTI);

            for (int i = 0; i < 6; i++)
            {
                listView.add("Item"+i, null);
            }

            capListview.setText("Multi ListView");
            capListview.setTrailingText("?!?!");
            capListview.setImage(this.imageSMALL);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        if (event.widget == this.cmdSortedLists)
        {
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;

            CaptionedControl capsorted = new CaptionedControl(this.shell, SWT.NONE);
            capsorted.setLayoutData(data);
            // create listBox in capcontrol
            SortedList sortedlist = new SortedList(capsorted,SWT.SINGLE);

            for (int i = 0; i < 6; i++)
            {
                sortedlist.add("Item"+i);
            }

            capsorted.setText("SortedList");
            capsorted.setTrailingText("?!?!");
            capsorted.setImage(this.imageSMALL);

            CaptionedControl capsortedfilter = new CaptionedControl(this.shell, SWT.NONE);
            capsortedfilter.setLayoutData(data);
            // create listBox in capcontrol
            SortedList sortedlist2 = new SortedList(capsortedfilter,SWT.SINGLE,SortedList.FILTER);

            for (int i = 0; i < 6; i++)
            {
                sortedlist2.add("Item"+i);
            }

            capsortedfilter.setText("Filter SortedList");
            capsortedfilter.setTrailingText("?!?!");
            capsortedfilter.setImage(this.imageSMALL);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }
        // Update Command
        if (event.widget == this.cmdUpdate)
        {
            this.update();
            return;
        }
        // Clear command
        if (event.widget == this.cmdClear)
        {
            // dispose setting fields
            Control[] controls = this.settingFields.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].dispose();
            }
            // dispose captioned control
            if (this.field != null && !this.field.isDisposed())
            {
                this.field.dispose();
            }
            // dispose all the controls fields
            controls = this.shell.getChildren();
            for (int i=0; i<controls.length; i++)
            {
                controls[i].dispose();
            }
            this.cmdUpdate.setEnabled(false);
            this.cmdCreate.setEnabled(true);
            // update the shell
            this.shell.layout();
            this.shell.update();
            return;
        }

        if (event.widget == this.cmdExit)
        {
            // dispose images
            this.imageSMALL.dispose();
            this.imageSMALL = null;
            this.imageBIG.dispose();
            this.imageBIG = null;
        }

        super.widgetSelected(event);
    }

    private Table createTable(int style, int rows, int cols, boolean pack, Composite parent)
    {

        Table table;
        if (pack && style == SWT.MULTI)
        {
            table = new Table(parent,style|SWT.CHECK);
        }
        else
        {
            table = new Table(parent,style);
        }

        final int ROWS = rows;
        final int COLS = cols;
        for (int i = 0; i < ROWS; ++i)
        {
            new TableItem(table, 0);
        }
        for (int c = 0; c < COLS; ++c)
        {
            TableColumn column = new TableColumn(table, 0);
            column.setText("Column " + c);
            for (int i = 0; i < ROWS; ++i)
            {
                table.getItem(i).setText(c, "Item " + c + ", " + i);
            }
            if (pack)
                column.pack();
            else
            {
                column.setWidth((int)(this.shell.getClientArea().width/2));
                column.setAlignment(SWT.CENTER);
            }
        }
        table.setHeaderVisible(true);
        table.setLinesVisible(true);
        table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        if (style == SWT.SINGLE)
        {
            table.addSelectionListener(this);
        }
        return table;
    }
    protected void populateTree(Tree tree,int root,int child)
    {
        TreeItem myitem = null, myitem2 = null;
        int ROOT_ITEM_COUNT = root;
        int CHILD_ITEM_COUNT = child;
        for (int i=0; i<ROOT_ITEM_COUNT; i++)
        {
            TreeItem iItem = new TreeItem(tree, 0);
            iItem.setText("Item (0) -" + i);
            if (i == 2)
                myitem = iItem;
            for (int j=0; j<CHILD_ITEM_COUNT; j++)
            {
                TreeItem jItem = new TreeItem(iItem, SWT.CHECK);
                jItem.setText("TreeTreeItem (1) - " + j);

                if (i == 2 && j == 2)
                    myitem2 = jItem;
                for (int k=0; k<4; k++)
                {
                    TreeItem kItem = new TreeItem(jItem, 0);
                    kItem.setText("I (2) - " + k);

                    for (int l=0; l<CHILD_ITEM_COUNT; l++)
                    {
                        TreeItem lItem = new TreeItem(kItem, 0);
                        lItem.setText("TreeItem (3) - " + l);
                    }
                }
            }
        }

    }

    private Tree createTree(int style, int root, int child, Composite parent)
    {

        Tree tree = null;
        if (style == SWT.MULTI)
        {
            tree = new Tree(parent,style|SWT.CHECK);
        }
        else
        {
            tree = new Tree(parent,style);
        }

        this.populateTree(tree,root,child);

        tree.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        tree.addSelectionListener(this);
        return tree;
    }
    /**
     * Create CaptionedControl field.
     */
    private void create()
    {
        try
        {
            // create new captioned control
            this.field = new CaptionedControl(this.shell, SWT.NONE);
            GridData data = new GridData(SWT.CENTER, SWT.CENTER, true, true);
            data.horizontalSpan = 2;
            this.field.setLayoutData(data);

            // set text
            this.setText();

            // set trailling text
            this.setTrailingText();

            // set image
            this.setImage();

            // create child control
            this.control = this.createControl();

            // update the shell
            this.shell.layout();
            this.shell.update();
        }
        catch (Exception ex)
        {
            MessageBox msg = new MessageBox(this.shell, SWT.BORDER);
            msg.setMessage("Exception: " + ex.getMessage());
            msg.open();
        }
    }

    /**
     * Update ConstrainedControl.
     */
    private void update()
    {
        try
        {
            // set text
            this.setText();

            // set trailing text
            this.setTrailingText();

            // set an image
            this.setImage();

            // create and set control
            if (this.control != null && !this.control.isDisposed())
            {
                this.control.dispose();
            }
            this.control = this.createControl();

            // update the field and the shell
            this.field.pack();
            this.field.layout();
            this.field.update();
            this.shell.layout();
            this.shell.update();

        }
        catch (Exception ex)
        {
            MessageBox msg = new MessageBox(this.shell, SWT.BORDER);
            msg.setMessage("Exception: " + ex.getMessage());
            msg.open();
        }
    }

    /**
     * Create new child control.
     * @return created control
     */
    private Control createControl()
    {
        Control result = null;

        String type = "Label";// default
        if (this.controlField != null && !this.controlField.isDisposed())
        {
            type = this.controlField.getText();
        }
        if ("Label".equals(type))
        {
            Label control = new Label(this.field, SWT.NONE);
            control.setText("Label");
            result = control;
        }
        else if ("HyperLink".equals(type))
        {
            HyperLink control =
                new HyperLink(this.field, SWT.NONE, HyperLink.EMAIL);
            control.setText("hyperlink@foo.com");
            result = control;
        }
        else if ("Text".equals(type))
        {
            Text control = new Text(this.field, SWT.BORDER);
            control.setText("Text");
            result = control;
        }
        else if ("ConstrainedText".equals(type))
        {
            ConstrainedText control = new ConstrainedText(this.field,
                    SWT.BORDER, ConstrainedText.NUMERIC);
            control.setText("12345");
            result = control;
        }
        else if ("DateEditor".equals(type))
        {
            DateEditor control = new DateEditor(this.field, SWT.BORDER);
            control.setDate(new Date());
            result = control;
        }
        else if ("Button".equals(type))
        {
            Button control = new Button(this.field, SWT.PUSH);
            control.setText("Button");
            result = control;
        }
        else if ("Slider".equals(type))
        {
            Slider control = new Slider(this.field, SWT.NONE);
            control.setMaximum(20);
            result = control;
        }
        else if ("ProgressBar".equals(type))
        {
            ProgressBar control = new ProgressBar(this.field, SWT.NONE);
            control.setMaximum(20);
            result = control;
        }

        return result;
    }

    /**
     * Create new Text setting field.
     */
    private Text createTextField(String title)
    {
        Label label = new Label(this.settingFields, SWT.NONE);
        label.setText(title);
        label.setLayoutData(
            new GridData(SWT.RIGHT, SWT.CENTER, false, false));

        Text result = new Text(this.settingFields, SWT.BORDER);
        result.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        return result;
    }

    /**
     * Create info label.
     */
    private Label createInfoLabel(String title, String value)
    {
        Label label = new Label(this.settingFields, SWT.NONE);
        label.setText(title);
        label.setLayoutData(
            new GridData(SWT.RIGHT, SWT.CENTER, false, false));

        Label result = new Label(this.settingFields, SWT.NONE);
        result.setText(value);
        result.setLayoutData(new GridData(SWT.LEFT, SWT.CENTER, true, false));

        return result;
    }

    /**
     * Set text.
     */
    private void setText()
    {
        if (this.textField != null && !this.textField.isDisposed())
        {
            if (this.textField.isEnabled())
            {
                this.field.setText(this.textField.getText());
            }
            else
            {
                this.field.setText(null);
            }
        }
    }

    /**
     * Set trailing text.
     */
    private void setTrailingText()
    {
        if (this.trailField != null && !this.trailField.isDisposed())
        {
            if (this.trailField.isEnabled())
            {
                this.field.setTrailingText(this.trailField.getText());
            }
            else
            {
                this.field.setTrailingText(null);
            }
        }
    }

    /**
     * Set image.
     */
    private void setImage()
    {
        if (this.imageField != null && !this.imageField.isDisposed())
        {
            if (this.imageField.isEnabled())
            {
                String type = this.imageField.getText();
                if ("SMALL".equals(type))
                {
                    this.field.setImage(this.imageSMALL);
                }
                if ("BIG".equals(type))
                {
                    this.field.setImage(this.imageBIG);
                }
                if ("null".equals(type))
                {
                    this.field.setImage(null);
                }
            }
            else
            {
                this.field.setImage(null);
            }
        }
    }
}
