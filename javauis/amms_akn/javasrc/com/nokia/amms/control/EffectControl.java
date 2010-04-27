/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for Effect controls
*
*/

package com.nokia.amms.control;

import javax.microedition.media.MediaException;
import com.nokia.mj.impl.rt.legacy.NativeError;

public class EffectControl
        extends com.nokia.amms.control.ControlImpl
        implements javax.microedition.amms.control.EffectControl
{
    /**
     * Constructor
     */
    public EffectControl()
    {
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setEnabled(boolean aEnable)
    {
        checkValid();

        int err = _setEnabled(iEventSource, iControlHandle, aEnable);

        if (err == NOT_READY)
        {
            throw new IllegalStateException(
                "Enabling effect failed: Symbian OS error " + err);
        }
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isEnabled()
    {
        checkValid();

        // must ask the actual value from the native side
        // because the effect can be dropped (if it's not enforced)

        int err = _isEnabled(iEventSource, iControlHandle);
        NativeError.check(err);

        return err != 0;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setScope(int aScope) throws MediaException
    {
        checkValid();

        String KScopeNotSupported = "Only SCOPE_LIVE_ONLY scope is supported";

        // Only SCOPE_LIVE_ONLY is supported at the moment.
        if (aScope != SCOPE_LIVE_ONLY)
        {
            throw new MediaException(KScopeNotSupported);
        }

        int err = _setScope(iEventSource, iControlHandle, aScope);
        if (err < NO_ERROR)
        {
            if (err == NOT_READY)
            {
                throw new IllegalStateException(
                    "Setting scope failed: Symbian OS error " + err);
            }
            else if (err == NOT_SUPPORTED)
            {
                throw new MediaException(KScopeNotSupported);
            }
            else
            {
                NativeError.check(err);
            }
        }
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public int getScope()
    {
        checkValid();

        int err = _getScope(iEventSource, iControlHandle);
        NativeError.check(err);

        return err;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setEnforced(boolean aEnforced)
    {
        checkValid();

        int err = _setEnforced(iEventSource, iControlHandle, aEnforced);
        NativeError.check(err);
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public boolean isEnforced()
    {
        checkValid();

        int err = _isEnforced(iEventSource, iControlHandle);
        NativeError.check(err);

        return err != 0;
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public void setPreset(String aPreset)
    {
        checkValid();

        if (aPreset == null)
        {
            throw new IllegalArgumentException("Preset is null");
        }

        int err = _setPreset(iEventSource, iControlHandle, aPreset);

        if (err == NOT_FOUND)
        {
            throw new IllegalArgumentException("Preset is not available");
        }
        else
        {
            NativeError.check(err);
        }
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public String getPreset()
    {
        checkValid();

        String[] preset = new String[1];
        int err = _getPreset(iEventSource, iControlHandle, preset);
        if (err < NO_ERROR)
        {
            throw new IllegalStateException();
        }

        return preset[0];
    }

    /**
     * Please refer to JSR 234 for more details.
     */
    public String[] getPresetNames()
    {
        checkValid();

        String[] presets = _getPresetNames(iEventSource, iControlHandle);

        // native function must not return null, unless an error occures. If
        // there is no supported types zero length array will be returned.
        if (presets == null)
        {
            throw new OutOfMemoryError();
        }
        return presets;
    }

    // native methods
    private static native int _setEnabled(
        int aEventSource,
        int aControlHandle,
        boolean aEnable);
    private static native int _isEnabled(
        int aEventSource,
        int aControlHandle);
    private static native int _setScope(
        int aEventSource,
        int aControlHandle,
        int aScope);
    private static native int _getScope(
        int aEventSource,
        int aControlHandle);
    private static native int _setEnforced(
        int aEventSource,
        int aControlHandle,
        boolean aEnforced);
    private static native int _isEnforced(
        int aEventSource,
        int aControlHandle);
    private static native int _setPreset(
        int aEventSource,
        int aControlHandle,
        String aPreset);
    private static native int _getPreset(
        int aEventSource,
        int aControlHandle,
        String[] aPreset);
    private static native String[] _getPresetNames(
        int aEventSource,
        int aControlHandle);
}
