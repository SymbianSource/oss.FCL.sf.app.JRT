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
* Description:  PlugIn used to extend mma
*
*/


package com.nokia.microedition.media;

import java.io.IOException;
import javax.microedition.media.protocol.DataSource;
import javax.microedition.media.MediaException;

/**
 * This interface can be used extend Mobile Media API. This interface must
 * be registered to <code>ManagerImpl</code> with <code>addPlugIn</code>
 * method. <code>ManagerImpl</code> instance can be obtained with
 * <code>ManagerImpl.getInstance</code> method.
 * <code>getSupportedContentTypes</code> and <code>getSupportedProtocols</code>
 * methods will be called when equivalent Manager methods are called.
 *
 * @see ManagerImpl.addPlugin
 */
public interface PlugIn
{
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
    String[] getSupportedContentTypes(String aProtocol);

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
    String[] getSupportedProtocols(String aContentType);

    /**
     * Create a <code>Player</code> for a <code>DataSource</code>.
     * This method will be called if MMA was not able to create
     * Player for the DataSource.
     *
     * @param aDataSource The <CODE>DataSource</CODE> that provides
     * the media content.
     * @return A new <code>Player</code>.
     * @exception IllegalArgumentException Thrown if <code>source</code>
     * is <code>null</code>.
     * @exception MediaException Thrown if a <code>Player</code> cannot
     * be created for the given <code>DataSource</code>.
     * @exception IOException Thrown if there was a problem connecting
     * with the source.
     * @exception SecurityException Thrown if the caller does not
     * have security permission to create the <code>Player</code>.
     */
    InternalPlayer createPlayer(DataSource aDataSource)
    throws MediaException, IOException;

    /**
     * Method used to prepare player. This method is used only for
     * MMA internal extensions. If PlugIn does not support given player
     * this method must not do anything.
     *
     * @param aPlayer Player to prepare.
     * @exception MediaException Thrown if a <code>InternalPlayer</code> cannot
     * be prepared.
     */
    void preparePlayer(InternalPlayer aPlayer) throws MediaException;
}
