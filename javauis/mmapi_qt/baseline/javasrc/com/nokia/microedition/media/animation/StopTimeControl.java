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
