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

package com.nokia.mj.impl.javacontrolpanel;

import com.nokia.mj.impl.rt.support.Jvm;
import org.eclipse.swt.layout.*;
import org.eclipse.ercp.swt.mobile.*;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.graphics.*;

import com.nokia.mj.impl.utils.Logger;

class JavaControlPanelUi
{
    private JavaCaptain iCaptain;
    private Redirector iRedirector;
    private SecurityConfig iSecurity;

    private Shell iShell;
    private Button iJavaCaptain;
    private Button iPrewarm;
    private Button iThreadDump;
    private Button iJavaDebug;
    private Button iRedirect;
    private Combo iSecurityWarnings;
    private Combo iSecurityPolicy;

    static
    {
        Jvm.loadSystemLibrary("javacontrolpanel");
    }

    public JavaControlPanelUi()
    {
        iCaptain = new JavaCaptain();
        iRedirector = new Redirector();
        iSecurity = new SecurityConfig();
    }

    public void show()
    {
        Display display = new Display();
        iShell = new Shell(display);

        createUi();
        iShell.pack();
        iShell.open();
        handleOrientationChange();

        while (!iShell.isDisposed())
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
        iRedirector.stop();
        iShell.dispose();
    }

    private void createUi()
    {
        try
        {
            setLayout();
            createJavaCaptainUi();
            createPrewarmUi();
            createThreadDumpUi();
            createJavaDebugUi();
            createSecurityWarningsUi();
            createSecurityPolicyUi();
            createRedirectLogsUi();
            createCommands();
            platformAdaptation();
        }
        catch (Throwable e)
        {
            Logger.ELOG(Logger.EUtils, "createUi failed", e);
            exitEventLoop();
        }
    }

    private void setLayout()
    {
        iShell.setText("Java Control Panel");
        iShell.setLayout(new GridLayout(2, false));
    }

