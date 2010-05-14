/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.media;

import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;
import java.util.Enumeration;
import javax.microedition.media.protocol.DataSource;
import javax.microedition.media.TimeBase;
import javax.microedition.media.Player;
import javax.microedition.media.MediaException;
//import com.symbian.lcdjava.lang.ObjectExtensions;
//import com.symbian.epoc.events.MIDEventServer;
//import com.symbian.midp.runtime.ToolkitInvoker;
//import com.symbian.midp.runtime.ToolkitObserver;

import com.nokia.microedition.media.animation.AnimationPlayerFactory;
import com.nokia.microedition.media.protocol.ProtocolFactory;
import com.nokia.microedition.media.tone.PlayToneImpl;

import com.nokia.microedition.volumekeys.ForegroundListener;
import com.nokia.mj.impl.rt.support.Finalizer;

//To get the shutdown event from the midlet
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.utils.Logger;

/**
 * ManagerImpl implements the functionality specified in
 * javax.microedition.media.Manager class.
 * This class is a singleton and instance can be got with getInstance method.
 */
public class ManagerImpl implements PlugIn
{
    // ManagerImpl native instance
    private static int sManagerHandle;

    // CMMAEventSource
    private static int iFunctionSourceHandle;

    // Static instance, can be got with getInstace method
    private static ManagerImpl sManager;

    private final TimeBase iSystemTimeBase = new SystemTimeBase();

