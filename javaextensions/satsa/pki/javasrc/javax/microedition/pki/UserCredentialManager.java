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



package javax.microedition.pki;


import javax.microedition.securityservice.CMSMessageSignatureServiceException;
import com.nokia.mj.impl.rt.support.Jvm;




public final class UserCredentialManager
{

    /**
    * Algorithm identifier for a DSA signature key.
    */
    public static final java.lang.String ALGORITHM_DSA = "1.2.840.10040.4.1";


    /**
    * Algorithm identifier for an RSA signature key.
    */
    public static final java.lang.String ALGORITHM_RSA = "1.2.840.113549.1.1";


    /** Indicates a key used for authentication. */
    public static final int KEY_USAGE_AUTHENTICATION = 0;


    /** Indicates a key used for digital signatures. */
    public static final int KEY_USAGE_NON_REPUDIATION = 1;

    /** The only instance of this class */
    private static UserCredentialManager sInstance;

    /** Handle to the native side */
    private int iManagerHandle;

    /** Status code received from native side, no errors occured */
    private static final int NO_ERROR = 0;

    /** Status code received from native side, user cancelled */
    private static final int NOT_COMPLETED = 1;

    /** Error code received from native side */
    private static final int ERR_NOT_FOUND = -1;

    /** Error code received from native side */
    private static final int ERR_CANCEL = -3;

    /** Error code received from native side */
    private static final int ERR_NOT_SUPPORTED = -5;

    /** Error code received from native side */
    private static final int ERR_ARGUMENT = -6;

    /** Corresponding message */
    private static final String ERR_ARGUMENT_STRING= "Argument is out of range";

    /** Error code from native side */
    private static final int ERR_ALREADY_EXISTS = -11;

    /** Error code received from native side */
    private static final int ERR_NOT_READY = -18;

    /** Error code received from native side */
    private static final int ERR_CORRUPT = -20;

    /** Error code from native side, PIN is blocked */
    private static final int ERR_LOCKED = -22;

    /** Corresponding message */
    private static final String ERR_LOCKED_STRING = "False PIN code: not authorized to access certificate store";

    /** Error code received from native side, indicates bad (e.g. already used)
        name */
    private static final int ERR_BAD_NAME = -28;

    /** Corresponding message */
    private static final String ERR_BAD_NAME_STRING = "Certificate Display Name must be in standard format";

    /** Error code received from native side */
    private static final int ERR_KEY_USAGE = -11007;

    /** Corresponding message */
    private static final String ERR_KEY_USAGE_STRING = "Invalid keyUsage type";

    /** Error code received from native side */
    private static final int ERR_SE_NOT_FOUND = -4001;

    /** Error code received from native side */
    private static final int ERR_SE_NO_KEYS = -4002;

    /** Error code received from native side */
    private static final int ERR_ALREADY_REGISTERED = -4003;

    /** Corresponding message */
    private static final String ERR_ALREADY_REGISTERED_STRING =
        "Certificate Display Name or PKIPath is already registered";

    /** Error code from native side, CA name is invalid */
    private static final int ERR_INVALID_CA_NAME = -4004;

    /** Error message for invalid ca name */
    private static final String ERR_INVALID_CA_NAME_STRING =
        "Invalid CA name";

    /** Error code from native side, CA name has invalid characters*/
    private static final int ERR_INVALID_CHARACTERS_IN_CA_NAME = -4005;

    /** Error message for invalid ca name */
    private static final String ERR_INVALID_CHARACTERS_IN_CA_NAME_STRING=
        "Invalid characters in CA name";

