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
* Description:  a Base class for starting the JVM.
*
*/


#ifndef JVMSTARTERIMPL_H
#define JVMSTARTERIMPL_H

#include "jvmstarter.h"

#include <algorithm>
#include <string.h>
#include <string>
#include <list>

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

/**
 * This class implements partially the interface JvmStarter. The class mainly
 * contains argument storing and modifying features. The static instance
 * getters and virtual JVM starting methods are left to be implemented by
 * the JVM specific starter which will be inherited from this class.
 */


OS_NONSHARABLE_CLASS(JvmStarterImpl) : public JvmStarter
{
public:

    /**
     * Destructor of the JvmStarterImpl.
     */
    virtual ~JvmStarterImpl();

    /**
     * @see jvmstarter.h
     */
    virtual void appendClassPath(const std::wstring& file);

    /**
     * @see jvmstarter.h
     */
    virtual void appendBootClassPath(const std::wstring& file);

    /**
     * @see jvmstarter.h
     */
    virtual void prependBootClassPath(const std::wstring& file);

    /**
     * @see jvmstarter.h
     */
    virtual void appendExtensionPath(const std::wstring& extensionPath);

    /**
     * @see jvmstarter.h
     */
    virtual void setMainClass(const std::wstring& mainClass);

    /**
     * @see jvmstarter.h
     */
    virtual void appendApplicationArgument(const std::wstring& argument);

    /**
     * @see jvmstarter.h
     */
    virtual void appendSystemProperty(const std::wstring& systemproperty);

    /**
     * @see jvmstarter.h
     */
    virtual void appendRawJvmArgument(const std::wstring& argument);

    /**
     * @see jvmstarter.h
     */
    virtual void disableJit();

    /**
     * @see jvmstarter.h
     */
    virtual void enableThreadDumping();

protected: // Definitions

    /**
     * Clears all the std string & Lists.
     */
    virtual void clear();

    typedef std::list<std::wstring> JvmArgs_t;

    // A simple class for storing char* array that is deleted automatically
    // when going out of scope.
    class ScopedCharPointerArray
    {
    public:

        ScopedCharPointerArray(int size) : mBuffer(0), mSize(size)
        {
            mBuffer = new char* [size];
            memset(mBuffer, 0, mSize * sizeof(char*));
        }

        ~ScopedCharPointerArray()
        {
            for (int i = 0; i < mSize; i++)
            {
                delete[] mBuffer[i];
            }
            delete[] mBuffer;
        }

        char** get()
        {
            return mBuffer;
        }

    private:

        /*
         * No copy constructor allowed.
         */
        ScopedCharPointerArray(const ScopedCharPointerArray&);

        /*
         * No Assignment operator allowed.
         */
        ScopedCharPointerArray& operator= (const ScopedCharPointerArray&);

    private:
        char** mBuffer;
        int    mSize;
    };

    // A simple class for storing char* pointers that is deleted automatically
    // when going out of scope.
    class ScopedCharPointer
    {
    public:

        ScopedCharPointer()
        {
        }

        ScopedCharPointer(char* ptr) : mBuffer(ptr)
        {
        }

        ~ScopedCharPointer()
        {
            delete[] mBuffer;
        }

        void set(char* ptr)
        {
            mBuffer = ptr;
        }

        char* get()
        {
            return mBuffer;
        }

    private:

        /*
         * No copy constructor allowed.
         */
        ScopedCharPointer(const ScopedCharPointer&);

        /*
         * No Assignment operator allowed.
         */
        ScopedCharPointer& operator= (const ScopedCharPointer&);

    private:
        char* mBuffer;
    };


protected: //Methods
    JvmStarterImpl();   // Default constructor allowed for inherited classes

    /**
     * Appends the given source path to the given destination path. If there
     * already exists a path a platform specific classpath separator is added
     * before doing the appending Converts the path separtors to platform
     * specifc ones.
     * @param source The source path to be appended.
     * @param destination The destination path where to be appended.
     */
    void appendAndConvertClassPath(const std::wstring& source,
                                   std::wstring& destination);


protected: // Members

    std::wstring      mIdentifier;        // For identifying the runtime.
    std::wstring      mClassPath;         // The classpath of the application
    std::wstring      mBootClassPathAppend;  // The appended boot classpath
    std::wstring      mBootClassPathPrepend; // The prepended boot classpath
    std::wstring      mExtensionPath;     // The extension path to be used
    std::wstring      mMainClass;         // The main class of the application
    bool              mJitDisabled;       // Is the JIT disabled
    bool              mTheadDumpEnabled;  // Is the thread dump feature enabled
    Configuration     mConfiguration;     // CLDC or CDC
    JvmArgs_t         mJvmArgs;           // A list containing Jvm arguments.
    // This doesn't contain such
    // that has dedicated member variable
    // like the classpath
    JvmArgs_t         mAppAndArgs;        // A list containing application
    // class and arguments for the
    // application

private: // Methods
    /**
     * No copy constructor allowed
     */
    JvmStarterImpl(const JvmStarterImpl&);

    /**
     * No Assignment operator allowed
     */
    JvmStarterImpl& operator= (const JvmStarterImpl&);

};

} // end namespace runtime
} // end namespace java


#endif // JVMSTARTERIMPL_H
