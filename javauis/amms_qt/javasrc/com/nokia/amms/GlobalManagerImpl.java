/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.amms;

import javax.microedition.media.MediaException;
import javax.microedition.amms.EffectModule;
import javax.microedition.amms.SoundSource3D;
import javax.microedition.amms.MediaProcessor;
import javax.microedition.media.Manager;
import java.util.Vector;

/**
 * GlobalManager implementation used by
 * javax.microedition.amms.GlobalManager.
 */
public class GlobalManagerImpl extends ModuleBase
{
    /**
     * There is no supported media processor input types.
     */
    private static final String[] SUPPORTED_MEDIA_PROCESSOR_INPUT_TYPES =
        new String[ 0 ];

    // Constants needed when getting supported player content types.
    private static final String AUDIO_STRING = "audio";
    private static final String VIDEO_STRING = "video";
    private static final String CAPTURE_STRING = "capture";
    private static final String DEVICE_STRING = "device";
    private static final String PROTOCOL_SEPARATOR = "://";
    private static final char MEDIA_SEPARATOR = '/';

    /**
     * Singleton instance.
     */
    static private GlobalManagerImpl iInstance;

    /**
     * Implements Spectator functionality.
     */
    private SpectatorImpl iSpectator;

    /**
     * Private constructor not to allow direct construction, because
     * this class is singleton.
     */
    private GlobalManagerImpl(int aEventSourceHandle)
    {
        com.nokia.mj.impl.rt.support.Jvm.loadSystemLibrary("javaamms");

        iEventSourceHandle = aEventSourceHandle;

        // Create native instance
        iModuleHandle = _createGlobalManager(iEventSourceHandle);


        if (iModuleHandle < 0)
        {
            // Could not create native object
            throw new OutOfMemoryError();
        }
    }

    /**
     * Creates static GlobalManagerImpl instance. Instance can be obtained
     * with getInstance method.
     * @param aEventSourceHandle Handle to native event source.
     */
    public static void invoke(int aEventSourceHandle)
    {
        // create static instance
        iInstance = new GlobalManagerImpl(aEventSourceHandle);
    }

    /**
     * Returns static GlobalManagerImpl instance. This method will be called
     * from GlobalManager's static block when it is loaded for the first time.
     * @return GlobalManagerImpl instance
     */
    public static GlobalManagerImpl getInstance()
    {
        if (iInstance == null)
        {
            // GlobalManagerImpl is not initialized because iInstance is null.
            // Invoke MMA1.1 by calling one of Manager's static methods.
            // MMA will invoke its invoke listener that will result a call to
            // GlobalManagerImpl::invoke method which creates static global
            // manager instance.
            Manager.getSystemTimeBase();
        }

        // if spectator isn't null init is already called
        if (iInstance.iSpectator == null)
        {
            iInstance.init();
        }
        return iInstance;
    }

    /**
     * Finalizes GlobalManagerImpl's initialization.
     * This method creates native global controls and corresponding Java
     * controls.
     */
    private void init()
    {
        // Initialize native global manager instance.
        int err = _init(iInstance.iEventSourceHandle,
                        iInstance.iModuleHandle);


        // Initialize super class which will add global controls
        init(iEventSourceHandle, iModuleHandle);

        if (err < 0)
        {
            // Init failed. AMMS functionality cannot be used.
            throw new OutOfMemoryError();
        }
        int handle = _createSpectator(iEventSourceHandle, iModuleHandle);
        if (handle < 0)
        {
            // Could not create native object
            throw new OutOfMemoryError();
        }

        // Create Java object for native spectator
        iSpectator = new SpectatorImpl(iEventSourceHandle, handle);
    }

    /**
     * Creates an <code>EffectModule</code>.
     *
     * @throws MediaException if creation of <code>EffectModule</code>
     * is not supported.
     * @return An <code>EffectModule</code> object that may be used to group
     * <code>Player</code>s.
     */
    public EffectModule createEffectModule() throws MediaException
    {
        int handle = _createEffectModule(iEventSourceHandle, iModuleHandle);
        if (handle < 0)
        {
            throw new MediaException();
        }
        return new EffectModuleImpl(iEventSourceHandle,
                                    handle,
                                    iModuleHandle);
    }

    /**
     * Creates a <code>SoundSource3D</code>.
     *
     * @throws MediaException if creation of <code>SoundSource3D</code>
     * is not supported.
     * @return A <code>SoundSource3D</code> object that represents
     * a virtual sound source and that may be used to group
     * <code>Player</code>s.
     */
    public SoundSource3D createSoundSource3D() throws MediaException
    {
        int handle = _createSoundSource3D(iEventSourceHandle, iModuleHandle);
        if (handle < 0)
        {
            throw new MediaException();
        }
        return new SoundSource3DImpl(iEventSourceHandle,
                                     handle,
                                     iModuleHandle);
    }

