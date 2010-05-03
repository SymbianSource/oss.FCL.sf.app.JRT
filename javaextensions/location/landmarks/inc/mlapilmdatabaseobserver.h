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
* Description:  Interface for listening native database events
 *
*/


#ifndef MLAPILMDATABASEOBSERVER_H
#define MLAPILMDATABASEOBSERVER_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <e32def.h>
#include    <EPos_Landmarks.h>

/**
 * Interface for listening native database events
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(MLAPILmDatabaseObserver)
{
public: // New functions

    /**
     * This function is called if a change occurs in the native database
     * Currently, the notified events are only related to categories and
     * landmarks
     *
     * @param aEventType The type of the database event
     * @param aItemId The item to which the event is related to
     */
    virtual void NotifyEvent(
        TPosLmEventType& aEventType,
        TPosLmItemId& aItemId) = 0;

protected:

    /**
     * Destructor. Destruction is not allowed via this interface
     */
    virtual ~MLAPILmDatabaseObserver()
    {}

}
;

#endif // MLAPILMDATABASEOBSERVER_H
// End of file
