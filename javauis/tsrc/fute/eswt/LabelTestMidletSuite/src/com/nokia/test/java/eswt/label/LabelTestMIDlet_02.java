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

package com.nokia.test.java.eswt.label;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the label component
 * - creating separator label
 */
public class LabelTestMIDlet_02 extends TestBaseMIDlet
{
    /** label instance */
    private Label seplabel = null;
    /** menuitems... */
    private MenuItem mnuHorizontal = null;
    private MenuItem mnuVertical = null;
    private MenuItem mnuSIn = null;
    private MenuItem mnuSOut = null;
    private MenuItem mnuSNone = null;

    private MenuItem VFillLayout = null;
    private MenuItem HFillLayout = null;
    private MenuItem smallLabels = null;

    /** separator type */
    private int type;
    /** separator style */
    private int style;

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

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("Label 2");
        this.shell.setLayout(new FillLayout());
        // default values
        this.type = SWT.HORIZONTAL;
        this.style = SWT.SHADOW_IN;

        // set up the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "type" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Type");
        item.setMenu(menu);
        // "horizontal" menuitem for "type" submenu
        this.mnuHorizontal = new MenuItem(menu, SWT.PUSH);
        this.mnuHorizontal.setText("* Horizontal");
        this.mnuHorizontal.addSelectionListener(this);
        // "vertical" menuitem for "type" submenu
        this.mnuVertical = new MenuItem(menu, SWT.PUSH);
        this.mnuVertical.setText("Vertical");
        this.mnuVertical.addSelectionListener(this);
        // "style submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Style");
        item.setMenu(menu);
        // "shadow_in" menuitem for "style" submenu
        this.mnuSIn = new MenuItem(menu, SWT.PUSH);
        this.mnuSIn.setText("* SHADOW_IN");
        this.mnuSIn.addSelectionListener(this);
        // "shadow_out" menuitem for "style" submenu
        this.mnuSOut = new MenuItem(menu, SWT.PUSH);
        this.mnuSOut.setText("SHADOW_OUT");
        this.mnuSOut.addSelectionListener(this);
        // "shadow_none" menuitem for "style" submenu
        this.mnuSNone = new MenuItem(menu, SWT.PUSH);
        this.mnuSNone.setText("SHADOW_NONE");
        this.mnuSNone.addSelectionListener(this);
        // "create" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Create");
        item.addSelectionListener(this);

        this.VFillLayout = new MenuItem(mainmenu, SWT.PUSH);
        this.VFillLayout.setText("Vertical Fill");
        this.VFillLayout.addSelectionListener(this);

        this.HFillLayout = new MenuItem(mainmenu, SWT.PUSH);
        this.HFillLayout.setText("Horizontal Fill");
        this.HFillLayout.addSelectionListener(this);

