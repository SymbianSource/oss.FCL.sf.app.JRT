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

package com.nokia.mj.impl.fileutils;

interface FileUtilityErrorStrings
{
    public static String TARGET_POINTS_TO_DIRECTORY = "Path specified is a directory";

    public static String TARGET_POINTS_TO_FILE = "Path specified specified is a file";

    public static String TARGET_DOES_NOT_EXIST = "Path specified does not exist";

    public static String UNABLE_TO_CREATE_FILE = "Unable to create file";

    public static String TEMP_DIR_NOT_PRESENT = "Directory to create temp file does not exist";

    public static String SOURCE_NOT_PRESENT = "Source not present";

    public static String SOURCE_NOT_DIRECTORY = "Source not a directory";

    public static String SOURCE_NOT_FILE = "Source not a file";

    public static String DESTINATION_IS_DIRECTORY = "Destination is a directory";

    public static String DESTINATION_ALREADY_PRESENT = "Destination already present";
}
