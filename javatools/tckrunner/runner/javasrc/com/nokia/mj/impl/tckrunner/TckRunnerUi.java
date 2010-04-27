/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.tckrunner;

import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.layout.RowData;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Button;

import com.nokia.mj.impl.utils.Logger;

import com.nokia.mj.impl.tckrunner.TckRunner;
import com.nokia.mj.impl.tckrunner.TckRunnerStateListener;
import com.nokia.mj.impl.tckrunner.UrlHistory;

public class TckRunnerUi implements TckRunnerStateListener
{
    private Text iUrl;
    private Label iStatus;
    private Button iSkipJarCheckButton;
    private String iURL;
    private UrlHistory iHistory;
    private int iState;
    private int iRetryCount;

    private TckRunner iTckRunner;
    private Thread iTckRunnerThread;
    private boolean iRunning = false;

    private boolean iExiting = false;

    public TckRunnerUi()
    {
        iTckRunner = new TckRunner();
        iTckRunner.setListener(this);
        iHistory = new UrlHistory();
    }

    public void show()
    {
        Display display = new Display();
        Shell shell = new Shell(display);

        createUi(shell);

        shell.pack();
        shell.open();
        while (!iExiting)
        {
            if (!display.readAndDispatch())
            {
                display.sleep();
            }
        }
        display.dispose();
    }

    private void exitEventLoop()
    {
        iExiting = true;
        Display.getDefault().wake();
    }

    private void createUi(Shell shell)
    {
        try
        {
            shell.setText("TCK Runner");
            RowLayout layout = new RowLayout();
            layout.type = SWT.VERTICAL;
            layout.pack = false;
            shell.setLayout(layout);

            new Label(shell, SWT.NONE).setText("Set TCK Harness URL:");
            iUrl = new Text(shell, SWT.NONE);
            iUrl.setText(iHistory.loadUrl());

            iSkipJarCheckButton = new Button(shell, SWT.CHECK | SWT.WRAP);
            iSkipJarCheckButton.setText("Skip install time class checks");

            new Label(shell, SWT.NONE).setText("TCK Runner State:");
            iStatus = new Label(shell, SWT.NONE);
            iStatus.setText("Not running");

            Command start = new Command(shell, Command.OK, 0);
            start.setText("Start");
            start.addSelectionListener(new SelectionListener()
            {
                public void widgetDefaultSelected(SelectionEvent aEvent)
                {
                    widgetSelected(aEvent);
                }
                public void widgetSelected(SelectionEvent event)
                {
                    iStatus.setText("Starting...");
                    startTckRunner();
                }
            });

            Command stop = new Command(shell, Command.OK, 0);
            stop.setText("Stop");
            stop.addSelectionListener(new SelectionListener()
            {
                public void widgetDefaultSelected(SelectionEvent aEvent)
                {
                    widgetSelected(aEvent);
                }
                public void widgetSelected(SelectionEvent event)
                {
                    iStatus.setText("Stopping...");
                    stopTckRunner();
                }
            });

            Command exit = new Command(shell, Command.EXIT, 0);
            exit.setText("Exit");

            exit.addSelectionListener(new SelectionListener()
            {
                public void widgetDefaultSelected(SelectionEvent aEvent)
                {
                    widgetSelected(aEvent);
                }
                public void widgetSelected(SelectionEvent aEvent)
                {
                    iStatus.setText("exit pressed");
                    iHistory.saveUrl(iUrl.getText());
                    stopTckRunner();
                    exitEventLoop();
                }
            });

        }
        catch (Throwable e)
        {
            Logger.ELOG(Logger.ETckRunner, "createUi failed", e);
            exitEventLoop();
        }
    }

    private void startTckRunner()
    {
        if (!iRunning)
        {
            iURL = iUrl.getText();
            final boolean skipJarCheck = iSkipJarCheckButton.getSelection();
            iSkipJarCheckButton.setEnabled(false);
            iTckRunnerThread = new Thread(new Runnable()
            {
                public void run()
                {
                    String[] aArgs = new String[0];
                    if (skipJarCheck)
                    {
                        aArgs = new String[1];
                        aArgs[0] = "-skipjarcheck";
                    }
                    iTckRunner.start(iURL, aArgs);  // blocks here until stopped
                    iRunning = false;
                }
            });
            iRunning = true;
            iTckRunnerThread.start();
        }
        else
        {
            iStatus.setText("Started");
        }
    }

    private void stopTckRunner()
    {
        iSkipJarCheckButton.setEnabled(true);
        if (iRunning)
        {
            iTckRunner.stop();
            try
            {
                iTckRunnerThread.join();
            }
            catch (InterruptedException e) {}
        }
        else
        {
            iStatus.setText("Stopped");
        }
    }

    public void stateChanged(int aState)
    {
        iState = aState;
        Display.getDefault().asyncExec(new Runnable()
        {
            public void run()
            {
                if (!iStatus.isDisposed())
                {
                    iStatus.setText(stateToString(iState));
                }
            }
        });
    }

    public void operationFailed(int aState, int aRetryCount)
    {
        iState = aState;
        iRetryCount = aRetryCount;
        Display.getDefault().asyncExec(new Runnable()
        {
            public void run()
            {
                if (!iStatus.isDisposed())
                {
                    iStatus.setText(stateToString(iState) + " - retry " + iRetryCount);
                }
            }
        });
    }

    private String stateToString(int aState)
    {
        String state;
        switch (aState)
        {
        case TckRunnerStateListener.STATE_INSTALL_APPLICATION:
            state = "Downloading TCK Agent";
            break;
        case TckRunnerStateListener.STATE_UNINSTALL_APPLICATION:
            state = "Removing TCK Agent";
            break;
        case TckRunnerStateListener.STATE_RUN_APPLICATION:
            state = "Running TCK Agent";
            break;
        case TckRunnerStateListener.STATE_NOT_RUNNING:
            state = "Not running";
            break;
        default:
            state = "";
            break;
        }
        return state;
    }

}





