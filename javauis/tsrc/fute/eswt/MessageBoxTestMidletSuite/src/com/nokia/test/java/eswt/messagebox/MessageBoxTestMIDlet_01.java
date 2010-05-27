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

package com.nokia.test.java.eswt.messagebox;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

/**
 * this class is intended to test the messagebox component
 * - changing messagebox content
 * - creating messagebox
 * - scrolling in messagebox
 * - softkey mapping in messagebox
 */
public class MessageBoxTestMIDlet_01 extends TestBaseMIDlet
{
    /** error messagebox */
    private MessageBox errmsgbox = null;
    /** information messagebox */
    private MessageBox infomsgbox = null;
    /** question messagebox */
    private MessageBox qstmsgbox = null;
    /** warning messagebox */
    private MessageBox warnmsgbox = null;
    /** working messagebox */
    private MessageBox workmsgbox = null;
    /** label to report the selected softkey */
    private Label keylabel = null;

    /**
     * initialization
     */
    protected void init()
    {
        // customize the shell
        this.shell.setText("MessageBox");
        this.shell.setLayout(new GridLayout());

        // represent the selected softkey
        Label label = new Label(this.shell, SWT.NONE);
        label.setText("SoftKey:");
        label.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));
        this.keylabel = new Label(this.shell, SWT.NONE);
        this.keylabel.setText("");
        this.keylabel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // setup the messageboxes
        this.errmsgbox = new MessageBox(this.shell, SWT.ICON_ERROR | SWT.OK);
        this.errmsgbox.setText("Error...");
        this.errmsgbox.setMessage("This is an \"error\" messagebox.");
        this.infomsgbox = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK | SWT.CANCEL);
        this.infomsgbox.setText("Information...");
        this.infomsgbox.setMessage("This is an \"information\" messagebox.");
        this.qstmsgbox = new MessageBox(this.shell, SWT.ICON_QUESTION | SWT.YES | SWT.NO);
        this.qstmsgbox.setText("Question...");
        this.qstmsgbox.setMessage("This is a \"question\" messagebox.");
        this.warnmsgbox = new MessageBox(this.shell, SWT.ICON_WARNING | SWT.YES | SWT.NO | SWT.CANCEL);
        this.warnmsgbox.setText("Warning...");
        this.warnmsgbox.setMessage("This is a \"warning\" messagebox.");
        this.workmsgbox = new MessageBox(this.shell, SWT.ICON_WORKING | SWT.RETRY | SWT.CANCEL);
        this.workmsgbox.setText("Working...");
        this.workmsgbox.setMessage("This is a \"working\" messagebox.");

        // setup the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);
        // "show" submenu
        Menu menu = new Menu(mainmenu);
        MenuItem item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Show");
        item.setMenu(menu);
        // "error" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Error");
        item.addSelectionListener(this);
        // "information" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Information");
        item.addSelectionListener(this);
        // "question" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Question");
        item.addSelectionListener(this);
        // "warning" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Warning");
        item.addSelectionListener(this);
        // "working" menuitem for "show" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Working");
        item.addSelectionListener(this);
        // "change title" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Change Title");
        item.setMenu(menu);
        // "null" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Null");
        item.addSelectionListener(this);
        // "empty" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Empty");
        item.addSelectionListener(this);
        // "short" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Short");
        item.addSelectionListener(this);
        // "long" menuitem for "change title" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Long");
        item.addSelectionListener(this);
        // "change text" submenu
        menu = new Menu(mainmenu);
        item = new MenuItem(mainmenu, SWT.CASCADE);
        item.setText("Change Text");
        item.setMenu(menu);
        // "null" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Null");
        item.addSelectionListener(this);
        // "empty" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Empty");
        item.addSelectionListener(this);
        // "short" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Short");
        item.addSelectionListener(this);
        // "long" menuitem for "change text" submenu
        item = new MenuItem(menu, SWT.PUSH);
        item.setText("Long");
        item.addSelectionListener(this);
    }

    /** display the pressed softkey */
    private void key(int code)
    {
        switch (code)
        {
        case SWT.OK:
            this.keylabel.setText("Ok selected.");
            break;
        case SWT.CANCEL:
            this.keylabel.setText("Cancel selected.");
            break;
        case SWT.YES:
            this.keylabel.setText("Yes selected.");
            break;
        case SWT.NO:
            this.keylabel.setText("No selected.");
            break;
        case SWT.RETRY:
            this.keylabel.setText("Retry selected.");
            break;
        default:
            this.keylabel.setText("Unexpected keycode.");
            break;
        }
    }

    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {
        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem) event.widget).getText();
            if ("Error".equals(scmd))
            {
                // display the error mesagebox
                this.key(this.errmsgbox.open());
                return;
            }
            if ("Information".equals(scmd))
            {
                // display the information mesagebox
                this.key(this.infomsgbox.open());
                return;
            }
            if ("Question".equals(scmd))
            {
                // display the question mesagebox
                this.key(this.qstmsgbox.open());
                return;
            }
            if ("Warning".equals(scmd))
            {
                // display the warning mesagebox
                this.key(this.warnmsgbox.open());
                return;
            }
            if ("Working".equals(scmd))
            {
                // display the working mesagebox
                this.key(this.workmsgbox.open());
                return;
            }
            if ("Null".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                try
                {
                    if ("Change Title".equals(scmd))
                        // try to set the title text to null
                        this.errmsgbox.setText(null);
                    if ("Change Text".equals(scmd))
                        // try to set the message text to null
                        this.errmsgbox.setMessage(null);
                }
                catch (Exception ex)
                {
                    // display the exception
                    MessageBox msg = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
                    msg.setText("Exception");
                    msg.setMessage(ex.getMessage());
                    msg.open();
                }
                return;
            }
            if ("Empty".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Change Title".equals(scmd))
                {
                    // change titles to an empty text
                    this.errmsgbox.setText("");
                    this.infomsgbox.setText("");
                    this.qstmsgbox.setText("");
                    this.warnmsgbox.setText("");
                    this.workmsgbox.setText("");
                    return;
                }
                if ("Change Text".equals(scmd))
                {
                    // change the message text to an empty text
                    this.errmsgbox.setMessage("");
                    this.infomsgbox.setMessage("");
                    this.qstmsgbox.setMessage("");
                    this.warnmsgbox.setMessage("");
                    this.workmsgbox.setMessage("");
                    return;
                }
            }
            if ("Short".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Change Title".equals(scmd))
                {
                    // change titles to a short title
                    this.errmsgbox.setText("Error...");
                    this.infomsgbox.setText("Information...");
                    this.qstmsgbox.setText("Question...");
                    this.warnmsgbox.setText("Warning...");
                    this.workmsgbox.setText("Working...");
                    return;
                }
                if ("Change Text".equals(scmd))
                {
                    // change the message text to a short text
                    this.errmsgbox.setMessage("This is an \"error\" messagebox.");
                    this.infomsgbox.setMessage("This is an \"information\" messagebox.");
                    this.qstmsgbox.setMessage("This is a \"question\" messagebox.");
                    this.warnmsgbox.setMessage("This is a \"warning\" messagebox.");
                    this.workmsgbox.setMessage("This is a \"working\" messagebox.");
                    return;
                }
            }
            if ("Long".equals(scmd))
            {
                scmd = ((MenuItem) event.widget).getParent().getParentItem().getText();
                if ("Change Title".equals(scmd))
                {
                    // change titles to a long title
                    this.errmsgbox.setText("Long title for Error messagebox...");
                    this.infomsgbox.setText("Long title for Information messagebox...");
                    this.qstmsgbox.setText("Long title for Question messagebox...");
                    this.warnmsgbox.setText("Long title for Warning messagebox...");
                    this.workmsgbox.setText("Long title for Working messagebox...");
                    return;
                }
                if ("Change Text".equals(scmd))
                {
                    // change the message text to a long text
                    this.errmsgbox.setMessage("It should be scrollable in case the whole message text does not fit on the screen. In that case it can be scrolled page by page with the Up and Down keys. The graphic icon within the message box does not scroll and it is always shown on the message box...");
                    this.infomsgbox.setMessage("It should be scrollable in case the whole message text does not fit on the screen. In that case it can be scrolled page by page with the Up and Down keys. The graphic icon within the message box does not scroll and it is always shown on the message box...");
                    this.qstmsgbox.setMessage("It should be scrollable in case the whole message text does not fit on the screen. In that case it can be scrolled page by page with the Up and Down keys. The graphic icon within the message box does not scroll and it is always shown on the message box...");
                    this.warnmsgbox.setMessage("It should be scrollable in case the whole message text does not fit on the screen. In that case it can be scrolled page by page with the Up and Down keys. The graphic icon within the message box does not scroll and it is always shown on the message box...");
                    this.workmsgbox.setMessage("It should be scrollable in case the whole message text does not fit on the screen. In that case it can be scrolled page by page with the Up and Down keys. The graphic icon within the message box does not scroll and it is always shown on the message box...");
                    return;
                }
            }
        }
        super.widgetSelected(event);
    }
}
