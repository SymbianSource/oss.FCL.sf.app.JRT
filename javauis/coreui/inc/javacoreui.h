/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Interface for creating and destroying the Core UI
*
*/

#ifndef JAVACOREUI_H
#define JAVACOREUI_H

#include <memory>

#include "libraryloaderexception.h"
#include "dynamiclibloader.h"
#include "javauid.h"
#include "javacoreuiparams.h"
#include "logger.h"

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

class CoreUi;

typedef CoreUi& (*GetUiInstance)();
typedef void (*ReleaseUi)();

/**
 *  CoreUi offers a static method for getting an object
 *  implementing
 *  the interface and loading the shared library containing the actual
 *  implementation. The interface itself offers a way control the lifetime
 *  of the UI.
 */

class CoreUi
{
public:

    /**
    * This method will ask the UI to start the main loop
    * @param appUid The Uid of the application.
    * @param uiParams Ui configuration parameters. If it
    *        is null, the default arguments will be used.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    virtual void start(const java::util::Uid& appUid,
                       CoreUiParams* uiParams = 0) = 0;

    //Destructor
    virtual ~CoreUi();

    /**
    * This inline method will load the shared library of the core UI.
    *
    * It is important that the user of this method is aware that the
    * implementation is available ONLY when the instance of
    * DynamicLibLoader is not deleted. When the user is absolutely sure
    * that no code is run from the loaded shared library, user should delete
    * the instance of DynamicLibLoader. The deletion will lead to
    * unloading the shared library.
    *
    * @param[out] loader User must store this argument as long as it needs
    *                    to run any code in the shared library. No other
                         APIs of the loader is needed to use.
    * @return true if the dll could be loaded and the start method was
    *         succesfully called.
    * success case.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    static bool start(std::auto_ptr<java::util::DynamicLibLoader>& loader,
                      const java::util::Uid& appUid,
                      CoreUiParams* uiParams = 0);

    /**
    * This inline method will close the core UI unless already closed and
    * clean resources.
    *
    * @param[in] loader A loeader that was used to load the dll.
    * @return true if the dll could be loaded and the release method was
    *         succesfully called.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    static bool releaseUi(std::auto_ptr<java::util::DynamicLibLoader>& loader);
};

} // end namespace ui
} // end namespace java


const char* const COREUI_LIB_NAME = "javacoreui";
// START OF INLINE METHODS
inline java::ui::CoreUi::~CoreUi() {}

inline bool java::ui::CoreUi::start(
    std::auto_ptr<java::util::DynamicLibLoader>& loader,
    const java::util::Uid& appUid,
    CoreUiParams* uiParams)
{
    if (loader.get() == 0)
    {
        // Create an instance of DynamicLibLoader.
        loader.reset(new java::util::DynamicLibLoader(COREUI_LIB_NAME));
    }

    try
    {
        // Load the javaui and locates method getUiInstance. If getFunction
        // succeeds were are certain that createUiFunc points to valid method.
        GetUiInstance getUiInstance =
            reinterpret_cast<GetUiInstance>(loader->getFunction("getUiInstance",
                                            true));
        // Call the method which will create the UI.
        getUiInstance().start(appUid, uiParams);
        return true;
    }
    catch (java::util::LibraryLoaderException& ex)
    {
        loader.reset();
        LOG1(EJavaUI, EInfo, "No coreUi available (start): %s", ex.toString().c_str());
    }
    return false;
}

inline bool java::ui::CoreUi::releaseUi(
    std::auto_ptr<java::util::DynamicLibLoader>& loader)
{
    if (loader.get() == 0)
    {
        // Create an instance of DynamicLibLoader.
        loader.reset(new java::util::DynamicLibLoader(COREUI_LIB_NAME));
    }
    try
    {
        ReleaseUi releaseUi =
            reinterpret_cast<ReleaseUi>(loader->getFunction("releaseUi", true));

        // Call the method which will release UI resources.
        releaseUi();
        return true;
    }
    catch (java::util::LibraryLoaderException& ex)
    {
        loader.reset();
        LOG1(EJavaUI, EInfo, "No coreUi available (release): %s", ex.toString().c_str());
    }
    return false;
}

#endif // JAVACOREUI_H
