/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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

#include <eikenv.h>
#include <eiksrvc.h>
#include <eikappui.h>
#include <Lcdglue.h>
#include <j2me/jdebug.h>
// needed for javaregistry
#include <javaregistryentry.h>
#include <javaregistry.h>
#include <javaattribute.h>
#ifdef RD_JAVA_NGA_ENABLED
#include <goommonitorsession.h>
#include <goommonitorplugin.hrh>
#endif // RD_JAVA_NGA_ENABLED
#include "CMIDAppUi.h"
// using CMIDApplication API for iApp
#include "CMIDApplication.h"
// needed for creating CMIDComponentFactory
#include "CMIDComponentFactory.h"
// CMIDDisplayable class for iCurrentDisplayable
#include "CMIDDisplayable.h"

#include "LcduiThread.h"

#include "logger.h"

using namespace java::ui;
using namespace Java;

// class CMIDAppUi
CMIDAppUi::CMIDAppUi() : iCurrentDisplayable(NULL),
    iPendingOrientationChange(EFalse), iPauseApp(EFalse)
{
    mJavaAppUi = java::ui::CoreUiAvkonLcdui::getInstance().getJavaAknAppUi();
    mCoreAppUi = java::ui::CoreUiAvkonLcdui::getInstance().getJavaUiAppUi();
}

void CMIDAppUi::ConstructL()
{
    DEBUG("+ CMIDAppUi::ConstructL()");
    // get uid of the midlet
    RLcdui::Get()->AttachL(this);
    DEBUG("- CMIDAppUi::ConstructL()");
}

CMIDAppUi::~CMIDAppUi()
{
    iDisplayables.Close();
    RLcdui::Get()->Detach();
}

void CMIDAppUi::SetCurrentDisplayable(CMIDDisplayable* aDisplayable)
{
    iCurrentDisplayable = aDisplayable;
}

void CMIDAppUi::HandleCommandL(TInt aCommand)
{
    switch (aCommand)
    {
        // Help command received (i.e. after the EStdKeyHelp button was pressed)
    case EAknCmdHelp:
        if (iCurrentDisplayable)
        {
            // Let the current displayable to process help command
            iCurrentDisplayable->HandleHelpCommandL();
        }
        break;
    case EEikCmdExit:
        if (iObserver)
        {
            iObserver->HandleExitL();
        }
        else
        {
            //
            //
            //
        }
        break;
    }
}

void CMIDAppUi::HandleSwitchOnEventL(CCoeControl*)
{
    if (iObserver)
    {
        iObserver->HandleSwitchOnEventL();
    }
    else
    {
        //
        //
        //
    }
}

void CMIDAppUi::HandleForegroundEventL(TBool aForeground)
{
    if (iObserver)
    {
        iObserver->HandleForegroundL(aForeground);
    }
    else
    {
        //
        //
        //
    }
}

void CMIDAppUi::HandleResourceChangeL(TInt aType)
{
    if (iObserver)
    {
        iObserver->HandleResourceChangeL(aType);
    }
    else
    {
        if (aType == KEikDynamicLayoutVariantSwitch)
        {
            // Observer not set yet. Storing it for use after it is set.
            iPendingOrientationChange= ETrue;
        }
    }
}

void CMIDAppUi::HandleApplicationSpecificEventL(TInt /*aType*/, const TWsEvent& /*aEvent*/)
{
}

