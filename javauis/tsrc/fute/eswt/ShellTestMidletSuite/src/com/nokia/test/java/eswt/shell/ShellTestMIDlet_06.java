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

package com.nokia.test.java.eswt.shell;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the hell component
 * - shell containing composites
 */
public class ShellTestMIDlet_06 extends TestBaseMIDlet
{
    /** composite instances */
    private Composite comp1 = null;
    private Composite comp2 = null;
    private Composite comp3 = null;

    private Button txtButton = null;
    private Button txtButton2 = null;
    private Button txtButton3 = null;

    private Button numButton = null;
    private Button numButton2 = null;
    private Button numButton3 = null;


    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Shell Test 6");
        this.shell.setLayout(new FillLayout(SWT.VERTICAL));

        // "composite 1" left softkey command
        Command cmd = new Command(this.shell, Command.SELECT, 0);
        cmd.setText("Composite 1");
        cmd.addSelectionListener(this);
        // "composite 2" left softkey command
        cmd = new Command(this.shell, Command.SELECT, 1);
        cmd.setText("Composite 2");
        cmd.addSelectionListener(this);
        // "composite 3" left softkey command
        cmd = new Command(this.shell, Command.SELECT, 2);
        cmd.setText("Composite 3");
        cmd.addSelectionListener(this);

        // setup composite 1
        this.comp1 = new Composite(this.shell, SWT.BORDER | SWT.V_SCROLL);
        this.comp1.setLayout(new FillLayout());
        this.comp1.setVisible(false);
        this.txtButton = new Button(this.comp1, SWT.BORDER|SWT.PUSH);
        this.txtButton.setText("Comp1");
        this.numButton = new Button(this.comp1, SWT.BORDER|SWT.PUSH);
        numButton.setText("000,000");

        // "1st composite" left softkey command
        cmd = new Command(this.comp1, Command.SELECT, 0);
        cmd.setText("1st Composite");

        // setup composite 2
        this.comp2 = new Composite(this.shell, SWT.BORDER |SWT.H_SCROLL);
        this.comp2.setLayout(new RowLayout());
        this.comp2.setVisible(false);
        this.txtButton2 = new Button(this.comp2, SWT.BORDER|SWT.PUSH);
        this.txtButton2.setText("Comp2");
        this.numButton2 = new Button(this.comp2, SWT.BORDER|SWT.PUSH);
        numButton.setText("000,000");
        // "2nd composite" left softkey command
        cmd = new Command(this.comp2, Command.SELECT, 0);
        cmd.setText("2nd Composite");

        // setup composite 3
        this.comp3 = new Composite(this.shell, SWT.H_SCROLL | SWT.V_SCROLL);
        this.comp3.setLayout(new GridLayout(1,true));
        this.comp3.setVisible(false);
        this.txtButton3 = new Button(this.comp3, SWT.BORDER|SWT.PUSH);
        this.txtButton3.setText("Comp3");
        this.numButton3 = new Button(this.comp3, SWT.BORDER|SWT.PUSH);
        numButton.setText("000,000");
        // "3rd composite" left softkey command
        cmd = new Command(this.comp3, Command.SELECT, 0);
        cmd.setText("3rd Composite");
        this.shell.layout();
        numButton.setText(txtButton.getSize().x+","+txtButton.getSize().y);
        numButton2.setText(txtButton2.getSize().x+","+txtButton2.getSize().y);
        numButton3.setText(txtButton3.getSize().x+","+txtButton3.getSize().y);
    }

    private void chgComp(String title, boolean c1, boolean c2, boolean c3)
    {
        this.shell.setText(title);
        if (c1)
        {
            this.comp1.layout();
            this.comp1.setVisible(c1);
            this.comp2.setVisible(c2);
            this.comp3.setVisible(c3);
        }
        else if (c2)
        {

            this.comp2.layout();
            this.comp1.setVisible(c1);
            this.comp2.setVisible(c2);
            this.comp3.setVisible(c3);
        }
        else if (c3)
        {

            this.comp3.layout();
            this.comp1.setVisible(c1);
            this.comp2.setVisible(c2);
            this.comp3.setVisible(c3);
        }
    }
    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Command)
        {
            String scmd = ((Command) event.widget).getText();
            if ("Composite 1".equals(scmd))
            {
                // show the 1st composite
                this.chgComp("Composite 1", true, false, false);
                return;
            }
            if ("Composite 2".equals(scmd))
            {
                // show the 2nd composite
                this.chgComp("Composite 2", false, true, false);
                return;
            }
            if ("Composite 3".equals(scmd))
            {
                // show the 3rd composite
                this.chgComp("Composite 3", false, false, true);
                return;
            }
            if (event.widget == this.cmdExit)
            {
                // dispose the composites before exit
                if (this.comp1 != null && !this.comp1.isDisposed())
                    this.comp1.dispose();
                if (this.comp2 != null && !this.comp2.isDisposed())
                    this.comp2.dispose();
                if (this.comp3 != null && !this.comp3.isDisposed())
                    this.comp3.dispose();
            }
        }
        super.widgetSelected(event);
    }
}

