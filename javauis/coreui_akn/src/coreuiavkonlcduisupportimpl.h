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
* Description: A support class implementation for the LCDUI.
*
*/

#ifndef COREUIAVKONLCDUISUPPORTIMPL_H
#define COREUIAVKONLCDUISUPPORTIMPL_H

#include <memory>

#include "coreuiavkonlcduisupport.h"
#include "LcduiThread.h"
#include "monitor.h"

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

class CoreUiAvkonImpl;

NONSHARABLE_CLASS(CoreUiAvkonLcduiSupportImpl) : public CoreUiAvkonLcduiSupport
{
public:
    /**
     * CoreUiAvkonLcduiSupportImpl default constructor.
     */
    CoreUiAvkonLcduiSupportImpl();

    /**
     * CoreUiAvkonLcduiSupportImpl destructor.
     */
    virtual ~CoreUiAvkonLcduiSupportImpl();

    // From CoreUiAvkonLcduiSupport start.
    /**
     * Gets the RLcdui object. Should never
     * be null.
     */
    virtual RLcdui* getRlcdUi();

    /**
     * Gets the handle to event server handle. This handle is a pointer to
     * LCDUI event server object. The handle is is actually a pointer that
     * has been shifted two bits to right (using JavaMakeHandle from jutils.h).
     * When the user wants get the real pointer the result of this method call
     * must be converted by using method JavaUnhand (from jutils.h).
     */
    virtual int getEventServerHandle();
    // From CoreUiAvkonLcduiSupport end.

    /**
     * Gets the heap of the process. This is meant only for debugging purposes.
     * It returns valid heap only if the CoreUi has compiled with flag
     * RD_JAVA_UITHREAD_OWN_HEAP. If the flag is not set, this returns always
     * 0.
     */
    RHeap* getProcessHeap() const;

    /**
     * Gets the heap of the UI thread. This is meant only for debugging purposes.
     * It returns valid heap only if the CoreUi has compiled with flag
     * RD_JAVA_UITHREAD_OWN_HEAP. If the flag is not set, this returns always
     * 0.
     */
    RHeap* getUiThreadHeap() const;

    /**
     * A request to start the UI thread.
     * @param ui A reference to CoreUiAvkonImpl that will be passed to UI
     *           thread.
     */
    void startUiThread(CoreUiAvkonImpl& ui);

    /**
     * A request for sending a close request to the UI thread.
     */
    void closeUi();

private:
    /**
     * An entry point of the event server thread.
     * @param arg A void pointer to object implementing interface MEventServer.
     * @return status.
     */
    static int UiEventServerThread(void* arg);

    int                                 mHandle; // The handle to event server.
    RLcdui                              mRlcdui;
    std::auto_ptr<java::util::Monitor>  mMonitor;
    RHeap*                              mUiHeap;
    RHeap*                              mProcessHeap;
    CoreUiAvkonImpl*                    mCoreUi; // Not owned.

};

} // end namespace ui
} // end namespace java

#endif // COREUIAVKONLCDUISUPPORTIMPL_H
