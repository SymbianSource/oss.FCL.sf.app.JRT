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
* Description:
*
*/

#ifndef ICONSIZEUTILS_H
#define ICONSIZEUTILS_H

#include <aknlayoutscalable_avkon.cdl.h>
#include <AknUtils.h>

/**
 * Fallback value of expected size of raster icons in Menu & Home Screen.
 * Copy of KJavaIconWidth in Menu's mcsmenuiconutility.cpp.
 */
const TInt KIconInMenuFallbackSize = 88;

/**
 * Fallback value of expected size of raster icons in App Mgr.
 */
const TInt KIconInAppMgrFallbackSize = 46;

/**
 * Utility class packing all Java icon sizes needed troughout the system.
 */
class TIconSizes
{
public:
    TSize iMenuIconSize;
    TSize iAppMgrIconSize;
};

/**
 * Utility class dealing with Java icon sizes.
 */
class IconSizeUtils
{
public:
    /**
     * Helper for getting the ideal sizes for Java raster icons.
     * Accesses Akn layouts, therefore a Ui context is required
     * (CEikonEnv present in the calling thread).
     * This function has been inlined so that the Java App Mgr plugin
     * does not have to statically link against the ecom plugin dll.
     */
    static TIconSizes GetIconSizes()
    {
        TIconSizes res;

        // Menu & Home Screen icons size.
        // No api to get it so just use the fallback value for now.
        res.iMenuIconSize = TSize(KIconInMenuFallbackSize, KIconInMenuFallbackSize);

        // App manager icon size.
        // list_double_large_graphic_pane_vc_g1 = the icon of the app mgr list in landscape
        TAknLayoutRect layoutRect;
        layoutRect.LayoutRect(TRect(), 
            AknLayoutScalable_Avkon::list_double_large_graphic_pane_vc_g1(0).LayoutLine());
        TRect rect(layoutRect.Rect());
        TInt w(rect.Width());
        TInt h(rect.Height());
        if (w < 0 || h < 0)
        {
            w = KIconInAppMgrFallbackSize;
            h = KIconInAppMgrFallbackSize;
        }
        res.iAppMgrIconSize = TSize(w, h);

        return res;
    }
};

#endif // ICONSIZEUTILS_H
