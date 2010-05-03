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
* Description:  HttpSessionClient
*
*/

#include "httpsessionclient.h"
#include "chttptransactionclient.h"
#include "javavalidationfilter.h"
#include <http/thttpfilteriterator.h>
#include <httpstringconstants.h>
#include <e32svr.h>
#include "logger.h"
#include <commdbconnpref.h>

#include <http/cecomfilter.h>
#include <httpfilterproxyinterface.h>
#include <httpfiltercommonstringsext.h>


//const TUint32 KMIDletSuiteAPNNotSpecified   = KMaxTUint32;

HttpSessionClient::HttpSessionClient()
{

}


/*HttpSessionClient* HttpSessionClient::NewL()
    {
        LOG(ESOCKET,EInfo,"+HttpSessionClient::NewL ");
    return HttpSessionClient::NewL(-1, -1);
    }*/

HttpSessionClient* HttpSessionClient::NewL(TInt aType,TInt aAPNId,TInt * apnerr)
{
    HttpSessionClient* self =   new(ELeave) HttpSessionClient();
    CleanupStack::PushL(self);
    self->ConstructL(aType,aAPNId,apnerr);
    CleanupStack::Pop(self);
    return self;
}

/*
* -j2me expects the http stack to be able to post body data with no content type
*   the native stack default validation filter does not allow this.
* -j2me expects the http stack to allow post requests to have body data
*   the native stack default validation filter does not allow this.
*/

/**
 * To remove default Redirection filter.
 *
 * Preconditions. The following conditions must be true prior to calling
 * this method; otherwise, its operation is undefined.
 * - None.
 *
 * Postconditions. The following condition is true immediately after
 * returning from this method.
 * - Java HTTP session does not have Redirection filter.
 *
 * @param None.
 * @return None.
 * @exception None.
 */
void HttpSessionClient::RemoveRedirectionFilter()
{
    RHTTPFilterCollection filtColl = iHttpSession.FilterCollection();

    if (filtColl.CanChangeFilters())
    {

        // Remove default redirection filter.
        RStringF valFilter = iHttpSession.StringPool().StringF(HTTP::ERedirect, RHTTPSession::GetTable());
        filtColl.RemoveFilter(valFilter);
    }
}

void HttpSessionClient::ReplaceValidationFilterL()
{
    RHTTPFilterCollection filtColl = iHttpSession.FilterCollection();
    if (filtColl.CanChangeFilters())
    {
        RStringF valFilter = iHttpSession.StringPool().StringF(HTTP::EValidation,RHTTPSession::GetTable());
        //Remove default validation filter
        filtColl.RemoveFilter(valFilter);

        valFilter = iHttpSession.StringPool().StringF(HTTP::EDeflate, RHTTPSession::GetTable());
        //Remove default deflate filter
        filtColl.RemoveFilter(valFilter);

        valFilter = iHttpSession.StringPool().StringF(HTTP::EUserAgent, RHTTPSession::GetTable());
        //Remove default useragent filter
        filtColl.RemoveFilter(valFilter);

        //Install the java validation filter
        iValidationFilter = CJavaValidationFilter::InstallFilterL(&iHttpSession);
    }
}

