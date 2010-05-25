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
package com.nokia.test.java.eswt.combo;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;


import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class ComboTestMIDlet_01 extends TestBaseMIDlet
{

    private String[] comboitems = new String[] {"Item 1",
            "Item 2", "Item 3", "Item 4", "Item 5",
            "Item 6", "Item 7", "Item 8", "Item 9", "Item 10"
                                               };

    private String[] longcomboitems = new String[] {"A Very very Long Combo Item 1",
            "A Very very Long Combo Item 2", "A Very very Long Combo Item 3",
            "A Very very Long Combo Item 4", "A Very very Long Combo Item 5",
            "A Very very Long Combo Item 6", "A Very very Long Combo Item 7",
            "A Very very Long Combo Item 8", "A Very very Long Combo Item 9",
            "A Very very Long Combo Item 10"
                                                   };

    private Combo combo = null;
    private Combo longCombo = null;
    private Combo verylongCombo = null;

    private Shell testshell = null;

    private int columns;

    private Command cmdAddFirst             = null;
    private Command cmdAddLast              = null;

    private Command cmdRemove               = null;
    private Command cmdRemoveAll            = null;
    private Command cmdRemoveAdded          = null;
    private Command cmdRemoveAllButFirst    = null;

    private Command cmdClear                = null;

    private Command cmdSetSelected          = null;
    private Command cmdSetDefaults          = null;

    private Command cmdSetDialog            = null;
    private Command cmdSetOnTop             = null;
    private Command cmdSetNoTrim            = null;
    private Command cmdClose                = null;
    private Command cmdSetLayout            = null;
    private Command cmdSetApplicationModal  = null;
    protected void init()
    {
        // customize the shell
        this.shell.setText("Combo Test");
        this.shell.setLayout(new GridLayout());
        columns = 1;

        // create a combo with 10 items
        combo = new Combo(this.shell, SWT.DROP_DOWN | SWT.BORDER);
        combo.setText("Simple Combo Control");
        combo.setItems(comboitems);
        this.combo.select(this.comboitems.length - 1);
        this.combo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        // create a combo with 10 long items
        longCombo = new Combo(this.shell, SWT.DROP_DOWN);
        longCombo.setText("Long Combo Control");
        longCombo.setItems(longcomboitems);
        this.longCombo.select(0);
        this.longCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        // create a combo with 100 items
        verylongCombo = new Combo(this.shell, SWT.DROP_DOWN);
        verylongCombo.setText("Long Combo Control");
        for (int i=0; i<100; i++)
        {
            verylongCombo.add("Combo Item "+i);
        }
        this.verylongCombo.select(0);
        this.verylongCombo.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        this.menuSetup();
        this.shell.pack();
        this.shell.layout();
        this.shell.update();
    }
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

    private void menuSetup()
    {
        // create menu commands

        // Add submenu
        Command add = new Command(this.shell, Command.COMMANDGROUP, 4);
        add.setText("Add");

        this.cmdAddFirst = new Command(add, Command.GENERAL, 9);
        this.cmdAddFirst.setText("First");
        this.cmdAddFirst.addSelectionListener(this);

        this.cmdAddLast = new Command(add, Command.GENERAL, 8);
        this.cmdAddLast.setText("Last");
        this.cmdAddLast.addSelectionListener(this);

        // Remove submenu
        Command removeSubmenu = new Command(this.shell, Command.COMMANDGROUP, 3);
        removeSubmenu.setText("Remove");

        this.cmdRemove = new Command(removeSubmenu, Command.GENERAL, 0);
        this.cmdRemove.setText("Selected");
        this.cmdRemove.addSelectionListener(this);

        this.cmdRemoveAll =
            new Command(removeSubmenu, Command.GENERAL, 1);
        this.cmdRemoveAll.setText("All");
        this.cmdRemoveAll.addSelectionListener(this);

        this.cmdRemoveAdded = new Command(removeSubmenu, Command.GENERAL, 0);
        this.cmdRemoveAdded.setText("\"Last Item\"");
        this.cmdRemoveAdded.addSelectionListener(this);

        this.cmdRemoveAllButFirst =
            new Command(removeSubmenu, Command.GENERAL, 1);
        this.cmdRemoveAllButFirst.setText("All but first");
        this.cmdRemoveAllButFirst.addSelectionListener(this);

        this.cmdClear =
            new Command(this.shell, Command.GENERAL, 0);
        this.cmdClear.setText("Clear Selection");
        this.cmdClear.addSelectionListener(this);

        // Setting submenu
        Command set = new Command(this.shell, Command.COMMANDGROUP, 2);
        set.setText("Set");

        this.cmdSetSelected =
            new Command(set, Command.GENERAL, 0);
        this.cmdSetSelected.setText("Selected");
        this.cmdSetSelected.addSelectionListener(this);

        this.cmdSetDefaults =
            new Command(set, Command.GENERAL, 0);
        this.cmdSetDefaults.setText("Defaults");
        this.cmdSetDefaults.addSelectionListener(this);

        this.cmdSetLayout =
            new Command(this.shell, Command.GENERAL, 0);
        this.cmdSetLayout.setText("Change column count");
        this.cmdSetLayout.addSelectionListener(this);

        // location submenu
        Command parentmenu = new Command(this.shell, Command.COMMANDGROUP, 2);
        parentmenu.setText("Parent");

        this.cmdSetDialog =
            new Command(parentmenu, Command.GENERAL, 0);
        this.cmdSetDialog.setText("Dialog Shell");
        this.cmdSetDialog.addSelectionListener(this);

        this.cmdSetOnTop =
            new Command(parentmenu, Command.GENERAL, 0);
        this.cmdSetOnTop.setText("On Top Shell");
        this.cmdSetOnTop.addSelectionListener(this);

        this.cmdSetNoTrim =
            new Command(parentmenu, Command.GENERAL, 0);
        this.cmdSetNoTrim.setText("No Trim Shell");
        this.cmdSetNoTrim.addSelectionListener(this);

        this.cmdSetApplicationModal =
            new Command(parentmenu, Command.GENERAL, 0);
        this.cmdSetApplicationModal.setText("Application modal Shell");
        this.cmdSetApplicationModal.addSelectionListener(this);
    }

    /**
     * Return currently focused button.
     * @return focused button
     */
    private Combo getFocusedCombo()
    {
        Combo result = null;

        Control[] combos = this.shell.getChildren();
        for (int i=0; i<combos.length; i++)
        {
            if (combos[i].isFocusControl())
            {
                result = (Combo)combos[i];
                break;
            }
        }

        return result;
    }
    /*
     *  (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (!(event.widget instanceof Command)) return;

        boolean changed = false;
        try
        {

            Combo combo = this.getFocusedCombo();

            // Add commands
            if (event.widget == this.cmdAddFirst)
            {
                combo.add("First Item",0);
                changed = true;
            }
            if (event.widget == this.cmdAddLast)
            {
                combo.add("Last Item",0);
                changed = true;
            }
            // Remove commands
            if (event.widget == this.cmdRemove)
            {

                if (combo.getSelectionIndex() != -1)
                {
                    combo.remove(combo.getSelectionIndex());
                    changed = true;
                }
            }
            if (event.widget == this.cmdRemoveAll)
            {
                combo.removeAll();
                changed = true;
            }
            if (event.widget == this.cmdRemoveAdded)
            {

                try
                {
                    combo.remove("Last Item");
                    changed = true;
                }
                catch (Exception ex)
                {
                    // show a messagebox about the exception
                    this.showException(ex.toString()+": Could not found item");
                }
            }
            if (event.widget == this.cmdRemoveAllButFirst)
            {
                if (combo.getVisibleItemCount() > 1)
                {
                    combo.remove(1,combo.getVisibleItemCount()-1);
                    changed = true;
                }
            }
            if (event.widget == this.cmdClear)
            {
                combo.clearSelection();
                changed = true;
            }
            if (event.widget == this.cmdSetSelected)
            {
                if (combo.getSelectionIndex() > -1)
                {
                    combo.setItem(combo.getSelectionIndex(), "This item is set");
                    changed = true;
                }
            }
            if (event.widget == this.cmdSetDefaults)
            {
                combo.setItems(this.comboitems);
                changed = true;
            }
            if (event.widget == this.cmdSetDialog)
            {
                // create a combo with 10 long items
                this.testshell = new Shell(this.shell,SWT.DIALOG_TRIM);
                this.testshell.setText("Combo in DialogShell");
                this.testshell.setLayout(new FillLayout());
                this.testshell.open();
                Combo dialogCombo = new Combo(this.testshell, SWT.DROP_DOWN);
                dialogCombo.setText("Dialog Combo Control");
                dialogCombo.setItems(longcomboitems);
                this.cmdClose = new Command(this.testshell,Command.EXIT, 0);
                this.cmdClose.setText("Close");
                this.cmdClose.addSelectionListener(this);

            }
            if (event.widget == this.cmdSetOnTop)
            {
                // create a combo with 10 long items
                this.testshell = new Shell(this.shell,SWT.ON_TOP);
                this.testshell.setLayout(new FillLayout());
                this.testshell.open();
                Combo dialogCombo = new Combo(this.testshell, SWT.DROP_DOWN);
                dialogCombo.setText("OnTop Combo Control");
                dialogCombo.setItems(longcomboitems);
                this.cmdClose = new Command(this.testshell,Command.EXIT, 0);
                this.cmdClose.setText("Close");
                this.cmdClose.addSelectionListener(this);
            }
            if (event.widget == this.cmdSetNoTrim)
            {
                // create a combo with 10 long items
                this.testshell = new Shell(this.shell,SWT.NO_TRIM);
                this.testshell.setLayout(new FillLayout());

                Combo dialogCombo = new Combo(this.testshell, SWT.DROP_DOWN);
                dialogCombo.setText("NoTrim Combo Control");
                dialogCombo.setItems(longcomboitems);
                this.cmdClose = new Command(this.testshell,Command.EXIT, 0);
                this.cmdClose.setText("Close");
                this.cmdClose.addSelectionListener(this);
                this.testshell.open();
            }
            if (event.widget == this.cmdSetApplicationModal)
            {
                // create a combo with 10 long items
                this.testshell = new Shell(this.shell,SWT.APPLICATION_MODAL);
                this.testshell.setLayout(new FillLayout());

                Combo dialogCombo = new Combo(this.testshell, SWT.DROP_DOWN);
                dialogCombo.setText("Long Combo Control");
                dialogCombo.setItems(longcomboitems);
                this.cmdClose = new Command(this.testshell,Command.EXIT, 0);
                this.cmdClose.setText("Close");
                this.cmdClose.addSelectionListener(this);
                this.testshell.open();
            }
            if (event.widget == this.cmdClose)
            {

                try
                {
                    if (this.testshell != null && !this.testshell.isDisposed())
                        this.testshell.dispose();
                }
                catch (Exception ex)
                {
                    System.out.println("closing testshell"+ex.getMessage());
                }

                changed = true;
            }
            if (event.widget == this.cmdSetLayout)
            {
                GridLayout gridLayout = new GridLayout();
                if (this.columns == 1)
                {
                    gridLayout.numColumns = 2;
                    this.columns = 2;
                }
                else
                {
                    gridLayout.numColumns = 1;
                }
                this.shell.setLayout(gridLayout);
                changed = true;
            }
        }
        catch (Exception ex)
        {
            this.showException(ex.getMessage());
        }
        // update the shell if needed
        if (changed)
        {
            this.shell.pack();
            this.shell.layout();
            this.shell.update();
            return;
        }

        super.widgetSelected(event);
    }

}
