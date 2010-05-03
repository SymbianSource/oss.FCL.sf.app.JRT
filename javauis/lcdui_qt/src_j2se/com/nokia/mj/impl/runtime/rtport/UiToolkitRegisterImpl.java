/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.runtime.rtport;

//import com.nokia.mj.impl.rt.legacy.LegacySupport;

/**
 * Common interface to UI Toolkit Register implementations.
 */
public class UiToolkitRegisterImpl implements UiToolkitRegister
{
    private String mRegisteredToolkit;
    private String[] mRegisteredToolkits;
    
    public void registerUiToolkit(String toolkitId)
    {
        if(mRegisteredToolkit != null)
        {
            throw new RuntimeException("Could not register toolkit: "+toolkitId);
        }
        mRegisteredToolkit = toolkitId;
        mRegisteredToolkits = new String[1];
        mRegisteredToolkits[0] = mRegisteredToolkit;
    }

    public void unregisterUiToolkit(String toolkitId)
    {
        if(mRegisteredToolkit != null && mRegisteredToolkit.equals(toolkitId))
        {
            mRegisteredToolkit = null;
            mRegisteredToolkits = null;
        }
    }

    public String[] getRegisteredUiToolkits()
    {
        if (mRegisteredToolkits == null)
        {
    		return new String[0];
        }
        return mRegisteredToolkits;
    }

    /**
     * Returns true if UI toolkit is foreground.
     *
     * @return true if toolkit is foreground. If toolkit has not set the value,
     * false is returned.
     */
    public boolean isToolkitForeground()
    {
        return false;
    }

    /**
     * Changes the value to be returned from isToolkitForeground.
     *
     * @param foreground true if switched to foreground
     */
    public void setToolkitForeground(boolean foreground)
    {
        // Stop the start screen if exists.
        //LegacySupport.stopStartScreen();
    }
}