    /**
     * Gets the supported player content types that can be added
     * to a <code>SoundSource3D</code>.
     * {@link javax.microedition.amms.control.AudioFormatControl}
     * specifies constants for content types commonly used with
     * <code>SoundSource3D</code>.
     *
     * @return a list of content types that can be used to create
     * <code>Player</code>s supported by <code>SoundSource3D</code>
     * (e.g. "audio/midi") and of device and capture locators that can
     * be used to create <code>Player</code>s supported by
     * <code>SoundSource3D</code> (e.g. "capture://radio")
     */
    public String[] getSupportedSoundSource3DPlayerTypes()
    {
        Vector resultVector = new Vector();

        // Get content types (e.g. audio/midi)
        playerTypes(null, resultVector);

        // Get device locators (e.g. "device://video)
        playerTypes(DEVICE_STRING, resultVector);


        // Convert the vector to string array.

        int resultLength = resultVector.size();

        String[] result = new String[ resultLength ];

        for (int i = 0; i < resultLength; i++)
        {
            result[ i ] = (String)resultVector.elementAt(i);
        }

        return result;
    }

    /**
     * Returns player types that supports the given protocol.
     * @param aProtocol Protocol (e.g. capture) or null if all protocols are used.
     * @param aResultVector Vector where the result is stored.
     */
    private void playerTypes(String aProtocol, Vector aResultVector)
    {
        String[] contentTypes = Manager.getSupportedContentTypes(aProtocol);

        int contents = contentTypes.length;

        for (int i = 0; i < contents; i++)
        {
            String contentType = contentTypes[ i ];

            // Select those types that are supported by AMMS.
            if (contentType.startsWith(AUDIO_STRING + MEDIA_SEPARATOR) ||
                    contentType.startsWith(VIDEO_STRING + MEDIA_SEPARATOR))
            {
                String playerType = contentType;

                // Add the protocol string if a specific one is used.
                if (aProtocol != null)
                {
                    int endIndex = contentType.indexOf(MEDIA_SEPARATOR);
                    if (endIndex >= 0)
                    {
                        contentType = contentType.substring(0, endIndex);
                    }

                    playerType = aProtocol + PROTOCOL_SEPARATOR + contentType;
                }

                // Add the type to the vector.
                if (!aResultVector.contains(playerType))
                {
                    aResultVector.addElement(playerType);
                }
            }
        }
    }

    /**
     * Gets the <code>Spectator</code>, which represents the listener
     * in the virtual acoustical space.
     *
     * @return the <code>Spectator</code>, which represents the listener
     * in the virtual acoustical space
     */
    public SpectatorImpl getSpectator()
    {
        return iSpectator;
    }

    /**
     * Creates a <code>MediaProcessor</code> object. Content type is passed
     * as a MIME type as specified in javax.microedition.media.Manager.
     * {@link FormatControl},
     * {@link javax.microedition.amms.control.ContainerFormatControl},
     * {@link javax.microedition.amms.control.VideoFormatControl},
     * {@link javax.microedition.amms.control.AudioFormatControl}
     * and {@link javax.microedition.amms.control.ImageFormatControl} specify
     * constants for a set of commonly used content types.
     *
     *
     * @param contentType the content type of the source data to be processed.
     * @throws MediaException if a <code>MediaProcessor</code> could
     * not be created for the given content type.
     * @return An instance of <code>MediaProcessor</code>
     */
    public MediaProcessor createMediaProcessor(String aContentType)
    throws MediaException
    {
        // MediaProcessor is not supported.
        throw new MediaException();
    }

    /**
     * Obtain the handle of the GlobalManager.
     *
     * @return the module handle.
     */
    public static int getHandle()
    {
        if (iInstance == null)
        {
            // GlobalManagerImpl is not initialized because iInstance is null.
            // Invoke MMA1.1 by calling one of Manager's static methods.
            // MMA will invoke its invoke listener that will result a call to
            // GlobalManagerImpl::invoke method which creates static global
            // manager instance.
            Manager.getSystemTimeBase();
        }

        return iInstance.iModuleHandle;
    }

    /**
     * Gets the supported MediaProcessor input content types.
     *
     * @return Supported MediaProcessor input content types.
     */
    public String[] getSupportedMediaProcessorInputTypes()
    {
        return SUPPORTED_MEDIA_PROCESSOR_INPUT_TYPES;
    }

    /**
     * @param aEventSourceHandle Handle to native event source.
     * @return Handle to native object or an error code.
     */
    private static native int _createGlobalManager(int aEventSourceHandle);

    /**
     * Initializes native object.
     *
     * @param aEventSourceHandle Handle to native event source.
     * @param aGlobalManagerHandle Handle to native global manager.
     * @return 0 if success or an error code.
     */
    private static native int _init(int aEventSourceHandle,
                                    int aGlobalManagerHandle);

    /**
     * @param aEventSourceHandle Handle to native event source.
     * @param aGlobalManagerHandle Handle to native global manager.
     * @return Handle to native object or an error code.
     */
    private static native int _createEffectModule(int aEventSourceHandle,
            int aGlobalManagerHandle);

    /**
     * @param aEventSourceHandle Handle to native event source.
     * @param aGlobalManagerHandle Handle to native global manager.
     * @return Handle to native object or an error code.
     */
    private static native int _createSoundSource3D(int aEventSourceHandle,
            int aGlobalManagerHandle);

    /**
     * @param aEventSourceHandle Handle to native event source.
     * @param aGlobalManagerHandle Handle to native global manager.
     * @return Handle to native object or an error code.
     */
    private static native int _createSpectator(int aEventSourceHandle,
            int aGlobalManagerHandle);

}
