/*******************************************************************************
 * Copyright (c) 2005, 2007 Nokia.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile.internal;

import java.util.Vector;
import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.internal.symbian.MobileCommands;

/**
 */
public class CommandCollection implements MobileCommands
{
    Vector commands = new Vector();
    boolean commandsAreBeingDisposed = false;

    private CommandCollection()
    {
    }

    static public CommandCollection getCommandChildren(Control control)
    {
        CommandCollection commandChildren = (CommandCollection)(control.internal_getCommandChildren());
        if (commandChildren == null)
        {
            commandChildren = new CommandCollection();
            control.internal_setCommandChildren(commandChildren);
        }
        return commandChildren;
    }

    public void release()
    {
        commandsAreBeingDisposed = true;
        int count = commands.size();
        for (int i=0; i<count; ++i)
        {
            Command command = (Command)commands.elementAt(i);
            // internal_release is protected, workaround with commandsAreBeingDisposed
            //if (!command.isDisposed()) command.internal_release();
            if (!command.isDisposed()) command.dispose();
        }
        commands.removeAllElements();
        commands = null;
        commandsAreBeingDisposed = false;
    }

    public void addCommand(Command command)
    {
        commands.addElement(command);
    }
    public void removeCommand(Command command)
    {
        // Do not disturb if we are in release method
        if (!commandsAreBeingDisposed)
        {
            commands.removeElement(command);
        }
    }
}
