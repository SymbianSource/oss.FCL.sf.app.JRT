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
* Description: Classes for creating the Avkon based UI framework.
*
*/

#ifndef JAVAUIAVKONIMPL_H
#define JAVAUIAVKONIMPL_H

// Includes
#include <aknapp.h>
#include <AknDoc.h>
#include <aknappui.h>

#include "coreuiappui.h"
#include "coreuiavkonimpl.h"

// Forward declaration
class CEikonEnv;
class CApaWindowGroupName;
class CStartScreenContainer;

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

class JavaUiAvkonAppUi;
class JavaUiAvkon;

NONSHARABLE_CLASS(JavaUiAvkonAppUi) : public CAknAppUi, public CoreUiAvkonAppUi
{
public:
    JavaUiAvkonAppUi();
    virtual ~JavaUiAvkonAppUi();
    /**
     * Called by the Avkon framework.
     * @see CAknAppUi::ConstructL.
     */
    void ConstructL();

    /**
     * @see CAknAppUi::HandleCommandL.
     */
    void HandleCommandL(TInt aCommand);

    /**
     * @see CAknAppUi::HandleForegroundEventL.
     */
    void HandleForegroundEventL(TBool aForeground);

    /**
     * @see CAknAppUi::HandleSwitchOnEventL.
     */
    void HandleSwitchOnEventL(CCoeControl* aDestination);

    /**
     * @see CAknAppUi::HandleResourceChangeL.
     */
    void HandleResourceChangeL(TInt aType);

    /**
     * @see CAknAppUi::HandleApplicationSpecificEventL.
     */
    void HandleApplicationSpecificEventL(TInt aType,const TWsEvent& aEvent);

    /**
     * @see CAknAppUi::HandleWsEventL.
     */
    void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);

    /**
     * For asking whether it is ok to start closing procedures.
     */
    bool isClosingPossible() const;

    /**
     * Get the moment of startup.
     */
    TInt64 startupTime() const;
    
    /**
     * Returns true, after the app has started to be destroyed.
     */
    bool destroying() const {return mDestroying;};

public: // From CoreUiAvkonAppUi
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
    virtual void setLcduiChild(CoreAppUiChild* child, int resourceFileOffset);

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
    virtual void setEswtChild(CoreAppUiChild* child);

    /**
     * Removes the given child. This means that the window server events are no
     * longer propagated to this child. If the child to be removed was active
     * and there was another toolkit as a secondary child, the secondary child
     * will be set as active child. Calling this method will lead to deletion
     * of the child.
     */
    virtual void removeChild(CoreAppUiChild* child);

    /**
     * Gets the LCDUI child.
     */
    virtual CoreAppUiChild* getLcduiChild() const;

    /**
     * Checks if the appui is still having the start screen.
     */
    virtual bool hasStartScreen() const;

    /**
     * Stops and destroys the start screen. This must be done right after the
     * app draws for the first time.
     */
    virtual void stopStartScreen(bool aFullScreenUi = true);

    /**
     * Gets whether the application is in foreground.
     */
    virtual bool isForeground() const;

    /**
     * A delegating method for calling CAknAppUi::SetKeyBlockMode which
     * is a protected method.
     * @see CAknAppUi::SetKeyBlockMode
     */
    virtual void glueSetKeyBlockMode(TAknKeyBlockMode aMode);

    /**
     * Check if status pane indicators are hidden.
     */
    virtual bool hidesIndicators() const;

    /**
     * Check if the startup has been cancelled.
     */
    virtual bool isStartupCancelled() const;

private:
    /**
     * Call back for the timer in case where the orientation timer elapses.
     */
    static TInt orientationTimerCallback(TAny*);

    /**
     * Handler for the timer case where the orientation timer elapses.
     */
    void handleElapsedTimer();

    /**
     * Sets a process to use default priority.
     */
    /**
     * Sets a process to use default priorityfor given time.
     * @param time A time to use default priority until changed to background
     *             priority. Zero means that no timer should be started.
     * @return KErrNone in success cases, Symbian error code in failure cases.
     */
    int setDefaultProcessPriority(TInt time);

    /**
     * Sets a process to use background priority for given time.
     * @param time A time to use background priority until changed to default
     *             priority. Zero means that no timer should be started.
     * @return KErrNone in success cases, Symbian error code in failure cases.
     */
    int setBackgroundProcessPriority(TInt time);

    /**
     * Starts the timer for given time. If the timer is already running, the
     * timer will be reset.
     * @param time A time for the timer in milliseconds.
     */
    void startTimerL(TInt time);

    /**
     * Notification received when the start screen finished saving to disk.
     */
    static TInt AsyncSaveComplete(TAny* aObj);
    void DoAsyncSaveComplete();

private:
    CStartScreenContainer*  mAppView;
    bool                    mAppViewExiting;
    CoreAppUiChild*         mActiveChild;
    CoreAppUiChild*         mPassiveChild;
    CoreAppUiChild*         mEswtChild;
    CoreAppUiChild*         mLcduiChild;
    TInt                    mLcduiResourceFile;
    bool                    mIsForeground;
    CPeriodic*              mOrientationTimer;
    bool                    mIsDefaultPriorityInUse;
    TInt64                  mStartupTime;
    bool                    mStartupCancelled;
    bool                    mDestroying;
};

NONSHARABLE_CLASS(JavaUiAvkonApplication): public CAknApplication
{
public:
    /**
     * @see CAknApplication::AppDllUid.
     */
    TUid          AppDllUid() const;

    /**
     * @see CAknApplication::CreateDocumentL.
     */
    CApaDocument* CreateDocumentL();

    /**
     * @see CAknApplication::ResourceFileName.
     */
    TFileName     ResourceFileName() const;
};

NONSHARABLE_CLASS(JavaUiAvkonDocument) : public CAknDocument
{
public:
    JavaUiAvkonDocument(JavaUiAvkonApplication& application);
public:
    /**
     * @see CAknDocument::CreateAppUiL.
     */
    CAknAppUi* CreateAppUiL();
};
} // end namespace ui
} // end namespace java

CApaApplication* newJavaApplication();

#endif // JAVAUIAVKONIMPL_H
