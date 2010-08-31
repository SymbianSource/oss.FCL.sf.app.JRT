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


package com.nokia.microedition.media;

/**
 * This interface is used notify other components when MMA event source is
 * created.
 */
public interface MMAInvokeListener
{
    /**
     * This method is called when MMA is invoked for the first time.
     * @param aEventSourceHandle Handle to native MMA event source.
     */
    void notifyInvoke(int aEventSourceHandle);
}