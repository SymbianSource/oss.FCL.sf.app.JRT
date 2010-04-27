/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32base.h>
#include <ecom.h>
#include <implementationproxy.h>
#include <ct.h>
#include <mctwritablecertstore.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <vector>

using namespace std;

const int LEAVE = 1;
const int EMPTY = 2;

typedef struct config_data_st
{
    long listCompletionCode;
    long getCertCompletionCode;
    long retrieveCompletionCode;
    long tokenTypeListCompletionCode;
    long openTokenCompletionCode;
    long getInterfaceCompletionCode;
    vector<char*> certs_paths;
    bool untrustedCert;
    long trustedCompletionCode;
} CONFIG_DATA;

_LIT(KTokenInfo, "Smart Card Test Token");

// forward declarations
void CompleteRequest(TRequestStatus&, TInt);
char * readFile(const char * fileName, long * length);
void ReadConfigFile(CONFIG_DATA&);

class CTestSmartCardCertStore : public CBase, public MCTWritableCertStore
{
public:
    CTestSmartCardCertStore(MCTToken& aToken);
    virtual ~CTestSmartCardCertStore();
    virtual MCTToken& Token();

    virtual void Add(const TDesC& aLabel, TCertificateFormat aFormat,
                     TCertificateOwnerType aCertificateOwnerType,
                     const TKeyIdentifier* aSubjectKeyId,
                     const TKeyIdentifier* aIssuerKeyId,
                     const TDesC8& aCert, TRequestStatus& aStatus);
    virtual void CancelAdd();
    virtual void Remove(const CCTCertInfo& aCertInfo, TRequestStatus& aStatus);
    virtual void CancelRemove();
    virtual void SetApplicability(const CCTCertInfo& aCertInfo,
                                  const RArray<TUid>& aApplications,
                                  TRequestStatus &aStatus);
    virtual void CancelSetApplicability();
    virtual void SetTrust(const CCTCertInfo& aCertInfo, TBool aTrusted,
                          TRequestStatus& aStatus);
    virtual void CancelSetTrust();
    virtual void List(RMPointerArray<CCTCertInfo>& aCerts,
                      const CCertAttributeFilter& aFilter, TRequestStatus& aStatus);
    virtual void CancelList();
    virtual void GetCert(CCTCertInfo*& aCertInfo, const TCTTokenObjectHandle& aHandle,
                         TRequestStatus& aStatus);
    virtual void CancelGetCert();
    virtual void Applications(const CCTCertInfo& aCertInfo, RArray<TUid>& aApplications,
                              TRequestStatus& aStatus);
    virtual void CancelApplications();
    virtual void IsApplicable(const CCTCertInfo& aCertInfo, TUid aApplication,
                              TBool& aIsApplicable, TRequestStatus& aStatus);
    virtual void CancelIsApplicable();
    virtual void Trusted(const CCTCertInfo& aCertInfo, TBool& aTrusted,
                         TRequestStatus& aStatus);
    virtual void CancelTrusted();
    virtual void Retrieve(const CCTCertInfo& aCertInfo, TDes8& aEncodedCert,
                          TRequestStatus& aStatus);
    virtual void CancelRetrieve();

private:
    MCTToken& iToken;
};


CTestSmartCardCertStore::CTestSmartCardCertStore(MCTToken& aToken)
        : iToken(aToken)
{
}

CTestSmartCardCertStore::~CTestSmartCardCertStore()
{
}

MCTToken& CTestSmartCardCertStore::Token()
{
    return iToken;
}

void CTestSmartCardCertStore::Add(const TDesC& /*aLabel*/, TCertificateFormat,
                                  TCertificateOwnerType,
                                  const TKeyIdentifier* /*aSubjectKeyId*/,
                                  const TKeyIdentifier* /*aIssuerKeyId*/,
                                  const TDesC8& /*aCert*/,
                                  TRequestStatus& aStatus)
{
    CompleteRequest(aStatus, KErrNotSupported);
}

void CTestSmartCardCertStore::CancelAdd()
{
}

