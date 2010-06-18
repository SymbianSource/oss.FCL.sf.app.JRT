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
package com.nokia.test.java.eswt.hyperlink;


import org.eclipse.ercp.swt.mobile.QueryDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.MenuItem;
import org.eclipse.swt.widgets.MessageBox;

import org.eclipse.ercp.swt.mobile.HyperLink;

import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class HyperlinkTestMIDlet_01 extends TestBaseMIDlet
{

    /**  push text button */
    private HyperLink url = null;
    /**  check button */
    private HyperLink email = null;
    /** radio button */
    private HyperLink phone = null;


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
        this.shell.setText("HyperLink Test");
        this.shell.setLayout(new GridLayout());

        // HyperLink url
        this.url = new HyperLink(this.shell, SWT.BORDER, HyperLink.URL);
        this.url.setText("www.google.com");
        this.url.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // HyperLink url
        this.email = new HyperLink(this.shell, SWT.BORDER, HyperLink.EMAIL);
        this.email.setText("some.body@somewhere.com");
        this.email.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // HyperLink url
        this.phone = new HyperLink(this.shell, SWT.BORDER, HyperLink.PHONE);
        this.phone.setText("+358504804394");
        this.phone.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // HyperLink no border url
        this.url = new HyperLink(this.shell, SWT.RIGHT, HyperLink.URL);
        this.url.setText("URL-no border");
        this.url.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // HyperLink no border email
        this.email = new HyperLink(this.shell, SWT.CENTER, HyperLink.EMAIL);
        this.email.setText("noborderEMAIL");
        this.email.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // HyperLink no border email
        this.phone = new HyperLink(this.shell, SWT.LEFT, HyperLink.PHONE);
        this.phone.setText("Phone_No_Border");
        this.phone.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false));

        // set up the menu
        Menu mainmenu = new Menu(this.shell, SWT.BAR);
        this.shell.setMenuBar(mainmenu);

        // "change text" menuitem
        MenuItem item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Change Text");
        item.addSelectionListener(this);

        //"set long text" menuitem

        item = new MenuItem(mainmenu, SWT.PUSH);
        item.setText("Set Long Text");
        item.addSelectionListener(this);

    }

    /**
     * Return currently focused HyperLink.
     * @return focused HyperLink
     */
    private HyperLink getFocusedHyperLink()
    {
        HyperLink result = null;

        Control[] links = this.shell.getChildren();
        for (int i=0; i<links.length; i++)
        {
            if (links[i].isFocusControl())
            {
                result = (HyperLink)links[i];
                break;
            }
        }

        return result;
    }


    /**
     * selectionlistener implementation
     */
    public void widgetSelected(SelectionEvent event)
    {

        if (event.widget instanceof MenuItem)
        {
            String scmd = ((MenuItem)event.widget).getText();


            if ("Change Text".equals(scmd))
            {
                HyperLink focused = this.getFocusedHyperLink();
                // query dialog for new label text
                QueryDialog dlg = new QueryDialog(this.shell, SWT.APPLICATION_MODAL, QueryDialog.STANDARD);
                dlg.setPromptText("Enter text:", focused.getText());
                dlg.setMinimum(1);
                dlg.setMaximum(50);
                String txt = dlg.open();
                if (txt != null)
                    focused.setText(txt);
                return;
            }
            if ("Set Long Text".equals(scmd))
            {
                HyperLink focused = this.getFocusedHyperLink();
                focused.setText("This is a very long text that should go to several rows. Let us see how it behaves!"+
                                "So, what will happen? It is interesting to see.");
                return;
            }

        }
        else if (event.widget == this.cmdExit)
        {

        }
        super.widgetSelected(event);
    }
}


