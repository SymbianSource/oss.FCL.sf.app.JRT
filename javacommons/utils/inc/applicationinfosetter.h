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
* Description:  ApplicationInfoSetter is is for setting the service provider
*                of the application info.
*
*/


#ifndef APPLICATIONINFOSETTER_H
#define APPLICATIONINFOSETTER_H

#include "javaosheaders.h"
#include "applicationinfo.h"
/**
 * Available in library: javautils
 *
 */

namespace java
{
namespace util
{

OS_IMPORT void
setApplicationInfoProvider(const java::runtime::ApplicationInfo& appInfo);

} // end namespace util
} // end namespace java
#endif // APPLICATIONINFOSETTER_H

