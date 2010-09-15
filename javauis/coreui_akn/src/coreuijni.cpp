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
* Description: Avkon CoreUI JNI methods.
*
*/

#include "com_nokia_mj_impl_coreuiavkon_CoreUiImpl.h"

#include <apgwgnam.h>

#include "logger.h"
#include "coreuiavkonimpl.h"
#include "javauiavkonimpl.h"
#include "coreuieventsource.h"

#include "exceptionbase.h"
#include "javaoslayer.h"
#include "javauid.h"

#include "javajniutils.h"

using namespace java::ui;
using namespace java::util;

/**
 * A method for connecting to the core UI. The peer object is told
 * to the coreUi in order ro make up call in case of shut down.
 * @returns true on success case, false otherwise.
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_coreuiavkon_CoreUiImpl__1connect
(JNIEnv* env, jobject /*peer*/)
{
    JELOG2(EJavaUI);
    bool ok = false;
    try
    {
        CoreUiAvkonImpl& coreUi = CoreUiAvkonImpl::getInstanceImpl();
        ok = coreUi.setJavaVm(env);
    }

    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaUI,"ERROR in core UI connect. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {

        ELOG1(EJavaUI,"ERROR in core UI connect. std::exception: %s",
              e.what());
    }
    return ok;
}

/**
 * A method for creating the core UI. This method will create the core
 * UI instance and start the default screen.
 * @param juid UID of the application as String.
 * @param orientation the orientation requested by the application.
 * @param selectedScreenMode the start screen type requestedby the application.
 * @param rootpath Path to the application root. Null if not needed.
 * @param backGroundStart Should the UI been created into background.
 * @returns true on success case, false otherwise.
 */
JNIEXPORT
jboolean JNICALL Java_com_nokia_mj_impl_coreuiavkon_CoreUiImpl__1createUi
(JNIEnv* env, jobject /*peer*/,jstring juid, jint orientation,
 jint selectedScreenMode, jstring rootpath, jboolean backGroundStart)
{
    JELOG2(EJavaUI);
    jboolean success = false;

    try
    {
        // Get the UI instance.
        CoreUiAvkonImpl& coreUi = CoreUiAvkonImpl::getInstanceImpl();
        coreUi.setJavaVm(env);

        // Convert the UID.
        Uid uid(JniUtils::jstringToWstring(env, juid));

        // For setting the requested parameters.
        CoreUiParams uiParams;

        // Set the requested orientation.
        if (orientation == com_nokia_mj_impl_coreuiavkon_CoreUiImpl_OrientationPortrait)
        {
            uiParams.setOrientation(PORTRAIT);
        }
        else if (orientation == com_nokia_mj_impl_coreuiavkon_CoreUiImpl_OrientationLandscape)
        {
            uiParams.setOrientation(LANDSCAPE);
        }

        // Set the requested start screen mode.
        if (selectedScreenMode == com_nokia_mj_impl_coreuiavkon_CoreUiImpl_ScreenModeNoStartScreen)
        {
            uiParams.setScreenMode(NO_START_SCREEN);
        }
        else if (selectedScreenMode == com_nokia_mj_impl_coreuiavkon_CoreUiImpl_ScreenModeMidletDefinedScreen)
        {
            uiParams.setScreenMode(MIDLET_DEFINED_SCREEN);
        }
        else
        {
            uiParams.setScreenMode(DEFAULT_START_SCREEN);
        }

        if (selectedScreenMode != com_nokia_mj_impl_coreuiavkon_CoreUiImpl_ScreenModeNoStartScreen && rootpath != 0)
        {
            uiParams.setImagePath(JniUtils::jstringToWstring(env, rootpath));
        }


        // Set the Background start info.
        uiParams.setBackgroundStart(backGroundStart);

        // Start the UI.
        coreUi.start(uid, &uiParams);
        success = true;
    }

    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaUI,"ERROR in core UI create ui. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {

        ELOG1(EJavaUI,"ERROR in core UI create ui. std::exception: %s",
              e.what());
    }
    return success;
}

void hideFromTaskManager(TBool aHide)
{
    CApaWindowGroupName* wgName = java::ui::CoreUiAvkonLcdui::getInstance().getWindowGroupName();
    if (wgName)
    {
        wgName->SetHidden(aHide);
        // Refresh displayed name in tasklist
        wgName->SetWindowGroupName(CCoeEnv::Static()->RootWin());
    }
}

void bringToForeGround()
{
    CCoeEnv::Static()->RootWin().SetOrdinalPosition(0);
}

/**
 * A method for setting the app to foreground.
 */
JNIEXPORT
void JNICALL Java_com_nokia_mj_impl_coreuiavkon_CoreUiImpl__1toForeground
(JNIEnv* env, jobject peer)
{
    JELOG2(EJavaUI);

    try
    {
        if (CoreUiAvkonImpl::isCoreUiCreated())
        {
            int esHandle = CoreUiAvkonImpl::getInstanceImpl().getLcduiSupport().getEventServerHandle();
            if (esHandle > 0)
            {
                int handle = CoreUiEventSource::New(*env, peer, esHandle);
                if (handle > 0)
                {
                    CoreUiEventSource* es = JavaUnhand<CoreUiEventSource>(handle);
                    es->ExecuteV(&bringToForeGround);
                    es->Dispose(*env);
                }
            }
        }

    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaUI,"ERROR in core UI to FG. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaUI,"ERROR in core UI to FG. std::exception: %s",
              e.what());
    }
}

/**
 * A method for checking if the UI is in foreground.
 * @returns true if in foreground, false otherwise.
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_coreuiavkon_CoreUiImpl__1isForeground
(JNIEnv* /*env*/, jobject /*peer*/)
{
    JELOG2(EJavaUI);
    bool fg = true;
    try
    {
        CoreUiAvkonImpl& coreUi = CoreUiAvkonImpl::getInstanceImpl();
        CoreUiAvkonAppUi* coreUiAvkonAppUi = coreUi.getJavaUiAppUi();
        if (coreUiAvkonAppUi)
        {
            fg = coreUiAvkonAppUi->isForeground();
        }
    }

    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaUI,"ERROR in core UI isForeground. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {

        ELOG1(EJavaUI,"ERROR in core UI isForeground. std::exception: %s",
              e.what());
    }
    return fg;
}


JNIEXPORT void JNICALL Java_com_nokia_mj_impl_coreuiavkon_CoreUiImpl__1hideApplication
(JNIEnv* env, jobject peer, jboolean hide)
{
    try
    {
        if (CoreUiAvkonImpl::isCoreUiCreated())
        {
            int esHandle = CoreUiAvkonImpl::getInstanceImpl().getLcduiSupport().getEventServerHandle();
            if (esHandle > 0)
            {
                int handle = CoreUiEventSource::New(*env, peer, esHandle);
                if (handle > 0)
                {
                    CoreUiEventSource* es = JavaUnhand<CoreUiEventSource>(handle);
                    es->ExecuteV(&hideFromTaskManager, (TBool)hide);
                    es->Dispose(*env);
                }
            }
        }
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaUI,"ERROR in core UI to hideApplication. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaUI,"ERROR in core UI to hideApplication. std::exception: %s",
              e.what());
    }
}
