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

#ifndef COREUITESTIMPL_H
#define COREUITESTIMPL_H


#include "javacoreui.h"

#ifdef __SYMBIAN32__

#include "javasymbianoslayer.h"
/**
 * For accessing the static methods using dynamic dll loading.
 * @param funcName The name of the method to be searched.
 * @return a pointer to found function, 0 if not found.
 */
IMPORT_C FuncPtr findDllMethod(const char* funcName);

#endif //__SYMBIAN32__

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

OS_NONSHARABLE_CLASS(CoreUiTestHandler)
{
public:
    virtual void startCalled(const java::util::Uid& midletUid,
                             CoreUiParams* uiParams) = 0;
    virtual void releaseCalled() = 0;
    virtual void error(const std::string& info) = 0;
};

OS_NONSHARABLE_CLASS(CoreUiTestImpl) : public CoreUi
{
public:
    /**
     * CoreUiTestImpl default constructor.
     */
    CoreUiTestImpl();

    /**
     * CoreUiTestImpl destructor.
     */
    virtual ~CoreUiTestImpl();

    OS_IMPORT static void setTestHandler(CoreUiTestHandler* testHadler);

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

public:
    CoreUiTestHandler* mTestHandler;
};


} // end namespace ui
} // end namespace java


#endif // COREUITESTIMPL_H
