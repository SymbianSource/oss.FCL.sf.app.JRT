/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ApplicationRuntimeEventsInterface
*
*/

#ifndef APPLICATIONRUNTIMEEVENTSINTERFACE_H
#define APPLICATIONRUNTIMEEVENTSINTERFACE_H

#include "javauid.h"

using java::util::Uid;

namespace java
{
namespace captain
{

class ApplicationRuntimeEventsInterface
{
public:
    virtual ~ApplicationRuntimeEventsInterface() {};

    virtual void arLaunched(const Uid& aUID, const int& aRuntimeCommsAddress) = 0;
    virtual void arTerminated(const Uid& aUID, const int& aExitCode) = 0;

};

} // namespace captain
} // namespace java

#endif // APPLICATIONRUNTIMEEVENTSINTERFACE_H
