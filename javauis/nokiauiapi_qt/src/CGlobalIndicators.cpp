/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Controls the native global indicators.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <e32property.h> // Publish & Subscribe
//#include <coreapplicationuisdomainpskeys.h>
#include "CGlobalIndicators.h"
//#include "com_nokia_mid_ui_GlobalIndicators.h"
#include <logger.h>

//  CONSTANTS
namespace
{
// These constants are from JNI header, just defined shorter names here
// If any other values are used for the indicator or its value, they are
// passed directly to P&S.
const TInt KPropertyGlobalMailIndicator = 0;
    //com_nokia_mid_ui_GlobalIndicators_MAIL_INDICATOR;

const TInt KPropertyGlobalChatIndicator = 0;
    //com_nokia_mid_ui_GlobalIndicators_CHAT_INDICATOR;

const TInt KPropertyGlobalIndicatorUninitialized = 0;
    //com_nokia_mid_ui_GlobalIndicators_INDICATOR_UNINITIALIZED;

const TInt KPropertyGlobalIndicatorHide = 0;
    //com_nokia_mid_ui_GlobalIndicators_INDICATOR_HIDE;

const TInt KPropertyGlobalIndicatorShow = 0;
    //com_nokia_mid_ui_GlobalIndicators_INDICATOR_SHOW;
}

// Required capabilites
// WriteDeviceData  // for setting the email status using RProperty::Set()
// ReadDeviceData   // for retrieving the email status using RProperty::Get()

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// SetGlobalIndicator
// Sets the value for given global indicator.
// -----------------------------------------------------------------------------
//
TInt CGlobalIndicators::SetGlobalIndicator(TInt aIndicator, TInt aValue)
{

    LOG2(EJavaGlobalInd, EInfo,
         "GlobalIndicators::SetGlobalIndicator indicator:%d, value:%d",
         aIndicator, aValue);


    // Make sure that indicator JNI constants match into P&S definitions used
    // in platform.
    TUint32 convertedIndicator = aIndicator;
    if (aIndicator == KPropertyGlobalMailIndicator)
    {
        //convertedIndicator = KCoreAppUIsNewEmailStatus;
    }
    if (aIndicator == KPropertyGlobalChatIndicator)
    {
        //convertedIndicator = KCoreAppUIsUipInd;
    }

    TInt readValue = KPropertyGlobalIndicatorUninitialized;
    RProperty property;

   /* TInt ret = property.Attach(KPSUidCoreApplicationUIs, convertedIndicator);
    if (KErrNone == ret)
    {
        ret = property.Get(readValue);
        TInt setValue = KPropertyGlobalIndicatorUninitialized;

        // For Email and Chat indicators, use the specific known P&S values
        if (convertedIndicator == KCoreAppUIsNewEmailStatus ||
                convertedIndicator == KCoreAppUIsUipInd)
        {
            if (aValue == KPropertyGlobalIndicatorShow)
            {
                // Always ok to turn on - just check if is was already on
                if (readValue != ECoreAppUIsNewEmail &&
                        readValue != ECoreAppUIsShow)
                {
                    if (convertedIndicator == KCoreAppUIsNewEmailStatus)
                    {
                        setValue = ECoreAppUIsNewEmail;
                    }
                    if (convertedIndicator == KCoreAppUIsUipInd)
                    {
                        setValue = ECoreAppUIsShow;
                    }

                    ret = property.Set(setValue);
                    readValue = KPropertyGlobalIndicatorShow;
                    LOG1(EJavaGlobalInd, EInfo,
                         "GlobalIndicators::SetGlobalIndicator ON, value:%D",
                         setValue);
                }
            }
            else if (aValue == KPropertyGlobalIndicatorHide)
            {
                // When turning email inficator off bear in mind that there is
                // no way to know for sure if some other application still would
                // like to have the indicator turned on.
                if (readValue != ECoreAppUIsNoNewEmail &&
                        readValue != ECoreAppUIsDoNotShow)
                {
                    if (convertedIndicator == KCoreAppUIsNewEmailStatus)
                    {
                        setValue = ECoreAppUIsNoNewEmail;
                    }
                    if (convertedIndicator == KCoreAppUIsUipInd)
                    {
                        setValue = ECoreAppUIsDoNotShow;
                    }

                    ret = property.Set(setValue);
                    readValue = KPropertyGlobalIndicatorHide;
                    LOG1(EJavaGlobalInd, EInfo,
                         "GlobalIndicators::SetGlobalIndicator OFF, value:%D",
                         setValue);
                }
            }
            else
            {
                // Value is not HIDE or SHOW. Try to set it anyway.
                ret = property.Set(aValue);
                readValue = aValue;
                LOG1(EJavaGlobalInd, EInfo,
                     "GlobalIndicators::SetGlobalIndicator value:%D",
                     aValue);
            }
        }

        else // other than Email or Chat indicator
        {
            ret = property.Set(aValue);
            readValue = aValue;
            LOG1(EJavaGlobalInd, EInfo,
                 "GlobalIndicators::SetGlobalIndicator [user defined] value:%D",
                 aValue);
        }

    }
    property.Close();
    if (KErrNone != ret)
    {
        LOG1(EJavaGlobalInd, EInfo,
             "GlobalIndicators::SetGlobalIndicator property error :%D",
             ret);
        readValue = ret;
    }*/

    // return the property value or an error code
    return readValue;
}

// -----------------------------------------------------------------------------
// GlobalIndicator
// Gets the value of given global indicator.
// -----------------------------------------------------------------------------
//
TInt CGlobalIndicators::GlobalIndicator(TInt aIndicator)
{
    LOG(EJavaGlobalInd, EInfo, "GlobalIndicators::GlobalIndicator");
    RProperty property;
    TInt readValue = 0;
    TInt ret = KErrNotFound;

    if (aIndicator == KPropertyGlobalMailIndicator)
    {
        //ret = property.Attach(
          //        KPSUidCoreApplicationUIs, KCoreAppUIsNewEmailStatus);
    }
    else if (aIndicator == KPropertyGlobalChatIndicator)
    {
      //  ret = property.Attach(
        //          KPSUidCoreApplicationUIs, KCoreAppUIsUipInd);
    }
    else
    {
        // A specific indicator (other than mail or chat indicator)
        //ret = property.Attach(
          //        KPSUidCoreApplicationUIs, aIndicator);
    }

    // If Attach has succeeded, try reading the Property value
    if (KErrNone == ret)
    {
        ret = property.Get(readValue);
        property.Close();
    }

    // If also reading has succeeded, return the Property value
    if (KErrNone == ret)
    {
        return readValue ;
    }
    else
    {
        return ret;
    }
}

//  End of File
