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

import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the label component
 * - changing the aligment of label
 * - changing title & image
 */
public class LabelTestMIDlet_01 extends TestBaseMIDlet
{
    /** text label */
    private Label txtLabel = null;
    /** horizontal separator label */
    private Label hsepLabel = null;
    /** vertical separator label */
    private Label vsepLabel = null;
    /** image label */
    private Label imgLabel = null;
    /** small image */
    private Image smallimg = null;
    /** big image */
    private Image bigimg = null;

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
        this.shell.setText("Label 1");
        this.shell.setLayout(new GridLayout());

        // try to initialize the images
        try
        {
            this.smallimg = new Image(this.display, this.getClass().getResourceAsStream("/res/SmallLabel.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Small image could not be loaded");
            this.smallimg = null;
        }
        try
        {
            this.bigimg = new Image(this.display, this.getClass().getResourceAsStream("/res/BigLabel.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Big image could not be loaded");
            this.bigimg = null;
        }

        // text label
        this.txtLabel = new Label(this.shell, SWT.BORDER);
        this.txtLabel.setText("label");
        this.txtLabel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        // horizontal separator label
        try
        {
            this.hsepLabel = new Label(this.shell, SWT.SEPARATOR | SWT.HORIZONTAL);
            // vertical separator label
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException(ex.getMessage());
            this.hsepLabel = null;
        }
        try
        {
            this.vsepLabel = new Label(this.shell, SWT.SEPARATOR | SWT.VERTICAL);
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException(ex.getMessage());
            this.vsepLabel = null;
        }
        // image label
        this.imgLabel = new Label(this.shell, SWT.BORDER);
        this.imgLabel.setImage(this.smallimg);
        this.imgLabel.setLayoutData(new GridData(SWT.FILL | SWT.FILL, SWT.FILL, true, true));

        // set up the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "alignment" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Alignment");
        item.setMenu(menu);
        // "left" menuitem for "alignment" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Left");
        item.addSelectionListener(this);
        // "center" menuitem for "alignment" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Center");
        item.addSelectionListener(this);
        // "right" menuitem for "alignment" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Right");
        item.addSelectionListener(this);
        // "image" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Image");
        item.setMenu(menu);
        // "null" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Null");
        item.addSelectionListener(this);
        // "small" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Small");
        item.addSelectionListener(this);
        // "big" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Big");
        item.addSelectionListener(this);
        // "invalid" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Invalid");
        item.addSelectionListener(this);
        // "change text" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Change Text");
        item.addSelectionListener(this);
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem) event.widget).getText();
            if ("Left".equals(scmd))
            {
                // set left alignment for all labels
                this.txtLabel.setAlignment(SWT.LEFT);
                this.imgLabel.setAlignment(SWT.LEFT);
                return;
            }
            if ("Center".equals(scmd))
            {
                // set center alignment for all labels
                this.txtLabel.setAlignment(SWT.CENTER);
                this.imgLabel.setAlignment(SWT.CENTER);
                return;
            }
            if ("Right".equals(scmd))
            {
                // set right alignment for all labels
                this.txtLabel.setAlignment(SWT.RIGHT);
                this.imgLabel.setAlignment(SWT.RIGHT);
                return;
            }
            if ("Null".equals(scmd))
            {
                // change image label to null
                this.imgLabel.setImage(null);
                return;
            }
            if ("Small".equals(scmd))
            {
                // change image label to small image
                this.imgLabel.setImage(this.smallimg);
                return;
            }
            if ("Big".equals(scmd))
            {
                // change image label to big image
                this.imgLabel.setImage(this.bigimg);
                return;
            }

            if ("Invalid".equals(scmd))
            {
                // try to change the image label to an invalid image
                try
                {
                    this.imgLabel.setImage(new Image(this.display, this.getClass().getResourceAsStream("/res/invalid.png")));
                }
                catch (Exception ex)
                {
                    // display the exception
                    this.showException(ex.getMessage());
                }
                return;
            }

            if ("Change Text".equals(scmd))
            {
                // query dialog for new label text
                QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
                dlg.setPromptText("Enter text:", this.txtLabel.getText());
                dlg.setMinimum(1);
                dlg.setMaximum(50);
                String txt = dlg.open();
                if (txt != null)
                    this.txtLabel.setText(txt);
                return;
            }
        }
        else if (event.widget == this.cmdExit)
        {
            // dispose the images before exit
            if (this.smallimg != null && !this.smallimg.isDisposed())
                this.smallimg.dispose();
            if (this.bigimg != null && !this.bigimg.isDisposed())
                this.bigimg.dispose();
        }
        super.widgetSelected(event);
    }
}