void CTestSmartCardCertStore::Remove(const CCTCertInfo& /*aCertInfo*/,TRequestStatus& aStatus)
{
    CompleteRequest(aStatus, KErrNotSupported);
}

void CTestSmartCardCertStore::CancelRemove()
{
}

void CTestSmartCardCertStore::SetApplicability(const CCTCertInfo& /*aCertInfo*/,
        const RArray<TUid>& /*aApplications*/,
        TRequestStatus &aStatus)
{
    CompleteRequest(aStatus, KErrNotSupported);
}

void CTestSmartCardCertStore::CancelSetApplicability()
{
}

void CTestSmartCardCertStore::SetTrust(const CCTCertInfo& /*aCertInfo*/,
                                       TBool /*aTrusted*/,
                                       TRequestStatus& aStatus)
{
    CompleteRequest(aStatus, KErrNotSupported);
}

void CTestSmartCardCertStore::CancelSetTrust()
{
}

void CTestSmartCardCertStore::List(RMPointerArray<CCTCertInfo>& aCerts,
                                   const CCertAttributeFilter& /*aFilter*/,
                                   TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.listCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    if (configData.listCompletionCode == EMPTY)
    {
        CompleteRequest(aStatus, 0);
        return;
    }
    for (int i=0; i<configData.certs_paths.size(); i++)
    {
        // read the content from that path and return it as cert
        CCTCertInfo* certInfo = 0;
        _LIT(KCert, "TestCert");
        CCTTokenType& tokenTypeObj = static_cast<CCTTokenType&>(Token().TokenType());
        long certSize;
        char * cert = readFile(configData.certs_paths[i], &certSize);
        int err;
        TRAP(err,certInfo = CCTCertInfo::NewL(KCert,EX509Certificate,
                                              ECACertificate,certSize,
                                              NULL,NULL,Token(),i,
                                              true));
        aCerts.Append(certInfo);
        delete[] cert;
        cert = NULL;
        delete[] configData.certs_paths[i];
        configData.certs_paths[i] = NULL;
    }
    CompleteRequest(aStatus, configData.listCompletionCode);
}

void CTestSmartCardCertStore::CancelList()
{
}


void CTestSmartCardCertStore::GetCert(CCTCertInfo*& /*aCertInfo*/,
                                      const TCTTokenObjectHandle& /*aHandle*/,
                                      TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    CompleteRequest(aStatus, configData.getCertCompletionCode);
}

void CTestSmartCardCertStore::CancelGetCert()
{
}

void CTestSmartCardCertStore::Applications(const CCTCertInfo& /*aCertInfo*/,
        RArray<TUid>& /*aApplications*/,
        TRequestStatus& aStatus)
{
    CompleteRequest(aStatus, KErrNotSupported);
}

void CTestSmartCardCertStore::CancelApplications()
{
}

void CTestSmartCardCertStore::IsApplicable(const CCTCertInfo& /*aCertInfo*/,
        TUid /*aApplication*/,
        TBool& /*aIsApplicable*/,
        TRequestStatus& aStatus)
{
    CompleteRequest(aStatus, KErrNotSupported);
}

void CTestSmartCardCertStore::CancelIsApplicable()
{
}

void CTestSmartCardCertStore::Trusted(const CCTCertInfo& /*aCertInfo*/,
                                      TBool& aTrusted,
                                      TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.trustedCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    if (configData.untrustedCert)
    {
        aTrusted = false;
    }
    else
    {
        aTrusted = true;
    }
    CompleteRequest(aStatus, configData.trustedCompletionCode);
}

void CTestSmartCardCertStore::CancelTrusted()
{
}

