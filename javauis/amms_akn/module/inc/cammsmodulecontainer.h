/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for CAMMSModule objects.
*
*/


#ifndef CAMMSMODULECONTAINER_H
#define CAMMSMODULECONTAINER_H

//  INCLUDES
#include <e32base.h>
#include "cammsmodule.h"

// FORWARD DECLARATIONS
class CMMAPlayer;

// CLASS DECLARATION

/**
*
*  This class is a container for CAMMSModule objects. All modules added to
*  this class with CArrayPtrSeg::AppendL method are owned and deleted in the
*  destructor.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSModuleContainer): public CArrayPtrSeg< CAMMSModule >
{
public:
    /**
    * C++ default constructor.
    */
    CAMMSModuleContainer();

    /**
    * Destructor.
    */
    ~CAMMSModuleContainer();

public: // New functions
    /**
     * Removes player from all modules. Player states are not checked.
     * @param aPlayer player to remove
     */
    void RemovePlayer(CMMAPlayer* aPlayer);

    /**
     * Removes module from container. The module itself is not deleted
     * by this method, so it has to be deleted elsewhere.
     * Method returns silently if module is not found.
     * @param aModule Module to remove.
     */
    void RemoveModule(CAMMSModule* aModule);

};

#endif // CAMMSMODULECONTAINER_H


