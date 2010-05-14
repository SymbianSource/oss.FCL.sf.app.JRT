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
/**
 * 
 */
package com.nokia.microedition.media.animation;

import javax.microedition.media.Player;

import com.nokia.microedition.media.control.ControlImpl;

/**
 * @author d35kumar
 *
 */
public class StopTimeControl extends ControlImpl implements
		javax.microedition.media.control.StopTimeControl {

	long iStopTime=RESET;
	/**
	 * 
	 */
	public StopTimeControl(Player aPlayer) {
		this.iPlayer=aPlayer;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.StopTimeControl#getStopTime()
	 */
	public long getStopTime() {
		 checkState();
		return iStopTime;
	}

	/* (non-Javadoc)
	 * @see javax.microedition.media.control.StopTimeControl#setStopTime(long)
	 */
	public void setStopTime(long aStopTime) {
		checkState();
		if (iPlayer.getState() == Player.STARTED && getStopTime() != RESET) {
			throw new IllegalStateException(
					"Player is STARTED or setStopTime() is already called successfully");
		}
		iStopTime = aStopTime;
	}
}
