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
* Description: A support interface for the eSWT.
*
*/

#ifndef COREUIAVKONESWTSUPPORT_H
#define COREUIAVKONESWTSUPPORT_H


#include <e32std.h>


class CSwtDisplay;


namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

/**
 * CoreUiAvkonLcduiSupport is a class for eSWT for storing and doing
 * eSWT specific operations. The CoreUi store singleton instance of
 * this class that can be get by calling
 * CoreUiAvkonEswtSupport::getInstance().getEswtSupport().
 */
NONSHARABLE_CLASS(CoreUiAvkonEswtSupport)
{
public:
    /**
     * CoreUiAvkonEswtSupport default constructor.
     */
    CoreUiAvkonEswtSupport() {}

    /**
     * CoreUiAvkonEswtSupport destructor.
     */
    virtual ~CoreUiAvkonEswtSupport() {}

    /**
     * A request to dispose the CoreUi. If there are no more childs
     * the UI will be destoyed.
     */
    virtual void dispose() = 0;
#ifdef _DEBUG // codescanner::ifpreprocessor
    virtual RThread& thread() = 0;
#endif
    /**
     * For getting a pointer to stored CSwtDisplay object.
     */
    virtual CSwtDisplay* display() const = 0;

    /**
     * For setting the CSwtDisplay object.
     * @param display The CSwtDisplay object.
     */
    virtual void setDisplay(CSwtDisplay* display) = 0;

    /**
     * Gets a handle to eSWT server.
     */
    virtual RServer2 getServerHandle() const = 0;
};

} // end namespace ui
} // end namespace java

#endif // COREUIAVKONESWTSUPPORT_H