void CTestSmartCardCertStore::Retrieve(const CCTCertInfo& /*aCertInfo*/,
                                       TDes8& aEncodedCert,
                                       TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.retrieveCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    for (int i=0; i<configData.certs_paths.size(); i++)
    {
        // read the content from that path and return it as cert
        if (configData.retrieveCompletionCode == KErrNone)
        {
            long certSize;
            char * cert = readFile(configData.certs_paths[i], &certSize);
            TPtr8 ptr8((TUint8 *)cert, certSize);
            ptr8.SetLength(certSize);
            aEncodedCert.Copy(ptr8);
            delete[] cert;
            cert = NULL;
        }
        delete[] configData.certs_paths[i];
        configData.certs_paths[i] = NULL;
    }
    CompleteRequest(aStatus, configData.retrieveCompletionCode);
}

void CTestSmartCardCertStore::CancelRetrieve()
{
}

class CTestSmartCardToken : public CBase, public MCTToken
{
public:
    CTestSmartCardToken(MCTTokenType& aTokenType);
    virtual ~CTestSmartCardToken();

    virtual void DoGetInterface(TUid aRequiredInterface,
                                MCTTokenInterface*& aReturnedInterface,
                                TRequestStatus& aStatus);
    virtual TBool DoCancelGetInterface();
    virtual const TDesC& Label();
    virtual MCTTokenType& TokenType();
    virtual TCTTokenHandle Handle();
    virtual const TDesC& Information(TTokenInformation);
protected:
    virtual TInt& ReferenceCount();

private:
    MCTTokenType& iTokenType;
    TInt iRefCount;
};

CTestSmartCardToken::CTestSmartCardToken(MCTTokenType& aTokenType)
        : iTokenType(aTokenType), iRefCount(0)
{
}

CTestSmartCardToken::~CTestSmartCardToken()
{
}

const TDesC& CTestSmartCardToken::Label()
{
    _LIT(KLabel, "TestSmartCardTokenLabel");
    return KLabel;
}

MCTTokenType& CTestSmartCardToken::TokenType()
{
    return iTokenType;
}

TCTTokenHandle CTestSmartCardToken::Handle()
{
    return TCTTokenHandle(TokenType().Type(), 1);
}

void CTestSmartCardToken::DoGetInterface(TUid /*aRequiredInterface*/,
        MCTTokenInterface*& aReturnedInterface,
        TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.getInterfaceCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    aReturnedInterface = new CTestSmartCardCertStore(*this);
    CompleteRequest(aStatus, configData.getInterfaceCompletionCode);
}

TBool CTestSmartCardToken::DoCancelGetInterface()
{
    return EFalse;
}


TInt& CTestSmartCardToken::ReferenceCount()
{
    return iRefCount;
}

const TDesC& CTestSmartCardToken::Information(TTokenInformation)
{
    _LIT(KInformation, "TestSmartCardTokenInfo");
    return KInformation();
}

class CTestSmartCardTokenType : public CCTTokenType
{
public:
    static CTestSmartCardTokenType* NewL();

    virtual void List(RCPointerArray<HBufC>& aTokens,
                      TRequestStatus& aStatus);
    virtual void CancelList();
    virtual void OpenToken(const TDesC& aTokenInfo, MCTToken*& aToken,
                           TRequestStatus& aStatus);
    virtual void OpenToken(TCTTokenHandle aHandle, MCTToken*& aToken,
                           TRequestStatus& aStatus);
    virtual void CancelOpenToken();

    virtual ~CTestSmartCardTokenType();
};

void CTestSmartCardTokenType::List(RCPointerArray<HBufC>& aTokens,
                                   TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.tokenTypeListCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    _LIT(KName, "TestSmartCardTokens");
    HBufC* name = KName().Alloc();
    aTokens.Append(name);
    CompleteRequest(aStatus, configData.tokenTypeListCompletionCode);
}

void CTestSmartCardTokenType::CancelList()
{
}

void CTestSmartCardTokenType::OpenToken(const TDesC& /*aTokenInfo*/,
                                        MCTToken*& aToken,
                                        TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.openTokenCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    aToken = new CTestSmartCardToken(*this);
    CompleteRequest(aStatus, configData.openTokenCompletionCode);
    IncReferenceCount();
}

