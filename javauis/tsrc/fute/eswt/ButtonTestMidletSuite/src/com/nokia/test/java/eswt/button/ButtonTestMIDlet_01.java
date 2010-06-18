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
package com.nokia.test.java.eswt.button;

import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the label component
 * - changing the aligment of label
 * - changing title & image
 */
public class ButtonTestMIDlet_01 extends TestBaseMIDlet
{
    /**  push text button */
    private Button txtButton = null;
    /**  check button */
    private Button checkButton = null;
    /** radio button */
    private Button radioButton = null;
    /**  toggle button */
    private Button toggleButton = null;

    /**  counter button */
    private Button counterButton = null;

    /** push image button */
    private Button imgButton = null;
    /** check image button */
    private Button imgCheckButton = null;
    /** radio image button */
    private Button imgRadioButton = null;
    /** toggle image button */
    private Button imgToggleButton = null;

    /** small image */
    private Image smallimg = null;
    /** big image */
    private Image bigimg = null;
    /** trans image */
    private Image transimg = null;

    private Shell tooltip = null;

    private Button tooltipButton = null;
    private Button tooltipCloseButton = null;

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
        this.shell.setText("Button test");
        this.shell.setLayout(new GridLayout());

        // try to initialize the images
        try
        {
            this.smallimg = new Image(this.display, this.getClass().getResourceAsStream("/res/sphere.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Small image could not be loaded");
            this.smallimg = null;
        }
        try
        {
            this.bigimg = new Image(this.display, this.getClass().getResourceAsStream("/res/SmallLabel.jpg"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Big image could not be loaded");
            this.bigimg = null;
        }
        try
        {
            this.transimg = new Image(this.display, this.getClass().getResourceAsStream("/res/alphatest_small.png"));
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Transparent image could not be loaded");
            this.transimg = null;
        }

        // text buttons

        // image buttons

        // set up the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "add" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Add");
        item.setMenu(menu);
        // "Push" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Push");
        item.addSelectionListener(this);
        // "Check" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Check");
        item.addSelectionListener(this);
        // "Radio" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Radio");
        item.addSelectionListener(this);
        // "Radio" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Radio Group");
        item.addSelectionListener(this);
        // "Toggle" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Toggle");
        item.addSelectionListener(this);
        // "Push" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Push-image");
        item.addSelectionListener(this);
        // "Check" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Check-image");
        item.addSelectionListener(this);
        // "Radio" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Radio-image");
        item.addSelectionListener(this);
        // "Toggle" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Toggle-image");
        item.addSelectionListener(this);
        // "Toggle" menuitem for "add" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Counter");
        item.addSelectionListener(this);
        // "flat" submenu
        Menu menu2 = new Menu(menu);
        item = new MenuItem(menu, SWT.CASCADE);
        item.setText("Flat");
        item.setMenu(menu2);
        // "Push" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Push");
        item.addSelectionListener(this);
        // "Check" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Check");
        item.addSelectionListener(this);
        // "Radio" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Radio");
        item.addSelectionListener(this);
        // "Toggle" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Toggle");
        item.addSelectionListener(this);
        // "Push" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Push-image");
        item.addSelectionListener(this);
        // "Check" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Check-image");
        item.addSelectionListener(this);
        // "Radio" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Radio-image");
        item.addSelectionListener(this);
        // "Toggle" menuitem for "Flat" submenu
        item = new MenuItem(menu2, SWT.PUSH);
        item.setText("Flat Toggle-image");
        item.addSelectionListener(this);
        // "alignment" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
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
        // "trans" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Trans");
        item.addSelectionListener(this);
        // "invalid" menuitem for "image" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Invalid");
        item.addSelectionListener(this);
        // "change text" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Change Text");
        item.addSelectionListener(this);
        // "default button" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Set as Default Button");
        item.addSelectionListener(this);
        // "other shell" menuitem
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("New Shell");
        item.addSelectionListener(this);
        // "Selection" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Set Selection");
        item.setMenu(menu);
        // "selected" menuitem for "Selection" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Unselect All");
        item.addSelectionListener(this);
        // "all" menuitem for "Selection" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Select All");
        item.addSelectionListener(this);
        // "Clear" submenu
        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Clear All");
        item.addSelectionListener(this);

    }

    /**
     * Return currently focused button.
     * @return focused button
     */
    private Button getFocusedButton()
    {
        Button result = null;

        Control[] buttons = this.shell.getChildren();
        for (int i=0; i<buttons.length; i++)
        {
            if (buttons[i].isFocusControl())
            {
                result = (Button)buttons[i];
                break;
            }
        }

        return result;
    }
    /**
     * Return currently focused button.
     * @return focused button
     */
    private void changeAlignment(int align)
    {

        Control[] buttons = this.shell.getChildren();
        for (int i=0; i<buttons.length; i++)
        {
            ((Button)buttons[i]).setAlignment(align);
        }
    }
    private void changeSelectionForAll(boolean select)
    {

        Control[] buttons = this.shell.getChildren();
        for (int i=0; i<buttons.length; i++)
        {
            ((Button)buttons[i]).setSelection(select);
        }
    }
    private void clearShell()
    {
        Control[] buttons = this.shell.getChildren();
        for (int i=0; i<buttons.length; i++)
        {
            buttons[i].dispose();
        }
    }
    private void createToolTip(String msg,Button button)
    {

        // create a modeless shell with border
        int style = SWT.NONE;

        if (msg.equals("Push"))
            style = SWT.APPLICATION_MODAL | SWT.BORDER;
        else if (msg.equals("Flat Push"))
            style = SWT.APPLICATION_MODAL | SWT.NO_FOCUS | SWT.DIALOG_TRIM;
        else if (msg.equals("Toggle"))
            style = SWT.MODELESS | SWT.DIALOG_TRIM;
        else if (msg.equals("Flat Toggle"))
            style = SWT.SHELL_TRIM;
        else if (msg.equals("Push-image"))
            style = SWT.APPLICATION_MODAL | SWT.DIALOG_TRIM;
        else if (msg.equals("Toggle-image"))
            style = SWT.MODELESS | SWT.BORDER;


        this.tooltip = new Shell(this.shell, style);
        this.tooltip.setLayout(new RowLayout(SWT.VERTICAL));

        // create a label to be used as a tooltip text
        Label label = new Label(this.tooltip, SWT.NONE);
        label.setText(msg);

        tooltipButton = new Button(this.tooltip,SWT.BORDER|SWT.PUSH);
        tooltipButton.setText("Maximize");
        tooltipButton.addSelectionListener(this);

        tooltip.setDefaultButton(tooltipButton);

        if (msg.equals("Toggle"))
        {
            tooltipCloseButton = new Button(this.tooltip,SWT.BORDER|SWT.PUSH);
            tooltipCloseButton.setText("Close");
            tooltipCloseButton.addSelectionListener(this);
            tooltip.setDefaultButton(tooltipButton);
        }

        // set the location so the tooltip will be shown in
        // middle of the screen
        this.tooltip.setLocation(10, Display.getCurrent().getBounds().height/2);
        Menu mainmenu = new Menu(this.tooltip, SWT.BAR);
        this.tooltip.setMenuBar(mainmenu);

        // "Close" menuitem for shell
        MenuItem item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Close");
        item.addSelectionListener(this);

        // finalize
        this.tooltip.pack();
        this.tooltip.layout();
        this.tooltip.open();

    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof Button)
        {
            String scmd = ((Button) event.widget).getText();
            if ("Push".equals(scmd) || "Flat Push".equals(scmd))
            {
                this.createToolTip(scmd,(Button)event.widget);
                this.tooltip.setVisible(true);
            }
            else if ("Toggle".equals(scmd) || "Flat Toggle".equals(scmd))
            {
                this.createToolTip(scmd,(Button)event.widget);
                return;
            }
            else if ("Maximize".equals(scmd))
            {
                this.tooltip.setMaximized(true);
                this.tooltipButton.setText("Restore");
                return;
            }
            else if ("Restore".equals(scmd))
            {
                this.tooltip.setMaximized(false);
                this.tooltipButton.setText("Maximize");
                return;
            }
            else if (event.widget.equals(this.counterButton))
            {
                this.counterButton.setText(this.counterButton.getText()+1);
                return;
            }
            else if (event.widget.equals(this.tooltipCloseButton))
            {
                this.tooltip.setVisible(false);
                this.tooltip.dispose();
                return;
            }

            return;
        }
        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem) event.widget).getText();
            if ("Push".equals(scmd) || "Flat Push".equals(scmd) || "Counter".equals(scmd))
            {
                if ("Push".equals(scmd))
                {
                    this.txtButton = new Button(this.shell, SWT.BORDER|SWT.PUSH);
                    this.txtButton.setText("Push");
                }
                else if ("Counter".equals(scmd))
                {
                    this.counterButton = new Button(this.shell, SWT.BORDER|SWT.PUSH);
                    this.counterButton.setText("0");
                    this.counterButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                    this.counterButton.addSelectionListener(this);
                    this.shell.layout();
                    this.shell.update();
                    return;
                }
                else
                {
                    this.txtButton = new Button(this.shell, SWT.BORDER |SWT.FLAT);
                    this.txtButton.setText("Flat Push");
                }
                this.txtButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                this.txtButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            else if ("Check".equals(scmd) || "Flat Check".equals(scmd))
            {
                if ("Check".equals(scmd))
                {
                    this.checkButton = new Button(this.shell, SWT.BORDER|SWT.CHECK);
                    this.checkButton.setText("Check");
                }
                else
                {
                    this.checkButton = new Button(this.shell, SWT.BORDER|SWT.CHECK|SWT.FLAT);
                    this.checkButton.setText("Flat Check");
                }
                this.checkButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                this.checkButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            else if ("Radio".equals(scmd) || "Flat Radio".equals(scmd))
            {
                if ("Radio".equals(scmd))
                {
                    this.radioButton = new Button(this.shell, SWT.BORDER|SWT.RADIO);
                    this.radioButton.setText("Radio");
                }
                else
                {
                    this.radioButton = new Button(this.shell, SWT.BORDER|SWT.RADIO|SWT.FLAT);
                    this.radioButton.setText("Radio");
                }
                this.radioButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                this.radioButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            else if ("Radio Group".equals(scmd))
            {

                Button radioButton1 = new Button(this.shell, SWT.BORDER|SWT.RADIO);
                radioButton1.setText("Item 1");
                radioButton1.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                radioButton1.addSelectionListener(this);
                Button radioButton2 = new Button(this.shell, SWT.BORDER|SWT.RADIO);
                radioButton2.setText("Item 2");
                radioButton2.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                radioButton2.addSelectionListener(this);
                Button radioButton3 = new Button(this.shell, SWT.BORDER|SWT.RADIO);
                radioButton3.setText("Item 3");
                radioButton3.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                radioButton3.addSelectionListener(this);

                this.shell.layout();
                this.shell.update();
                return;

            }
            else if ("Toggle".equals(scmd) || "Flat Toggle".equals(scmd))
            {
                if ("Toggle".equals(scmd))
                {
                    this.toggleButton = new Button(this.shell, SWT.BORDER|SWT.TOGGLE);
                    this.toggleButton.setText("Toggle");
                }
                else
                {
                    this.toggleButton = new Button(this.shell, SWT.BORDER|SWT.TOGGLE |SWT.FLAT);
                    this.toggleButton.setText("Flat Toggle");
                }
                this.toggleButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                this.toggleButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            else if ("Push-image".equals(scmd) || "Flat Push-image".equals(scmd))
            {
                if ("Push-image".equals(scmd))
                {
                    this.imgButton = new Button(this.shell, SWT.BORDER);
                    this.imgButton.setImage(this.smallimg);
                }
                else
                {
                    this.imgButton = new Button(this.shell, SWT.BORDER|SWT.FLAT);
                    this.imgButton.setImage(this.smallimg);
                }
                this.imgButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
                this.imgButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            else if ("Check-image".equals(scmd) || "Flat Check-image".equals(scmd))
            {
                if ("Check-image".equals(scmd))
                {
                    this.imgCheckButton = new Button(this.shell, SWT.CHECK);
                    this.imgCheckButton.setImage(this.smallimg);
                }
                else
                {
                    this.imgCheckButton = new Button(this.shell, SWT.CHECK|SWT.FLAT);
                    this.imgCheckButton.setImage(this.smallimg);
                }
                this.imgCheckButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                this.imgCheckButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            else if ("Radio-image".equals(scmd) || "Flat Radio-image".equals(scmd))
            {
                if ("Radio-image".equals(scmd))
                    this.imgRadioButton = new Button(this.shell, SWT.BORDER|SWT.RADIO);
                else
                    this.imgRadioButton = new Button(this.shell, SWT.BORDER|SWT.RADIO|SWT.FLAT);

                this.imgRadioButton.setImage(this.smallimg);
                this.imgRadioButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                this.imgRadioButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            else if ("Toggle-image".equals(scmd) || "Flat Toggle-image".equals(scmd))
            {
                if ("Toggle-image".equals(scmd))
                    this.imgToggleButton = new Button(this.shell, SWT.TOGGLE);
                else
                    this.imgToggleButton = new Button(this.shell, SWT.TOGGLE|SWT.FLAT);

                this.imgToggleButton.setImage(this.smallimg);
                this.imgToggleButton.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
                this.imgToggleButton.addSelectionListener(this);
                this.shell.layout();
                this.shell.update();
                return;
            }
            if ("Left".equals(scmd))
            {
                // set left alignment for all buttons
                this.changeAlignment(SWT.LEFT);
                return;
            }
            if ("Center".equals(scmd))
            {
                // set center alignment for all buttons
                this.changeAlignment(SWT.CENTER);
                return;
            }
            if ("Right".equals(scmd))
            {
                // set right alignment for all buttons
                this.changeAlignment(SWT.RIGHT);
                return;
            }
            if ("Null".equals(scmd))
            {
                // change image label to null
                this.imgButton.setImage(null);

                this.shell.layout();
                this.shell.redraw();
                this.shell.update();
                return;
            }
            if ("Small".equals(scmd))
            {
                // change image label to small image
                this.imgButton.setImage(this.smallimg);
                this.shell.layout();
                this.shell.redraw();
                this.shell.update();
                return;
            }
            if ("Big".equals(scmd))
            {
                // change image label to big image
                this.imgButton.setImage(this.bigimg);
                this.shell.layout();
                this.shell.redraw();
                this.shell.update();
                return;
            }
            if ("Trans".equals(scmd))
            {
                // change image label to big image
                this.imgButton.setImage(this.transimg);
                this.shell.layout();
                this.shell.redraw();
                this.shell.update();
                return;
            }

            if ("Invalid".equals(scmd))
            {
                // try to change the image label to an invalid image
                try
                {
                    this.imgButton.setImage(new Image(this.display, this.getClass().getResourceAsStream("/res/invalid.png")));
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
                Button focused = this.getFocusedButton();
                // query dialog for new label text
                QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
                dlg.setPromptText("Enter text:", focused.getText());
                dlg.setMinimum(1);
                dlg.setMaximum(50);
                String txt = dlg.open();
                if (txt != null)
                    focused.setText(txt);
                this.shell.update();
                return;
            }
            if ("Set as Default Button".equals(scmd))
            {
                Button focused = this.getFocusedButton();
                this.shell.setDefaultButton(focused);
                return;
            }
            if ("New Shell".equals(scmd))
            {
                Button focused = this.getFocusedButton();
                this.createToolTip("New shell",focused);
                this.tooltip.setVisible(true);
                return;
            }
            if ("Close".equals(scmd))
            {
                this.tooltip.setVisible(false);
                this.tooltip.dispose();
                return;
            }
            if ("Select All".equals(scmd))
            {
                this.changeSelectionForAll(true);
                return;
            }
            if ("Unselect All".equals(scmd))
            {
                this.changeSelectionForAll(false);
                return;
            }
            if ("Clear All".equals(scmd))
            {
                this.clearShell();
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

