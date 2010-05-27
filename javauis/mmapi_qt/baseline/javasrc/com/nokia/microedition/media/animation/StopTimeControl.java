/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: StopTimeControl
*
*/

package com.nokia.microedition.media.animation;

import javax.microedition.media.Player;

import com.nokia.microedition.media.control.ControlImpl;


/**
 * <code>StopTimeControl</code> allows one to specify a preset stop time for
 * a <code>Player</code>.
 * <p>
 */
public class StopTimeControl extends ControlImpl implements
		javax.microedition.media.control.StopTimeControl {

	private long iStopTime=RESET;
	/**
	 * 
	 */
	public StopTimeControl(Player aPlayer) {
		this.iPlayer=aPlayer;
	}

	/**
     * Gets the last value successfully set by <CODE>setStopTime</CODE>.
     *
     * Returns the constant <CODE>RESET</CODE> if no stop time is set.
     * This is the default.
     *
     * @return The current stop time in microseconds.
     * @see #setStopTime
     */
	public long getStopTime() {
		 checkState();
		return iStopTime;
	}

	 /**
    *
    * Sets the <i>media time</i> at which you want the <code>Player</code>
    * to stop.
    * The <code>Player</code> will stop when its <i>media time</i>
    * reaches the stop-time.
    * A <code>STOPPED_AT_TIME</code> event
    * will be delivered through the <code>PlayerListener</code>.
    * <p>
    * The <code>Player</code> is guaranteed
    * to stop within one second past the preset stop-time
    * (i.e. <code>stop-time <= current-media-time <= stop-time + 1 sec.</code>);
    * unless the current media time is already passed the preset stop time
    * when the stop time is set.
    * If the current media time is already past the stop time set,
    * the <code>Player</code> will stop immediately.  A
    * <code>STOPPED_AT_TIME</code> event will be delivered.
    * After the <code>Player</code> stops due to the stop-time set,
    * the previously set stop-time will be cleared automatically.
    * Alternatively, the stop time can be explicitly removed by
    * setting it to: <code>RESET</code>.
    * <p>
    *
    * You can always call <code>setStopTime</code> on a stopped
    * <code>Player</code>.
    * To avoid a potential race condition, it is illegal to
    * call <code>setStopTime</code> on a started <code>Player</code> if a
    * <i>media stop-time</i> has already been set.
    *
    * @param aStopTime The time in microseconds at which you want the
    * <code>Player</code> to stop, in <i>media time</i>.
    * @exception IllegalStateException Thrown if
    * <code>aStopTime</code> is called on a started
    * <code>Player</code> and the
    * <i>media stop-time</i> has already been set.
    * @see #getStopTime
    */
	public void setStopTime(long aStopTime) {
		checkState();
		if (iPlayer.getState() == Player.STARTED && getStopTime() != RESET) {
			throw new IllegalStateException(
					"Player is STARTED or setStopTime() is already called successfully");
		}
		iStopTime = aStopTime;
		((AnimationPlayer)iPlayer).setiStopTime(iStopTime);
	}
}
