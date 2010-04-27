/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#ifndef RUNTIME_UI_QT_H
#define RUNTIME_UI_QT_H

#include "javaosheaders.h"
#include <vector>

namespace java
{
namespace runtimeui
{
class RuntimeUiQt
{
public:
    /**
     * Show error dialog to user. This is blocking call and execution does not proceed until
     * has pressed OK button.
     *
     * @param aAppName Application name error occurred.
     * @param aShortMesg Short error description.
     * @param aDetailedMsg Detailed error description.
     */
    OS_IMPORT static void errorL(const TDesC& aAppName, const TDesC& aShortMsg, const TDesC& aDetailedMsg);

    /**
     * Request permission to user. This is blocking call and execution does not proceed until
     * has accepted or denied the permission.
     *
     * @param aAppName Application name requesting confirmation.
     * @param aQuestion Security question.
     * @return 1 if user accepts the request, 0 otherwise.
     */
    OS_IMPORT static int confirmL(const TDesC& aAppName, const TDesC& aQuestion);
};

} //end namespace runtimeui
} //end namespace java

#endif //RUNTIME_UI_QT_H
