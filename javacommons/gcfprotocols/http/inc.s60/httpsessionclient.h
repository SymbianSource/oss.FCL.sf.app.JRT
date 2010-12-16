/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef HTTPSESSIONCLIENT_H
#define HTTPSESSIONCLIENT_H

#include <e32std.h>
#include <e32base.h>
#include <http/rhttpsession.h>
#include <es_sock.h>
#include "functionserver.h"

using namespace java::util;

class CHttpTransactionClient;
class CJavaValidationFilter;
//class MHttpClientCustomiser;

class HttpSessionClient
{
public:
    //static HttpSessionClient* NewL();
    static HttpSessionClient* NewL(TInt ,TInt, TInt *);
    ~HttpSessionClient();
    // RStringPool StringPool() const;
    inline RHTTPSession& Session()
    {
        return iHttpSession;
    }
    /**
         *  To restart a connection
         */
    void RestartConnection();
    /* A licenceee may have written their own Http plug in proxy
    * If this is the case then this should be used by the
    * Java HTTP implementation (PREQ 666)
    */
    //inline const TBool UsePlugInProxy() const { return iUsePlugInProxy; }
    //void CustomiseHeadersL(RHTTPHeaders& aHeaders);

private:
    HttpSessionClient();
private:
    void ConstructL(TInt,TInt, TInt *);
    void ReplaceValidationFilterL();
    void LoadProxyLibraryL(TUint32);
    //void LoadCustomiserLibrary();

    /**
     * To remove default Redirection filter.
     */
    void RemoveRedirectionFilter();
    // from CActive
    /*
    void DoCancel() { }
    void RunL() { }
    TInt RunError( TInt aError ) { return 0;} */

private:
    RHTTPSession iHttpSession;
    CJavaValidationFilter* iValidationFilter;
    //TBool iUsePlugInProxy;
    RLibrary iCustomiserLib;
    RSocketServ iSocketServ;
    RConnection iConnection;
    TInt     iApnId ;       // stores the ap id, could be SNAP id or IAP id depending on iApType
    TInt  iApType;          // stores the access point type, 2= SNAP, 3=IAP
    //MHttpClientCustomiser* iCustomiser;
};

#endif // HTTPSESSIONCLIENT_H
