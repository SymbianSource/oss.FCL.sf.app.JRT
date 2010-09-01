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
//
// CommandPeer.java
//
package javax.microedition.lcdui;

import com.nokia.mj.impl.rt.support.Finalizer;

/**
 * Displayable/Item scoped native Command peer.
 *
 * Has to be at least display/toolkit scoped as commands may be
 * added to multiple displayables/items in potentially multiple
 * midlets in a suite - each of which would have their own
 * UI thread. Peer lifetime is limited by the ui thread that
 * created it.
 */
class CommandPeer
{
    /**
     * Special command id's
     */
    static final int NULL_CMD_ID    = 0;
    static final int DISMISS_CMD_ID = 1;
    static final int SELECT_CMD_ID  = 2;
    static final int USER_CMD_ID    = 4;    // start of user command range

    final CommandSet    iContainer;
    final Command       iCommand;
    int                 iHandle;
    int                 iID;

    private Finalizer mFinalizer;

    CommandPeer(CommandSet aContainer, Command aCommand)
    {
        iContainer=aContainer;
        iCommand=aCommand;

        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

        Toolkit toolkit = aContainer.getToolkit();
        synchronized (toolkit)
        {
            if (Alert.DISMISS_COMMAND == iCommand)
            {
                iID = DISMISS_CMD_ID;
            }
            else
            {
                iID = iContainer.allocID();
            }
            iHandle  = Toolkit.checkHandle(
                           _create(
                               toolkit.getHandle(),
                               aCommand.getLabel(),
                               aCommand.getLongLabel(),
                               aCommand.getCommandType(),
                               aCommand.getPriority(),
                               iID
                           )
                       );
        }
    }

    int getHandle()
    {
        if (iHandle <= 0)
        {
            throw new RuntimeException("bad handle");
        }
        return iHandle;
    }

    /**
     *@return the command peer id - note this is distinct from the
     * Command id which only takes two 'special' values.
     */
    int getId()
    {
        return iID;
    }

    Command getCommand()
    {
        return iCommand;
    }

    void dispose()
    {
        Toolkit toolkit = iContainer.getToolkit();
        synchronized (toolkit)
        {
            iContainer.freeID(iID);
            iID = 0;
            toolkit.disposeObject(iHandle);
            iHandle =0;
        }
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    private void registeredFinalize()
    {
        dispose();
    }

    private native int _create(int aToolkit, String aLabel, String aLongLabel, int aType, int aPriority, int aID);
}