void HttpSessionClient::ConstructL(TInt aType, TInt aAPNId, TInt * apnerr)
{
    LOG(ESOCKET,EInfo,"+HttpSessionClient::ConstructL ");

    /*CActiveScheduler * scheduler = new CActiveScheduler();
    CActiveScheduler::Install(scheduler);
    CActiveScheduler::Add(this);*/

    iHttpSession.OpenL();
    LOG(ESOCKET,EInfo,"+HttpSessionClient::ConstructL- RHttpSession opened ");
    //Update the exiting Http filter with a java rules based filter
    ReplaceValidationFilterL();

    RemoveRedirectionFilter();
    if (aType == 2) // SNAP ID
    {
        LOG(ESOCKET,EInfo,"type is snap id");
        if (aAPNId != -1)
        {
            iApnId = aAPNId;
            // Creates connection with selected SNAP ID
            int ret = iSocketServ.Connect();
            LOG1(ESOCKET,EInfo,"iSocketServ.Connect() returned %d",ret);
            if (ret < 0)
            {
                LOG(ESOCKET,EInfo,"+HttpSessionClient::iSocketServ.Connect() failed ");
                *apnerr = ret;
                return;
            }
            ret = iConnection.Open(iSocketServ);
            LOG1(ESOCKET,EInfo,"iConnection.Open returned %d",ret);
            if (ret < 0)
            {
                LOG(ESOCKET,EInfo,"+HttpSessionClient::iConnection.Open failed ");
                *apnerr = ret;
                return;
            }

            TCommSnapPref connPref;
            connPref.SetSnap(aAPNId);
            ret = iConnection.Start(connPref);
            LOG1(ESOCKET,EInfo,"iConnection.Start returned %d",ret);
            if (ret < 0)
            {
                *apnerr = ret;
                //   return;
            }
            else
            {
                RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
                connInfo.SetPropertyL(iHttpSession.StringPool().StringF(
                                          HTTP::EHttpSocketServ, RHTTPSession::GetTable()), THTTPHdrVal(iSocketServ.Handle()));
                TInt connPtr = REINTERPRET_CAST(TInt, &iConnection);
                connInfo.SetPropertyL(iHttpSession.StringPool().StringF(
                                          HTTP::EHttpSocketConnection, RHTTPSession::GetTable()), THTTPHdrVal(connPtr));
            }
        } // end of if(apnid != -1)
        LOG1(ESOCKET,EInfo,"apnerr = %d",*apnerr);
    } // end of if(aType == 3 )
    else
    {
        if (aType == 3) // IAP Id
        {
            if (aAPNId != -1)
            {
                // Creates connection with selected IAP ID
                int ret = iSocketServ.Connect();
                if (ret < 0)
                {
                    LOG(ESOCKET,EInfo,"+HttpSessionClient::iSocketServ.Connect() failed ");
                    *apnerr = ret;
                    return;
                }
                ret = iConnection.Open(iSocketServ);
                if (ret < 0)
                {
                    LOG(ESOCKET,EInfo,"+HttpSessionClient::iConnection.Open failed ");
                    *apnerr = ret;
                    return;
                }
                TCommDbConnPref pref;
                pref.SetIapId(aAPNId);
                pref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
                ret = iConnection.Start(pref);
                if (ret < 0)
                {
                    *apnerr = ret;
                    //return;
                }
                RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
                connInfo.SetPropertyL(iHttpSession.StringPool().StringF(
                                          HttpFilterCommonStringsExt::EAccessPointID, HttpFilterCommonStringsExt::GetTable()), aAPNId);
            } // end of if ( aAPNId != -1)
        } // end of if(aType == 4)
        else
        {
            // Creates connection with no selected Network Access.
        }

    } // end of else

    LOG1(ESOCKET,EInfo,"apnerr = ",*apnerr);

    // This is special case when that IAP/SNAP is not found
    // Override the http stack's feaute of using device default
    if (*apnerr < 0)
    {
        LOG(ESOCKET,EInfo,"ECommDbDialogPrefPrompt set for the http session");
        int ret = iSocketServ.Connect();
        ret = iConnection.Open(iSocketServ);
        TCommDbConnPref pref;
        pref.SetDialogPreference(ECommDbDialogPrefPrompt);
        ret = iConnection.Start(pref);

        RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
        connInfo.SetPropertyL(iHttpSession.StringPool().StringF(
                                  HTTP::EHttpSocketServ, RHTTPSession::GetTable()), THTTPHdrVal(iSocketServ.Handle()));
        TInt connPtr = REINTERPRET_CAST(TInt, &iConnection);
        connInfo.SetPropertyL(iHttpSession.StringPool().StringF(
                                  HTTP::EHttpSocketConnection, RHTTPSession::GetTable()), THTTPHdrVal(connPtr));
    }

    //Add proxy support
    TRAP_IGNORE(LoadProxyLibraryL(aAPNId));

    // Try to load the customiser plugin, which enables customisation
    // of the loaded filters and headers.
    /*    LoadCustomiserLibrary();
        if ( iCustomiser )
            {
            iCustomiser->CustomiseFiltersL(iHttpSession);
            }   */
}

//_LIT(KHttpClientCustomiserPath, "MIDP2HTTPClientCustomiser.dll");

//typedef MHttpClientCustomiser* (*GetCustomiser)(void);

/*void HttpSessionClient::LoadCustomiserLibrary()
{
    TInt err = iCustomiserLib.Load(KHttpClientCustomiserPath);
    if ( KErrNone == err )
        {
        iCustomiser = reinterpret_cast<GetCustomiser>(iCustomiserLib.Lookup(1))();
        }

}*/

void HttpSessionClient::LoadProxyLibraryL(TUint32 /*aAPNId*/)
{
    iHttpSession.StringPool().OpenL(HttpFilterCommonStringsExt::GetTable());
    CHttpFilterProxyInterface::InstallFilterL(iHttpSession) ;
}

HttpSessionClient::~HttpSessionClient()
{
    iHttpSession.Close();
    iConnection.Close();
    iSocketServ.Close();

    /* if ( iCustomiser )
         {
         delete iCustomiser;
         iCustomiser = NULL;
         iCustomiserLib.Close();
         }*/
}

/* RStringPool HttpSessionClient::StringPool() const
    {
    return iHttpSession.StringPool();
    }
*/
/*void HttpSessionClient::CustomiseHeadersL(RHTTPHeaders &aHeaders)
{
    if ( iCustomiser )
        {
        iCustomiser->CustomiseHeadersL(iHttpSession, aHeaders);
        }
}*/

void HttpSessionClient::RestartConnection()
{
    LOG(ESOCKET,EInfo,"+HttpSessionClient::RestartConnection12 + ");
    iConnection.Close();
    TInt ret = iConnection.Open(iSocketServ);

    TCommSnapPref connPref;
    if (iApnId!=-1)
        connPref.SetSnap(iApnId);
    ret = iConnection.Start(connPref);
    LOG(ESOCKET,EInfo,"+HttpSessionClient::RestartConnection + ");

}
