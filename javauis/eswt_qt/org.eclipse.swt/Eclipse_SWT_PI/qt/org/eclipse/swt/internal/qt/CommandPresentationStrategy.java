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
import org.eclipse.swt.internal.qt.CommandArranger.CommandCollection;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
/**
 * This is the interface that all the {@link Command} presentation is delegated to.
 * The implementations of this abstract class are expected to provide the
 * presentation logic for their respective uses.
 *
 * The implementations of this class can be specific to a platform, toolkit or a
 * combination of factors. Refer to {@link CommandArranger} on the loading mechanism
 * of the {@link CommandPresentationStrategy} implementation.
 *
 * Since the implementations can be specific to a toolkit implementations are expected to
 * handle the following.
 * <ul>
 * 	<li>Ordering of the {@link Command}s</li>
 * 	<li>Positioning {@link Command}s to correct soft keys or menus so that they are accessible</li>
 * </ul>
 *
 */
public abstract class CommandPresentationStrategy {


    /**
     * CommandArranger delegates to this method when the currently active menu bar has changed.
     * This does not include the cases where menu bar changes due to active window change which
     * causes the menu bar to change on some platforms.
     *
     * @param newMenuBar
     * @param commands
     */
    protected abstract void handleMenuBarChanged( int newMenuBar, CommandCollection commands );

    /**
     * CommandArranger delegates to this method when the currently focused control has changed.
     * On some platforms a focus change can also be accompanied with a menu bar change. Such menu bar
     * changes due to active window change must be handled here.
     *
     * @param focused-currently focused control
     * @param oldCommands-the commands of the previous focus context
     * @param commands-of the current focus context
     *
     * @see CommandArranger#focusedControlChanged()
     */
    protected abstract void handleFocusChange( Control focused, CommandCollection oldCommands, CommandCollection commands );

    /**
     * CommandArranger delegates to this method, an added or removed Command is in the current
     * focus context and needs to be presented. In other words, this method does not get notification of
     * all added or removed Commands but only those that has some impact on the representation.
     *
     * @param added Command
     * @param removed Command
     * @param commands including the added/removed command
     *
     * @see CommandArranger#commandAdded(Command)
     * @see CommandArranger#commandRemoved(Command)
     */
    protected abstract void handleCommandListChange(Command added, Command removed, CommandCollection commands );

    /**
     * CommandArranger reports the default command changes to this method. However implementations
     * should take into account default commands on focus changes.
     *
     * @param defaultCommand
     */
    protected abstract void handleDefaultCommandChange( Command defaultCommand );
    /**
     * This method is called when the CommandArranger is disposed as part of
     * the disposal of the {@link Display} or
     * if it is replaced by a new one on the CommandArranger.
     *
     * On Display dispose, the method is called after all the Shells are disposed but before the Display is cleaned.
     *
     * @see CommandArranger#setPresentationStrategy(CommandPresentationStrategy)
     */
    protected abstract void dispose();

}
