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
* Description:
*
*/


package javax.microedition.securityservice;


import java.io.UnsupportedEncodingException;
import javax.microedition.pki.UserCredentialManagerException;

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.pki.SignaturePermission;
import com.nokia.mj.impl.rt.support.ApplicationUtils;


/**
 * Refer JSR-177 Specification for details
 */
public final class CMSMessageSignatureService
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasatsa");
        }
        catch (Exception e)
        {

        }

    }


    /**
    * Includes the user certificate in the signature.
    */
    public static final int SIG_INCLUDE_CERTIFICATE = 2;

    /**
    * Includes the content that was signed in the signature.
    */
    public static final int SIG_INCLUDE_CONTENT = 1;

    /** MIDP 2.0 security service permission */
    private static final String MIDP_PERMISSION =
        "javax.microedition.securityservice.CMSMessageSignatureService";

    /** The only instance of this class */
    private static CMSMessageSignatureService sInstance;

    /** Handle to the native side */
    private int iServiceHandle;

    /** Status code received from native side, no errors occured */
    private static final int NO_ERROR = 0;

    /** Error code from native side, certificate not found */
    private static final int NOT_FOUND = -1;

    /** Error code from native side, some of the arguments is invalid */
    private static final int ERR_ARGUMENT = -6;

    /** Corresponding message */
    private static final String ERR_ARGUMENT_STRING = "Argument is out of range";

    /** Error code from native side, PIN is blocked */
    private static final int ERR_LOCKED = -22;

    /** Corresponding message */
    private static final String ERR_LOCKED_STRING = "False PIN code: not authorized to access certificate store";

    /** Error message for null/empty ca name */
    private static final String ERR_NULL_CA_NAME_STRING =
        "CA name is null or empty";

    /** Error code from native side, CA name is invalid */
    private static final int ERR_INVALID_CA_NAME = -4004;

    /** Error message for invalid ca name */
    private static final String ERR_INVALID_CA_NAME_STRING =
        "Invalid CA name";

    /** Error code from native side,invalid characters in CA name */
    private static final int ERR_INVALID_CHARACTERS_IN_CA_NAME = -4005;

    /** Error message for invalid ca name */
    private static final String ERR_INVALID_CHARACTERS_IN_CA_NAME_STRING =
        "Invalid characters in CA name";


    /** Encoding used for the generation of signatures for strings */
    private static final String STRING_ENCODING = "UTF-8";

    /** Unexpected error */
    private static final String ERR_UNEXPECTED =
        "Unexpected error: ";

    static
    {
        // create instance
        sInstance = new CMSMessageSignatureService();
    }



    /**
    * Refer JSR-177 Specification for details
    */
    public static final byte[] authenticate(byte[] aByteArrayToAuthenticate,
                                            int aOptions, String[] aCaNames,
                                            String aSecurityElementPrompt)
    throws CMSMessageSignatureServiceException,
                UserCredentialManagerException
    {
        // null / empty aCaNames means "use any"
        if (null == aCaNames)
        {
            aCaNames = new String[0];
        } // end of if (null == aCaNames)

        // check caNames for nulls and empty strings
        for (int i = 0; i < aCaNames.length; i++)
        {
            if ((aCaNames[ i ] == null) || (aCaNames[ i ] == ""))
            {
                throw new IllegalArgumentException(ERR_NULL_CA_NAME_STRING);
            }
        } // end of for (int i = 0; i < aCaNames.length; i++)

        if ((aOptions < 0) || (aOptions > 3))
        {
            throw new IllegalArgumentException("Options argument out of range in authenticate()");
        } // end of if (( aOptions < 0 ) || ( aOptions > 3 ))


        // Check MIDP Permission
        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        SignaturePermission permission = new SignaturePermission("pki://*",
                "authenticate");
        appUtils.checkPermission(permission);


        int error[] = new int[ 1 ];
        byte retVal[] = sInstance._authenticate(sInstance.iServiceHandle,
                                                aByteArrayToAuthenticate,
                                                aOptions,
                                                aCaNames,
                                                aSecurityElementPrompt,
                                                false,
                                                error);
        checkError(error[ 0 ]);

        return retVal;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public static final byte[] authenticate(String aStringToAuthenticate,
                                            int aOptions, String[] aCaNames,
                                            String aSecurityElementPrompt)
    throws CMSMessageSignatureServiceException,
                UserCredentialManagerException
    {
        // null / empty aCaNames means "use any"
        if (null == aCaNames)
        {
            aCaNames = new String[0];
        } // end of if (null == aCaNames)

        // check caNames for nulls and empty strings
        for (int i = 0; i < aCaNames.length; i++)
        {
            if ((aCaNames[ i ] == null) || (aCaNames[ i ] == ""))
            {
                throw new IllegalArgumentException(ERR_NULL_CA_NAME_STRING);
            }
        } // end of for (int i = 0; i < aCaNames.length; i++)

        if ((aOptions < 0) || (aOptions > 3))
        {
            throw new IllegalArgumentException("Options argument out of range in authenticate()");
        } // end of if (( aOptions < 0 ) || ( aOptions > 3 ))

        int error[] = new int[ 1 ];
        byte[] bytesToAuthenticate = null;
        try
        {
            bytesToAuthenticate =
                aStringToAuthenticate.getBytes(STRING_ENCODING);
        }
        catch (UnsupportedEncodingException e)
        {
            throw new CMSMessageSignatureServiceException(
                CMSMessageSignatureServiceException.CRYPTO_FAILURE);
        } // end of try-catch

        byte retVal[] = sInstance._authenticate(
                            sInstance.iServiceHandle,
                            bytesToAuthenticate,
                            aOptions, aCaNames, aSecurityElementPrompt, true, error);

        checkError(error[ 0 ]);

        return retVal;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public static final byte[] sign(String aStringToSign, int aOptions,
                                    String[] aCaNames,
                                    String aSecurityElementPrompt)
    throws CMSMessageSignatureServiceException,
                UserCredentialManagerException
    {
        // null / empty aCaNames means "use any"
        if (null == aCaNames)
        {
            aCaNames = new String[0];
        } // end of if (null == aCaNames)

        // check caNames for nulls and empty strings
        for (int i = 0; i < aCaNames.length; i++)
        {
            if ((aCaNames[ i ] == null) || (aCaNames[ i ] == ""))
            {
                throw new IllegalArgumentException(ERR_NULL_CA_NAME_STRING);
            }
        } // end of for (int i = 0; i < aCaNames.length; i++)

        if ((aOptions < 0) || (aOptions > 3))
        {
            throw new IllegalArgumentException("Options argument out of range in sign(");
        } // end of if (( aOptions < 0 ) || ( aOptions > 3 ))

        int error[] = new int[ 1 ];
        byte[] bytesToSign = null;
        try
        {
            bytesToSign =
                aStringToSign.getBytes(STRING_ENCODING);
        }
        catch (UnsupportedEncodingException e)
        {
            throw new CMSMessageSignatureServiceException(
                CMSMessageSignatureServiceException.CRYPTO_FAILURE);
        } // end of try-catch

        byte retVal[] = sInstance._sign(
                            sInstance.iServiceHandle,
                            bytesToSign,
                            aOptions, aCaNames, aSecurityElementPrompt, error);

        checkError(error[ 0 ]);

        return retVal;
    }

    /**
    * Creates a new <code>CMSMessageSignatureService</code> instance.
    *
    */
    private CMSMessageSignatureService()
    {
        iServiceHandle = _createSignatureService();
        if (iServiceHandle <= 0)
        {
            throw new OutOfMemoryError(ERR_UNEXPECTED + iServiceHandle);
        } // end of if ( iServiceHandle < 0)


    }

    /**
    * Checks the error code received from native side and throws exceptions
    * accordingly
    */
    private static void checkError(int aError)
    throws CMSMessageSignatureServiceException
    {
        switch (aError)
        {
        case NO_ERROR:
        {
            // everything is ok
            break;
        }
        case NOT_FOUND:
        {
            throw new CMSMessageSignatureServiceException(
                CMSMessageSignatureServiceException.CRYPTO_NO_CERTIFICATE);
        }
        case ERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ERR_ARGUMENT_STRING);
        }
        case ERR_LOCKED:
        {
            throw new SecurityException(ERR_LOCKED_STRING);
        }

        case ERR_INVALID_CA_NAME:
        {
            throw new
            IllegalArgumentException(ERR_INVALID_CA_NAME_STRING);
        }
        case ERR_INVALID_CHARACTERS_IN_CA_NAME:
        {
            throw new
            IllegalArgumentException(
                ERR_INVALID_CHARACTERS_IN_CA_NAME_STRING);
        }

        default:
        {
            throw new CMSMessageSignatureServiceException(
                CMSMessageSignatureServiceException.CRYPTO_FAILURE);
        }
        } // end of switch (error[ 0 ])
    }


    /**
    * Creates the native side signature service
    */
    private native int _createSignatureService();

    /**
    * Creates a signature for authentication purposes.
    *
    */
    private native byte[] _authenticate(int aServiceHandle,
                                        byte aByteArrayToAuthenticate[],
                                        int aOptions, String aCaNames[],
                                        String aSecurityElementPrompt,
                                        boolean aShowData,
                                        int aError[]);

    /**
    * Creates a signed message
    *
    */
    private native byte[] _sign(int aServiceHandle,
                                byte aByteArrayToSign[],
                                int aOptions, String aCaNames[],
                                String aSecurityElementPrompt,
                                int aError[]);
} // CMSMessageSignatureService
