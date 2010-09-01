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
* Description: A common interface for the toolkits to access the core UI.
*
*/

#ifndef COREUIAVKON_H
#define COREUIAVKON_H

#include"coreuiappui.h"
#include "javacoreuiparams.h"

class CApaWindowGroupName;

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

class JavaUiAvkonContainer;
class JavaUiAvkonAppUi;
class CoreUiParams;


/**
 * CoreUiAvkon offers an interface for the toolkits (LCDUI & eSWT) for
 * accessing the CorUiAvkon. This is a base class and there are toolkit
 * specific interfaces that extend this interface. The toolkit specific
 * interface provides a toolkit specific static accessor to get a
 * reference to CoreUiAvkon instance.
 */
NONSHARABLE_CLASS(CoreUiAvkon)
{
public:

    /**
     * CoreUiAvkon default constructor.
     */
    CoreUiAvkon() {}

    /**
     * CoreUiAvkon destructor.
     */
    virtual ~CoreUiAvkon() {}

    /**
     * Ensures that the UI has been created. After returning from this method
     * the user can be sure that the UI is available.
     * @throws std::exception on error cases.
     */
    virtual void ensureInitialized(const TUid& appUid, CoreUiParams* params = 0) = 0;

    /**
     * A getter for the CoreUiAvkonAppUi instance.
     * @return A pointer to CoreUiAvkonAppUi.
     */
    virtual CoreUiAvkonAppUi* getJavaUiAppUi() const = 0;

    /**
     * A getter for the CAknAppUi instance.
     * @return A pointer to CAknAppUi.
     */
    virtual CAknAppUi* getJavaAknAppUi() const = 0;

    /**
     * A getter for the CApaWindowGroupName instance.
     * @return A pointer to CApaWindowGroupName.
     */
    virtual CApaWindowGroupName* getWindowGroupName() const = 0;

    /**
     * A request for disposing the UI. The UI will be disposed if
     * all the childs has been unregistered. If this leads to disposing
     * the UI it should be noted, that after returning from this method
     * there has been sent a request to UI thread, that it should shut down.
     * There might be a small gap until the UI is really closed.
     */
    virtual void dispose() = 0;

    /**
     * For informing the coreUI that a shutdown event from the window server
     * has been received. The coreUi informs the corresponding JavaRuntime
     * about the shutdown event an ensures that the application is eventually
     * closed.
     */
    virtual void shutDownRequestFromWindowServer() = 0;

    /**
     * A method for checking whether the Coreui _instance_ has been created.
     * Even if the instance would have been created, it doesn't necessarily
     * mean that the UI has been created.
     * @return true if the instance has been created, false otherwise.
     */
    IMPORT_C static bool isCoreUiCreated();

    /**
     * Gets the heap of the process. This is meant only for debugging purposes.
     * It returns valid heap only if the CoreUi has compiled with flag
     * RD_JAVA_UITHREAD_OWN_HEAP. If the flag is not set, this returns always
     * 0.
     */
    virtual RHeap* getProcessHeap() const = 0;

    /**
     * Gets the heap of the UI thread. This is meant only for debugging purposes.
     * It returns valid heap only if the CoreUi has compiled with flag
     * RD_JAVA_UITHREAD_OWN_HEAP. If the flag is not set, this returns always
     * 0.
     */
    virtual RHeap* getUiThreadHeap() const = 0;

};

} // end namespace ui
} // end namespace java

#endif // COREUIAVKON_H
