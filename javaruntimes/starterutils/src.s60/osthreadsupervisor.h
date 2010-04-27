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
* Description:  Utility class for mointoring thread death.
*
*/


#ifndef OSTHREADSUPERVISOR_H
#define OSTHREADSUPERVISOR_H

#include "javaosheaders.h"

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

/**
 * This class is responsible for detecting abnormal thread death beloning to
 * own process. If the abnormal death happens this will terminate the whole
 * process. This is for Symbian.
 */
OS_NONSHARABLE_CLASS(OsThreadSupervisor)
{
public:

    /**
     * Constructor of the OsThreadSupervisor.
     * @param tryThreadDumping Whether to try to do thread dump in panic case.
     *                         Might cause another panic.
     * @throws std::exception on error cases.
     */
    OsThreadSupervisor(bool tryThreadDumping);

    /**
     * Destructor of the RuntimeStarterUtils. Will stop the thread monitoring.
     * The class is not meant to be inherited, so destructor can be non
     * virtual.
     */
    ~OsThreadSupervisor();


private: // Member variables.

    TRequestStatus  mRequestStatus;
    RThread         mSupervisorThread;
    bool            mDoExit;
    bool            mTryThreadDump;

private: // Methods.

    /**
     * Starts the undertaker thread in Symbian.
     * @throw exception on error cases.
     */
    void startUndertakerThread();

    /**
     * The undertaker thread takes out the VM process if any of its threads
     * panics. This prevents deadlocked zombie VM processes from hanging in
     * the system if something go wrong.
     * @param starter A pointer to instance of OsThreadSupervisor.
     * @return status of the thread execution.
     */
    static int underTakerThreadMain(void* supervisor);

    /**
     * No default constructor.
     */
    OsThreadSupervisor();

    /*
     * No copy constructor allowed.
     */
    OsThreadSupervisor(const OsThreadSupervisor&);

    /*
     * No Assignment operator allowed.
     */
    OsThreadSupervisor& operator= (const OsThreadSupervisor&);
};

} // end namespace runtime
} // end namespace java


#endif // OSTHREADSUPERVISOR_H
