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



#ifndef JAVAVALIDATIONFILTER_H
#define JAVAVALIDATIONFILTER_H

#include <e32std.h>
#include <e32base.h>
#include <http/mhttpfilter.h>
#include <http/rhttpsession.h>
#include <http/cecomfilter.h>


/** The validation filter checks requests and responses to ensure that they are valid. It
    also adds either a succeded or failed event. Most clients will only be intrested in this
    simplified reporting of success. Any error codes are still sent to the client.
*/

class CJavaValidationFilter : public CEComFilter, public MHTTPFilter
{
public:
    static CJavaValidationFilter* InstallFilterL(TAny* aSession);
    ~CJavaValidationFilter();

public: // Methods from MHTTPFilterBase

    /// @see MHTTPFilterBase::MHFRunL
    virtual void MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent);

    /// @see MHTTPFilterBase::MHFSessionRunL
    virtual void MHFSessionRunL(const THTTPSessionEvent& aEvent);

    /// @see MHTTPFilterBase::MHFRunError
    virtual TInt MHFRunError(TInt aError, RHTTPTransaction aTransaction,
                             const THTTPEvent& aEvent);

    /// @see MHTTPFilterBase::MHFSessionRunError
    virtual TInt MHFSessionRunError(TInt aError, const THTTPSessionEvent& aEvent);

public: // Methods from MHTTPFilter

    /// @see MHTTPFilter::MHFUnload
    virtual void MHFUnload(RHTTPSession aSession, THTTPFilterHandle aHandle);

    /// @see MHTTPFilter::MHFLoad
    virtual void MHFLoad(RHTTPSession aSession, THTTPFilterHandle aHandle);

private:
    // Constructor
    CJavaValidationFilter();
    // For 2 phase construction. Also installs the filter.
    void ConstructL(RHTTPSession aSession);

    /*void ValidateRequestMethodL(RHTTPTransaction aTransaction);

    TBool ValidateRequestHeadersL(RHTTPTransaction aTransaction);

    void ValidateRequestGetL(RHTTPTransaction aTransaction);
    void ValidateRequestPostL(RHTTPTransaction aTransaction);
    void ValidateRequestPutL(RHTTPTransaction aTransaction);
    void ValidateRequestTraceL(RHTTPTransaction aTransaction);
    void ValidateRequestHeadL(RHTTPTransaction aTransaction);

    void ValidateRequestOptionsL(RHTTPTransaction aTransaction);
    */

    /** Validate status codes. Anything from 400->599 is an error or warning
        @return ETrue if the status code indicates a problem with the transaction
    */
    TBool ValidateResponseStatusCode(RHTTPTransaction aTransaction);

    /** Fails a request if it contains a body.
        @return returns ETrue if the request did contain a body
    */
    //TBool RequestFailIfContainsBodyL(RHTTPTransaction aTransaction);

    /** Fails and cancels a transaction
        @param aTransaction The transaction to cancel
        @param aStatus The status or error that caused the transaction to fail
    */
    void FailAndCancelL(RHTTPTransaction aTransaction, THTTPEvent aStatus);


private:
    TBool iTransactionFailed;
    RHTTPMessage iCurrentMessage;   // it's a small object
    RStringPool iStringPool;
};


#endif // JAVAVALIDATIONFILTER_H