    /** Unexpected error */
    private static final String ERR_UNEXPECTED =
        "Unexpected error: ";

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasatsa");
        }
        catch (Exception e)
        {

        }
        // create instance
        sInstance = new UserCredentialManager();
    }



    /**
    * Refer JSR-177 Specification for details
    */
    public static final boolean addCredential(
        java.lang.String aCertDisplayName,
        byte[] aPkiPath,
        java.lang.String aUri)
    throws UserCredentialManagerException
    {
        if ((aCertDisplayName == null) ||
                (aCertDisplayName == "") ||
                (aPkiPath == null))
        {
            throw new IllegalArgumentException("Certificate Display Name or PKIPath is null, or empty string is passed as argument");
        }

        int error = sInstance._addCredential(
                        sInstance.iManagerHandle,
                        aCertDisplayName, aPkiPath);
        boolean retVal = false;
        switch (error)
        {
        case NO_ERROR:
        {
            retVal = true;
            break;
        }

        case ERR_CANCEL:
        case NOT_COMPLETED:
        {
            // retVal is already false
            break;
        }

        case ERR_ALREADY_REGISTERED:
        {
            throw new IllegalArgumentException(
                ERR_ALREADY_REGISTERED_STRING);
        }
        case ERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ERR_ARGUMENT_STRING);
        }
        case ERR_BAD_NAME:
        {
            throw new IllegalArgumentException(ERR_BAD_NAME_STRING);
        }

        case ERR_NOT_SUPPORTED:
        case ERR_NOT_READY:
        default:
        {
            throw new UserCredentialManagerException(
                UserCredentialManagerException.CREDENTIAL_NOT_SAVED);
        }
        } // end of switch (error)
        return retVal;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public static final byte[] generateCSR(
        java.lang.String aNameInfo,
        java.lang.String aAlgorithm,
        int aKeyLen,
        int aKeyUsage,
        java.lang.String aSecurityElementID,
        java.lang.String aSecurityElementPrompt,
        boolean aForceKeyGen)
    throws UserCredentialManagerException,
                CMSMessageSignatureServiceException
    {
        if ((aNameInfo == "") || (aAlgorithm == "") || (aKeyLen <= 0))
        {
            throw new IllegalArgumentException(ERR_ARGUMENT_STRING);
        }
        java.lang.String nameInfo = null;
        if (aNameInfo == null)
        {
            // empty string is used to signal native side to generate aNameInfo
            nameInfo = "";
        }
        else
        {
            nameInfo = aNameInfo;
        } // end of else

        // generating new keys is not supported
        if (aForceKeyGen)
        {
            throw new UserCredentialManagerException(
                UserCredentialManagerException.SE_NO_KEYGEN);
        } // end of if (aForceKeyGen)

        int error[] = new int[ 1 ];
        byte[] retVal =
            sInstance._generateCSR(sInstance.iManagerHandle, nameInfo,
                                   aAlgorithm, aKeyLen, aKeyUsage,
                                   aSecurityElementID,
                                   aSecurityElementPrompt,
                                   error);
        switch (error[ 0 ])
        {
        case NO_ERROR:
        {
            // everything is ok
            break;
        }

        case ERR_CANCEL:
        {
            retVal = null;
            break;
        }


        case ERR_CORRUPT:
        case ERR_ALREADY_EXISTS:
        case ERR_SE_NO_KEYS:
        {
            throw new UserCredentialManagerException(
                UserCredentialManagerException.SE_NO_KEYS);
        }

        case ERR_SE_NOT_FOUND:
        {
            throw new UserCredentialManagerException(
                UserCredentialManagerException.SE_NOT_FOUND);
        }

        case ERR_KEY_USAGE:
        {
            throw new IllegalArgumentException(ERR_KEY_USAGE_STRING);
        }
        case ERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ERR_ARGUMENT_STRING);
        }
        case ERR_INVALID_CHARACTERS_IN_CA_NAME:
        {
            throw new IllegalArgumentException(
                ERR_INVALID_CHARACTERS_IN_CA_NAME_STRING);
        }
        case ERR_INVALID_CA_NAME:
        {
            throw new IllegalArgumentException(
                ERR_INVALID_CA_NAME_STRING);
        }
        case ERR_LOCKED:
        {
            throw new SecurityException(ERR_LOCKED_STRING);
        }
        case ERR_NOT_SUPPORTED:
        case ERR_NOT_READY:
        default:
        {
            throw new OutOfMemoryError(ERR_UNEXPECTED + error[ 0 ]);
        }
        }
        return retVal;
    }

    /**
    * Refer JSR-177 Specification for details
    */
    public static final boolean removeCredential(
        java.lang.String aCertDisplayName,
        byte[] aIssuerAndSerialNumber,
        java.lang.String aSecurityElementID,
        java.lang.String aSecurityElementPrompt)
    throws UserCredentialManagerException
    {
        if ((aCertDisplayName == null) ||
                (aCertDisplayName == "") ||
                (aIssuerAndSerialNumber == null))
        {
            throw new IllegalArgumentException("Certificate Display Name or IssuerAndSerialNumber is null, or an empty string is passed as argument");
        }
        int error = sInstance._removeCredential(
                        sInstance.iManagerHandle,
                        aCertDisplayName, aIssuerAndSerialNumber, aSecurityElementID,
                        aSecurityElementPrompt);
        boolean retVal = false;
        switch (error)
        {
        case NO_ERROR:
        {
            retVal = true;
            break;
        }


        case ERR_CANCEL:
        case NOT_COMPLETED:
        {
            // retVal is already false
            break;
        }

        case ERR_NOT_FOUND:
        {
            throw new UserCredentialManagerException(
                UserCredentialManagerException.CREDENTIAL_NOT_FOUND);
        }

        case ERR_ARGUMENT:
        {
            throw new IllegalArgumentException(ERR_ARGUMENT_STRING);
        }

        case ERR_SE_NOT_FOUND:
        {
            throw new UserCredentialManagerException(
                UserCredentialManagerException.SE_NOT_FOUND);
        }

        case ERR_NOT_SUPPORTED:
        case ERR_NOT_READY:
        default:
        {
            throw new OutOfMemoryError(ERR_UNEXPECTED + error);
        }
        } // end of switch (error)
        return retVal;
    }

    /**
    * Creates a new <code>UserCredentialManager</code> instance.
    * The constructor is private as the users of this class are not supposed
    * to create instances; all public methods are static.
    */
    private UserCredentialManager()
    {

        iManagerHandle = _createUCManager();

        if (iManagerHandle <= 0)
        {
            throw new OutOfMemoryError(ERR_UNEXPECTED + iManagerHandle);
        } // end of if (iManagerHandle < 0)



    }

    /**
    * Creates the native side credential manager
    *
    * @return handle to native manager
    */
    private native int _createUCManager();


    /**
    * Adds a certificate to a certificate store
    *
    */
    private native int _addCredential(int aManagerHandle,
                                      java.lang.String aCertDisplayName,
                                      byte[] aPkiPath);

    /**
    * Removes a certifate from a certifate store
    *
    */
    private native int _removeCredential(
        int aManagerHandle,
        java.lang.String aCertDisplayName,
        byte[] aIssuerAndSerialNumber,
        java.lang.String aSecurityElementID,
        java.lang.String aSecurityElementPrompt);

    /**
    * Generates a certifate signing request.
    *
    */
    private native byte[] _generateCSR(
        int aManagerHandle,
        java.lang.String aNameInfo,
        java.lang.String aAlgorithm,
        int aKeyLen,
        int aKeyUsage,
        java.lang.String aSecurityElementID,
        java.lang.String aSecurityElementPrompt,
        int[] aError);

} // UserCredentialManager
