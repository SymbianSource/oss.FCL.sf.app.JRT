/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  m_javaregmmcobserver definition
*
*/


#ifndef MJAVAREGMMCOBSERVER_H
#define MJAVAREGMMCOBSERVER_H

namespace Java
{
namespace Manager
{
namespace Registry
{

/**
 * This class contains the abstract method which is called in case of
 * mmc status changing.
 * Class interested in any mmc event should inherit from this class.
 *
 * @since S60 v3.2
 */
class MMemoryCardObserver
{
public:
    /**
     * Implement this method to be notified when MMC status
     * changes.
     *
     * @param aDrive the drive number in which mmc status changed
     * @param aMmcId the mmc id or 0 if mmc not present
     * @since S60 v3.2
     */
    virtual void MMCStatusChangedL(TInt aDrive, TUint aMmcId) = 0;
};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // MJAVAREGMMCOBSERVER_H

// End of File
