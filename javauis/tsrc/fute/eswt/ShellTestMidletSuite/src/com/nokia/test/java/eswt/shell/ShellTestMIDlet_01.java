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

import org.eclipse.swt.SWT;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;

import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.GridData;

import org.eclipse.swt.events.FocusListener;
import org.eclipse.swt.events.FocusEvent;
import org.eclipse.swt.events.SelectionEvent;

import org.eclipse.swt.graphics.Image;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 *
 */
public class ShellTestMIDlet_01 extends TestBaseMIDlet
        implements FocusListener
{

    /** */
    private Shell tooltip = null;
    /** */
    private Text field    = null;

    private Image icon = null;

    /* (non-Javadoc)
     * @see TestCaseBaseMIDlet#init()
     */
    protected void init()
    {
        // set a title
        this.shell.setText("Modeless Shell test");

        // create and setup layout
        GridLayout layout = new GridLayout(2, false);
        layout.horizontalSpacing = 10;
        this.shell.setLayout(layout);

        // create text fields
        Label label1 = new Label(this.shell, SWT.NONE);
        label1.setText("First field: ");
        GridData l1data = new GridData();
        l1data.horizontalAlignment = SWT.RIGHT;
        label1.setLayoutData(l1data);

        Text first_field = new Text(this.shell, SWT.BORDER);
        first_field.setText("First");
        first_field.addFocusListener(this);
        GridData f1data = new GridData();
        f1data.horizontalAlignment = SWT.FILL;
        first_field.setLayoutData(f1data);

        Label label2 = new Label(this.shell, SWT.NONE);
        label2.setText("Second field: ");
        GridData l2data = new GridData();
        l2data.horizontalAlignment = SWT.RIGHT;
        label2.setLayoutData(l2data);

        this.field = new Text(this.shell, SWT.BORDER);
        this.field.setText("Second");
        this.field.addFocusListener(this);
        GridData f2data = new GridData();
        f2data.horizontalAlignment = SWT.FILL;
        this.field.setLayoutData(f2data);

        createToolTip("Second field is focused");
    }

    /**
     *
     */
    private void createToolTip(String msg)
    {

        // create a modeless shell with border
        this.tooltip = new Shell(this.shell, SWT.MODELESS | SWT.NO_FOCUS | SWT.BORDER);
        this.tooltip.setLayout(new RowLayout(SWT.HORIZONTAL));

        // create an image
        this.icon =
            new Image(this.display,
                      this.getClass().getResourceAsStream("/res/icon.png"));

        Label image = new Label(this.tooltip, SWT.NONE);
        image.setImage(icon);

        // create a label to be used as a tooltip text
        Label label = new Label(this.tooltip, SWT.NONE);
        label.setText(msg);

        // set the location so the tooltip will be shown in
        // middle of the screen
        this.tooltip.setLocation(10, Display.getCurrent().getBounds().height/2);

        // finilize
        this.tooltip.pack();
        this.tooltip.layout();
        this.tooltip.setVisible(false);
    }

    /* (non-Javadoc)
     * @see org.eclipse.swt.events.FocusListener#focusGained(org.eclipse.swt.events.FocusEvent)
     */
    public void focusGained(FocusEvent event)
    {
        if (event.widget == this.field)
        {
            if (this.tooltip != null)
                this.tooltip.setVisible(true);
        }
    }


    /* (non-Javadoc)
     * @see org.eclipse.swt.events.FocusListener#focusLost(org.eclipse.swt.events.FocusEvent)
     */
    public void focusLost(FocusEvent event)
    {
        if (event.widget == this.field)
        {
            if (this.tooltip != null)
            {
                this.tooltip.setVisible(false);
            }
        }
    }
    /* (non-Javadoc)
     * @see org.eclipse.swt.events.SelectionListener#widgetSelected(org.eclipse.swt.events.SelectionEvent)
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget.getData("text").equals("Exit"))
        {
            icon.dispose();
        }
        super.widgetSelected(event);
    }
}
