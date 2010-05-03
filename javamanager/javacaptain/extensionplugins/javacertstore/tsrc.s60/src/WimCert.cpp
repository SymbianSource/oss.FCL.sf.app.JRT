/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Reads certificates bypassing crypto token framework
*
*/



#include "wimcert.h"
#include "wimdummies.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>

const int LEAVE = 1;

typedef struct config_data_st
{
    long initCompletionCode;
    long getExtrasCompletionCode;
    char trustedUsage[50];
} CONFIG_DATA;

void ReadConfigFile(CONFIG_DATA&);
void CompleteRequest(TRequestStatus&, TInt);

EXPORT_C CWimCert* CWimCert::NewL()
{
    CWimCert * self = new CWimCert();
    return self;
}

EXPORT_C CWimCert::~CWimCert()
{

}

EXPORT_C void CWimCert::Initialize(TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.initCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    CompleteRequest(aStatus, configData.initCompletionCode);
}

EXPORT_C void CWimCert::CancelInitialize()
{

}

EXPORT_C void CWimCert::GetExtras(const TDesC8& aKeyId, RCPointerArray<HBufC>& aTrustedUsages, TCertLocation& aLocation, TRequestStatus& aStatus)
{
    CONFIG_DATA configData;
    ReadConfigFile(configData);
    if (configData.getExtrasCompletionCode == LEAVE)
    {
        User::Leave(KErrUnknown);// codescanner::leave
    }
    int len = strlen(configData.trustedUsage);
    if (len > 0)
    {
        HBufC * usage = NULL;
        TRAP_IGNORE(usage = HBufC::NewL(len));
        TPtr8 configUsage((TUint8*)configData.trustedUsage, len);
        configUsage.SetLength(len);
        usage->Des().Copy(configUsage);
        aTrustedUsages.Append(usage);
    }
    CompleteRequest(aStatus, configData.getExtrasCompletionCode);
}

EXPORT_C void CWimCert::CancelGetExtras()
{

}

CWimCert::CWimCert()
{
}

void ReadConfigFile(CONFIG_DATA& configData)
{
    // read the completion code from external file
    FILE * config_file = fopen("c:\\java\\javacertstore\\config.txt","r");
    if (config_file != NULL)
    {
        // identifier for the metadata value being read
        const int OP_UNDEFINED = -1;
        const int READ_INIT_COMPLETION_CODE = 1;
        const int READ_GET_EXTRAS_COMPLETION_CODE = 2;
        const int READ_TRUSTED_USAGE = 3;

        // init_completion_code
        int init_completion_code_index = 0;
        char init_completion_code[50];

        // get_extras_completion_code
        int get_extras_completion_code_index = 0;
        char get_extras_completion_code[50];

        // protection_domain
        int trusted_usage_index = 0;
        char trusted_usage[50];

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
                case 'E':
                    op = READ_INIT_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'U':
                    op = READ_GET_EXTRAS_COMPLETION_CODE;
                    key_ss = false;
                    break;
                case 'P':
                    op = READ_TRUSTED_USAGE;
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
                }
                else if (key_es)
                {
                    switch (op)
                    {
                    case READ_INIT_COMPLETION_CODE:
                        init_completion_code[init_completion_code_index] = (char)retval;
                        init_completion_code_index++;
                        init_completion_code[init_completion_code_index] = '\0';
                        break;
                    case READ_TRUSTED_USAGE:
                        trusted_usage[trusted_usage_index] = (char)retval;
                        trusted_usage_index++;
                        trusted_usage[trusted_usage_index] = '\0';
                        break;
                    case READ_GET_EXTRAS_COMPLETION_CODE:
                        get_extras_completion_code[get_extras_completion_code_index] = (char)retval;
                        get_extras_completion_code_index++;
                        get_extras_completion_code[get_extras_completion_code_index] = '\0';
                        break;
                    }
                }
            }
        }
        char *endpt = NULL;
        configData.initCompletionCode = strtol(init_completion_code, &endpt, 0);
        configData.getExtrasCompletionCode = strtol(get_extras_completion_code, &endpt, 0);
        if (trusted_usage_index > 0)
        {
            strcpy(configData.trustedUsage,trusted_usage);
        }
        else
        {
            configData.trustedUsage[0] = '\0';
        }
    }
}

void CompleteRequest(TRequestStatus& aStatus, TInt aCompletionCode)
{
    TRequestStatus* status = &aStatus;
    User::RequestComplete(status,aCompletionCode);
}

