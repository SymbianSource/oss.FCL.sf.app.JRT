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
package javax.microedition.lcdui;

import org.eclipse.swt.graphics.Point;

/**
 * Class representing the Spacer item.
 */
public class Spacer extends Item {

    private int minimumWidth = -1;
    private int minimumHeight = -1;

    /**
     * Constructor.
     * 
     * @param minW minimum width of the Spacer.
     * @param minH minimum height of the Spacer.
     */
    public Spacer(int minW, int minH) {
        setMinimumSize(minW, minH);
    }

    /**
     * Overrides setLabel in Item class.
     * 
     * @param newLabel Spacer cannot have any label.
     */
    public void setLabel(String newLabel) {
        throw new IllegalStateException(
                MsgRepository.SPACER_EXCEPTION_INVALID_STATE_LABEL);
    }

    /**
     * Overrides method in Item class.
     * 
     * @param cmd Spacer cannot have command associated with.
     */
    public void addCommand(Command cmd) {
        throw new IllegalStateException(
                MsgRepository.SPACER_EXCEPTION_INVALID_STATE_COMMAND);
    }

    /**
     * Overrides method in Item class.
     * 
     * @param newCmd Spacer cannot have command associated with.
     */
    public void setDefaultCommand(Command newCmd) {
        throw new IllegalStateException(
                MsgRepository.SPACER_EXCEPTION_INVALID_STATE_COMMAND);
    }

    /**
     * Set minimum size for a spacer.
     * 
     * @param minW new minimum width of spacer.
     * @param minH new minimum height of spacer.
     */
    public void setMinimumSize(int minW, int minH) {
        if (minW < 0 || minH < 0) {
            throw new IllegalArgumentException(
                    MsgRepository.SPACER_EXCEPTION_INVALID_WIDTH_HEIGHT);
        }
        else {
            int updateReason = Item.UPDATE_NONE;
            if (minW != getMinimumWidth()) {
                minimumWidth = minW;
                updateReason |= Item.UPDATE_WIDTH_CHANGED;
            }
            if (minH != getMinimumHeight()) {
                minimumHeight = minH;
                updateReason |= Item.UPDATE_HEIGHT_CHANGED;
            }
            updateParent(updateReason);
        }
    }

    /**
     * Calculates minimum size of this item.
     * 
     * @return Minimum size.
     */
    Point calculateMinimumSize() {
        return new Point(minimumWidth, minimumHeight);
    }

    /**
     * Calculates preferred size of this item.
     * 
     * @return Preferred size.
     */
    Point calculatePreferredSize() {
        return new Point(minimumWidth, minimumHeight);
    }

}
