/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: An interface for the toolkits to access
*              the core AppUi
*
*/

#ifndef COREUIAPPUI_H
#define COREUIAPPUI_H

#include "coreuiappuichild.h"

/**
 * CoreUiAvkonAppUi is an interface that the toolkits can use to access
 * the CAknAppUi of the CoreUi. The toolkit gets access to to this interface
 * by calling CoreUiAvkonLcdui::getInstance().getJavaUiAppUi() or
 * CoreUiAvkonEswt::getInstance().getJavaUiAppUi() method depending on the
 * toolkit.
 */
namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

NONSHARABLE_CLASS(CoreUiAvkonAppUi)
{
public:

    /**
     * CoreUiAvkonAppUi default constructor.
     */
    CoreUiAvkonAppUi() {}

    /**
     * CoreUiAvkonAppUi destructor.
     */
    virtual ~CoreUiAvkonAppUi() {}

    /**
     * Sets the LCDUI as a child. This also sets the LCDUI to be the active
     * child, if the eSWT has been previously set as active child. When the
     * toolkit sets a child this means in practice that all the window server
     * events are propagated to the child as long as it is active child. NOTE:
     * Setting a child doesn't mean that the toolkit's window would be shown
     * to the user. It is shown to the user after the toolkit calls
     * stopStartScreen method.
     * @param child The LCDUI child object. CoreUi takes the ownership of the
     *              child.
     * @param resourceFileOffset Offset of the resource file of the LCDUI. This
     *                           is needed when deleting the resource file by
     *                           using CCoeEnv::Static()->DeleteResourceFile().
     */
    virtual void setLcduiChild(CoreAppUiChild* child, int resourceFileOffset) = 0;

    /**
     * Sets the eSWT as a child. This also sets the eSWT to be the active
     * child, if the LCDUI has been previously set as active child. When the
     * toolkit sets a child this means in practice that all the window server
     * events are propagated to the child as long as it is active child. NOTE:
     * Setting a child doesn't mean that the toolkit's window would be shown
     * to the user. It is shown to the user after the toolkit calls
     * stopStartScreen method.
     * @param child The eSWT child object. CoreUi takes the ownership of the
     *              child.
     */
    virtual void setEswtChild(CoreAppUiChild* child) = 0;

    /**
     * Removes the given child. This means that the window server events are no
     * longer propagated to this child. If the child to be removed was active
     * and there was another toolkit as a secondary child, the secondary child
     * will be set as active child. Calling this method will lead to deletion
     * of the child.
     */
    virtual void removeChild(CoreAppUiChild* child) = 0;

    /**
     * Gets the LCDUI child.
     */
    virtual CoreAppUiChild* getLcduiChild() const = 0;

    /**
     * Checks if the appui is still having the start screen.
     */
    virtual bool hasStartScreen() const = 0;

    /**
     * Stops the start screen. This must be done right after the app draws
     * for the first time.
     * @param aFullScreenUi If true, indicates that the app's UI is using the
     *                      whole screen. If false the start screen will not
     *                      save a screenshot of the ui.
     */
    virtual void stopStartScreen(bool aFullScreenUi = true) = 0;

    /**
     * Gets whether the application is in foreground.
     */
    virtual bool isForeground() const = 0;

    /**
     * A delegating method for calling CAknAppUi::SetKeyBlockMode which
     * is a protected method.
     * @see CAknAppUi::SetKeyBlockMode
     */
    virtual void glueSetKeyBlockMode(TAknKeyBlockMode aMode) = 0;

    /**
     * Check if status pane indicators are hidden
     */
    virtual bool hidesIndicators() const = 0;

    /**
     * Check if the startup has been cancelled.
     */
    virtual bool isStartupCancelled() const = 0;
};

} // end namespace ui
} // end namespace java

#endif // COREUIAPPUI_H
