/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.lcdui;

import java.util.Enumeration;
import com.nokia.mj.impl.rt.legacy.NativeError;

public class Alert extends Screen
{
    public static final int FOREVER = -2;

    //
    // This is the default Command that is sent to the command listener
    // when the alert is dismissed by the user or when it times out.
    //
    public static final Command DISMISS_COMMAND = new Command(Command.OK);

    private static final AlertCommandListener DEFAULT_LISTENER = new AlertCommandListener();

    private static final int MODAL_COMMAND_COUNT = 2;

    private String iString;
    private Image iImage;
    private Gauge iIndicator;

    private AlertType iType;

    private int iTime;

    //
    private Displayable iReturn;

    public Alert(String aTitle)
    {
        this(aTitle,null,null,null);
    }

    public Alert(String aTitle, String aAlertText, Image aImage, AlertType aAlertType)
    {
        super();
        synchronized (iToolkit)
        {
            iString = aAlertText;
            iImage  = aImage;
            iType   = aAlertType;

            final int toolkit = iToolkit.getHandle();
            final int image   = iImage == null ? 0 : iImage.getHandle(true);
            final int type    = aAlertType == null ? -1 : aAlertType.Type();

            final int[] createAlert = new int[3];
            NativeError.check(_create(toolkit, getContainerHandle(), aAlertText, image, type, createAlert));
            setContent(createAlert[0], createAlert[1], createAlert[2]);

            DoAddCommand(DISMISS_COMMAND);
        }
        setTitle(aTitle);
        setTimeout(getDefaultTimeout());
        setCommandListener(DEFAULT_LISTENER);
    }


    public void setTimeout(int aTime)
    {
        if (aTime <= 0 && aTime != FOREVER)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            NativeError.check(_setTimeout(getContentHandle(), toolkit, aTime));
            iTime = aTime;
        }
    }

    public int getTimeout()
    {
        final int time;
        synchronized (iToolkit)
        {
            if (IsModal())
            {
                time = FOREVER;
            }
            else
            {
                time = iTime;
            }
        }
        return time;
    }

    public int getDefaultTimeout()
    {
        synchronized (iToolkit)
        {
            return _getDefaultTimeout(getContentHandle(),iToolkit.getHandle());
        }
    }

    public void setType(AlertType aType)
    {
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int type = aType == null ? -1 : aType.Type();

            NativeError.check(_setType(getContentHandle(), toolkit, type));

            iType = aType;
        }
    }

    public AlertType getType()
    {
        return iType;
    }

    /**
     * Image snapshots are taken on the plugin side
     */
    public void setImage(Image aImage)
    {
        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();
            final int image = aImage == null ? 0 : aImage.getHandle(true);

            NativeError.check(_setImage(getContentHandle(), toolkit, image));

            iImage = aImage;
        }
    }

    public Image getImage()
    {
        return iImage;
    }

    public void setString(String aString)
    {

        synchronized (iToolkit)
        {
            final int toolkit = iToolkit.getHandle();

            NativeError.check(_setString(getContentHandle(), toolkit, aString));

            iString = aString;
        }
    }

    public String getString()
    {
        return iString;
    }

    public void setIndicator(Gauge aIndicator)
    {
        if ((aIndicator != null) && !aIndicator.isAlertGauge())
        {
            throw new IllegalArgumentException();
        }

        if (iIndicator == aIndicator)
        {
            return;
        }

        Gauge removed = aIndicator;
        try
        {
            if (aIndicator != null)
            {
                aIndicator.addNotify(this);
            }
            synchronized (iToolkit)
            {
                final int toolkit = iToolkit.getHandle();
                final int gauge   = aIndicator==null?0:aIndicator.getHandle();
                NativeError.check(_setIndicator(getContentHandle(), toolkit, gauge));
                removed    = iIndicator;
                iIndicator = aIndicator;
            }
        }
        finally
        {
            if (removed != null)
            {
                removed.removeNotify(this);
            }
        }
    }

    public Gauge getIndicator()
    {
        return iIndicator;
    }

    public void addCommand(Command aCommand)
    {
        if (aCommand != DISMISS_COMMAND)
        {
            synchronized (iToolkit)
            {
                if (DoAddCommand(aCommand) > MODAL_COMMAND_COUNT)
                {
                    NativeError.check(_setModal(getContentHandle(),iToolkit.getHandle()));
                }
                DoRemoveCommand(DISMISS_COMMAND);
            }
        }
    }

    public void removeCommand(Command aCommand)
    {
        if (aCommand != DISMISS_COMMAND)
        {
            synchronized (iToolkit)
            {
                if (DoRemoveCommand(aCommand) == 0)
                {
                    DoAddCommand(DISMISS_COMMAND);
                }
            }
        }
    }

    public void setCommandListener(CommandListener aListener)
    {
        if (aListener == null)
        {
            aListener = DEFAULT_LISTENER;
        }
        super.setCommandListener(aListener);
    }

    void SetReturnScreen(Displayable aScreen)
    {
        if ((null != aScreen) && (aScreen instanceof Alert))
        {
            throw new IllegalArgumentException("Alert cannot return to Alert");
        }
        iReturn = aScreen;
    }

    /**
     * Called from native code when Alert timer completes. Sends current command.
     * This will go to the default listener (this) or the application listener.
     */
    void handleEvent(int aEvent,int aData0, int aData1)
    {
        switch (aEvent)
        {
        case Toolkit.EVENT_DISMISS:
        {
            /*
             * For a EVENT_DISMISS to have been received, the alert
             * must have been non-modal and have timed out. A non-modal
             * alert here can only have one Command which by default
             * will be Alert.DISMISS_COMMAND although it may be another
             * command set by the application.
             */
            Command command = DISMISS_COMMAND;
            synchronized (iToolkit)
            {
                Enumeration e = iCommands.elements();
                if (e.hasMoreElements())
                {
                    CommandPeer peer = (CommandPeer)e.nextElement();
                    command = peer.getCommand();
                }
            }
            callCommandAction(command);
        }
        break;
        default:
            super.handleEvent(aEvent, aData0, aData1);
            break;
        }
    }

    /**
     * Called by when the Alert is made current.
     * If the Alert is not modal, the timer will be started.
     */
    private boolean IsModal()
    {
        return _isModal(getContentHandle(),iToolkit.getHandle());
    }

    void dismiss()
    {
        final Display display;
        final Displayable screen;
        synchronized (iToolkit)
        {
            display = iDisplay;
            screen  = iReturn;
            iReturn = null;
        }
        display.setCurrent(screen);
    }

    private native int _create(int aToolkit, int aDisplayable, String aString, int aImage, int aType, int[] aCreateAlertReturn);
    private native int _setString(int aAlert,int aToolkit,String aString);
    private native int _setImage(int aAlert,int aToolkit,int aImage);
    private native int _setIndicator(int aAlert,int aToolkit,int aGauge);
    private native int _setType(int aAlert,int aToolkit,int aType);
    private native int _getDefaultTimeout(int aAlert,int aToolkit);
    private native int _setTimeout(int aAlert,int aToolkit,int aTimeout);
    private native int _setModal(int aAlert,int aToolkit);
    private native boolean _isModal(int aAlert,int aToolkit);
}

/**
 * Default command listener.
 * All Commands clear the Alert.
 */
class AlertCommandListener implements CommandListener
{
    public void commandAction(Command aCommand,Displayable aSource)
    {
        final Alert alert = (Alert)aSource;
        alert.dismiss();
    }
}


