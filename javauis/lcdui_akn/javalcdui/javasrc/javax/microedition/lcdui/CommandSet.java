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
//
//
package javax.microedition.lcdui;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;

class CommandSet
{
    //
    // IMPROVEMENT: better index
    //
    Toolkit iToolkit;
    Hashtable iMap;

    /**
     * V. simple command peer id allocator.
     *
     * If very long running MIDlet created staggeringly huge numbers of transient
     * commands we would have to replace this with a free-list allocator.
     *
     */
    int iNextId = CommandPeer.USER_CMD_ID;

    synchronized int allocID()
    {
        return iNextId+=CommandPeer.USER_CMD_ID;
    }

    synchronized void freeID(int aID)
    {
        /* nop */
    }

    CommandSet(Toolkit aToolkit)
    {
        iToolkit = aToolkit;
        iMap = new Hashtable();
    }

    CommandPeer getPeerById(int aId)
    {
        //
        // Improvement: consider id index
        //
        Enumeration e = iMap.elements();
        while (e.hasMoreElements())
        {
            CommandPeer peer = (CommandPeer)e.nextElement();
            if (peer.getId() == aId)
            {
                return peer;
            }
        }
        return null;
    }

    Command getCommandById(int aId)
    {
        CommandPeer peer = getPeerById(aId);
        if (null != peer)
        {
            return peer.getCommand();
        }
        return null;
    }

    CommandPeer add(Command aCommand)
    {
        CommandPeer peer = getPeer(aCommand);   // throws NPE
        if (null == peer)
        {
            peer = new CommandPeer(this, aCommand);
            iMap.put(aCommand, peer);
            return peer;
        }
        return null;
    }

    void remove(CommandPeer aPeer)
    {
        Command command = aPeer.getCommand();
        CommandPeer peer = getPeer(command);    // throws NPE
        if (peer == aPeer)
        {
            iMap.remove(command);
            peer.dispose();
        }
    }

    CommandPeer getPeer(Command aCommand) // throws NPE
    {
        return (CommandPeer)iMap.get(aCommand);
    }

    int count()
    {
        return iMap.size();
    }

    Enumeration elements()
    {
        return iMap.elements();
    }

    Toolkit getToolkit()
    {
        return iToolkit;
    }
}