void CTestSmartCardTokenType::OpenToken(TCTTokenHandle /*aHandle*/,
                                        MCTToken*& aToken,
                                        TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.openTokenCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    aToken = new CTestSmartCardToken(*this);
    CompleteRequest(aStatus, configData.openTokenCompletionCode);
    IncReferenceCount();
}

void CTestSmartCardTokenType::CancelOpenToken()
{
}


CTestSmartCardTokenType::~CTestSmartCardTokenType()
{
}

CTestSmartCardTokenType* CTestSmartCardTokenType::NewL()
{
    CTestSmartCardTokenType* self = new(ELeave) CTestSmartCardTokenType;
    return self;
}

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(0x101F5B72, CTestSmartCardTokenType::NewL)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

TInt E32Dll()
{
    return KErrNone;
}

void CompleteRequest(TRequestStatus& aStatus, TInt aCompletionCode)
{
    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;
    User::RequestComplete(status,aCompletionCode);
}

void ReadConfigFile(CONFIG_DATA& configData)
{
    // read the completion code from external file
    FILE * config_file = fopen("c:\\java\\javacertstore\\config.txt","r");
    if (config_file != NULL)
    {
        // identifier for the metadata value being read
        const int OP_UNDEFINED = -1;
        const int READ_LIST_COMPLETION_CODE = 1;
        const int READ_GET_CERT_COMPLETION_CODE = 2;
        const int READ_RETRIEVE_COMPLETION_CODE = 3;
        const int READ_CERT_PATH = 4;
        const int READ_TOKEN_TYPE_LIST_COMPLETION_CODE = 5;
        const int READ_OPEN_TOKEN_COMPLETION_CODE = 6;
        const int READ_GET_INTERFACE_COMPLETION_CODE = 7;
        const int READ_UNTRUSTED_CERT = 8;
        const int READ_TRUSTED_COMPLETION_CODE = 9;

        // list_completion_code
        int list_completion_code_index = 0;
        char list_completion_code[50];

        // get_cert_completion_code
        int get_cert_completion_code_index = 0;
        char get_cert_completion_code[50];

        // retrieve_completion_code
        int retrieve_completion_code_index = 0;
        char retrieve_completion_code[50];

        // certificate
        int cert_path_index = 0;
        char cert_path[50];

        // token_type_list_completion_code
        int token_type_list_completion_code_index = 0;
        char token_type_list_completion_code[50];

        // open_token_completion_code
        int open_token_completion_code_index = 0;
        char open_token_completion_code[50];

        // get_interface_completion_code
        int get_interface_completion_code_index = 0;
        char get_interface_completion_code[50];

        // untrusted cert
        bool untrustedCert = false;

        // trusted_completion_code
        int trusted_completion_code_index = 0;
        char trusted_completion_code[50];

        // start&end separators for the config's keys
        bool key_ss = true;
        bool key_es = false;
        int retval;

        // identifier for the operation being performed
        int op = 0;
        while ((int)(retval = getc(config_file))!= EOF)
        {
            if (key_ss)
            {
                // start of reading something new
                switch ((char)retval)
                {
                case 'I':
                    op = READ_GET_INTERFACE_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'T':
                    op = READ_TOKEN_TYPE_LIST_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'O':
                    op = READ_OPEN_TOKEN_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'L':
                    op = READ_LIST_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'G':
                    op = READ_GET_CERT_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'R':
                    op = READ_RETRIEVE_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'C':
                    op = READ_CERT_PATH;
                    key_ss = false;
                    break;
                case 'S':
                    op = READ_UNTRUSTED_CERT;
                    key_ss = false;
                    break;
                case 'V':
                    op = READ_TRUSTED_COMPLETION_CODE;
                    key_ss = false;
                    break;
                default:
                    key_ss = false;
                    op = OP_UNDEFINED;
                    break;
                }
            }
            else
            {
                if ((char)retval == '=')
                {
                    // end separator
                    key_es = true;
                }
                else if (retval == 10 || retval == 13 /* CR or LF */)
                {
                    key_ss = true;
                    key_es = false;
                    if (cert_path_index > 0)
                    {
                        char * path = new char[cert_path_index + 1];
                        memmove(path, cert_path, cert_path_index + 1);
                        configData.certs_paths.push_back(path);
                        cert_path_index = 0;
                        cert_path[0] = '\0';
                    }
                }
                else if (key_es)
                {
                    switch (op)
                    {
                    case READ_OPEN_TOKEN_COMPLETION_CODE:
                        open_token_completion_code[open_token_completion_code_index] = (char)retval;
                        open_token_completion_code_index++;
                        open_token_completion_code[open_token_completion_code_index] = '\0';
                        break;
                    case READ_GET_INTERFACE_COMPLETION_CODE:
                        get_interface_completion_code[get_interface_completion_code_index] = (char)retval;
                        get_interface_completion_code_index++;
                        get_interface_completion_code[get_interface_completion_code_index] = '\0';
                        break;
                    case READ_TOKEN_TYPE_LIST_COMPLETION_CODE:
                        token_type_list_completion_code[token_type_list_completion_code_index] = (char)retval;
                        token_type_list_completion_code_index++;
                        token_type_list_completion_code[token_type_list_completion_code_index] = '\0';
                        break;
                    case READ_LIST_COMPLETION_CODE:
                        list_completion_code[list_completion_code_index] = (char)retval;
                        list_completion_code_index++;
                        list_completion_code[list_completion_code_index] = '\0';
                        break;
                    case READ_GET_CERT_COMPLETION_CODE:
                        get_cert_completion_code[get_cert_completion_code_index] = (char)retval;
                        get_cert_completion_code_index++;
                        get_cert_completion_code[get_cert_completion_code_index] = '\0';
                        break;
                    case READ_RETRIEVE_COMPLETION_CODE:
                        retrieve_completion_code[retrieve_completion_code_index] = (char)retval;
                        retrieve_completion_code_index++;
                        retrieve_completion_code[retrieve_completion_code_index] = '\0';
                        break;
                    case READ_CERT_PATH:
                        cert_path[cert_path_index] = (char)retval;
                        cert_path_index++;
                        cert_path[cert_path_index] = '\0';
                        break;
                    case READ_UNTRUSTED_CERT:
                        untrustedCert = true;
                        break;
                    case READ_TRUSTED_COMPLETION_CODE:
                        trusted_completion_code[trusted_completion_code_index] = (char)retval;
                        trusted_completion_code_index++;
                        trusted_completion_code[trusted_completion_code_index] = '\0';
                        break;
                    }
                }
            }
        }
        char *endpt = NULL;
        configData.listCompletionCode = strtol(list_completion_code, &endpt, 0);
        configData.getCertCompletionCode = strtol(get_cert_completion_code, &endpt, 0);
        configData.retrieveCompletionCode = strtol(retrieve_completion_code, &endpt, 0);
        configData.tokenTypeListCompletionCode = strtol(token_type_list_completion_code, &endpt, 0);
        configData.openTokenCompletionCode = strtol(open_token_completion_code, &endpt, 0);
        configData.getInterfaceCompletionCode = strtol(get_interface_completion_code, &endpt, 0);
        configData.untrustedCert = untrustedCert;
        configData.trustedCompletionCode = strtol(trusted_completion_code, &endpt, 0);
        if (cert_path_index > 0)
        {
            char * path = new char[cert_path_index + 1];
            memmove(path, cert_path, cert_path_index + 1);
            configData.certs_paths.push_back(path);
            cert_path_index = 0;
            cert_path[0] = '\0';
        }
    }
}

char * readFile(const char * fileName, long * length)
{
    char * data = NULL;
    FILE    *jarFile;
    long    len = 0;
    jarFile = fopen(fileName, "r");
    if (jarFile != NULL)
    {
        fseek(jarFile, 0L, SEEK_END);
        len = ftell(jarFile);
        fseek(jarFile, 0L, SEEK_SET);
        data = new char[len];
        if (data != NULL)
        {
            fread(data, sizeof(char), len, jarFile);
            fclose(jarFile);
        }
    }
    *length = len;
    return data;
}
