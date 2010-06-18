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
package com.nokia.test.java.eswt.browser;

import java.io.*;
import javax.microedition.io.file.*;
import javax.microedition.io.*;

import org.eclipse.swt.SWT;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.browser.Browser;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.ProgressEvent;
import org.eclipse.swt.browser.ProgressListener;
import org.eclipse.swt.browser.StatusTextEvent;
import org.eclipse.swt.browser.StatusTextListener;
import org.eclipse.swt.browser.TitleEvent;
import org.eclipse.swt.browser.TitleListener;
import com.nokia.test.java.eswt.midlet.TestBaseMIDlet;

public class BrowserTestMIDlet_01 extends TestBaseMIDlet implements LocationListener,
        ProgressListener, StatusTextListener, TitleListener
{

    Browser browser_top = null;
    Browser browser_top_right = null;
    Browser browser_bottom = null;
    Browser browser_bottom_right = null;

    Command okCMD = null;

    Command browser4 = null;
    Command browser3 = null;
    Command browser2 = null;
    Command browser1 = null;
    Command showBrowserCMD = null;
    Command showImageCMD = null;
    Command showHtmlInFile_LinkCMD = null;
    Command showHtmlInFile_Link_BackCMD = null;
    Command showURLCMD = null;
    Command showBookMarkCMD_1 = null;
    Command showBookMarkCMD_2 = null;
    Command showBookMarkCMD_3 = null;
    Command showBookMarkCMD_4 = null;
    Command showBookMarkCMD_5 = null;
    Command showBookMarkCMD_6 = null;
    Command showBookMarkCMD_7 = null;
    Command showBookMarkCMD_8 = null;

    Command showhttpsCMD = null;
    Command showNotAvailableCMD = null;
    Command backCMD = null;
    Command forwardCMD = null;
    Command refreshCMD = null;
    Command stopCMD = null;
    Command addLocationListener = null;
    Command addProgressListener = null;
    Command addStatusListener = null;
    Command addTitleListener = null;
    Command removeLocationListener = null;
    Command removeProgressListener = null;
    Command removeStatusListener = null;
    Command removeTitleListener = null;
    Image bigimg = null;

    final String testresource_dir = "file:///C:/data/eSWTBrowserTestResorces/";
    final String bigimagefilename = testresource_dir+"big.png";
    final String smallimagefilename = testresource_dir+"small.gif";
    final String labelimagefilename = testresource_dir+"SmallLabel.jpg";
    final String linkfilename = testresource_dir+"SelectLink.htm";
    final String firstresultfilename = testresource_dir+"link1.htm";
    final String secondsresultfilename = testresource_dir+"link2.htm";
    final String linkbackfilename = testresource_dir+"SelectLinkBak.htm";

    private String[] resource_destination = new String[]
    {
        this.bigimagefilename,
        this.smallimagefilename,
        this.labelimagefilename,
        this.linkfilename,
        this.firstresultfilename,
        this.secondsresultfilename,
        this.linkbackfilename
    };
    private String[] resource_urls = new String[]
    {
        "/res/big.png",
        "/res/small.gif",
        "/res/SmallLabel.jpg",
        "/htmlfiles/SelectLink.htm",
        "/htmlfiles/link1.htm",
        "/htmlfiles/link2.htm",
        "/htmlfiles/SelectLinkBak.htm"
    };
    private String[] bookmarks_urls = new String[]
    {
        "http://www.s60.com/",
        "http://elisa.net/",
        "http://www.eclipse.org/ercp/",
        "http://www.yahoo.com",
        "http://www.nokia.com/",
        "http://www.nokia.fi/",
        "http://www.wikipedia.org/",
        "http://java.sun.com/"
    };
    private String[] bookmarks = new String[]
    {
        "s60.com",
        "elisa.net",
        "eclipse.org",
        "yahoo.com",
        "nokia.com",
        "www.nokia.fi",
        "wikipedia.org",
        "java.sun.com"
    };
    protected void showException(String title,String msg)
    {
        MessageBox mb = new MessageBox(this.shell, SWT.ICON_INFORMATION | SWT.OK);
        mb.setText(title);
        mb.setMessage(msg);
        mb.open();
    }

    protected void init()
    {
        this.shell.setText("Browser Test");
        this.shell.setLayout(new FillLayout());

        try
        {
            FileConnection dirConnection = (FileConnection)Connector.open(this.testresource_dir, Connector.READ_WRITE);
            if (!dirConnection.exists())
            {
                dirConnection.mkdir();
                dirConnection.close();
            }
            for (int i = 0; i < resource_urls.length; i++)
            {

                FileConnection fileConnection = (FileConnection)Connector.open(this.resource_destination[i], Connector.READ_WRITE);
                if (!fileConnection.exists())
                {
                    fileConnection.create();

                    this.copyResToFile(this.resource_urls[i], fileConnection);

                    fileConnection.close();
                }
            }
            this.showException("Complete","Resources in folder:"+this.testresource_dir);
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Exception","Resource could not be loaded:"+ex.getMessage());

        }

        // listener adding submenu
        Command cmdBrowser = new Command(this.shell, Command.COMMANDGROUP, 1);
        cmdBrowser.setText("Create Browser");

        this.browser1 = new Command(cmdBrowser,Command.GENERAL, 1);
        this.browser1.setText("One");
        this.browser1.addSelectionListener(this);

        this.browser2 = new Command(cmdBrowser,Command.GENERAL, 1);
        this.browser2.setText("Two");
        this.browser2.addSelectionListener(this);

        this.browser3 = new Command(cmdBrowser,Command.GENERAL, 1);
        this.browser3.setText("Three");
        this.browser3.addSelectionListener(this);

        this.browser4 = new Command(cmdBrowser,Command.GENERAL, 1);
        this.browser4.setText("Four");
        this.browser4.addSelectionListener(this);

        this.showBrowserCMD = new Command(this.shell, Command.GENERAL, 1);
        this.showBrowserCMD.setText("Show - HTML");
        this.showBrowserCMD.addSelectionListener(this);

        this.showImageCMD = new Command(this.shell, Command.GENERAL, 1);
        this.showImageCMD.setText("Show - HTML with image");
        this.showImageCMD.addSelectionListener(this);

        this.showURLCMD = new Command(this.shell, Command.GENERAL, 1);
        this.showURLCMD.setText("Show - HTTP");
        this.showURLCMD.addSelectionListener(this);

        this.showhttpsCMD = new Command(this.shell, Command.GENERAL, 1);
        this.showhttpsCMD.setText("Show - HTTPS");
        this.showhttpsCMD.addSelectionListener(this);

        this.showNotAvailableCMD = new Command(this.shell, Command.GENERAL, 1);
        this.showNotAvailableCMD.setText("Show - Not Available");
        this.showNotAvailableCMD.addSelectionListener(this);

        this.showHtmlInFile_LinkCMD = new Command(this.shell, Command.GENERAL, 1);
        this.showHtmlInFile_LinkCMD.setText("Show - Link");
        this.showHtmlInFile_LinkCMD.addSelectionListener(this);

        this.showHtmlInFile_Link_BackCMD = new Command(this.shell, Command.GENERAL, 1);
        this.showHtmlInFile_Link_BackCMD.setText("Show - Link back");
        this.showHtmlInFile_Link_BackCMD.addSelectionListener(this);

        // listener adding submenu
        Command cmdBookmarks = new Command(this.shell, Command.COMMANDGROUP, 0);
        cmdBookmarks.setText("Bookmarks");

        for (int i =0; i < this.bookmarks.length; i++)
        {
            Command showBookMarkCMD = new Command(cmdBookmarks, Command.GENERAL, 1);
            showBookMarkCMD.setText(this.bookmarks[i]);
            showBookMarkCMD.addSelectionListener(this);
        }
    }

    private void createBrowsers(int count)
    {

        switch (count)
        {
        case 1:
            this.shell.setLayout(new FormLayout());
            this.browser_top = new Browser(this.shell,SWT.BORDER | SWT.H_SCROLL | SWT.V_SCROLL);
            this.browser_top.setBounds(10,10,200,300);
            this.addCommands(browser_top);
            break;
        case 2:
            FillLayout fillLayout = new FillLayout();
            fillLayout.type = SWT.VERTICAL;
            this.shell.setLayout(fillLayout);
            this.browser_top = new Browser(this.shell,SWT.H_SCROLL | SWT.V_SCROLL);
            this.browser_bottom = new Browser(this.shell, SWT.V_SCROLL);
            this.addCommands(browser_top);
            this.addCommands(browser_bottom);
            break;
        case 3:
            FillLayout fillLayout2 = new FillLayout();
            fillLayout2.type = SWT.VERTICAL;
            this.shell.setLayout(fillLayout2);
            this.browser_top = new Browser(this.shell,SWT.H_SCROLL | SWT.V_SCROLL);
            this.browser_bottom = new Browser(this.shell, SWT.V_SCROLL);
            this.browser_bottom_right = new Browser(this.shell,SWT.H_SCROLL);
            this.addCommands(browser_top);
            this.addCommands(browser_bottom);
            this.addCommands(browser_bottom_right);

            break;
        case 4:
            this.shell.setLayout(new GridLayout(2,true));
            GridData data = new GridData();
            data.grabExcessHorizontalSpace = true;
            data.horizontalAlignment = SWT.CENTER;
            data.grabExcessVerticalSpace = true;
            data.verticalAlignment = SWT.CENTER;

            this.browser_top = new Browser(this.shell,SWT.H_SCROLL | SWT.V_SCROLL);
            this.browser_top.setLayoutData(data);
            this.browser_top_right = new Browser(this.shell,SWT.BORDER);
            this.browser_top_right.setLayoutData(data);
            this.browser_bottom = new Browser(this.shell,SWT.BORDER);
            this.browser_bottom.setLayoutData(data);
            this.browser_bottom_right = new Browser(this.shell,SWT.H_SCROLL | SWT.V_SCROLL);
            this.browser_bottom_right.setLayoutData(data);
            this.addCommands(browser_top);
            this.addCommands(browser_top_right);
            this.addCommands(browser_bottom);
            this.addCommands(browser_bottom_right);
        default:
            break;
        }
        this.shell.layout();
        this.shell.update();

    }

    private void addCommands(Browser browser)
    {
        okCMD = new Command(browser, Command.GENERAL, 1);
        okCMD.setText("Ok");
        okCMD.addSelectionListener(this);

        backCMD = new Command(browser, Command.GENERAL, 4);
        backCMD.setText("Previous page");
        backCMD.addSelectionListener(this);
        forwardCMD = new Command(browser, Command.GENERAL, 3);
        forwardCMD.setText("Next page");
        forwardCMD.addSelectionListener(this);
        refreshCMD = new Command(browser, Command.GENERAL, 2);
        refreshCMD.setText("Refresh page");
        refreshCMD.addSelectionListener(this);
        stopCMD = new Command(browser, Command.GENERAL, 1);
        stopCMD.setText("Stop loading");
        stopCMD.addSelectionListener(this);

        // listener adding submenu
        Command cmdAddListener = new Command(browser, Command.COMMANDGROUP, 0);
        cmdAddListener.setText("Add Listener");
        this.addLocationListener = new Command(cmdAddListener, Command.GENERAL, 0);
        this.addLocationListener.setText("LocationListener");
        this.addLocationListener.addSelectionListener(this);

        this.addProgressListener = new Command(cmdAddListener, Command.GENERAL, 0);
        this.addProgressListener.setText("ProgressListener");
        this.addProgressListener.addSelectionListener(this);

        this.addStatusListener = new Command(cmdAddListener, Command.GENERAL, 0);
        this.addStatusListener.setText("StatusTextListener");
        this.addStatusListener.addSelectionListener(this);

        this.addTitleListener = new Command(cmdAddListener, Command.GENERAL, 0);
        this.addTitleListener.setText("TitleListener");
        this.addTitleListener.addSelectionListener(this);

        // listener removing submenu
        Command cmdRemoveListener = new Command(browser, Command.COMMANDGROUP, 0);
        cmdRemoveListener.setText("Remove Listener");
        this.removeLocationListener = new Command(cmdRemoveListener, Command.GENERAL, 0);
        this.removeLocationListener.setText("LocationListener");
        this.removeLocationListener.addSelectionListener(this);

        this.removeProgressListener = new Command(cmdRemoveListener, Command.GENERAL, 0);
        this.removeProgressListener.setText("ProgressListener");
        this.removeProgressListener.addSelectionListener(this);

        this.removeStatusListener = new Command(cmdRemoveListener, Command.GENERAL, 0);
        this.removeStatusListener.setText("StatusTextListener");
        this.removeStatusListener.addSelectionListener(this);

        this.removeTitleListener = new Command(cmdRemoveListener, Command.GENERAL, 0);
        this.removeTitleListener.setText("TitleListener");
        this.removeTitleListener.addSelectionListener(this);

    }

    /**
     * Return currently focused browser.
     * @return focused browser
     */
    private Browser getFocusedBrowser()
    {
        Browser result = null;

        Control[] browsers = this.shell.getChildren();
        for (int i=0; i<browsers.length; i++)
        {
            if (browsers[i].isFocusControl())
            {
                result = (Browser)(browsers[i]);
                break;
            }
        }
        return result;
    }

    public void widgetSelected(SelectionEvent e)
    {

        if (e.widget == this.okCMD)
        {

            return;
        }
        if (e.widget == this.browser1)
        {
            this.createBrowsers(1);
            return;
        }
        if (e.widget == this.browser2)
        {
            this.createBrowsers(2);
            return;
        }
        if (e.widget == this.browser3)
        {
            this.createBrowsers(3);
            return;
        }
        if (e.widget == this.browser4)
        {
            this.createBrowsers(4);
            return;
        }
        if (e.widget == this.showBrowserCMD)
        {

            this.getFocusedBrowser().setText(
                "<HTML><HEAD><TITLE>Browser</TITLE></HEAD>HTML in Browser</HTML>");
            return;

        }
        if (e.widget == this.showImageCMD)
        {
            try
            {
                this.getFocusedBrowser().setText(
                    "<HTML>" +
                    "<HEAD><TITLE>Browser</TITLE></HEAD>" +
                    "<BODY>" +
                    "HTML in Browser with gif image" +
                    "<img src="+'"'+this.smallimagefilename+'"'+"/>" +
                    "And with png image" +
                    "<img src="+'"'+this.bigimagefilename+'"'+"/>" +
                    "And with jpg image" +
                    "<img src="+'"'+this.labelimagefilename+'"'+"/>" +
                    "</BODY>" +
                    "</HTML>");
            }
            catch (Exception ex)
            {
                this.showException("Exception", ex.toString());
            }
            return;

        }

        if (e.widget == this.showURLCMD)
        {
            try
            {
                this.getFocusedBrowser().setUrl("http://www.google.fi/");
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }


        if (e.widget == this.showhttpsCMD)
        {
            try
            {
                this.getFocusedBrowser().setUrl("https://www1.ietf.org/mailman/listinfo/simple");
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.showNotAvailableCMD)
        {
            try
            {
                this.getFocusedBrowser().setUrl("http://www.notavailable.fi");
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.showHtmlInFile_LinkCMD)
        {
            try
            {
                this.getFocusedBrowser().setUrl(this.linkfilename);
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.showHtmlInFile_Link_BackCMD)
        {
            try
            {
                this.getFocusedBrowser().setUrl(this.linkbackfilename);
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.backCMD)
        {
            try
            {
                this.getFocusedBrowser().back();
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.forwardCMD)
        {

            try
            {
                this.getFocusedBrowser().forward();
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.refreshCMD)
        {
            try
            {
                this.getFocusedBrowser().refresh();
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.stopCMD)
        {
            try
            {
                this.getFocusedBrowser().stop();
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                this.showException("Exception", ex.toString());
            }
            return;
        }
        if (e.widget == this.addLocationListener)
        {

            this.getFocusedBrowser().addLocationListener(this);

            return;
        }
        if (e.widget == this.addProgressListener)
        {

            this.getFocusedBrowser().addProgressListener(this);

            return;
        }
        if (e.widget == this.addStatusListener)
        {

            this.getFocusedBrowser().addStatusTextListener(this);

            return;
        }
        if (e.widget == this.addTitleListener)
        {

            this.getFocusedBrowser().addTitleListener(this);

            return;
        }
        if (e.widget == this.removeLocationListener)
        {

            this.getFocusedBrowser().removeLocationListener(this);

            return;
        }
        if (e.widget == this.removeProgressListener)
        {

            this.getFocusedBrowser().removeProgressListener(this);

            return;
        }
        if (e.widget == this.removeStatusListener)
        {

            this.getFocusedBrowser().removeStatusTextListener(this);

            return;
        }
        if (e.widget == this.removeTitleListener)
        {

            this.getFocusedBrowser().removeTitleListener(this);

            return;
        }
        if (e.widget instanceof Command)
        {
            String scmd = ((Command) e.widget).getText();
            for (int i =0; i < this.bookmarks.length; i++)
            {
                if (this.bookmarks[i].equals(scmd))
                {

                    try
                    {
                        this.getFocusedBrowser().setUrl(this.bookmarks_urls[i]);
                    }
                    catch (Exception ex)
                    {
                        ex.printStackTrace();
                        this.showException("Exception", ex.toString());
                    }
                    return;
                }
            }
        }
        super.widgetSelected(e);
    }

    public void changed(LocationEvent event)
    {
        this.showException("Location - changed",event.location);
    }
    public void changing(LocationEvent event)
    {
        this.showException("Location - changing",event.location);
    }
    public void changed(ProgressEvent event)
    {

    }
    public void completed(ProgressEvent event)
    {

    }
    public void changed(StatusTextEvent event)
    {
        this.showException("Status Text",event.text);
    }
    public void changed(TitleEvent event)
    {
        this.showException("Title",event.title);
    }

    public void copyResToFile(String resource, FileConnection destination)
    {
        OutputStream output = null;
        InputStream input = null;
        try
        {
            input = this.getClass().getResourceAsStream(resource);
            output = destination.openOutputStream();;

            byte[] buffer = new byte[1024];

            int bytesRead = input.read(buffer);

            while (bytesRead >= 0)
            {
                output.write(buffer, 0, bytesRead);
                bytesRead = input.read(buffer);
            }

            input.close();

            output.close();
        }
        catch (Exception ex)
        {
            // show a messagebox about the exception
            this.showException("Exception",resource +"could not be loaded:"+ex.getMessage());

        }
    }

}
