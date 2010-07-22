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
* Description:  Custom command utility class for AMMS custom commands.
*
*/


#ifndef CAMMSCUSTOMCOMMANDUTILITY_H
#define CAMMSCUSTOMCOMMANDUTILITY_H

//  INCLUDES
#include <e32base.h>
#include <CustomCommandUtility.h>
#include <mmf/common/mmfcontroller.h>

// CLASS DECLARATION

/**
*
*  Custom command Utility implementation class.
*  This class is used to deliver custom commands to MMFController.
*
*
*  @since 3.0
*/
NONSHARABLE_CLASS(CAMMSCustomCommandUtility): public CCustomCommandUtility
{
public:  // Constructors and destructor
    /**
    * Two-phased constructor.
    */
    static CAMMSCustomCommandUtility* NewL(
        RMMFController& aMMFController);

    /**
    * Two-phased constructor.
    */
    static CAMMSCustomCommandUtility* NewLC(
        RMMFController& aMMFController);

    /**
    * Destructor.
    */
    ~CAMMSCustomCommandUtility();

public: // New functions

public: // Functions from base classes

    TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
                           TInt                         aFunction,
                           const TDesC8&                      aDataTo1,
                           const TDesC8&                      aDataTo2,
                           TDes8&                       aDataFrom);

    TInt CustomCommandSync(const TMMFMessageDestinationPckg&  aDestination,
                           TInt                         aFunction,
                           const TDesC8&                      aDataTo1,
                           const TDesC8&                      aDataTo2);

    void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
                            TInt                        aFunction,
                            const TDesC8&                     aDataTo1,
                            const TDesC8&                     aDataTo2,
                            TDes8&                      aDataFrom,
                            TRequestStatus&             aStatus);

    void CustomCommandAsync(const TMMFMessageDestinationPckg& aDestination,
                            TInt                        aFunction,
                            const TDesC8&                     aDataTo1,
                            const TDesC8&                     aDataTo2,
                            TRequestStatus&             aStatus);

private:
    /**
    * C++ constructor.
    */
    CAMMSCustomCommandUtility(RMMFController& aMMFController);

private:  // Data

    RMMFController& iMMFController;

};

#endif // CAMMSCUSTOMCOMMANDUTILITY_H


