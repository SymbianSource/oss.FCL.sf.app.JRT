/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.file;

import com.nokia.mj.impl.utils.ResourceLoader;

public final class FileConnectionTexts
{
    public static final String FILE_IMAGES = "photos_name";
    public static final String FILE_VIDEOS = "videos_name";
    public static final String FILE_GRAPHICS = "graphics_name";
    public static final String FILE_TONES = "tones_name";
    public static final String FILE_MUSIC = "music_name";
    public static final String FILE_RECORDINGS = "recordings_name";
    public static final String FILE_MEMORYCARD = "memorycard_name";
    public static final String FILE_PRIVATE = "private_name";
    public static final String FILE_C_DRIVE = "c_drive";
    public static final String FILE_D_DRIVE = "d_drive";
    public static final String FILE_Z_DRIVE = "z_drive";
    public static final String FILE_HARDDRIVE = "hard_drive";
    public static final String FILE_INTERNAL_N = "internal_n_drive";
    public static final String FILE_MEMORYCARD_N = "memorycard_n_drive";
    public static final String FILE_REMOTEDRIVE_N = "remote_n_drive";
    private static final String FILE_RES_START = "qtn_fileconn_dir_";
    private static final String FILE_RES_NAME = "javafileconnection";

    private static ResourceLoader iRes = null;

    /**
     * Method for retrieving a text string with given id and no parameters.
     *
     * @param aTextId
     *            id for the text string
     * @return The corresponding text
     */
    public static String get(String aTextId)
    {
        return get(aTextId, (Object[]) null);
    }

    /**
     * Method for retrieving a text string with given id and parameters.
     *
     * @param aTextId
     *            id for the text string
     * @param aTextParameters
     *            parameters to be filled into the text
     * @return The corresponding text including the provided parameters
     */
    public static String get(String aTextId, Object[] aTextParameters)
    {
        if (iRes == null)
        {
            iRes = ResourceLoader.getInstance(FILE_RES_NAME, FILE_RES_START);
        }
        return iRes.format(aTextId, aTextParameters);
    }
}