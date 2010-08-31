/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JavaCommDB class implementation
 *
*/

#ifndef IAINFOSESSION_H
#define IAINFOSESSION_H


#include <commsdattypesv1_1.h>
#include <commsdat.h>
#include <metadatabase.h>  
#include "functionserver.h"
#include <e32base.h>

using namespace java::util;

class IapInfoSession  : public FunctionServer 
{
  public:  static IapInfoSession* NewL();
    
  private:
            IapInfoSession();
            void ConstructL();

    /**
     * Pointer to the actual DB Session object.
     * Initialized in the Open() function.
     */
  public:
        CommsDat::CMDBSession* iDb;            
          



};

#endif // IAINFOSESSION_H