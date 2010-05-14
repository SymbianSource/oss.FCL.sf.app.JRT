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
* Description: Control for routing the player's audio output.
*
*/

package com.nokia.mid.media;

/**
 * <code>AudioOutputControl</code> is an interface for setting application 
 * preferred audio output mode.
 * <p>
 * <strong><u>SYMBIAN SPECIFIC BEHAVIOR:</u></strong> <BR><BR>
 * Audio output mode is within the scope of a player instance and can be set
 * when the MIDlet which set the audio routing is finished with the audio
 * when the player is in REALIZED, PREFETCHED, STARTED or STOPPED state. 
 * The preferences are applied or imposed when media is played. <BR><BR>
 * Audio output control is retrieved using 'Player' interface by calling 
 * <code>Control getControl ( java.lang.string controlType ) </code> 
 * The control type to be passed as an argument is 
 * <code>com.nokia.mid.media.AudioOutputControl</code>
 * <p>
 * <strong><u>S40 SPECIFIC BEHAVIOR:</u></strong> <BR><BR>
 * The AudioOutputControl is a Global Control and will effect all players.
 * When the MIDlet which set the audio routing is finished with the audio
 * change then it must set it back to DEFAULT.
 * <p>
 * Audio output control is retrieved using 'GlobalManager' interface by calling 
 * <code>Control getControl ( java.lang.string controlType ) </code> 
 * The control type to be passed as an argument is 
 * <code>com.nokia.mid.media.AudioOutputControl</code>
 * <p>
 * <strong><u>Code Snippet:</u></strong><BR> 
 * <BR>
 * <strong><u>1. Getting AudioOutputControl:</u></strong><BR><BR>
 * <strong><u>In SYMBIAN:</u></strong> <BR><BR>
 * <code>Player p =  
 *         Manager.createPlayer(getClass().getResourceAsStream
 *         ("nokia_tune.mid"), "audio/midi"); <BR>
 * p.realize(); <BR> 
 * AudioOutputControl iAudioOutputControl=  
 * (AudioOutputControl)p.getControl("com.nokia.mid.media.AudioOutputControl");<BR>
 * p.addPlayerListener(this); </code> <BR> <BR>
 * <strong><u>In S40:</u></strong> <BR><BR>
 * <code>import javax.microedition.amms.*; <BR> 
 * iAudioOutputControl = (AudioOutputControl) GlobalManager.getControl("com.nokia.mid.media.AudioOutputControl"); <BR>
 * </code> <BR> <BR>
 * <strong><u>2. Setting and getting Audio Output Mode:</u></strong> <BR><BR>
 * <strong><u>2.1 Setting PRIVATE mode:</u></strong> <BR><BR>
 * <code>iAudioOutputControl.setOutputMode(PRIVATE);</code><BR> <BR>
 * <strong><u>2.2 Getting Current Mode:</u></strong> <BR><BR> 
 * <code>AudioOutput audioOutput = iAudioOutputControl.getCurrent();</code><BR>
 * <code>int mode = audioOutput.getOutputMode();</code> <BR><BR>
 * <strong><u>3. Getting notification:</u></strong> <BR><BR>
 * Notifications are supported only on SYMBIAN. <BR>
 * The notification about the preference change is done using 
 * PlayerListener's playerUpdate() function. 
 * Whereas, this specific Event type is "com.nokia.audiooutputchange.event" 
 * and Event data is AudioOutput object. <BR><BR>
 * <code> 
 * public void playerUpdate(Player aPlayer, String aEvent, Object aEventData){<BR>
 * <DD>if(aEvent.equals("com.nokia.audiooutputchange.event")){ </DD>
 *     <DD><DD>helloForm.append("com.nokia.audiooutputchange.event"); </DD></DD>
 *     <DD><DD>// get the mode </DD></DD>
 *     <DD><DD>Int mode = ((AudioOutput)aEventData).getActiveOutputMode();</DD></DD>
 *   <DD>}</DD><BR>
 * } </code> <BR>
 * <p>
 * <p>
 * @since 1.4
 * <p>   
 */


