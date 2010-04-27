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
* Description:  PmcInterface
*
*/

#ifndef PMCINTERFACE_H
#define PMCINTERFACE_H

#include <string>
#include <vector>

namespace java
{
namespace captain
{

const int PROC_LAUNCH_OPTIONS_NONE = 0;
const int PROC_LAUNCH_OPTIONS_AUTORESTART = 1;

typedef std::vector<std::string> cmdLine_t;

class PmcInterface
{
public:
    virtual ~PmcInterface() {};

    // returns pid. > 0 if success, <=0 on fail
    virtual int launch(const cmdLine_t& cmdLine, const int& options) = 0;
    virtual int terminate(const int& pid) = 0;
    virtual int kill(const int& pid) = 0;
};

} // namespace captain
} // namespace java

#endif // PMCINTERFACE_H

