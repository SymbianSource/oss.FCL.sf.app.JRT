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
* Description: Interface for creating and destroying the Core UI
*
*/

#ifndef JAVACOREUI_H
#define JAVACOREUI_H

#include <memory>

#include "dynamiclibloader.h"
#include "javauid.h"
#include "javacoreuiparams.h"

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
    * @return A reference to instance implementing CoreUi in
    * success case.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    static CoreUi&
    getUiInstance(std::auto_ptr<java::util::DynamicLibLoader>& loader);

    /**
    * This inline method will close the core UI unless already closed and
    * clean resources.
    *
    * @param[in] loader A loeader that was used to load the dll.
    * @throws java::util::ExceptionBase or std::exception in error cases.
    */
    static void releaseUi(const std::auto_ptr<java::util::DynamicLibLoader>& loader);
};

#if defined RD_JAVA_UI_QT
// This is an empty CoreUi impl for Java 3.0
class CoreUiStub : public CoreUi
{
public:
    CoreUiStub() {}
    virtual ~CoreUiStub() {}
    virtual void init(const java::util::Uid& /*midletUid*/,
                      CoreUiParams* /*uiParams*/) {}

    virtual void start(const java::util::Uid& appUid,
                       CoreUiParams* uiParams = 0) {}
};
#endif // RD_JAVA_UI_QT

} //end namespace ui
} //end namespace java


//START OF INLINE METHODS
inline java::ui::CoreUi::~CoreUi() {}

inline java::ui::CoreUi& java::ui::CoreUi::getUiInstance
(std::auto_ptr<java::util::DynamicLibLoader>& loader)
{
#if defined RD_JAVA_UI_QT

    // The stub impl leaks memory, but is ok, since this
    // is not in use in MCL, but is for future development
    // enabler.
    CoreUiStub* stub = new CoreUiStub();
    return *stub;

#else // RD_JAVA_UI_QT

    if (loader.get() == 0)
    {
        //Create an instance of DynamicLibLoader.
        loader.reset(new java::util::DynamicLibLoader("javacoreui"));
    }

    //Load the javaui and locates method getUiInstance. If getFunction
    //succeeds were are certain that createUiFunc points to valid method.
    GetUiInstance getUiInstance =
        reinterpret_cast<GetUiInstance>(loader->getFunction("getUiInstance",
                                        true));

    //Call the method which will create the UI.
    return getUiInstance();

#endif // RD_JAVA_UI_QT
}

inline void java::ui::CoreUi::releaseUi(
    const  std::auto_ptr<java::util::DynamicLibLoader>& loader)
{
#ifndef RD_JAVA_UI_QT
    //Load the javaui and locates method getUiInstance. If getFunction
    //succeeds were are certain that createUiFunc points to valid method.
    ReleaseUi releaseUi =
        reinterpret_cast<ReleaseUi>(loader->getFunction("releaseUi", true));

    //Call the method which will release UI resources.
    releaseUi();

#else // RD_JAVA_UI_QT

    return;

#endif // RD_JAVA_UI_QT
}

#endif // JAVACOREUI_H