    private final Vector iPlugIns = new Vector();
    private final ProtocolFactory iProtocolFactory = new ProtocolFactory();
    private final ForegroundListener iForegroundListener;
    private Finalizer iFinalizer;

    Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
        public void finalizeImpl()
        {
            doFinalize();
        }
    };
    }

    // Play tone implementation
    private PlayToneImpl iPlayToneImpl = new PlayToneImpl();

    static
    {
        // This is called when class is loaded for the first time
        sManager = new ManagerImpl();

        try
        {
            // Invoke external components
            Setup.setup(iFunctionSourceHandle);
        }
        catch (OutOfMemoryError oome)
        {
            // External setup failed clean MMA native resources and throw oome
            sManager.doFinalize();
            throw oome;
        }
    }

    /**
     * This private constructor can be called only from staic block.
     */
    private ManagerImpl()
    {
        // Will cause registeredFinalize() to be called when ObjectExtensions
        // is finalized.
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"ManagerImpl constructor before loading library");
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javamobilemedia");
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"ManagerImpl constructor library loaded.. going to register for finalizer");
        //ObjectExtensions.registerForFinalization(this);
        iFinalizer = registerForFinalization();
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"ManagerImpl constructor ... after registering for finalizer");

        // Event server contructor needs new String object,
        // otherwise it don't work..

        iFunctionSourceHandle = _createEventSource();
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"ManagerImpl.java : iFunctionSourceHandle is " + iFunctionSourceHandle);
        if (iFunctionSourceHandle < NativeError.KErrNone)
        {
            throw new OutOfMemoryError("SymbianOS error: " + iFunctionSourceHandle);
        }

        sManagerHandle = _createManager(iFunctionSourceHandle,
                                        ApplicationInfo.getInstance().getSuiteUid().hashCode());
        if (sManagerHandle < NativeError.KErrNone)
        {
            throw new OutOfMemoryError();
        }
        //Use ShutdownListener to get notification of exit and release the resource
		//MMAPI UI 3.x work

		setShutdownListener();
		// support for gif animation player
		iPlugIns
				.addElement(new com.nokia.microedition.media.animation.AnimationPlayerFactory());

        // ManagerImpl is also a PlugIn that getAllSupportedContentTypes,
        // getAllSupportedProtocols and createPlayer methods can be used
        // through PlugIn interface.
        iPlugIns.addElement(this);

        // support for device://tone and jts
        iPlugIns.addElement(
            new com.nokia.microedition.media.protocol.device.tone.Protocol());

        // Create foreground listener which listens the state of the midlet
        // This feature is a part of the media keys feature so it is flagged
         Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"before constructing ForegroundListener....");
         iForegroundListener = new ForegroundListener(iFunctionSourceHandle);
        iForegroundListener.init();
    }
    
    
    private void setShutdownListener()
    {
        // Get the insatnce of ApplicationUtils.
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get the name of the application.
        appUtils.addShutdownListener(new ShutdownListener()
        {
            //The method that gets called when Application is shutting down
            public void shuttingDown()
            {
               
                doFinalize();
              
             
            }
        });
        
    }    

    /**
     * Returns MMA event source handle
     */
    public static int getEventSource()
    {
        return iFunctionSourceHandle;
    }
    /**
     * Returns native handle to Manager
     */
    public static int getHandle()
    {
        return sManagerHandle;
    }

    /**
     * Return ManagerImpl instance
     */
    public static ManagerImpl getInstance()
    {
        return sManager;
    }

    /**
     * Adds new PlugIn to PlugIns array.
     * @param aPlugIn New PlugIn.
     */
    public void addPlugIn(PlugIn aPlugIn)
    {
        iPlugIns.addElement(aPlugIn);
    }




    /**
     * This will be called when ObjectExtensions is finalized.
     */
    synchronized final void doFinalize()
    {
	_dispose(iFunctionSourceHandle);
	iFunctionSourceHandle = 0;
    }

    /**
     * This method is called in Toolkit's destroyNotify call.
     * This will release convenient native resources. All native resource
     * will be deleted in registeredFinalize() method.
     */
    synchronized final void release()
    {
     //   _release(iFunctionSourceHandle);
    }

    /**
     * Create String array from Vector and remove String duplicates.
     * @param aVector Vector containing String objects.
     */
    private String[] createStringArray(Vector aVector)
    {
        // remove all duplicates from the vector
        for (int i = 0; i < aVector.size(); i++)
        {
            String element = (String)aVector.elementAt(i);
            for (int j = i + 1; j < aVector.size();)
            {
                if (element.equals((String)aVector.elementAt(j)))
                {
                    aVector.removeElementAt(j);
                }
                else
                {
                    j++;
                }
            }
        }

        // Create new array for vector elements and copy elements
        String[] s = new String[ aVector.size()];
        aVector.copyInto(s);
        return s;
    }

    /**
     * Return the list of supported content types for the given protocol.
     * <p>
     * See <a href="#content-type">content types</a> for the syntax
     * of the content types returned.
     * See <a href="#media-protocol">protocol name</a> for the syntax
     * of the protocol used.
     * <p>
     * For example, if the given <code>protocol</code>
     * is <code>"http"</code>,
     * then the supported content types that can be played back
     * with the <code>http</code> protocol will be returned.
     * <p>
     * If <code>null</code> is passed in as the <code>protocol</code>,
     * all the supported content types for this implementation
     * will be returned.  The returned array must be non-empty.
     * <p>
     * If the given <code>protocol</code> is an invalid or
     * unsupported protocol, then an empty array will be returned.
     *
     * @param aProtocol The input protocol for the supported content types.
     * @return The list of supported content types for the given protocol.
     */
    public String[] getAllSupportedContentTypes(String aProtocol)
    {
        if ((aProtocol != null) && (aProtocol.length() == 0))
        {
            // No supported types for 0 length string.
            return new String[ 0 ];
        }
        Vector contentTypes = new Vector();

        Enumeration plugIns = iPlugIns.elements();

        // go through all plugins and get content types
        while (plugIns.hasMoreElements())
        {
            PlugIn plugIn = (PlugIn)plugIns.nextElement();
            String[] types = plugIn.getSupportedContentTypes(aProtocol);

            // Add all types to vector
            for (int i = 0; i < types.length; i++)
            {
                contentTypes.addElement(types[ i ]);
            }
        }
        return createStringArray(contentTypes);
    }

    /**
     * Return the list of supported protocols given the content
     * type.  The protocols are returned
     * as strings which identify what locators can be used for creating
     * <code>Player</code>'s.
     * <p>
     * See <a href="#media-protocol">protocol name</a> for the syntax
     * of the protocols returned.
     * See <a href="#content-type">content types</a> for the syntax
     * of the content type used.
     * <p>
     * For example, if the given <code>content_type</code>
     * is <code>"audio/x-wav"</code>, then the supported protocols
     * that can be used to play back <code>audio/x-wav</code>
     * will be returned.
     * <p>
     * If <code>null</code> is passed in as the
     * <code>content_type</code>,
     * all the supported protocols for this implementation
     * will be returned.  The returned array must be non-empty.
     * <p>
     * If the given <code>content_type</code> is an invalid or
     * unsupported content type, then an empty array will be returned.
     *
     * @param aContentType The content type for the supported protocols.
     * @return The list of supported protocols for the given content type.
     */
    public String[] getAllSupportedProtocols(String aContentType)
    {
        String contentType = aContentType;
        if ((contentType != null) &&  contentType.equals(""))
        {
            return new String[ 0 ];
        }

        Vector protocols = new Vector();
        Enumeration plugIns = iPlugIns.elements();
        while (plugIns.hasMoreElements())
        {
            PlugIn plugIn = (PlugIn)plugIns.nextElement();
            String[] types = plugIn.getSupportedProtocols(aContentType);
            for (int i = 0; i < types.length; i++)
            {
                protocols.addElement(types[ i ]);
            }
        }
        return createStringArray(protocols);
    }

    /**
     * From PlugIn. Get MMA supported protocols.
     */
    public String[] getSupportedProtocols(String aContentType)
    {
        String[] protocols = _getSupportedProtocols(iFunctionSourceHandle,
                             sManagerHandle,
                             aContentType);
        NativeError.checkOOM(protocols);
        return protocols;
    }

    /**
     * From PlugIn. Get MMA supported types.
     */
    public String[] getSupportedContentTypes(String aProtocol)
    {
        String[] types = _getSupportedContentTypes(iFunctionSourceHandle,
                         sManagerHandle,
                         aProtocol);
        NativeError.checkOOM(types);
        return types;
    }

    /**
     * From PlugIn.
     */
    public InternalPlayer createPlayer(DataSource aSource)
    throws MediaException, IOException
    {
        InternalPlayer player = null;
        if (aSource.getContentType() != null)
        {
            // Create player from content type
            if (isValidContentType(aSource.getContentType()))
            {
                player = NativePlayerFactory.createPlayer(aSource.getContentType(),
                         aSource);
            }
            else
            {
                throw new MediaException(
                    "Content type not supported: " + aSource.getContentType());
            }
        }

        if ((player == null) &&
                (aSource.getLocator() != null))
        {
            // Create player from locator
            player = NativePlayerFactory.createPlayer(
                         new Locator(aSource.getLocator()),
                         aSource);
        }

        if (player == null)
        {
            // Could not create player from content-type or locator,
            // try to create player from header data
            player = NativePlayerFactory.createPlayer(
                         ((BufferDataSource)aSource).getHeader(),
                         aSource);
        }

        return player;
    }

    /**
     * From PlugIn. Empty implemation.
     */
    public void preparePlayer(InternalPlayer aPlayer) throws MediaException
    {
    }

    /**
     * This method calls preparePlayer to all PlugIns.
     */
    private void pluginsPreparePlayer(InternalPlayer aPlayer)
    throws MediaException
    {
    	
    		
        // Call preparePlayer to all plugins
        Enumeration plugins = iPlugIns.elements();
        while (plugins.hasMoreElements())
        {
            ((PlugIn)plugins.nextElement()).preparePlayer(aPlayer);
        }
    }

    /**
     * Create a <code>Player</code> from an input locator.
     *
     * @param aLocator A locator string in URI syntax that describes
     * the media content.
     * @return A new <code>Player</code>.
     * @exception IllegalArgumentException Thrown if <code>locator</code>
     * is <code>null</code>.
     * @exception MediaException Thrown if a <code>Player</code> cannot
     * be created for the given locator.
     * @exception IOException Thrown if there was a problem connecting
     * with the source pointed to by the <code>locator</code>.
     * @exception SecurityException Thrown if the caller does not
     * have security permission to create the <code>Player</code>.
     */
    public Player createPlayer(String aLocator)
    throws IOException, MediaException
    {
        if (aLocator == null)
        {
            throw new IllegalArgumentException("Locator is null.");
        }
        InternalPlayer player = iProtocolFactory.createPlayer(
                                    new Locator(aLocator));
        if (player == null)
        {
            throw new MediaException("Locator not supported: " +
                                     aLocator);
        }
        pluginsPreparePlayer(player);
        return player;
    }

    /**
     * Create a <code>InternalPlayer</code> for a <code>DataSource</code>.
     */
    public InternalPlayer createInternalPlayer(DataSource aSource)
    throws IOException, MediaException
    {
        // Throw IllegalArgumentException if source  is null.
        if (aSource == null)
        {
            throw new IllegalArgumentException("DataSource is null.");
        }
        aSource.connect(); // Ensure that external source is connected.
        if (aSource.getStreams() == null ||
                aSource.getStreams().length == 0)
        {
            // There must be atleast one stream in the DataSource
            throw new MediaException(
                "There must be at least one stream in datasource");
        }

        BufferDataSource bdc = null;
        if (aSource instanceof BufferDataSource)
        {
            bdc = (BufferDataSource)aSource;
        }
        else
        {
            bdc = new BufferDataSource(aSource);
        }

        InternalPlayer player = null;
        Enumeration plugins = iPlugIns.elements();
        // Loop through all plugins, stop if player was created
        while (plugins.hasMoreElements() &&
                (player == null))
        {
            PlugIn tmp = (PlugIn)plugins.nextElement();
            player = tmp.createPlayer(bdc);
        }

        if (player == null)
        {
            // MMA or plugins could not create player
            bdc.disconnect();

            throw new MediaException("Could not create player.");
        }

        return player;
    }

    /**
     * Create a <code>Player</code> to play back media from an
     * <code>InputStream</code>.
     * <p>
     * The <code>type</code> argument
     * specifies the content-type of the input media.  If
     * <code>null</code> is given, <code>Manager</code> will
     * attempt to determine the type.  However, since determining
     * the media type is non-trivial for some media types, it
     * may not be feasible in some cases.  The
     * <code>Manager</code> may throw a <code>MediaException</code>
     * to indicate that.
     *
     * @param aStream The <code>InputStream</code> that delivers the
     * input media.
     * @param aType The <code>ContentType</code> of the media.
     * @return A new <code>Player</code>.
     * @exception IllegalArgumentException Thrown if <code>stream</code>
     * is <code>null</code>.
     * @exception MediaException Thrown if a <code>Player</code> cannot
     * be created for the given stream and type.
     * @exception IOException Thrown if there was a problem reading data
     * from the <code>InputStream</code>.
     * @exception SecurityException Thrown if the caller does not
     * have security permission to create the <code>Player</code>.
     */
    public Player createPlayer(InputStream aStream, String aType)
    throws IOException, MediaException
    {
        if (aStream == null)
        {
            throw new IllegalArgumentException("InputStream is null.");
        }

        InputStreamSourceStream sourceStream =
            new InputStreamSourceStream(aStream);

        // Create data source without locator.
        DataSource dataSource = new InputStreamDataSource(sourceStream,
                aType);
        InternalPlayer player = createInternalPlayer(dataSource);

        if (player != null)
        {
            // Call preparePlayer to all plugins
            pluginsPreparePlayer(player);
        }

        return player;
    }

    /**
     * Play back a tone as specified by a note and its duration.
     * A note is given in the range of 0 to 127 inclusive.  The frequency
     * of the note can be calculated from the following formula:
     * <pre>
     *     SEMITONE_CONST = 17.31234049066755 = 1/(ln(2^(1/12)))
     *     note = ln(freq/8.176)*SEMITONE_CONST
     *     The musical note A = MIDI note 69 (0x45) = 440 Hz.
     * </pre>
     * This call is a non-blocking call. Notice that this method may
     * utilize CPU resources significantly on devices that don't
     * have hardware support for tone generation.
     *
     * @param aNote Defines the tone of the note as specified by the
     * above formula.
     * @param aDuration The duration of the tone in milli-seconds.
     * Duration must be positive.
     * @param aVolume Audio volume range from 0 to 100.  100 represents
     * the maximum
     * volume at the current hardware level.  Setting the volume to a
     * value less
     * than 0 will set the volume to 0.  Setting the volume to greater than
     * 100 will set the volume to 100.
     *
     * @exception IllegalArgumentException Thrown if the given note or
     * duration is out of range.
     * @exception MediaException Thrown if the tone cannot be played
     * due to a device-related problem.
     */
    public void playTone(int aNote, int aDuration, int aVolume)
    throws MediaException
    {
        iPlayToneImpl.playTone(aNote, aDuration, aVolume);
    }

    /**
     * Get the time-base object for the system.
     * @return The system time base.
     */
    public TimeBase getSystemTimeBase()
    {
        return iSystemTimeBase;
    }

    public boolean isValidContentType(String contentType)
    {
        for (int i=0; i < contentType.length(); i++)
        {
            if ((contentType.charAt(i) >= 0 && contentType.charAt(i) <= 31) || contentType.charAt(i) == 127)
                return false;
        }
        return true;
    }



// MMAPI UI 3.x req
/**
 * get midlet state
 */

	public boolean isForground()
	{
		return iForegroundListener.isForeground();
	}

    private native int _createManager(int aEventSourceHandle,
                                      int aMIDletSuiteID);
    private native int _createEventSource();
    private native void _dispose(int aEventSourceHandle);

    /**
     * Releases native resources.
     * @param aEventSourceHandle Handle to native CMMAEventSource instance.
     */
    private native void _release(int aFunctionSourceHandle);

    private static native String[] _getSupportedContentTypes(int aFunctionSourceHandle,
            int aManagerHandle,
            String aContentType);

    private static native String[] _getSupportedProtocols(int aFunctionSourceHandle,
            int aManagerHandle,
            String aProtocol);
}

// End of File
