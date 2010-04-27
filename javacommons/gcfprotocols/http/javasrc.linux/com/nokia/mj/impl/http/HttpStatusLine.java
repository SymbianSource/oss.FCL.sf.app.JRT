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


package com.nokia.mj.impl.http;

import java.io.IOException;

/**
 * Simple HttpStatusLine. Used to store the Http response line contents.
 *
 * @version 1.1
 */
public class HttpStatusLine
{
    protected String mHttpVersion;

    protected int mStatusCode; // status code of the response

    protected String mReasonPhrase; // status code reason phrase
}