    private void createJavaCaptainUi()
    {
        Label label = new Label(iShell, SWT.NONE);
        label.setText("Java Captain");
        label.setLayoutData(new GridData(SWT.CENTER));

        iJavaCaptain = new Button(iShell, SWT.TOGGLE);
        iJavaCaptain.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        iJavaCaptain.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent event)
            {
                javaCaptainPressed();
            }
        });
        toggleJavaCaptain(iCaptain.isRunning());
    }

    private void createPrewarmUi()
    {
        Label label = new Label(iShell, SWT.NONE);
        label.setText("Prewarm");
        label.setLayoutData(new GridData(SWT.CENTER));

        iPrewarm = new Button(iShell, SWT.TOGGLE);
        iPrewarm.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        iPrewarm.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent event)
            {
                prewarmPressed();
            }
        });
        togglePrewarm(iCaptain.isPrewarmEnabled());
    }

    private void createThreadDumpUi()
    {
        Label label = new Label(iShell, SWT.NONE);
        label.setText("Thread Dump");
        label.setLayoutData(new GridData(SWT.CENTER));

        iThreadDump = new Button(iShell, SWT.PUSH);
        iThreadDump.setText("Dump");
        iThreadDump.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        iThreadDump.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent event)
            {
                threadDumpPressed();
            }
        });
    }

    private void createJavaDebugUi()
    {
        Label label = new Label(iShell, SWT.NONE);
        label.setText("Java Debug");
        label.setLayoutData(new GridData(SWT.CENTER));

        iJavaDebug = new Button(iShell, SWT.TOGGLE);
        iJavaDebug.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        iJavaDebug.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent event)
            {
                javaDebugPressed();
            }
        });
        toggleJavaDebug(iCaptain.isDebugEnabled());
    }

    private void createSecurityWarningsUi()
    {
        Label label = new Label(iShell, SWT.NONE);
        label.setText("Security Warnings");
        label.setLayoutData(new GridData(SWT.CENTER));

        iSecurityWarnings = new Combo(iShell, SWT.DROP_DOWN | SWT.READ_ONLY);
        for(int i = 0; i < SecurityConfig.MODES.length; i++)
        {
            iSecurityWarnings.add(SecurityConfig.MODES[i]);
        }
        iSecurityWarnings.select(iSecurity.getWarningsMode());
        iSecurityWarnings.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        iSecurityWarnings.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent event)
            {
                securityWarningsSelected();
            }
        });
    }

    private void createSecurityPolicyUi()
    {
        Label label = new Label(iShell, SWT.NONE);
        label.setText("Security Policy");
        label.setLayoutData(new GridData(SWT.CENTER));

        iSecurityPolicy = new Combo(iShell, SWT.DROP_DOWN | SWT.READ_ONLY);
        for(int i = 0; i < SecurityConfig.POLICIES.length; i++)
        {
            iSecurityPolicy.add(SecurityConfig.POLICIES[i]);
        }
        iSecurityPolicy.select(iSecurity.getSecurityPolicy());
        iSecurityPolicy.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        iSecurityPolicy.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent event)
            {
                securityPolicySelected();
            }
        });
    }

    private void createRedirectLogsUi()
    {
        Label label = new Label(iShell, SWT.NONE);
        label.setText("Redirect Logs");
        label.setLayoutData(new GridData(SWT.CENTER));

        iRedirect = new Button(iShell, SWT.TOGGLE);
        iRedirect.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        iRedirect.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent event)
            {
                redirectPressed();
            }
        });
        toggleRedirect(iRedirector.isRedirecting());
    }

    private void createCommands()
    {
        Command exit = new Command(iShell, Command.EXIT, 0);
        exit.setText("Exit");
        exit.addSelectionListener(new SelectionListener()
        {
            public void widgetDefaultSelected(SelectionEvent aEvent)
            {
                widgetSelected(aEvent);
            }
            public void widgetSelected(SelectionEvent aEvent)
            {
                exitEventLoop();
            }
        });
    }

    private void platformAdaptation()
    {
        if (isLinux())
        {
            configureUi();
        }
    }

    private void configureUi()
    {
        // add second dummy command so that menu bar is visible
        new Command(iShell, Command.GENERAL, 0).setText("");

        // disable not supported services
        iRedirect.setEnabled(false);
        iSecurityPolicy.setEnabled(false);
        iSecurityWarnings.setEnabled(false);
    }

    private void handleOrientationChange()
    {
        iShell.addControlListener(new ControlListener()
        {
            private boolean handleEvent = false;
            public void controlMoved(ControlEvent e)
            {
            }
            public void controlResized(ControlEvent e)
            {
                // ignore first event that comes during startup
                if (handleEvent)
                {
                    // avoid flickering during orientation change
                    iShell.setVisible(false);
                    iShell.layout();
                    iShell.setVisible(true);
                }
                handleEvent = true;
            }
        });
    }


    private void javaCaptainPressed()
    {
        if (iJavaCaptain.getSelection())
        {
            iCaptain.start();
            javaCaptainStarted();
        }
        else
        {
            iCaptain.stop();
        }
        toggleJavaCaptain(iJavaCaptain.getSelection());
    }

    private void toggleJavaCaptain(boolean aSelected)
    {
        iJavaCaptain.setText( aSelected ? "Running" : "Stopped");
        iJavaCaptain.setSelection(aSelected);
    }

    private void prewarmPressed()
    {
        iCaptain.enablePrewarm(iPrewarm.getSelection());
        togglePrewarm(iPrewarm.getSelection());
    }

    private void togglePrewarm(boolean aSelected)
    {
        iPrewarm.setText( aSelected ? "Enabled" : "Disabled");
        iPrewarm.setSelection(aSelected);
    }

    private void threadDumpPressed()
    {
        iCaptain.doThreadDump();
    }

    private void javaDebugPressed()
    {
        iCaptain.enableDebug(iJavaDebug.getSelection());
        toggleJavaDebug(iJavaDebug.getSelection());
    }

    private void toggleJavaDebug(boolean aSelected)
    {
        iJavaDebug.setText( aSelected ? "Enabled" : "Disabled");
        iJavaDebug.setSelection(aSelected);
    }

    private void securityWarningsSelected()
    {
        iSecurity.setWarningsMode(iSecurityWarnings.getSelectionIndex());
    }

    private void securityPolicySelected()
    {
        iSecurity.setSecurityPolicy(iSecurityPolicy.getSelectionIndex());
    }

    private void redirectPressed()
    {
        if (iRedirect.getSelection())
        {
            iRedirector.start();
        }
        else
        {
            iRedirector.stop();
        }
        toggleRedirect(iRedirect.getSelection());
    }

    private void toggleRedirect(boolean aSelected)
    {
        iRedirect.setText( aSelected ? "Yes" : "No");
        iRedirect.setSelection(aSelected);
    }

    private void javaCaptainStarted()
    {
        // some JavaCaptain services return to default settings
        // when JC is restarted so refresh states
        togglePrewarm(iCaptain.isPrewarmEnabled());
        toggleJavaDebug(iCaptain.isDebugEnabled());
    }

    private boolean isLinux()
    {
        String platform = System.getProperty("os.name");
        if (platform != null && platform.equalsIgnoreCase("linux"))
        {
            return true;
        }
        return false;
    }

}





