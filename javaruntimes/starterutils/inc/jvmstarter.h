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
* Description:  This interface is meant for used when starting the JVM.
*
*/


#ifndef JVMSTARTER_H
#define JVMSTARTER_H

#include "javaoslayer.h"

#include <string>
#include <map>

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

/**
 * This class is responsible for generalizing the start of the platform
 * specfic JVM. It provides a set of methods that can be used in platform
 * agnostic way when setting certain arguments passed to the JVM. It also
 * includes a set of methods that assume that the caller sets the arguments
 * in platform specific format.
 * <p>
 * By default the starter sets JVM specific arguments that are optimized
 * for the MIDPruntime. However the class provides methods for overriding
 * these arguments.
 * <p>
 * There are two ways to start the JVM. The recommended way is to first
 * set the arguments using the setter and append methods and the start the
 * JVM. Using this approach there is limited need to implement platform
 * dependent code. This approach also supports the argument modification
 * from some external source during runtime. The other way to start the
 * JVM is to directly start the JVM with platform specific arguments. In this
 * case the arguments are passed directly to JVM wihtout any extra processing.
 * <p>
 * Possibility of overriding JVM arguments during runtime from external source:
 * This feature allows user to modify set of JVM arguments. Before starting
 * the JVM, JvmStarter tries to load javavmargsmodifier plugin. If the
 * loading succeeds, then the plugin is able to provide a list of arguments
 * to be appended or overridden. The actual overriging is done by the
 * starter. It has a control whether to accept the modifications or not.
 * An identifier is a passed to the plugin in order to be able to variate
 * the behavior depending on the runtime. The runtime can be e.g. MIDP,
 * installer, TCK runner or someone else.
 * main() method.
 * <p>
 * Setting JVM into debug mode:
 * This feature seemless to the JvmStarter. It is responsibility of the
 * Runtime specific starter to append the debug arguments using the
 * appendRawJvmArgument() method.
 * <p>
 * JvmStarter is not thread safe.
 * <p>
 * Usage of approach 1:
 * @code
 * std::auto_ptr<JvmStarter> jvm
 *            (JvmStarter::getJvmStarterInstance(JvmStarter::CLDC, L"Midp"));
 * std::wstring classPath(getMidletClassPath());
 * jvm->appendClassPath(classPath);
 * jvm->appendSystemProperty(L"-Dcom.nokia.legacy.support=symbian");
 * jvm->appendApplicationArgument(L"arg1");
 * jvm->appendApplicationArgument(L"arg2");
 * jvm->appendApplicationArgument(L"arg3");
 * jvm->appendApplicationArgument(L"arg4");
 * int status = jvm->startJvm();
 * return status;
 * @endcode
 * <p>
 * Usage of approach 2:
 * @code
 * std::auto_ptr<JvmStarter> jvm
 *          (JvmStarter::getJvmStarterInstance(JvmStarter::CDC, L"CDC_MAIN"));
 * std::vector<char*> args;
 * args.push_back("c:\\");
 * args.push_back("-jcl:cdc11");
 * args.push_back("-classpath");
 * ScopedCharArray cp(JavaCommonUtils::wstringToUtf8(getClassPath()));
 * args.push_back(cp);
 * ScopedCharArray className(JavaCommonUtils::wstringToUtf8(getClassName()));
 * args.push_back(className);
 * args.push_back("App arg1 in UTF-8");
 * args.push_back("App arg2 in UTF-8");
 * const char** argv = &((args)[0]);
 * int argCount = args.size();
 * int status = jvm->startJvm(argCount, argv);
 * return status;
 * @endcode
 */