public interface AudioOutputControl extends javax.microedition.media.Control
{
   /** 
    * The DEFAULT audio output mode.   
    * <p>
    * No Preference. The audio routing is restored to the default value.
    * <p>
    * Value 0 is assigned to <code>DEFAULT</code>.
    * <p>
	* <strong><u>SYMBIAN SPECIFIC BEHAVIOR:</u></strong> <BR>
	* Audio is by default routed to loudspeaker. 
    * Audio output changes dynamically to headset, when headset is connected to 
    * the device and vice versa. Application is notified about audio output 
    * change with audio output change event.     
    * <p> 
   */
    public static final int DEFAULT = 0;    
    
   /** 
    * The ALL audio output mode.   
    * <p>
    * Audio is routed to one public and one private output at the same time. 
    * The primary output method within public and private outputs is selected 
    * by the device and can change dynamically when external peripherals are 
    * connected or disconnected by the user (see "public" and "private" for  
    * their priorities). 
    * <p>
    * Value 1 is assigned to <code>ALL</code>.
   */    
    public static final int ALL = 1;
    
   /** 
    * The NONE audio output mode.   
    * <p>
    * Audio is not routed to any output. 
    * <p>
    * Value 2 is assigned to <code>NONE</code>.
   */     
    public static final int NONE = 2;
    
   /** 
    * The PRIVATE audio output mode.   
    * <p>
    * Audio is routed to earpiece or wired / wireless(BT) headset if connected.
    * If there are several private output methods available at the same time 
    * audio is routed to only one of those selected with following priority 
     * order 1) wired headset 2) wireless headset 3) earpiece.
    * <p>
    * Value 3 is assigned to <code>PRIVATE</code>.
   */     
    public static final int PRIVATE = 3;
  
    /** 
     * The PUBLIC audio output mode.
     * <p>
     * Audio is routed to loudspeaker, wireless(BT) speakers or TV output. If
     * there are several public output methods available at the same time
     * audio is routed to only one of those selected with following priority
     * order: (1) wired headset 2) Loudspeaker
     * <p>
     * Value 4 is assigned to <code>PUBLIC</code>.
     */    
    public static final int PUBLIC = 4;    

    /**
     * Get all supported audio output modes
     * <p>
     * @return The currently supported output modes
     * <p>
     */
    int[] getAvailableOutputModes();

    /**
     * Get the currently set audio output mode. This is the last audioOutputMode
     * sent to setOutputMode or DEFAULT if none has been set.
     * <p>
     * This does not return the actual audio output mode of the device. The
     * AudioOutput can be used to provide the actual audio output mode.
     *
     * @return The last audio output mode which was sent to setOutputMode.
     */
    int getOutputMode();

    /**
     * returns an AudioOutput object which consists
     * Device Type and the mode of the audio output
     */
    com.nokia.mid.media.AudioOutput getCurrent();
	
    /**
     * Sets the preferred audio output mode.
     * <p>
	 * 
	 * @return audio output mode set or -1 if unable to set
	 * 
     * @exception IllegalArgumentException with message "audio output mode is
     * not valid" when the audio output mode is not valid
     *
     * @exception IllegalStateException with message "Accessory connected" if
     * unable to change the audio routing due to an accessory connected
     *
     * @exception SecurityException with message "Permission denied" if the
     * called does not have permission to set the audio routing.	 
     * <p>
	 * <strong><u>SYMBIAN SPECIFIC BEHAVIOR:</u></strong> <BR>
	 * This effects only a specific player instance. 
     * Preference can be set when Player is in REALIZED, PREFETCHED, 
     * STARTED or STOPPED state.
     * <p>
     * <strong><u>S40 SPECIFIC BEHAVIOR:</u></strong> <BR>
	 * This will effect all players	 
     */      
    int setOutputMode(int mode);

}