        this.smallLabels = new MenuItem(mainmenu, SWT.PUSH);
        this.smallLabels.setText("Small Separators");
        this.smallLabels.addSelectionListener(this);
    }

    /**
     * update the mark in "type" submenu
     * @param newtype the selected type
     * @param item the selected menuitem
     */
    private void chgType(int newType, MenuItem item)
    {
        if (this.type != newType)
        {
            if (this.type == SWT.HORIZONTAL)
                this.mnuHorizontal.setText("Horizontal");
            else
                this.mnuVertical.setText("Vertical");
            this.type = newType;
            item.setText("* "+item.getText());
        }
    }

    /**
     * update the mark in "style" submenu
     * @param newstyle the selected style
     * @param item the selected menuitem
     */
    private void chgStyle(int newStyle, MenuItem item)
    {
        if (this.style != newStyle)
        {
            switch (this.style)
            {
            case SWT.SHADOW_IN:
                this.mnuSIn.setText("SHADOW_IN");
                break;
            case SWT.SHADOW_OUT:
                this.mnuSOut.setText("SHADOW_OUT");
                break;
            case SWT.SHADOW_NONE:
                this.mnuSNone.setText("SHADOW_NONE");
                break;
            default:
                break;
            }
            this.style = newStyle;
            item.setText("* "+item.getText());
        }
    }



    public void LabelTestSeparatorWithVFillLayout()
    {

        shell.setLayout(new FillLayout(SWT.VERTICAL));

        Label label1 =  new Label(shell, SWT.SEPARATOR|SWT.HORIZONTAL);
        Label label2 =  new Label(shell, SWT.SEPARATOR|SWT.HORIZONTAL|SWT.RIGHT);
        Label label3 =  new Label(shell, SWT.SEPARATOR|SWT.HORIZONTAL|SWT.CENTER);

        Label label4 =  new Label(shell, SWT.SEPARATOR|SWT.HORIZONTAL|SWT.RIGHT_TO_LEFT);
        Label label5 =  new Label(shell, SWT.SEPARATOR|SWT.HORIZONTAL|SWT.RIGHT|SWT.RIGHT_TO_LEFT);
        Label label6 =  new Label(shell, SWT.SEPARATOR|SWT.HORIZONTAL|SWT.CENTER|SWT.RIGHT_TO_LEFT);
        shell.layout();

    }

    public void LabelTestSeparatorWithHFillLayout()
    {

        shell.setLayout(new FillLayout(SWT.HORIZONTAL));

        Label label1 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL);
        Label label2 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL|SWT.DOWN);
        Label label3 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL|SWT.CENTER);

        Label label4 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL|SWT.RIGHT_TO_LEFT);
        Label label5 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL|SWT.DOWN|SWT.RIGHT_TO_LEFT);
        Label label6 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL|SWT.CENTER|SWT.RIGHT_TO_LEFT);
        shell.layout();

    }

    public void LabelTestSmallSizeSeparators()
    {

        Label label1 =  new Label(shell, SWT.SEPARATOR|SWT.HORIZONTAL);
        label1.setBounds(10,10,16,8);

        Label label2 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.HORIZONTAL);
        label2.setBounds(10,30, 5, 7);

        Label label3 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.HORIZONTAL);
        label3.setBounds(10,50, 1, 8);

        Label label4 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL);
        label4.setBounds(10,100, 8, 16);

        Label label5 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL);
        label5.setBounds(40,100, 7, 2);

        Label label6 =  new Label(shell, SWT.BORDER|SWT.SEPARATOR|SWT.VERTICAL);
        label6.setBounds(70,100, 5, 3);

    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget == this.mnuHorizontal)
        {
            // set the type
            this.chgType(SWT.HORIZONTAL, this.mnuHorizontal);
            return;
        }
        if (event.widget == this.mnuVertical)
        {
            // set the type
            this.chgType(SWT.VERTICAL, this.mnuVertical);
            return;
        }
        if (event.widget == this.mnuSIn)
        {
            // set the style
            this.chgStyle(SWT.SHADOW_IN, this.mnuSIn);
            return;
        }
        if (event.widget == this.mnuSOut)
        {
            // set the style
            this.chgStyle(SWT.SHADOW_OUT, this.mnuSOut);
            return;
        }
        if (event.widget == this.mnuSNone)
        {
            // set the style
            this.chgStyle(SWT.SHADOW_NONE, this.mnuSNone);
            return;
        }
        if (event.widget == this.VFillLayout)
        {
            // set the labels
            this.LabelTestSeparatorWithVFillLayout();
            return;
        }
        if (event.widget == this.HFillLayout)
        {
            // set the labels
            this.LabelTestSeparatorWithHFillLayout();
            return;
        }
        if (event.widget == this.smallLabels)
        {
            // set the labels
            this.LabelTestSmallSizeSeparators();
            return;
        }
        if (event.widget instanceof MenuItem && "Create".equals(((MenuItem) event.widget).getText()))
        {
            try
            {
                // create a label with the selected type & style
                if (this.seplabel != null && !this.seplabel.isDisposed())
                    this.seplabel.dispose();
            }
            catch (Exception ex)
            {
                this.showException(ex.getMessage());
            }
            try
            {
                this.seplabel = new Label(this.shell, SWT.SEPARATOR | this.type | this.style);
            }
            catch (Exception ex)
            {
                this.showException(ex.getMessage());

            }

            this.shell.layout();
            return;
        }
        super.widgetSelected(event);
    }
}