TBool CMIDAppUi::HandleWsEventL(const TWsEvent& aEvent,
                                CCoeControl* /*aDestination*/)
{

    // Intercept end-key press when the Nokia-MIDlet-no-exit JAD-attribute is defined.
    // Intercept the WS event in case of red end-key press
    if (aEvent.Type() == KAknUidValueEndKeyCloseEvent)
    {
        // If JAD attribute Nokia-MIDlet-no-exit is defined, it will be used define End-key behavior
        if (iEnv && iEnv->MidletAttributeIsSetToVal(
                    LcduiMidletAttributes::KAttribNoExitKey,
                    LcduiMidletAttributeValues::KTrueValue))
        {
            return ETrue;
        }
        else
        {
            // No JAD attribute. Ask the CoreUi to shutdown the MIDlet.
            java::ui::CoreUiAvkonLcdui::getInstance().shutDownRequestFromWindowServer();
        }
    }

    if (aEvent.Type() == EEventUser)
    {
        TApaSystemEvent* eventData = reinterpret_cast<TApaSystemEvent*>(aEvent.EventData());
        if ((*eventData) == EApaSystemEventShutdown)
        {
#ifdef RD_JAVA_NGA_ENABLED
            // Check the reason
            eventData++;
            if ((*eventData) == KGoomMemoryLowEvent)
            {
                return HandleGoomMemoryLowEventL();
            }
#endif // RD_JAVA_NGA_ENABLED    
            // Oom or exit from task-list. Ask the CoreUi to shutdown the MIDlet.
            java::ui::CoreUiAvkonLcdui::getInstance().shutDownRequestFromWindowServer();
        }
    }

    // Workaround to send pauseApp with long-press of the Menu key
    // if no event was send yet
    if (aEvent.Type() == EEventKey)
    {
        if (aEvent.Key()->iScanCode == EStdKeyApplication0
                && aEvent.Key()->iRepeats == 1 && !iPauseApp)
        {
            if (iEnv && iEnv->MidletAttributeIsSetToVal(
                        LcduiMidletAttributes::KAttribFlipClose,
                        LcduiMidletAttributeValues::KPauseValue))
            {
                iEnv->PostMidletEvent(EPause);
                SetPauseAppState(ETrue);
            }
        }
    }

    if (aEvent.Type() == EEventKeyUp)
    {

        if (aEvent.Key()->iScanCode == EStdKeyDeviceA)   // if flip is opened
        {
            if (iEnv && iEnv->MidletAttributeIsSetToVal(
                        LcduiMidletAttributes::KAttribFlipClose,
                        LcduiMidletAttributeValues::KPauseValue))

            {
                iEnv->PostMidletEvent(EStart);
            }
        }

        if (aEvent.Key()->iScanCode == EStdKeyDeviceB)   // if flip is closed
        {
            if (iEnv && iEnv->MidletAttributeIsSetToVal(
                        LcduiMidletAttributes::KAttribFlipClose,
                        LcduiMidletAttributeValues::KPauseValue))
            {
                iEnv->PostMidletEvent(EPause);
            }
        }
    }

    if (aEvent.Type() == EEventKeyDown)
    {
        if (iEnv)
        {
            iEnv->SetLastKeyEvent(*aEvent.Key());
        }
    }

    // Handle application focus lost and notify current Displayable.
    if ((aEvent.Type() == KAknFullOrPartialForegroundLost)
            || (aEvent.Type() == EEventFocusLost))
    {
#ifdef RD_JAVA_NGA_ENABLED
        if (aEvent.Type() == KAknFullOrPartialForegroundLost && iObserver)
        {
            iObserver->HandleFullOrPartialForegroundL(EFalse);
        }
#endif // RD_JAVA_NGA_ENABLED

        if (iCurrentDisplayable && (iDisplayables.Count() > 0))
        {
            iCurrentDisplayable->HandleApplicationBackground();
        }
    }

    // Handle input language change and notify current Displayable
    if (aEvent.Type() == KEikInputLanguageChange)
    {
        if (iCurrentDisplayable)
        {
            iCurrentDisplayable->HandleResourceChange(aEvent.Type());
        }
    }

#ifdef RD_JAVA_NGA_ENABLED
    if (aEvent.Type() == KAknFullOrPartialForegroundGained && iObserver)
    {
        iObserver->HandleFullOrPartialForegroundL(ETrue);
    }
#endif // RD_JAVA_NGA_ENABLED

    return EFalse;
}

// Sets the link to MMIDEnv - used for jad attribute parsing
void CMIDAppUi::SetEnv(MMIDEnv& aEnv)
{
    iEnv = &aEnv;
}

void CMIDAppUi::UnSetEnv()
{
    iEnv = 0;
}

#ifdef RD_JAVA_NGA_ENABLED
TBool CMIDAppUi::HandleGoomMemoryLowEventL()
{
    if (iObserver)
    {
        iObserver->HandleFreeGraphicsMemory();
        RGOomMonitorSession session;
        User::LeaveIfError(session.Connect());
        session.ThisAppIsNotExiting(CCoeEnv::Static()->RootWin().Identifier());
        session.Close();
        return ETrue;
    }
    return EFalse;
}
#endif // RD_JAVA_NGA_ENABLED

//
// From MLcduiPlugin
//
void CMIDAppUi::SetObserverL(MMIDObserver* aObserver)
{
    iObserver = aObserver;
    if (iPendingOrientationChange)
    {
        // We have received orientation change during the construction phase.
        // Resending it again in order to ensure correct layout and
        // orientation.
        iObserver->HandleResourceChangeL(KEikDynamicLayoutVariantSwitch);
        iPendingOrientationChange = EFalse;
    }
}

//
// From MLcduiPlugin
//
MMIDComponentFactory* CMIDAppUi::CreateComponentFactoryL()
{
    return new(ELeave) CMIDComponentFactory;
}

//
// From MLcduiPlugin
//
void CMIDAppUi::SetEnv(MMIDEnv* aEnv)
{
    iEnv = aEnv;
}

RPointerArray<CMIDDisplayable>& CMIDAppUi::Displayables()
{
    return iDisplayables;
}

void CMIDAppUi::AddToStackL(CCoeControl *aControl, TInt aPriority, TInt aStackingFlags)
{
    JELOG2(EJavaUI);
    mJavaAppUi->AddToStackL(aControl, aPriority, aStackingFlags);
}

TRect CMIDAppUi::ApplicationRect() const
{
    JELOG2(EJavaUI);
    return mJavaAppUi->ApplicationRect();
}

TRect CMIDAppUi::ClientRect() const
{
    JELOG2(EJavaUI);
    return mJavaAppUi->ClientRect();
}

CAknToolbar* CMIDAppUi::CurrentFixedToolbar() const
{
    JELOG2(EJavaUI);
    return mJavaAppUi->CurrentFixedToolbar();
}

void CMIDAppUi::RemoveFromStack(CCoeControl *aControl)
{
    JELOG2(EJavaUI);
    mJavaAppUi->RemoveFromStack(aControl);
}

CEikStatusPane* CMIDAppUi::StatusPane()
{
    JELOG2(EJavaUI);
    return mJavaAppUi->StatusPane();
}

void CMIDAppUi::SetPauseAppState(TBool aState)
{
    JELOG2(EJavaUI);
    iPauseApp = aState;
}

TBool CMIDAppUi::GetPauseAppState()
{
    JELOG2(EJavaUI);
    return iPauseApp;
}

