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
* Description:  Provides static information about appointment lists and creates
 *                appointment list adapters.
 *
*/


#ifndef CPIMAPPTADAPTERMANAGER_H
#define CPIMAPPTADAPTERMANAGER_H

// INCLUDES
#include "cpimeventadaptermanager.h"

// CLASS DECLARATION

/**
 * PIM Appt Adapter Manager class
 */
NONSHARABLE_CLASS(CPIMApptAdapterManager): public CPIMEventAdapterManager
{
public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMApptAdapterManager* NewL(const TDesC& aListName);

    /**
     * Destructor.
     */
    ~CPIMApptAdapterManager();

public: // MPIMEventAdapterManager

    /**
     * Provides access to the \ref MPIMAdapterManager representation of
     * this object.
     *
     * @return The \ref MPIMAdapterManager representation of this object.
     */
    MPIMAdapterManager* GetAdapterManager();

private:

    /**
     * C++ default constructor.
     */
    CPIMApptAdapterManager();

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(const TDesC& aListName);

};

#endif // CPIMAPPTADAPTERMANAGER_H
// End of File

