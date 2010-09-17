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
* Description: A CoreUi avkon implementation.
*
*/

#ifndef COREUIQTIMPL_H
#define COREUIQTIMPL_H


#include "javacoreui.h"
#include "javasymbianoslayer.h"

#ifdef __SYMBIAN32__

/**
 * For accessing the static methods using dynamic dll loading.
 * @param funcName The name of the method to be searched.
 * @return a pointer to found function, 0 if not found.
 */
IMPORT_C FuncPtr findDllMethod(const char* funcName);

#endif //__SYMBIAN32__

/**
 * For getting a sinqleton instance of the CoreUi.
 */
java::ui::CoreUi& getUiInstance();

/**
 * Releases the UI. This means that the UI will be closed (if not done yet)
 * and freese all the resources.
 */
void releaseUi();

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

/**
 * The implementation of the CoreUiAvkon. This class provides CoreUi services
 * to CoreUi users.
 */
NONSHARABLE_CLASS(CoreUiQtImpl) : public CoreUi
{
public:
    /**
     * CoreUiQtImpl default constructor.
     */
    CoreUiQtImpl();

    /**
     * CoreUiQtImpl destructor.
     */
    virtual ~CoreUiQtImpl();

public: // From CoreUi
    /**
    * This method will ask the UI to start the splash screen.
    * @param appUid The Uid of the application.
    * @param uiParams Ui configuration parameters. If it
    *        is null, the default arguments will be used.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    virtual void start(const java::util::Uid& midletUid,
                       CoreUiParams* uiParams);
};


} // end namespace ui
} // end namespace java


#endif // COREUIQTIMPL_H