OS_NONSHARABLE_CLASS(JvmStarter)
{
public:
    /**
    * Enum for defining the used configuration for the JVM
    */
    enum Configuration // codescanner::enummembers::enumnames
    {
        CLDC,
        CDC,
        FOUNDATION,
        UNDEFINED
    };

    /**
     * Creates a platform dependent JvmStarter implementation. This method
     * should be used when approach 1 is selected.
     * @param configuration Used configuration.
     * @param indetifier A string identifier for different runtimes. This
     *        identifier is passed to the JVM args modifier.
     * @return valid pointer to JvmStarter instance. The platform specific
     *         default arguments have been set.
     * @throws std::exception on error cases
     */
    OS_IMPORT static JvmStarter*
    getJvmStarterInstance(const Configuration configuration,
                          const std::wstring& indetifier);

    /**
     * Creates a platform dependent JvmStarter implementation. This method
     * should be used when approach 2 is selected.
     * @return valid pointer to JvmStarter instance.
     * @throws std::exception on error cases
     */
    OS_IMPORT static JvmStarter* getJvmStarterInstance();

    /**
     * Destructor of the JvmStarter.
     */
    virtual ~JvmStarter() {}

    /**
     * Appends the provided file to the classpath. It is responsisbility of
     * the user to provide fully qualified classpath. The path separator
     * can be either '/' or '\' - the method will convert it to correct
     * format.
     * @param file Fully qualified file name to be appended to classpath.
     *             Assumed character encoding is UCS-2.
     * @throws std::exception on error cases
     */
    virtual void appendClassPath(const std::wstring& file) = 0;

    /**
     * Appends the provided file to the bootclasspath. The rules in
     * method appendClassPath() are also valid for this method.
     * @param file Fully qualified file name to be appended
     *             to bootclasspath.
     *             Assumed character encoding is UCS-2.
     * @throws std::exception on error cases
     */
    virtual void appendBootClassPath(const std::wstring& file) = 0;

    /**
     * Prepends the provided file to the bootclasspath. The rules in
     * method appendClassPath() are also valid for this method.
     * @param file Fully qualified file name to be appended
     *             to bootclasspath.
     *             Assumed character encoding is UCS-2.
     * @throws std::exception on error cases
     */
    virtual void prependBootClassPath(const std::wstring& file) = 0;

    /**
     * Appends the provided fully qualified path to be part of the extension
     * path of the JVM. The path separator can be either '/' or '\' -
     * the method will convert it to platform specific format.
     *
     * @param file Fully qualified file name to be appended
     *             to extension path.
     *             Assumed character encoding is UCS-2.
     * @throws std::exception on error cases
     */
    virtual void appendExtensionPath(const std::wstring& extensionPath) = 0;

    /**
     * Sets the main class of the runtime to be started.
     * The package seprator is '.'
     * @param mainClass The Java class containing the main() method to be
     *                  invoked.
     *                  Assumed character encoding is UCS-2.
     * @throws std::exception on error cases
     */
    virtual void setMainClass(const std::wstring& mainClass) = 0;

    /**
     * Appens argument that are the arguments of the Java main() method.
     * @param argument Argument to be appended.
     *                 Assumed character encoding is UCS-2.
     * @throws std::exception on error cases
     */
    virtual void appendApplicationArgument(const std::wstring& argument) = 0;

    /**
     * Adds system property. The property must be in format '-D<key>[:value]
     * @param systemproperty System property to be added.
     *                       Assumed character encoding is UCS-2.
     * @throws std::exception on error cases
     */
    virtual void appendSystemProperty(const std::wstring& systemproperty) = 0;

    /**
     * Starts the JVM. Before this method is called all the necessary
     * initializations must have been done. The method will block until the
     * JVM is shut down. If the JVM is not able to shut down
     * because some non daemon java thread is blocking the shut down, this
     * method will never return.
     * @return The status of the main method returned by the JVM
     * @throws std::exception on error cases
     */
    virtual int startJvm() = 0;

    /**
     * Starts the JVM with specified arguments. This call leads directly
     * launching the platform specific JVM. Caller of this method is
     * responsible for setting the arguments in correct JVM specific format.
     * The used encoding of the arguments is UTF-8. This is a raw way to
     * start the JVM. There usage of the other argument setter methods have
     * no effect to the arguments. The support of modifying the arguments
     * from some external source is not supported when this method is called.
     * The method will block until the JVM is shut down. If the JVM is not
     * able to shut down because some non daemon java thread is blocking
     * the shut down, this method will never return.
     * @param argc The number of the arguments.
     * @param argv Array of UTF-8 encoded char arrays.
     * @return The status of the main method returned by the JVM
     * @throws std::exception on error cases
     */
    virtual int startJvm(int argc, char** argv) = 0;

    /**
     * Overrides the default old heap size.
     * @param heapSize Heap size in kilo bytes.
     * @throws std::exception on error cases
     */
    virtual void overrideOldHeapSize(int heapSize) = 0;

    /**
     * Overrides the default new heap size.
     * @param heapSize Heap size in kilo bytes.
     * @throws std::exception on error cases
     */
    virtual void overrideNewHeapSize(int heapSize) = 0;

    /**
     * Overrides the default stack of the native thread.
     *
     * @param stackSize Stack size in kilo bytes.
     * @throws std::exception on error cases
     */
    virtual void overrideNativeStackSize(int stackSize) = 0;

    /**
     * Overrides the default stack of the java thread.
     *
     * @param stackSize Stack size in kilo bytes.
     * @throws std::exception on error cases
     */
    virtual void overrideJavaStackSize(int stackSize) = 0;

    /**
     * Adds or overwrites the defined JVM argument. It is reponsibility
     * of the caller that the passed argument is supported by the used
     * JVM.
     *
     * @param argument JVM argument.
     * @throws std::exception on error cases
     */
    virtual void appendRawJvmArgument(const std::wstring& argument) = 0;

    /**
     * Sets the VM to run without JIT
     */
    virtual void disableJit() = 0;

    /**
     * Enables thread dumping feature if possible.
     */
    virtual void enableThreadDumping() = 0;
};

} // end namespace runtime
} // end namespace java


#endif // JVMSTARTER_H
