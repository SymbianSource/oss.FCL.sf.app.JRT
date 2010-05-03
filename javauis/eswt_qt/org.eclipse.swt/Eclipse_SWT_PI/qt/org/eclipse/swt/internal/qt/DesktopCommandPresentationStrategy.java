/*******************************************************************************
 * Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
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
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;

public class DesktopCommandPresentationStrategy extends
        CommandPresentationStrategy {

    private int commandsMenu;
    private int commandsMenuAction;
    private Control previousFocus;

    public DesktopCommandPresentationStrategy(){
        super();
        initCommandsMenu();
    }

    protected void handleCommandListChange(Command added, Command removed,
            CommandCollection commands) {

        Shell shell = null;
        if ( added != null ){
            shell = added.control.getShell();
            OS.QMenu_addAction(commandsMenu, handle(added) );
        }
        else{
            shell = removed.control.getShell();
            OS.QWidget_removeAction(commandsMenu, handle(removed) );
        }

        OS.QMenuBar_addAction(shell.internal_getOwnMenuBar(), commandsMenuAction );
    }

    protected void handleFocusChange(Control focused,
            CommandCollection oldCommands, CommandCollection commands) {
        Shell shell = focused.getShell();

        if( oldCommands != null && oldCommands.getSize() >0 ){
            Command[] remove = oldCommands.getCommands(null);
            for(int i=0; i< remove.length; i++){
                if (previousFocus!= null && remove[i].control == previousFocus){
                    OS.QWidget_removeAction(handle(previousFocus), handle(remove[i]));
                }else{
                    OS.QWidget_removeAction(commandsMenu, handle(remove[i]));
                }
            }
        }

        if ( commands != null && commands.getSize() > 0 ){
            OS.QMenuBar_addAction(shell.internal_getOwnMenuBar(), commandsMenuAction );
            Command[] add = commands.getCommands(null);
            CommandUtils.sort(add);
            for(int i=0 ; i < add.length; i++){
                if ( add[i].control != focused ){
                    OS.QMenu_addAction( commandsMenu, handle(add[i]) );
                }
                else{
                    OS.QWidget_addAction(handle(focused), handle(add[i]));
                }
                if (add[i].isDefaultCommand() )OS.QMenu_setDefaultAction( commandsMenu, handle(add[i]) );
            }
        }
        //Store for use next time.
        previousFocus=focused;
    }

    protected void handleMenuBarChanged(int newMenuBar,
            CommandCollection commands) {
        OS.QMenuBar_addAction(newMenuBar, commandsMenuAction );
    }


    protected void dispose() {
        if (commandsMenu != 0 ){
            QObjectDeleteWrapper.deleteSafely( commandsMenu );
            commandsMenu = 0;
        }
        if (commandsMenuAction != 0 ){
            QObjectDeleteWrapper.deleteSafely( commandsMenuAction );
            commandsMenuAction = 0;
        }
    }

    private void initCommandsMenu (){
        commandsMenuAction = OS.QAction_new( 0 );
        OS.QAction_setText( commandsMenuAction, "Commands");
        commandsMenu = OS.QMenu_new( 0 );
        OS.QAction_setMenu( commandsMenuAction, commandsMenu );
    }

    protected void handleDefaultCommandChange(Command defaultCommand) {
        int handle = 0;
        if (defaultCommand != null ) handle = handle(defaultCommand);
        OS.QMenu_setDefaultAction( commandsMenu, handle );
    }


    private final int handle(Widget w) {
        return Internal_PackageSupport.handle(w);
    }
}
