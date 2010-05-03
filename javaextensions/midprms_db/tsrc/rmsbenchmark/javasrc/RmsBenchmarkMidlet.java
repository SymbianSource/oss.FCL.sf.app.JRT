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

import java.io.*;
import javax.microedition.lcdui.*;
import javax.microedition.midlet.*;

public class RmsBenchmarkMidlet extends MIDlet implements CommandListener
{
    private static final Command EXIT_CMD = new Command("Exit", 6, 0);
    private static final Command RUN_CMD = new Command("Quick Test", 6, 0);
    private static final Command FULL_CMD = new Command("Full Test", 6, 0);

    private Form myForm;
    private Display myDisplay;

    private BenchmarkRunner myBenchmarkRunner;
    private boolean myTestRunning = false;

    public RmsBenchmarkMidlet()
    {
        myDisplay = Display.getDisplay(this);
        myForm = new Form("RMS Benchmark");
        myBenchmarkRunner = new BenchmarkRunner(this);
    }

    protected void pauseApp()
    {
    }

    public void destroyApp(boolean flag)
    {
        notifyDestroyed();
    }

    public void startApp()
    {
        myDisplay.setCurrent(myForm);
        if (!myTestRunning)
        {
            myTestRunning = true;
            myBenchmarkRunner.start();
        }
    }

    public void updateForm(String txt)
    {
        myForm.append(txt + "\n");
    }

    public void testsCompleted()
    {
        System.out.println("RmsBenchmarkMidlet.testsCompleted()");
        myTestRunning = false;
        updateForm("Tests completed!");
        myForm.addCommand(EXIT_CMD);
        myForm.addCommand(RUN_CMD);
        myForm.addCommand(FULL_CMD);
        myForm.setCommandListener(this);
    }

    public void commandAction(Command command, Displayable displayable)
    {
        if (command == EXIT_CMD)
        {
            destroyApp(true);
            notifyDestroyed();
        }
        else if (command == RUN_CMD)
        {
            System.out.println("RUN_CMD");
            if (!myTestRunning)
            {
                System.out.println("Running quick benchmarks");
                myTestRunning = true;
                myBenchmarkRunner.start();
            }
            myForm.deleteAll();
        }
        else if (command == FULL_CMD)
        {
            System.out.println("FULL_CMD");
            if (!myTestRunning)
            {
                System.out.println("Running full benchmarks");
                myTestRunning = true;
                myBenchmarkRunner.startFull();
            }
            myForm.deleteAll();
        }
    }
}
