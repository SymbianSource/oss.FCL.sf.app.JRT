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
* Description:  This class is meant for starting the Linux JVM.
*
*/

#include <string>
#include <algorithm>

#include "jvmstarterjni.h"

#include "jvmargsmodifier.h"

#include "runtimeexception.h"
#include "logger.h"
#include "javaoslayer.h"
#include "dynamiclibloader.h"
#include "javacommonutils.h"

using namespace java::runtime;
using namespace java::util;

extern const wchar_t CLASS_PATH_SEPARATOR = L':';
extern const char PATH_SEPARATOR_FROM  = '\\';
extern const char PATH_SEPARATOR_TO    = '/';

JvmStarter*
JvmStarter::getJvmStarterInstance(const Configuration configuration,
                                  const std::wstring& indetifier)
{
    JELOG2(EJavaRuntime);
    return  new JvmStarterJni(configuration, indetifier);
}

JvmStarter*
JvmStarter::getJvmStarterInstance()
{
    JELOG2(EJavaRuntime);
    return new JvmStarterJni();
}

JvmStarterJni::JvmStarterJni()
{
    JELOG2(EJavaRuntime);
}

JvmStarterJni::JvmStarterJni(const Configuration configuration,
                             const std::wstring& indetifier)
{
    JELOG2(EJavaRuntime);
    mConfiguration = configuration;
    mIdentifier = indetifier;

    // In Linux the binary root varies depending on the user.
    std::string rootStr;
    JavaOsLayer::getResRoot(rootStr, false);
    rootStr += "jsr/classes/common/";
    mExtensionPath.assign(rootStr.begin(), rootStr.end());
}


JvmStarterJni::~JvmStarterJni()
{
    JELOG2(EJavaRuntime);
}

void JvmStarterJni::overrideOldHeapSize(int /*heapSize*/)
{
    // Not supported by the JVM
    JELOG2(EJavaRuntime);
}

void JvmStarterJni::overrideNewHeapSize(int /*heapSize*/)
{
    // Not supported by the JVM
    JELOG2(EJavaRuntime);
}

void JvmStarterJni::overrideNativeStackSize(int /*stackSize*/)
{
    // Not supported by the JVM
    JELOG2(EJavaRuntime);
}

void JvmStarterJni::overrideJavaStackSize(int stackSize)
{
    JELOG2(EJavaRuntime);
    std::wstring stackSizeStr = L"-Xss";
    stackSizeStr += JavaCommonUtils::intToWstring(stackSize);
    stackSizeStr += L"K";
    mJvmArgs.push_back(stackSizeStr);
}

int JvmStarterJni::startJvm()
{
    JELOG2(EJavaRuntime);

    // Set mJvmArgs container to contain all the JVM args and set mAppAndArgs
    // to contain the main class and the arguments.
    completeArgumentContainers();

    // Give arguments to modifyJvmArguments for modification. Args
    // are modified if the default empty implementation has been overridden
    // by eclipsing the modifyJvmArguments dll.
    modifyJvmArguments(mIdentifier, mJvmArgs, mAppAndArgs);

    // Allocate space for the raw JVM args.
    int rawJvmArgumentCount = mJvmArgs.size();
    ScopedCharPointerArray rawJvmArgs(rawJvmArgumentCount);

    // Adding the JVM args. Main class and applcation arguments are handled
    // later.
    int ind = 0;
    for (JvmArgs_t::iterator jvmArgsIter = mJvmArgs.begin();
            jvmArgsIter!= mJvmArgs.end();
            ++jvmArgsIter)
    {
        // Do character conversion while adding the arguments.
        rawJvmArgs.get()[ind++] = JavaCommonUtils::wstringToUtf8(*jvmArgsIter);
    }
    return startJvmInSeparateThread(rawJvmArgumentCount, rawJvmArgs.get());
}

int JvmStarterJni::startJvm(int argc, char** argv)
{
    JELOG2(EJavaRuntime);
    // Allocate space for the raw JVM args. This will contain only
    // JVM arguments. Main class and application arguments are handled
    // differently.
    ScopedCharPointerArray rawJvmArgs(argc);

    // Assuming that in the beginning of the list there are JVM arguments and
    // they start with '-'.
    bool handlingArguments = true;

    int argCount = 0;
    for (int i = 0; i < argc; i++)
    {
        if (argv[i][0] != '-') // codescanner::accessArrayElementWithoutCheck2
        {
            // First non JVM argument was found.
            handlingArguments = false;
        }
        if (handlingArguments)
        {
            if ((strcmp(argv[i], "-cp") == 0 || // codescanner::accessArrayElementWithoutCheck2
                    strcmp(argv[i], "-classpath") == 0) && // codescanner::accessArrayElementWithoutCheck2
                    (i + 1) < argc)
            {
                // There is class path available.
                std::string cp("-Djava.class.path=");
                cp += argv[i+1];
                i++;
                rawJvmArgs.get()[argCount] = strdup(cp.c_str());
            }
            else
            {
                // Normal JVM argument.
                rawJvmArgs.get()[argCount] = strdup(argv[i]); // codescanner::accessArrayElementWithoutCheck2
            }
            argCount++;
        }
        else
        {
            // The first non JVM argument can be considered as main class.
            // The rest of the arguments are arguments for the Java app.
            mAppAndArgs.push_back(JavaCommonUtils::utf8ToWstring(argv[i])); // codescanner::accessArrayElementWithoutCheck2
        }
    }
    return startJvmInSeparateThread(argCount, rawJvmArgs.get());
}


