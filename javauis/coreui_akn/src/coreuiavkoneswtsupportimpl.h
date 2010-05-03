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
* Description: A support class implementation for the eSWT.
*
*/

#ifndef COREUIAVKONESWTSUPPORTIMPL_H
#define COREUIAVKONESWTSUPPORTIMPL_H


#include "coreuiavkoneswtsupport.h"


class CSwtServer;


namespace java // codescanner::namespace
{
namespace ui // codescanner::namespace
{

class CoreUiAvkonImpl;

NONSHARABLE_CLASS(CoreUiAvkonEswtSupportImpl) : public CoreUiAvkonEswtSupport
{
public:
    /**
     * CoreUiAvkonEswtSupportImpl default constructor.
     */
    CoreUiAvkonEswtSupportImpl();

    /**
     * CoreUiAvkonEswtSupportImpl destructor.
     */
    virtual ~CoreUiAvkonEswtSupportImpl();

    /**
     * Called in the context of the UI thread so that
     * eSWT can do the needed initialization before
     * the scheduler of the UI is started.
     */
    void uiThreadInit();

    /**
     * A request to dispose the CoreUi. If there are no more childs
     * the UI will be destoyed.
     */
    void dispose();

    /**
     * For getting a pointer to stored CSwtDisplay object.
     */
    CSwtDisplay* display() const;

    /**
     * For setting the CSwtDisplay object.
     * @param display The CSwtDisplay object.
     */
    void setDisplay(CSwtDisplay* display);

    /**
      * Gets a handle to eSWT server.
      */
    RServer2 getServerHandle() const;


private:
    CSwtDisplay* mDisplay;
    CSwtServer* mServer;

#ifdef _DEBUG // codescanner::ifpreprocessor
public:
    RThread& thread();
private:
    RThread mThread;
#endif
};

} //end namespace ui
} //end namespace java

#endif // COREUIAVKONESWTSUPPORTIMPL_H
