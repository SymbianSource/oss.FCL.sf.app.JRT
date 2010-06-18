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
* Description: test midlet that shows the command line passed to it
*/

package base;

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.CommandListener;
import javax.microedition.lcdui.Displayable;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.StringItem;
import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

public abstract class RunnerBase extends MIDlet implements CommandListener
{

    private Form          mMainView;
    private Command       mExitCommand;
    private StringItem    mInstance;
    private StringItem    mCmdLine;
    private StringItem    mDifficulty;
    private StringItem    mWizard;
    private StringItem    mSounds;
    private StringItem    mExtra;
    private StringItem    mNotVisible;
    private StringItem    mLocale;

    protected RunnerBase(String name)
    {
        mMainView = new Form(name);
        mExitCommand = new Command("Exit", 1, 1);
        mMainView.addCommand(mExitCommand);
        mMainView.setCommandListener(this);

        mMainView.append(this.toString());
        mInstance = new StringItem("Instance","0");
        mMainView.append(mInstance);
        mCmdLine = new StringItem("CmdL","");
        mMainView.append(mCmdLine);
        mDifficulty = new StringItem("Diff","");
        mMainView.append(mDifficulty);
        mSounds = new StringItem("Sounds","");
        mMainView.append(mSounds);
        mWizard = new StringItem("Wiz","");
        mMainView.append(mWizard);
        mLocale = new StringItem("Locale","");
        mMainView.append(mLocale);

        mExtra = new StringItem("Extra","");
        mMainView.append(mExtra);
        mNotVisible = new StringItem("NotVisible","");
        mMainView.append(mNotVisible);


        Display.getDisplay(this).setCurrent(mMainView);
    }

    protected void startApp() throws MIDletStateChangeException
    {
        String prevInstance = mInstance.getText();
        setPropVal(mInstance, "com.nokia.mid.cmdline.instance");

        if (mInstance.getText().equals(prevInstance))
        {
            // No relaunch -> no new command line parameters
            return;
        }
        setPropVal(mCmdLine, "com.nokia.mid.cmdline");
        setPropVal(mDifficulty, "difficulty");
        setPropVal(mSounds, "sounds");
        setPropVal(mWizard, "wizard_mode");
        setPropVal(mLocale, "microedition.locale");
        setPropVal(mExtra, "com.test.extra");
        setPropVal(mNotVisible, "com.test.notVisible");
    }
    private void setPropVal(StringItem si, String property)
    {
        String propVal = System.getProperty(property);
        if (propVal == null)
        {
            propVal = "{null}";
        }
        si.setText(propVal);
    }

    public void commandAction(Command command, Displayable displayable)
    {
        if (command == mExitCommand)
        {
            destroyApp(true);
            notifyDestroyed();
        }
    }
    protected void pauseApp()
    {
    }

    protected void destroyApp(boolean p1)
    {
    }

};
