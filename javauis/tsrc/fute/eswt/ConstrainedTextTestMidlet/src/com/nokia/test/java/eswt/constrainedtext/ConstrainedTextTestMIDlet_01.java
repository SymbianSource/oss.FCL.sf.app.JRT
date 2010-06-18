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
package com.nokia.test.java.eswt.constrainedtext;

import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the constrainedtext component
 * - caret position in constrainedtext
 * - commands action in phonebook
 * - copy & paste in constrainedtext
 * - creating constrained text
 * - inserting characters over text limit to constrainedtext
 * - inserting characters  to constrainedtext control
 * - setting constrainedtext non-editable
 */
public class ConstrainedTextTestMIDlet_01 extends TestBaseMIDlet implements FocusListener
{
    /** number inupt */
    private ConstrainedText numCtext = null;
    /** decimal input */
    private ConstrainedText decCtext = null;
    /** phonenumber input */
    private ConstrainedText phnCtext = null;
    /** non-editable test input */
    private ConstrainedText nonCtext = null;
    /** "change state" menuitem */
    private MenuItem mnuChange = null;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Constrained Text");
        this.shell.setLayout(new GridLayout(2, false));

        // set up the constrained textfields
        // label for numeric text field
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("Numeric:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // the numeric input field
        this.numCtext = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.numCtext.setTextLimit(10);
        this.numCtext.setText(String.valueOf(123));
        this.numCtext.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        // label for decimal text field
        label = new Label(this.shell, SWT.NONE);
        label.setText("Decimal:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // the decimal input field
        this.decCtext = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.DECIMAL);
        this.decCtext.setTextLimit(15);
        this.decCtext.setText(String.valueOf(-1.23));
        this.decCtext.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        // label for phonenumber text field
        label = new Label(this.shell, SWT.NONE);
        label.setText("Phonenumber:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // the phonenumber input field
        this.phnCtext = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.PHONENUMBER);
        this.phnCtext.setTextLimit(20);
        this.phnCtext.setText("+1*P#G");
        this.phnCtext.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        // label for the non-editable test input field
        label = new Label(this.shell, SWT.NONE);
        label.setText("Non-editable:");
        label.setLayoutData(new GridData(SWT.RIGHT, SWT.CENTER, false, false));
        // the non-editable test input field
        this.nonCtext = new ConstrainedText(this.shell, SWT.BORDER, ConstrainedText.NUMERIC);
        this.nonCtext.setText("7357");
        this.nonCtext.setEditable(false);
        this.nonCtext.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        this.nonCtext.addFocusListener(this);

        // setup the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "insert" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Insert");
        item.setMenu(menu);
        // ""123"" menuitem for "insert" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"123\"");
        item.addSelectionListener(this);
        // ""-1.23"" menuitem for "insert" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"-1.23\"");
        item.addSelectionListener(this);
        // ""+1*P#G"" menuitem for "insert" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"+1*P#G\"");
        item.addSelectionListener(this);
        // ""test"" menuitem for "insert" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"test\"");
        item.addSelectionListener(this);
        // "append" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Append");
        item.setMenu(menu);
        // ""123"" menuitem for "append" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"123\"");
        item.addSelectionListener(this);
        // ""-1.23"" menuitem for "append" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"-1.23\"");
        item.addSelectionListener(this);
        // ""+1*P#G"" menuitem for "append" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"+1*P#G\"");
        item.addSelectionListener(this);
        // ""test"" menuitem for "append" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("\"test\"");
        item.addSelectionListener(this);
        // "change state" menuitem
        this.mnuChange = new MenuItem(mainmenu, SWT.PUSH);
        this.mnuChange.setText("Change State");
        this.mnuChange.addSelectionListener(this);
        // "set position (3)" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Set Position (3)");
        item.addSelectionListener(this);
    }

    /**
     * determine the focused input field
     * @return the focused constrainedtext field
     */
    private ConstrainedText getact()
    {
        if (this.numCtext.isFocusControl())
            return this.numCtext;
        else if (this.decCtext.isFocusControl())
            return this.decCtext;
        else if (this.phnCtext.isFocusControl())
            return this.phnCtext;
        else
            return this.nonCtext;
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem) event.widget).getText();
            ConstrainedText act = this.getact();
            try
            {
                // try to do the selected action
                if ("\"123\"".equals(scmd))
                {
                    scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                    if ("Insert".equals(scmd))
                        // insert "123" at the i-beam's position
                        act.insert("123");
                    else if ("Append".equals(scmd))
                        // append "123" to the focused field
                        act.append("123");
                    return;
                }
                if ("\"-1.23\"".equals(scmd))
                {
                    scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                    if ("Insert".equals(scmd))
                        // insert "-1.23" at the i-beam's position
                        act.insert("-1.23");
                    else if ("Append".equals(scmd))
                        // append "-1.23" to the focused field
                        act.append("-1.23");
                    return;
                }
                if ("\"+1*P#G\"".equals(scmd))
                {
                    scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                    if ("Insert".equals(scmd))
                        // insert "+1*P#G" at the i-beam's position
                        act.insert("+1*P#G");
                    else if ("Append".equals(scmd))
                        // append "+1*P#G" to the focused field
                        act.append("+1*P#G");
                    return;
                }
                if ("\"test\"".equals(scmd))
                {
                    scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                    if ("Insert".equals(scmd))
                        // insert "test" at the i-beam's position
                        act.insert("test");
                    else if ("Append".equals(scmd))
                        // append "test" to the focused field
                        act.append("test");
                    return;
                }
                if ("Set Position (3)".equals(scmd))
                {
                    // set i-beam's position after the 3rd character
                    act.setSelection(3);
                    return;
                }
            }
            catch (Exception ex)
            {
                // display the exception
                MessageBox msg = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                msg.setText("Exception");
                msg.setMessage(ex.getMessage());
                msg.open();
                return;
            }
            if (event.widget == this.mnuChange)
            {
                // change editable / non-editable state of the focused field
                act.setEditable(!act.getEditable());
                return;
            }
        }
        super.widgetSelected(event);
    }

    /**
     * focuslistener implementation; focus gained
     */
    public void focusGained(FocusEvent event)
    {
        if (event.widget == this.nonCtext)
            // disable changing the style of the non-editable test input field
            this.mnuChange.setEnabled(false);
    }

    /**
     * focuslistener implementation; focus lost
     */
    public void focusLost(FocusEvent event)
    {
        if (event.widget == this.nonCtext)
            // enable changing the style of other input fields
            this.mnuChange.setEnabled(true);
    }
}
