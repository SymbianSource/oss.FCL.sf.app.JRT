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
 * Description:  This class has been implemented for playing the GIF animation
 * Earlier implementation were dependent on native API, but now AnimationPlayer
 * will be not use any native code, and all the implementation here is using eswt API
 *
 */

package com.nokia.microedition.media.animation;

import java.io.IOException;

import javax.microedition.media.MediaException;
import javax.microedition.media.protocol.DataSource;

import org.eclipse.swt.SWTException;

import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.PlugIn;
import com.nokia.mj.impl.utils.Logger;

/**
 * This class is used for playing GIF image animation. This class also
 * implements PlugIn interface which is used from ManagerImpl.
 * Entire Animation playing is written using eSWT API.
 * There is no call to native.
 *
 */
public class AnimationPlayerFactory implements PlugIn
{

    // Used to recognize supported locators.
    // private static final String ANIMATION_FILE_EXTENSION = ".gif";

    // Used to get supported protocols and content type.
    private static final String ANIMATION_CONTENT_TYPE = "image/gif";
    private static final String ANIMATION_HTTP_PROTOCOL = "http";
    private static final String ANIMATION_HTTPS_PROTOCOL = "https";
    private static final String ANIMATION_FILE_PROTOCOL = "file";
    // There is no need to read the first 6 byte and compare it with following string
//  private static final String ANIMATION_GIF_HEADER="GIF89a";

    /**
     * From PlugIn
     */
    public InternalPlayer createPlayer(DataSource aDataSource)
    throws MediaException, IOException
    {
        final String DEBUG_STR="AnimationPlayerFactory::createPlayer()";
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR+"+");
        InternalPlayer player = null;
        String contentType = aDataSource.getContentType();
        // There is no difference in if and else block
        // in first if block only checking if the content type is available
        // Player is being created in the same way

        // First try to create player from content type
        if (contentType != null)
        {
            if (contentType.equals(ANIMATION_CONTENT_TYPE))
            {
                player = new AnimationPlayer(aDataSource);
            }
//          else{
//              throw new MediaException("Content type not supported: " + contentType);
//          }
        } //Since it was not possible to identify the player from content type, identify it from it's header
        // Is there any need to create player through Locator?

        else
        {
            // We need only 6 bytes to identify whether it's GIF image data or not.
            // But the problem here is that if we will read even a single byte from stream
            // that stream won't be useful for loading the image data through ImageLoader class
            // So better solution is to let the ImageLoader.load(InputStream ) get called
            // if it successfully load the image, then it means that stream is intended for this player only
            // otherwise it will throw the SWTException, catch it and return properly
            try
            {
                player = new AnimationPlayer(aDataSource);
            }
            catch (SWTException e)
            {
                // Simply ignore the exception
                e.printStackTrace();
            }
        }
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR+"-");
        return player;
    }

    /**
     * @param locator
     * @return Interplayer object
     * @throws IOException
     */
    public InternalPlayer createPlayer(String locator) throws IOException
    {
        final String DEBUG_STR="AnimationPlayerFactory::createPlayer(String locator )";
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR+"+");
        InternalPlayer player = null;
        try
        {
            player = new AnimationPlayer(locator);
        }
        catch (SWTException e)
        {
            // For all cases ImageLoader.load throws only SWTException
            // here we are finding why it has throws this Exception
            // if it is IOException we need to pass it to caller
            if (e.getCause().toString().indexOf((IOException.class.getName()))!=-1)
                throw new IOException();
            // if exception is due to any other reason, just ignore it
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR+"-");
        return player;
    }


    /**
     * From PlugIn
     */
    public String[] getSupportedContentTypes(String aProtocol)
    {
        String[] types = new String[0];
        if (aProtocol == null || aProtocol.equals(ANIMATION_HTTP_PROTOCOL)
                || aProtocol.equals(ANIMATION_HTTPS_PROTOCOL)
                || aProtocol.equals(ANIMATION_FILE_PROTOCOL))
        {
            types = new String[1];
            types[0] = ANIMATION_CONTENT_TYPE;
        }
        return types;
    }

    /**
     * From PlugIn
     */
    public String[] getSupportedProtocols(String aContentType)
    {
        String[] protocols = new String[0];
        if ((aContentType == null)
                || aContentType.equals(ANIMATION_CONTENT_TYPE))
        {
            protocols = new String[3];
            protocols[0] = ANIMATION_HTTP_PROTOCOL;
            protocols[1] = ANIMATION_HTTPS_PROTOCOL;
            protocols[2] = ANIMATION_FILE_PROTOCOL;
        }
        // else empty array is returned
        return protocols;
    }

    /**
     * From PlugIn. Empty implementation.
     */
    public void preparePlayer(InternalPlayer aPlayer) throws MediaException
    {

    }
}
