/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


#include "org_eclipse_swt_widgets_DCControl.h"

#include <e32std.h>

#include <eswt.h>
#include <mswtclient.h>
#include <swtcliententry.h>
#include <swtfunctor.h>
#include <swtinstancecounts.h>
#include <swtdisplay.h>
#include <jdebug.h>
#include <mmmadirectContent.h>

#include "swtdccontrol.h"


/**
 * Helper class to create CSwtDCControl
 */
class TSWTCreateDCControl: public MSwtFunctor
{
public:
    TSWTCreateDCControl(TSwtPeer aPeer,
                        MSwtDisplay* aDisplay,
                        MSwtComposite* aParent,
                        MMMADirectContent* aContent)
    {
        iPeer = aPeer;
        iDisplay = aDisplay;
        iParent = aParent;
        iContent = aContent;
        iDCControl = NULL;
    }

    void operator()() const
    {
        DEBUG("DCControl__1createDCControl TCreateDCControl()+");
        iDCControl = CSwtDCControl::NewL(*iDisplay,
                                         iPeer,
                                         iParent,
                                         iContent);
        DEBUG("DCControl__1createDCControl TCreateDCControl()-");
    }

    TSwtPeer iPeer;
    MSwtDisplay* iDisplay;
    MSwtComposite* iParent;
    MMMADirectContent* iContent;
    mutable CSwtDCControl* iDCControl;
};

JNIEXPORT jint JNICALL Java_org_eclipse_swt_widgets_DCControl__1createDCControl
(JNIEnv* aJniEnv,
 jobject aPeer,
 jint aParentHandle,
 jint aContentHandle)
{
    DEBUG_INT2("DCControl__1createDCControl parent = %d, content = %d",
               aParentHandle, aContentHandle);

    MSwtComposite* parent =
        reinterpret_cast< MSwtComposite* >(aParentHandle);
    MMMADirectContent* directContent =
        reinterpret_cast< MMMADirectContent* >(aContentHandle);

    MSwtClient* client = NULL;
    TRAP_IGNORE(client = SWT::CreateClientL());
    if (!client)
    {
        return KErrNotFound;
    }

    TSwtPeer peer = aJniEnv->NewWeakGlobalRef(aPeer);

    MSwtDisplay* display = client->Display();

    TSWTCreateDCControl createControl(peer, display, parent, directContent);
    TInt err = client->Execute(createControl);
    DEBUG_INT("DCControl__1createDCControl execute err = %d", err);

    if (err == KErrNone)
    {
        // ownership is transferred
        createControl.iDCControl->SetClient(client, User::Heap());
    }
    else
    {
        delete client;
        return err;
    }

    INCREASE_INSTANCE_COUNT_USE_DISPLAY(createControl.iDCControl, DCControl,
                                        static_cast< CSwtDisplay* >(display));

    return reinterpret_cast< jint >(createControl.iDCControl);
}

JNIEXPORT jint JNICALL Java_org_eclipse_swt_widgets_DCControl__1getControlHandle
(JNIEnv* /*aJniEnv*/, jobject /*aPeer*/, jint aDCControlHandle)
{
    // CBase class need to be casted to MSwtControl to allow usage in Control
    // base class
    CSwtDCControl* dcControl =
        reinterpret_cast< CSwtDCControl* >(aDCControlHandle);
    MSwtControl* control = dcControl;
    return reinterpret_cast< jint >(control);
}

JNIEXPORT void JNICALL Java_org_eclipse_swt_widgets_DCControl__1drawNow
(JNIEnv *, jobject, jint aDCControlHandle)
{
    CSwtDCControl* dcControl =
        reinterpret_cast< CSwtDCControl* >(aDCControlHandle);
    dcControl->MdcRepaint();
}

JNIEXPORT void JNICALL Java_org_eclipse_swt_widgets_DCControl__1setFullScreen
(JNIEnv *, jobject, jint aDCControlHandle, jboolean aFullScreen)
{
    CSwtDCControl* dcControl =
        reinterpret_cast< CSwtDCControl* >(aDCControlHandle);
    dcControl->MdcSetDisplayFullScreen(aFullScreen);
}
