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
* Description: A support interface for the LCDUI.
*
*/

#ifndef COREUIAVKONLCDUISUPPORT_H
#define COREUIAVKONLCDUISUPPORT_H

#include <e32base.h>
class RLcdui;

namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

/**
 * CoreUiAvkonLcduiSupport is a class for LCDUI for storing and doing
 * LCDUI specific operations. The CoreUi stores a singleton instance of
 * this class that can be get by calling
 * CoreUiAvkonLcdui::getInstance().getLcduiSupport() method.
 */
NONSHARABLE_CLASS(CoreUiAvkonLcduiSupport)
{
public:
    /**
     * CoreUiAvkonLcduiSupport default constructor.
     */
    CoreUiAvkonLcduiSupport() {}

    /**
     * CoreUiAvkonLcduiSupport destructor.
     */
    virtual ~CoreUiAvkonLcduiSupport() {}

    /**
     * Gets the RLcdui object. Should never
     * be null.
     */
    virtual RLcdui* getRlcdUi() = 0;

    /**
     * Gets the handle to event server handle. This handle is a pointer to
     * LCDUI event server object. The handle is is actually a pointer that
     * has been shifted two bits to right (using JavaMakeHandle from jutils.h).
     * When the user wants get the real pointer the result of this method call
     * must be converted by using method JavaUnhand (from jutils.h).
     */
    virtual int getEventServerHandle() = 0;
};

} // end namespace ui
} // end namespace java

#endif // COREUIAVKONLCDUISUPPORT_H
