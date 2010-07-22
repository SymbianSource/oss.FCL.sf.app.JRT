/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package org.eclipse.swt.internal.qt;

import org.eclipse.ercp.swt.mobile.Command;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Decorations;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;

/**
 * Manages the Command presentation cycle. This is a singleton object that gets
 * notified about the events that can have an effect on the way the Commands are
 * represented. It maintains an ordered list of Commands on the current focus
 * context with the help of {@link CommandCollection}. It also notifies the
 * currently active {@link CommandPresentationStrategy} so that the presentation
 * is updated if necessary.
 * 
 * @see Command
 * @see CommandCollection
 * @see CommandPresentationStrategy
 */
public class CommandArranger {

/**
 * A helper for keeping an ordered list of {@link Command}s in the current focus
 * context. The Commands are ordered according to their proximity to the
 * currently focused control. The ordered list starts with Commands from the
 * currently focused Control if any and ends with the active Shell and includes
 * all the Commands in between. If a Control has more than one Command they are
 * ordered among themselves according to creation order.
 */
public class CommandCollection {

private Command[] fCommands;

CommandCollection() {
    super();
}

/**
 * Adds the command to the collection. Warning: This does not make duplicate
 * control.
 * 
 * @param command
 */
void addCommand(Command command) {
    if (command == null)
        return;
    if (fCommands == null) {
        fCommands = new Command[1];
        fCommands[0] = command;
        return;
    }
    int size = fCommands.length + 1;
    Command[] newList = new Command[size];
    // find the insertion point so that the order is correct
    int insertPoint = 0;
    Shell activeShell = display.getActiveShell();
    Control ctrl = display.getFocusControl();
    while (ctrl != null && ctrl != activeShell) {
        if (ctrl == command.control) {
            // Adding a command to focused control increment by one.
            // adding Command.internal_getCommands(ctrl).length will
            // just duplicate the count for Commands already in array.
            insertPoint++;
            break;
        }
        insertPoint += Command.internal_getCommands(ctrl).length;
        ctrl = ctrl.getParent();
    }
    System.arraycopy(fCommands, 0, newList, 0, insertPoint);
    System.arraycopy(fCommands, insertPoint, newList, insertPoint + 1, fCommands.length
        - insertPoint);
    newList[insertPoint] = command;
    fCommands = newList;
}

/**
 * Adds the list of Commands to the collection.
 * 
 * @param commands
 */
void addCommand(Command[] commands) {
    if (commands == null || commands.length == 0)
        return;
    if (fCommands == null) {
        fCommands = new Command[commands.length];
        System.arraycopy(commands, 0, fCommands, 0, commands.length);
        return;
    }
    int size = commands.length + fCommands.length;
    Command[] newList = new Command[size];
    System.arraycopy(fCommands, 0, newList, 0, fCommands.length);
    System.arraycopy(commands, 0, newList, fCommands.length, commands.length);
    fCommands = newList;
}

/**
 * Removes the command from the list.
 * 
 * @param command
 */
void removeCommand(Command command) {
    if (command == null)
        return;
    if (fCommands == null || fCommands.length == 0)
        return;
    int removeIndex = -1;
    for (int i = 0; i < fCommands.length; i++) {
        if (fCommands[i] == command) {
            removeIndex = i;
        }
    }
    if (removeIndex == -1)
        return;
    Command[] newList = new Command[fCommands.length - 1];
    System.arraycopy(fCommands, 0, newList, 0, removeIndex);
    System.arraycopy(fCommands, removeIndex + 1, newList, removeIndex, (fCommands.length
        - removeIndex - 1));
    fCommands = newList;
}

/**
 * Returns the number of the Commands
 * 
 * @return boolean
 */
int getSize() {
    int size = 0;
    if (fCommands != null)
        size = fCommands.length;
    return size;
}

/**
 * Retrieves the Commands of the types indicated by the commandTypes array. The
 * order of the commandTypes array has no significance. Passing a null parameter
 * or an empty array retrieves all the available Commands.
 * 
 * @param commandTypes
 * @return Command list
 */
Command[] getCommands(int[] commandTypes) {
    if (commandTypes == null || commandTypes.length == 0) {
        return fCommands;
    }
    int size = getSize();
    Command[] filteredCommands = new Command[size];
    int index = 0;
    for (int i = 0; i < fCommands.length; i++) {
        for (int j = 0; j < commandTypes.length; j++) {
            if (fCommands[i].type == commandTypes[j]) {
                filteredCommands[index] = fCommands[i];
                index++;
                break;
            }
        }
    }
    if (size > (index)) {// Some commands filtered resize the Array
        Command[] shrunk = new Command[index];
        System.arraycopy(filteredCommands, 0, shrunk, 0, index);
        return shrunk;
    }
    return filteredCommands;
}

}// CommandCollection

private CommandCollection currentCommands;
private Command defaultCommand;
private Display display;
Control focusedControl;
Shell lastKnownActiveShell;
private Command[] positiveKeyCommands;
private Command negativeKeyCommand;


public CommandArranger(Display display) {
    super();
    this.display = display;
    currentCommands = new CommandCollection();
}

/**
 * Called when the application changes the QMenuBar. This method does not handle
 * the cases when the QMenuBar may change when the active top-level Shell
 * changes. Since this does not cause a menu bar change on all platforms.
 * 
 * @see org.eclipse.swt.widgets.Decorations#setMenuBar(Menu)
 * 
 **/
public void menuBarChanged(Decorations decorations) {
    if (currentCommands == null || currentCommands.getSize() < 1)
        return;
    // if the changed menu bar is not on the active shell ignore and leave
    // it to focus change.
    if (decorations.getShell() != decorations.getDisplay().getActiveShell())
        return;
    // Call internal_getOwnMenuBar because the menu bar can be set to null
    // in Decorations and
    // we may need to create the internal one.
    int menuBarHandle = decorations.internal_getOwnMenuBar();

    handleMenuBarChanged(menuBarHandle, currentCommands);
}

/**
 * Called when a new Shell becomes active.
 * 
 * @see Control#qt_swt_event_focusWasGained() //TODO
 */
public void shellActivityChanged() {
    Shell activeShell = display.getActiveShell();
    
    if (activeShell == lastKnownActiveShell) {
        return;
    }
    lastKnownActiveShell = activeShell;
    
    cleanPositiveCommands();
    cleanNegativeCommand();
    
    currentCommands = new CommandCollection();

    if (activeShell != null && Command.internal_getCommands(activeShell).length > 0) {
        currentCommands.addCommand(Command.internal_getCommands(activeShell));
    }

    // Determine where the commands go
    if (currentCommands.getSize() > 0) {
        Command[] add = currentCommands.getCommands(null);
        updateCommandPositions(add);
        placePositiveCommands();
        placeNegativeCommand();
    }
}

/**
 * Called when a new Command is created
 * 
 * @param command
 */
public void commandAdded(Command command) {
    if (isInFocusContext(command.control)) {
        currentCommands.addCommand(command);
        handleCommandListChange(command, null, currentCommands);
    }
}

/**
 * Called when a Command is disposed
 * 
 * @param command
 */
public void commandRemoved(Command command) {
    if (command == defaultCommand)
        defaultCommand = null;
    if (isInFocusContext(command.control)) {
        currentCommands.removeCommand(command);
        handleCommandListChange(null, command, currentCommands);
    }
}

/**
 * Called when the Display is getting disposed.
 */
public void dispose() {
    currentCommands = null;
    display = null;
    positiveKeyCommands = null;
    negativeKeyCommand = null;
    defaultCommand = null;
    lastKnownActiveShell = null;
}

/**
 * Called when a Command is set default
 * 
 * @param command
 * @see Command#setDefaultCommand();
 */
public void setDefaultCommand(Command command) {
    defaultCommand = command;
    if (isInFocusContext(command.control)) {
        handleDefaultCommandChange(command);
    }
}

/**
 * Returns the default command or null if there is none.
 * 
 * @return
 * @see Command#isDefaultCommand()
 */
public Command getDefaultCommand() {
    return defaultCommand;
}


private boolean isInFocusContext(Control control) {
    Display display = control.getDisplay();
    Shell activeShell = display.getActiveShell();
    return control == activeShell;
}

private void handleCommandListChange(Command added, Command removed, CommandCollection commands) {
    cleanNegativeCommand();
    cleanPositiveCommands();
    updateCommandPositions(commands.getCommands(null));
    placeNegativeCommand();
    placePositiveCommands();
}

private void handleDefaultCommandChange(Command defaultCommand) {
    this.defaultCommand = defaultCommand;
    cleanPositiveCommands();
    placePositiveCommands();
}

private void updateCommandPositions(Command[] commands) {
    positiveKeyCommands = new Command[commands.length];
    int positiveKeyIndex = 0;
    for (int i = 0; i < commands.length; i++) {
        Command cmd = commands[i];
        if (cmd.isDefaultCommand()) {
            defaultCommand = cmd;
            continue;
        }
        if (CommandUtils.isNegativeType(cmd.type)) {
            if (negativeKeyCommand == null || negativeKeyCommand.isDisposed()) {
                negativeKeyCommand = cmd;
            } else if (negativeKeyCommand.getPriority() <= cmd.getPriority()) {
                positiveKeyCommands[positiveKeyIndex] = negativeKeyCommand;
                positiveKeyIndex++;
                negativeKeyCommand = cmd;
            } else {
                positiveKeyCommands[positiveKeyIndex] = cmd;
                positiveKeyIndex++;
            }
            continue;
        }
        positiveKeyCommands[positiveKeyIndex] = cmd;
        positiveKeyIndex++;
    }
    if ((positiveKeyIndex) < positiveKeyCommands.length) {// needs to shrink
        Command[] rightSized = new Command[positiveKeyIndex];
        System.arraycopy(positiveKeyCommands, 0, rightSized, 0, rightSized.length);
        positiveKeyCommands = rightSized;
    }
}

private void cleanPositiveCommands() {
    boolean useBar = false;
    if ((positiveKeyCommands != null && positiveKeyCommands.length > 1)
        || (defaultCommand != null && positiveKeyCommands != null)) {
        useBar = true;
    }
    if (defaultCommand != null && !defaultCommand.isDisposed()
        && !defaultCommand.control.isDisposed()) {
        if (useBar) {
            OS.QWidget_removeAction(defaultCommand.control.getShell().internal_getOwnMenuBar(),
                topHandle(defaultCommand));
        } else {
            OS.QWidget_removeAction(topHandle(defaultCommand.control), topHandle(defaultCommand));
        }
    }
    if (positiveKeyCommands != null) {
        for (int i = 0; i < positiveKeyCommands.length; i++) {
            Command cmd = positiveKeyCommands[i];
            if (cmd == null || cmd.isDisposed() || cmd.control.isDisposed()) {
                continue;
            }
            int handle = 0;
            if (useBar) {
                handle = cmd.control.getShell().internal_getOwnMenuBar();
            } else {
                handle = topHandle(positiveKeyCommands[0].control);
            }
            OS.QWidget_removeAction(handle, topHandle(cmd));

        }
    }
}

private void cleanNegativeCommand() {
    if (negativeKeyCommand != null && !negativeKeyCommand.isDisposed()
        && !negativeKeyCommand.control.isDisposed()) {
        OS.QWidget_removeAction(topHandle(negativeKeyCommand.control),
            topHandle(negativeKeyCommand));
    }
}

private void placeNegativeCommand() {
    if (negativeKeyCommand != null) {
        OS.QWidget_addAction(Internal_PackageSupport.topHandle(negativeKeyCommand.control),
            topHandle(negativeKeyCommand));
    }
}

private void placePositiveCommands() {
    if (defaultCommand != null) {
        int defaultCmdHandle = topHandle(defaultCommand);
        if (positiveKeyCommands != null) {
            OS.QMenuBar_addAction(defaultCommand.control.getShell().internal_getOwnMenuBar(),
                defaultCmdHandle);
        } else {
            OS.QWidget_addAction(Internal_PackageSupport.topHandle(defaultCommand.control),
                defaultCmdHandle);
        }
    }
    if (positiveKeyCommands != null) {
        if (positiveKeyCommands.length == 1 && defaultCommand == null) {
            OS.QWidget_addAction(Internal_PackageSupport.topHandle(positiveKeyCommands[0].control),
                topHandle(positiveKeyCommands[0]));
        } else {
            CommandUtils.sort(positiveKeyCommands);
            for (int i = 0; i < positiveKeyCommands.length; i++) {
                OS.QMenuBar_addAction(positiveKeyCommands[i].control.getShell()
                    .internal_getOwnMenuBar(), topHandle(positiveKeyCommands[i]));
            }
        }
    }
}

private void handleMenuBarChanged(int newMenuBar, CommandCollection commands) {
    placePositiveCommands();
}

private static final int topHandle(Widget w) {
    return Internal_PackageSupport.topHandle(w);
}

}