void* JvmStarterJni::javaThreadMain(void* arg)
{
    JELOG2(EJavaRuntime);
    int result = -1;
    JvmStarterJni* jniStarter = (reinterpret_cast<JvmStarterJni*>(arg));
    try
    {
        result = jniStarter->startJvmImpl();
    }
    catch (RuntimeException& e)
    {
        ELOG1(EJavaRuntime, "JvmStarterJni::javaThreadMain() RuntimeException "
              "catched in VM thread. %s", e.toString().c_str());
    }
    catch (java::util::ExceptionBase& e)
    {
        ELOG1(EJavaRuntime, "JvmStarterJni::javaThreadMain() ExceptionBase "
              "catched in VM thread. %s", e.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime, "JvmStarterJni::javaThreadMain() std::Exception "
              "catched in VM thread. %s", e.what());
    }
    return reinterpret_cast<void*>(result);
}

int JvmStarterJni::startJvmInSeparateThread(int argc, char** argv)
{
    JELOG2(EJavaRuntime);

    // Store the argc and argv into member variables in order be avaliable
    // for the JVM starter thread.
    mArgCount = argc;
    mArgs = argv;
    // return startJvmImpl(); This could be used to start the JVM into same
    // thread.
    pthread_t threadId;
    void*     result;

    // Create the JVM thread.
    pthread_create(&threadId, 0, javaThreadMain, this);

    // Wait until the thread has died.
    pthread_join(threadId, &result);
    return reinterpret_cast<int>(result);
}


