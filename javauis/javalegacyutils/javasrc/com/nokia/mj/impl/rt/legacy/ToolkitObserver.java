/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.rt.legacy;

public interface ToolkitObserver
{
    /**
    Called prior to the Toolkit being destroyed.
    This must not callout into the application.
    NB At the point this is called Toolkit.getToolkit() will return null.
    */
    void destroyNotify();
}