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
* Description:
 *
*/


#ifndef MSTSCLOSEOBSERVER_H
#define MSTSCLOSEOBSERVER_H

namespace java
{
namespace satsa
{

// CLASS DECLARATION
// Specifies observer for to close connection

NONSHARABLE_CLASS(MSTSCloseObserver)
{
public: // data types
    enum TSTSCloseReason
    {
        ESTSCardNotAvailable,
        ESTSBTSapInUse
    };

public: // New functions


    // Closes the connection
    virtual void NotifyClose(TInt aReason) = 0;

}
;

} // namespace satsa
} // namespace java
#endif // MSTSCLOSEOBSERVER_H
// End of File
