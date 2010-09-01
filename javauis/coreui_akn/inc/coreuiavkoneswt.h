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
* Description: Interface for the eSWT to access the core UI.
*
*/

#ifndef COREUIAVKONESWT_H
#define COREUIAVKONESWT_H

#include "coreuiavkon.h"
#include "coreuiavkoneswtsupport.h"

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

/**
 * CoreUiAvkonEswt offers an interface for the eSWT for
 * accessing the CoreUiAvkon services.
 */
NONSHARABLE_CLASS(CoreUiAvkonEswt) : public CoreUiAvkon
{
public:
    /**
     * For getting the CoreUiAvkonEswt instance. The
     * CoreUi is a singleton instance and the first
     * instance getter instantiates the class.
     * @return a reference to CoreUiAvkonEswt
     */
    IMPORT_C static CoreUiAvkonEswt& getInstance();

    /**
     * CoreUiAvkonEswt default constructor.
     */
    CoreUiAvkonEswt() {}

    /**
     * CoreUiAvkonEswt destructor.
     */
    virtual ~CoreUiAvkonEswt() {}

    /**
     * For getting the CoreUiAvkonEswtSupport instance, which
     * holds all the necessary resources of the core eSWT.
     * @return a reference to CoreUiAvkonEswtSupport
     */
    virtual CoreUiAvkonEswtSupport& getEswtSupport() = 0;
};

} // end namespace ui
} // end namespace java

#endif // COREUIAVKONESWT_H
