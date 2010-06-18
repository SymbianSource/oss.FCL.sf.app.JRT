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
import org.eclipse.swt.internal.CommandPresentationStrategyWrapper;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Decorations;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Menu;
import org.eclipse.swt.widgets.Shell;
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
    Control focusedControl;

    /**
     * A helper for keeping an ordered list of {@link Command}s in the current focus context.
     * The Commands are ordered according to their proximity to the currently focused control.
     * The ordered list starts with Commands from the currently focused Control if any and ends with
     * the active Shell and includes all the Commands in between. If a Control has more
     * than one Command they are ordered among themselves according to creation order.
     */
    public class CommandCollection{

        private Command[] fCommands;

        CommandCollection(){
            super();
        }

        /**
         * Adds the command to the collection.
         * Warning: This does not make duplicate control.
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
            System.arraycopy(fCommands, insertPoint, newList, insertPoint + 1,
                    fCommands.length - insertPoint);
            newList[insertPoint] = command;
            fCommands = newList;
        }

        /**
         * Adds the list of Commands to the collection.
         * @param commands
         */
        void addCommand( Command[] commands ){
            if (commands == null || commands.length == 0 ) return;
            if (fCommands == null ){
                fCommands = new Command[ commands.length ];
                System.arraycopy(commands, 0, fCommands, 0, commands.length );
                return;
            }
            int size = commands.length + fCommands.length;
            Command[] newList = new Command[ size ];
            System.arraycopy(fCommands, 0, newList, 0, fCommands.length);
            System.arraycopy(commands, 0, newList, fCommands.length , commands.length );
            fCommands = newList;
        }

        /**
         * Removes the command from the list.
         * @param command
         */
        void removeCommand( Command command ){
            if ( command == null ) return;
            if (fCommands == null || fCommands.length == 0 ) return;
            int removeIndex = -1;
            for (int i = 0; i < fCommands.length; i++) {
                if(fCommands[i] == command ){
                    removeIndex = i;
                }
            }
            if ( removeIndex == -1 ) return;
            Command[] newList = new Command[ fCommands.length -1 ];
            System.arraycopy( fCommands, 0, newList, 0, removeIndex);
            System.arraycopy( fCommands, removeIndex + 1, newList, removeIndex, (fCommands.length - removeIndex -1) );
            fCommands = newList;
        }

        /**
         * Returns the number of the Commands
         *
         * @return boolean
         */
        public int getSize(){
            int size = 0;
            if (fCommands != null ) size = fCommands.length;
            return size;
        }

        /**
         * Retrieves the Commands of the types indicated by the commandTypes array.
         * The order of the commandTypes array has no significance. Passing a null parameter
         * or an empty array retrieves all the available Commands.
         *
         * @param commandTypes
         * @return Command list
         */
        public Command[] getCommands( int[] commandTypes ){
            if ( commandTypes == null || commandTypes.length == 0 ){
                return fCommands;
            }
            int size = getSize();
            Command[] filteredCommands = new Command[size];
            int index = 0;
            for (int i = 0; i < fCommands.length ; i++) {
                for(int j = 0; j<commandTypes.length; j++){
                    if ( fCommands[i].type == commandTypes[j] ){
                        filteredCommands[index]=fCommands[i];
                        index++;
                        break;
                    }
                }
            }
            if( size > (index) ){// Some commands filtered resize the Array
                Command[] shrunk = new Command[index];
                System.arraycopy( filteredCommands, 0, shrunk, 0, index);
                return shrunk;
            }
            return filteredCommands;
        }
    }// CommandCollection

    private CommandCollection currentCommands;
    private CommandPresentationStrategy strategy;
    private Command defaultCommand;
    private Display display;

    public CommandArranger(Display display){
        super();
        this.display = display;
        currentCommands = new CommandCollection();
        strategy = CommandPresentationStrategyWrapper.createStrategy();
    }



    /**
     * Called when the application changes the QMenuBar.
     * This method does not handle the cases when the QMenuBar
     * may change when the active top-level Shell changes. Since
     * this does not cause a menu bar change on all platforms.
     *
     * @see org.eclipse.swt.widgets.Decorations#setMenuBar(Menu)
     *
     **/
    public void menuBarChanged( Decorations decorations ){

        if ( currentCommands == null || currentCommands.getSize() < 1 ) return;
        // if the changed menu bar is not on the active shell ignore and leave it to focus change.
        if (decorations.getShell() != decorations.getDisplay().getActiveShell() )return;
        // Call internal_getOwnMenuBar because the menu bar can be set to null in Decorations and
        // we may need to create the internal one.
        int menuBarHandle = decorations.internal_getOwnMenuBar();

        strategy.handleMenuBarChanged( menuBarHandle, currentCommands );
    }

    /**
     * Called when a new Control gains focus.
     *
     * @see Control#qt_swt_event_focusWasGained()
     */
    public void focusedControlChanged(){
        Control focusControl = display.getFocusControl();
        if (focusControl == focusedControl) {
            return;
        }
        focusedControl = focusControl;

        Shell activeShell = display.getActiveShell();
        CommandCollection oldCollection = currentCommands;
        currentCommands = new CommandCollection();

        Control ctrl = focusControl;
        while ( ctrl!= null && ctrl != activeShell ){
            if ( Command.internal_getCommands(ctrl).length > 0 ){
                currentCommands.addCommand( Command.internal_getCommands(ctrl) );
            }
            ctrl = ctrl.getParent();
        }
        if (activeShell != null && Command.internal_getCommands(activeShell).length > 0 ){
            currentCommands.addCommand( Command.internal_getCommands(activeShell) );
        }

        if (strategy != null) {
            strategy.handleFocusChange(focusControl, oldCollection, currentCommands);
        }
    }

    /**
     * Called when a new Command is created
     * @param command
     */
    public void commandAdded( Command command ){
        if( isInFocusContext(command.control)){
            currentCommands.addCommand(command);
            strategy.handleCommandListChange(command, null, currentCommands);
        }
    }

    /**
     * Called when a Command is disposed
     * @param command
     */
    public void commandRemoved( Command command ){
        if ( command == defaultCommand ) defaultCommand = null;
        if (isInFocusContext(command.control)){
            currentCommands.removeCommand( command );
            strategy.handleCommandListChange(null, command, currentCommands);
        }
    }

    /**
     * Called when the Display is getting disposed.
     */
    public void dispose(){
        strategy.dispose();
        strategy = null;
        currentCommands = null;
        display = null;
    }
    /**
     * Called when a Command is set default
     *
     * @param command
     * @see Command#setDefaultCommand();
     */
    public void setDefaultCommand(Command command ){
        defaultCommand = command;
        if(isInFocusContext(command.control)){
            strategy.handleDefaultCommandChange(command);
        }
    }

    /**
     * Returns the default command or null if there is none.
     *
     * @return
     * @see Command#isDefaultCommand()
     */
    public Command getDefaultCommand(){
        return defaultCommand;
    }

    /**
     * Sets a new {@link CommandPresentationStrategy}. It also disposes to old one.
     *
     * @param commandPresentationStrategy
     */
    public void setPresentationStrategy( CommandPresentationStrategy commandPresentationStrategy ){
        CommandPresentationStrategy oldStrategy = strategy;
        strategy = commandPresentationStrategy;
        if (oldStrategy != null ){
            oldStrategy.dispose();
        }
    }

    private boolean isInFocusContext(Control control){
        Display display = control.getDisplay();
        Shell activeShell = display.getActiveShell();
        Control focused = display.getFocusControl();
        if( focused == null )return false;
        if(control.getShell() != activeShell) return false;
        if( control == focused ) return true;
        return isFocusAncestor(focused, control);
    }

    private boolean isFocusAncestor(Control node, Control parent){
        Control nodeParent = node.getParent();
        if( nodeParent == parent )return true;
        if (nodeParent == null || nodeParent == node.getShell() )return false;
        return isFocusAncestor(nodeParent, parent);
    }

}
