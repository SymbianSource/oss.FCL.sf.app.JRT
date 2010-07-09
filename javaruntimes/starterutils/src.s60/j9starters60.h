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
* Description:  This class is meant for starting the J9 JVM.
*
*/


#ifndef J9STARTERS60_H
#define J9STARTERS60_H

#include "jvmstarterimpl.h"
#include "javasymbianoslayer.h"

/**
 * Provides a utilities for starting the J9 JVM in Symbian OS.
 */

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

OS_NONSHARABLE_CLASS(J9StarterS60) : public JvmStarterImpl
{
public:
    /**
     * Default constructor of the J9StarterS60.
     */
    J9StarterS60();

    /**
     * Constructor of the J9StarterS60 with arguments. For the argument
     * description @see jvmstarte.h#getJvmStarterInstance method with
     * same arguments
     */
    J9StarterS60(const Configuration configuration,
                 const std::wstring& indetifier);

    /**
     * Destructor of the J9StarterS60.
     */
    virtual ~J9StarterS60();

    /**
     * @see jvmstarter.h
     */
    virtual int startJvm();
    /**
     * @see jvmstarter.h
     */
    virtual int startJvm(int argc, char** argv);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideOldHeapSize(int heapSize);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideNewHeapSize(int heapSize);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideMaxHeapSize(int heapSize);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideNativeStackSize(int stackSize);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideJavaStackSize(int stackSize);

private:

    /**
     * Sets the default arguments optimized for the MIDP runtime
     */
    void setDefaultArguments();

    /**
     * Adds the classpaths, main class, JIT configurations and thread
     * configurations to be part of the internal lists mJvmArgs and
     * mAppAndArgs.
     */
    void completeArgumentContainers();

    /**
     * Reads the platform ODC files from a file. Adds the ODC files
     * into bootclasspath and adds the defined system properties.
     */
    void setInternalOdcFiles();

    /**
     * Appends the odc file to J9 specific system property.
     * @param odcFile Th ODC file to be appended.
     */
    void appendOdcFile(const std::wstring& odcFile);
        
    /**
     * Internal utility for setting max heap sizes in kB. Used to
	 * set -Xmx and -Xmox vm args.
	 * @param arg String containing either -Xmx or -Xmox (not checked).
	 * @param size Size of the heap as string in kilo bytes.
     */
    void doOverideHeap(const std::wstring& arg, const std::wstring& size);


#ifdef __WINSCW__ // codescanner::ifpreprocessor
    /**
     * Gets the used J9 dll varint in winscw. This is related to probelm that
     * JVM is using WSD in implementation dlls. There are available two
     * copies of the J9 dlls having named *_2.dll and *_3.dll. This method
     * check which one should be used.
     */
    void getJ9VariantL(); // For triplicate j9 dlls

    std::string* mVariant;
    RLibrary     mLibLoader;

#endif

    /**
     * Enables the thread dumping feature.
     */
    void enableThreadDumpL();

private:

    /**
     * For adding the odc files.
     */
    std::wstring mOdcExtDirProperty;
};

} //end namespace runtime
} //end namespace java
#endif // J9STARTERS60_H
