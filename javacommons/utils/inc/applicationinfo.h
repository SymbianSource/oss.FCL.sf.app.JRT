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
* Description:  ApplicationInfo is an abstract base class for API implementations,
*                which allows getting application related static information.
*
*/


#ifndef APPLICATIONINFO_H
#define APPLICATIONINFO_H

#include "javaosheaders.h"
#include "javauid.h"

/**
 * Available in library: javautils
 *
 */

namespace java
{

namespace runtime
{

class ApplicationInfo
{

public:
    /**
     * The destructor
     */
    OS_IMPORT virtual ~ApplicationInfo();

    /**
     * Returns an instance of ApplicationInfo class which provides access to
     * application related static information.
     * <p>
     * Note! It is guaranteed that this service is available in MIDP
     * runtime. Other runtimes may not provide this service.
     *
     * @return the ApplicationInfo object, throws java::util::ExceptionBase
     *         if the service is not avalable.
     */
    OS_IMPORT static const ApplicationInfo& getInstance();

    /**
     * Returns the root path of the application. The root path means an
     * application specific directory where the different API implementations
     * are able to write their files. No other applications are able to access
     * files under the root path.
     * <p>
     * API implemetations must not rely that the JAR file(s) or descriptor file
     * can always be found from the root path, even if they sometimes might be
     * found.
     * @return application root path as wstring
     */
    virtual const std::wstring& getRootPath() const = 0;

    /**
     * Returns a unique application identifier.
     *
     * @return the Uid of the application.
     */
    virtual const java::util::Uid& getUid() const = 0;


protected:
    /**
     * The constructor. Only derived class is allowed to construct this class.
     */
    OS_IMPORT ApplicationInfo();

private: // Methods
    /**
     * No copy constructor allowed.
     */
    ApplicationInfo(const ApplicationInfo&);

    /**
     * No Assignment operator allowed.
     */
    ApplicationInfo& operator= (const ApplicationInfo&);

};

} // end namespace runtime
} // end namespace java
#endif // APPLICATIONINFO_H

