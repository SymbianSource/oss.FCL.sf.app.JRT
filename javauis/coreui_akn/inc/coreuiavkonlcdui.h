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
* Description: Interface for the LCDUI to access the core UI.
*
*/

#ifndef COREUIAVKONLCDUI_H
#define COREUIAVKONLCDUI_H

#include "coreuiavkon.h"
#include "coreuiavkonlcduisupport.h"

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

/**
 * CoreUiAvkonLcdui offers an interface for the LCDUI for
 * accessing the CoreUiAvkon services.
 */
NONSHARABLE_CLASS(CoreUiAvkonLcdui) : public CoreUiAvkon
{
public:
    /**
     * For getting the CoreUiAvkonLcdui instance. The
     * CoreUi is a singleton instance and the first
     * instance getter instantiates the class.
     * @return a reference to CoreUiAvkonLcdui
     */
    IMPORT_C static CoreUiAvkonLcdui& getInstance();

    /**
     * CoreUiAvkonLcdui default constructor.
     */
    CoreUiAvkonLcdui() {}

    /**
     * CoreUiAvkonLcdui destructor.
     */
    virtual ~CoreUiAvkonLcdui() {}

    /**
     * For getting the CoreUiAvkonLcduiSupport instance, which
     * holds all the necessary resources of the core LCDUI.
     * @return a reference to CoreUiAvkonLcduiSupport
     */
    virtual CoreUiAvkonLcduiSupport& getLcduiSupport() = 0;
};

} // end namespace ui
} // end namespace java

#endif // COREUIAVKONLCDUI_H
