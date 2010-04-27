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
* Description:  This class is for thread death monitoring.
*
*/

#include "osthreadsupervisor.h"

#include "runtimeexception.h"

#include "logger.h"
#include "javacommonutils.h"

using namespace java::runtime;
using namespace java::util;


OsThreadSupervisor::OsThreadSupervisor(bool /*tryThreadDumping*/)
{
    JELOG2(EJavaRuntime);
}

OsThreadSupervisor::~OsThreadSupervisor()
{
    JELOG2(EJavaRuntime);
}