int JvmStarterJni::startJvmImpl()
{
    JELOG2(EJavaRuntime);

    JavaVM* jvm;            // Denotes a Java VM.
    JNIEnv* env;            // Pointer to native method interface.
    JavaVMInitArgs vmArgs;  // VM initialization arguments.
    JavaVMOption vmOption;
    vmOption.extraInfo = 0;

    LOG(EJavaRuntime, EInfo, "VM args:");
    JvmOptionArgs_t jvmOptions;
    for (int i = 0; i < mArgCount; i++)
    {
        vmOption.optionString = mArgs[i];
        jvmOptions.push_back(vmOption);
        LOG1(EJavaRuntime, EInfo, " %s",mArgs[i]); // codescanner::accessArrayElementWithoutCheck2
    }
    vmArgs.version = JNI_VERSION_1_4;

    // Initializing JavaVMInitArgs.
    // Contiguity for std::vector<T> is mandated by the standard as long
    // as T is not bool. [See 23.2.4./1].
    vmArgs.options = &((jvmOptions)[0]); // codescanner::accessArrayElementWithoutCheck2
    vmArgs.nOptions = jvmOptions.size();
    vmArgs.ignoreUnrecognized = JNI_FALSE;

    JavaOsLayer::startUpTrace("Starting VM()", -1, -1);

    // Creating the JVM.
    int res = JNI_CreateJavaVM(&jvm, reinterpret_cast<void**>(&env), &vmArgs);
    LOG1(EJavaRuntime, EInfo, "JNI_CreateJavaVM() returned. st = %d", res);
    if (res == 0)
    {
        // Converting the '.' to '/' in the main class
        // (com.nokia.Foo -> com/nokia/Foo)
        std::wstring& appMain = mAppAndArgs.front();
        std::replace(appMain.begin(), appMain.end(), '.', '/');

        // Convert the main class to UTF-8.
        ScopedCharPointer main(JavaCommonUtils::wstringToUtf8(appMain));

        // Find the main class.
        jclass mainClass = env->FindClass(main.get());
        LOG2(EJavaRuntime, EInfo, " mainClass (%s): %p", main.get(), mainClass);
        if (mainClass != 0)
        {
            // Find method static void main(String[] args) from the main class.
            jmethodID mainMethod = env->GetStaticMethodID(mainClass, "main",
                                   "([Ljava/lang/String;)V");
            LOG1(EJavaRuntime, EInfo, " mainMethod: %p", mainMethod);
            if (mainMethod != 0)
            {
                // Call the method static void main().
                env->CallStaticVoidMethod(mainClass, mainMethod,
                                          getApplicationArguments(env));
                LOG(EJavaRuntime, EInfo, " CallStaticVoidMethod returned");
            }
            else
            {
                std::string errorStr("Not able to find main() method.");
                throw RuntimeException(errorStr,
                                       __FILE__, __FUNCTION__, __LINE__);
            }
        }
        else
        {
            std::string errorStr("Main class was not found.");
            throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
        jvm->DestroyJavaVM();
    }
    else
    {
        std::string errorStr("JNI_CreateJavaVM failed. Reason = .");
        errorStr += JavaCommonUtils::intToString(res);
        throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
    return res;
}


void JvmStarterJni::completeArgumentContainers()
{
    JELOG2(EJavaRuntime);

    // Set the used porting layer.
    mJvmArgs.push_back(L"-Dcom.nokia.jvm.port=sun.JvmPortJ2se");

    // Disable JIT, if requested.
    if (mJitDisabled)
    {
        mJvmArgs.push_back(L"-Xint");
    }

    // Add the classpath.
    if (mClassPath.length() > 0)
    {
        mClassPath.insert(0, L"-Djava.class.path=");
        mJvmArgs.push_front(mClassPath);
        LOG1(EJavaRuntime, EInfo, " mClassPath = %S", mClassPath.c_str());
    }

    // Add the extension classpath.
    if (mExtensionPath.length() > 0)
    {
        mExtensionPath.insert(0, L"-Djava.ext.dirs=");
        mJvmArgs.push_front(mExtensionPath);
        LOG1(EJavaRuntime, EInfo, " mExtensionPath = %S",
             mExtensionPath.c_str());
    }

    // Add the prepending boot classpath if set.
    if (mBootClassPathPrepend.length() > 0)
    {
        std::wstring bcpp(L"-Xbootclasspath/p:");
        bcpp += mBootClassPathPrepend;
        mJvmArgs.push_front(bcpp);
        LOG1(EJavaRuntime, EInfo, " bcpp = %S", bcpp.c_str());
    }

    // Add the appending boot classpath if set.
    if (mBootClassPathAppend.length() > 0)
    {
        std::wstring bcpa(L"-Xbootclasspath/a:");
        bcpa += mBootClassPathAppend;
        mJvmArgs.push_front(bcpa);
        LOG1(EJavaRuntime, EInfo, " bcpa = %S", bcpa.c_str());
    }

    std::wstring javaBinRoot;
    std::string binRoot;
    JavaOsLayer::getBinRoot(binRoot, false);
    javaBinRoot.assign(binRoot.begin(), binRoot.end());

    // Setting the java.library.path.
    std::wstring jlp(L"-Djava.library.path=");
    jlp += javaBinRoot;
    jlp +=  L"lib";
    mJvmArgs.push_front(jlp);
    LOG1(EJavaRuntime, EInfo, " jlp = %S", jlp.c_str());

    // Setting the java.home.
    std::wstring jh = L"-Djava.home=";
    const char* javaHome = getenv("JAVA_VM_HOME");
    if (javaHome == 0)
    {
        throw RuntimeException("JAVA_VM_HOME not defined",
                               __FILE__, __FUNCTION__, __LINE__);
    }
    std::string jhs(javaHome);
    jh.append(jhs.begin(), jhs.end());
    mJvmArgs.push_front(jh);
    LOG1(EJavaRuntime, EInfo, " jh = %S", jh.c_str());

    // Define emma.properties to point to emma.properties file
    // which is used when Java code coverage is measured.
    std::wstring emma(L"-Demma.properties=");
    emma += javaBinRoot;
    emma +=  L"emma.properties";
    mJvmArgs.push_front(emma);
    LOG1(EJavaRuntime, EInfo, " emma = %S", emma.c_str());

    // Add the main class.
    mAppAndArgs.push_front(mMainClass);
    LOG1(EJavaRuntime, EInfo, " mMainClass = %S", mMainClass.c_str());
}

jobjectArray JvmStarterJni::getApplicationArguments(JNIEnv* env)
{
    JELOG2(EJavaRuntime);
    // When using JNI_CreateJavaVM to start the JVM the arguments for the
    // Java application must be handled so that a jobjectArray is created
    // to contain Java Strings created using jni APIs.

    if (env == 0)
    {
        throw RuntimeException("JNIEnv was null",
                               __FILE__, __FUNCTION__, __LINE__);
    }

    // Create the jobjectArray. mAppAndArgs container contains the main
    // class which must be taken into account.
    jobjectArray array =
        (jobjectArray)env->NewObjectArray(mAppAndArgs.size() - 1,
                                          env->FindClass("java/lang/String"),
                                          env->NewStringUTF(""));
    LOG(EJavaRuntime, EInfo, "  Application arguments:");
    if (array != 0)
    {
        JvmArgs_t::iterator appAndArgsIter = mAppAndArgs.begin();
        // Skip the main class.
        ++appAndArgsIter;

        int i = 0;

        // Fill the object array with application arguments.
        for (; appAndArgsIter!= mAppAndArgs.end(); ++appAndArgsIter)
        {
            ScopedCharPointer appArg(JavaCommonUtils::wstringToUtf8(*appAndArgsIter));
            LOG1(EJavaRuntime, EInfo, "   %s", appArg.get());
            env->SetObjectArrayElement(array, i++,
                                       env->NewStringUTF(appArg.get()));
        }
    }
    else
    {
        throw RuntimeException("jobjectArray was null",
                               __FILE__, __FUNCTION__, __LINE__);
    }
    return array;
}


