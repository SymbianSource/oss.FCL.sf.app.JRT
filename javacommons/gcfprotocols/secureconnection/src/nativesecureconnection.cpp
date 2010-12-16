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
* Description:  ?Description
 *
*/


#include "nativesecureconnection.h"
#include "jniarrayutils.h"
#include "logger.h"
#include "nativecertificatemanager.h"

using namespace java;

NativeSecureConnection::NativeSecureConnection(const char * aName,
        int aMode, const char * aHost, int aPort) :
        NativeSocketConnection(aName, aMode, aHost, aPort)
{
    JELOG2(ESOCKET);
    mMode = aMode;
    mPort = aPort;
    mSecureSocketBuffer = NULL;
}

int NativeSecureConnection::doHandshake(int aSocket, int aType, int aApn, int * err1, int *err2)
{
    JELOG2(ESOCKET);
    int ret;
    //int temp;
    mSockDesc = aSocket;
    initialiseSslConnection();
    if (mSockDesc == -1)
    {
        mSockDesc = socketOpen(-1, aType, aApn,err1);
    }
    //LOG1(ESOCKET,EInfo ,"setdefault  =  %d",&temp);
    //err = &temp;
    LOG1(ESOCKET,EInfo, "setdefault  =  %d", *err1);
    if (mSockDesc < 0)
    {
        return mSockDesc;      // error code, whie creating socket
    }
    ret = secureHandshake();

    if (ret < 0)
    {
        // ssl handshake failed
        LOG(ESOCKET,EInfo,"secure handshake failed");
        ELOG1(ESOCKET, "handshake error code =  %d",
              ret);
    }
    else
    {  // handshake ok, verify the certificate
        X509 * peer = SSL_get_peer_certificate(mSslObj);
        getCertificateInformation(peer);
        int validflag = NativeCertificateManager::validateX509Certificate(peer);
        LOG1(ESOCKET,EInfo, " NativeCertificateManager::validateX509Certificate returned %d",validflag);
        if (validflag < 0)
        {
            *err2 = validflag;
            return 0;
        }
        ret = SSL_get_verify_result(mSslObj);

        LOG1(ESOCKET,EInfo, "Secure socket is open, socket descriptor is %d",
             mSockDesc);
    }
    return ret;
}

X509* NativeSecureConnection::getCertificate()
{
    X509 * peer = SSL_get_peer_certificate(mSslObj);
    return peer;
}

OS_EXPORT int NativeSecureConnection::readBytes(JNIEnv& aJni,
        jbyteArray aJavaBuffer)
{
    JELOG2(ESOCKET);
    if (mSecureSocketBuffer == NULL)
    {
        /*
        Memory is allocated to mSecureSocketBuffer only once during the first call of this readBytes() function
        Since the value of mBufferSize will be unknown(uninitialized) during the constructor call, it is not
        possible to do this operation inside the constructor.
        */

        mSecureSocketBuffer = new char[mBufferSize];
    }
    // secureSocketRead() function will actually make the OpenC call to read the data from the socket
    mBytesRead = secureSocketRead(mSecureSocketBuffer, mBufferSize);

    /* Copy the data read from the native buffer to the java buffer. Copy only "bytesRead" number of bytes.
       Because it is possible that the java request for some 'n' bytes to be read, but actually lesser bytes of data was
       read from the socket.
    */
    if (mBytesRead > 0)
    {
        JNIArrayUtils::CopyToJava(aJni, mSecureSocketBuffer, mBytesRead,
                                  aJavaBuffer, 0, mBytesRead);
    }
    else if (mBytesRead < 0)
        return mBytesRead;   // holds ssl read error code

    return mBytesRead;
}

OS_EXPORT int NativeSecureConnection::writeBytes(JNIEnv& aJni,
        jbyteArray aJavaBuffer, int aOffset, int aLength)
{
    JELOG2(ESOCKET);
    char* iWriteBuffer = new char[aLength + 1];

    /* Copy the data to be written from java buffer to the native buffer.  */
    JNIArrayUtils::CopyToNative(aJni, aJavaBuffer, aOffset, aLength,
                                iWriteBuffer);

    // secureSocketWrite() function will actually make the OpenC call to read the data from the socket
    int num = secureSocketWrite(iWriteBuffer, aLength);
    delete[] iWriteBuffer;
    iWriteBuffer = NULL;
    return num;
}


/* -------------------------------------------------------------
This function initialises all the SSL libraries

Open C APIs used

SSL_load_error_strings(): Loads and registers all the libssl error
                          strings

OpenSSL_add_all_algorithms(): adds all algorithms to the the internal
                              table.

SSL_library_init(): iniitializes SSL library by registering algorithms
                    This should be called before calling any SSL apis

SSL_CTX_new: creates a new context object as a framework to
             establish TLS/SSL enabled connections.
             It initializes the list of ciphers, the session cache
             setting,the callbacks, the keys and certificates, and
             the options to its default values

SSL_new : SSL_new Creates a new SSL structure and inherits the settings
          of underlying ctx

--------------------------------------------------------------*/

OS_EXPORT void NativeSecureConnection::initialiseSslConnection()
{
    JELOG2(ESOCKET);


    SSL_load_error_strings(); // No return value

    ERR_load_BIO_strings();   // No return value

    OpenSSL_add_all_algorithms(); // No return value


    SSL_library_init();


    mCtxObj = SSL_CTX_new(SSLv23_client_method());
    if (mCtxObj == NULL)
    {
        ELOG(ESOCKET, "Initialize ssl connection, ctx is null");
    }
    if ((mSslObj = SSL_new(mCtxObj)) == NULL)
    {
        ELOG(ESOCKET, "SSL_new failed");
    }
    else
    {
        ILOG(ESOCKET, "SSL_new success");
    }
}

/* -------------------------------------------------------------
This function performs SSL handshake with the server

Open C APIs used:

BIO_new_socket(): returns a socket BIO (I/O abstraction)

SSL_set_bio(): connect a SSL object with a BIO

SSL_connect(): SSL_connect()initiates the TLS/SSL handshake with a server.
              The communication channel must already have been set and assigned
              to the ssl by setting an underlying BIO.
-----------------------------------------------------------------*/

OS_EXPORT int NativeSecureConnection::secureHandshake()
{
    JELOG2(ESOCKET);
    int retVal;
    int error;
    // set up bio object
    mBio = BIO_new_socket(mSockDesc, BIO_NOCLOSE);
    if (mBio == NULL)
    {
        ELOG(ESOCKET, "BIO_new_socket failed");
    }
    else
    {
        ILOG(ESOCKET, "BIO_new_socket success");
    }

    //Set the bio object to SSL
    SSL_set_bio(mSslObj, mBio, mBio);
    retVal = SSL_connect(mSslObj);

    // If SSL_connect fails it returns -1. To get the error code SSL_get_error() api
    // is called along with return value and ssl object
    if (retVal < 0)
    {
        error = SSL_get_error(mSslObj, retVal);
        retVal = -error;

    }
    return retVal;

}


/*--------------------------------------------------------------
This function calls SSL_write() to send data over a secure socket

---------------------------------------------------------------*/

int NativeSecureConnection::secureSocketWrite(char *aBuf, int len)
{

    ILOG1(ESOCKET, "writebuffer is %s", aBuf);
    int error;
    int retVal = SSL_write(mSslObj, aBuf, len);
    ILOG1(ESOCKET, "--NativeSecureConnection::secureSocketWrite len=  %d", retVal);
    if (retVal < 0)
    {
        ELOG1(ESOCKET, "secure socket write returned %d", retVal);

        // If SSL_write fails it returns -1. To get the error code
        // SSL_get_error() api is called along with return value
        // and ssl object
        error = SSL_get_error(mSslObj, retVal);
        retVal = -error;
    }
    return retVal;
}


/*------------------------------------------------------------
This function calls SSL_read() to recieve data over a secure socket

-------------------------------------------------------------*/

int NativeSecureConnection::secureSocketRead(char *aBuf, int len)
{

    int error;
    int retVal = SSL_read(mSslObj, aBuf, len);
    ILOG1(ESOCKET, "no of bytes read is %d", retVal);
    if (retVal < 0)
    {
        // If SSL_read fails it returns -1. To get the error code
        // SSL_get_error() api is called along with return value
        // and ssl object
        error = SSL_get_error(mSslObj, retVal);
        retVal = -error;
    }
    return retVal;
}

OS_EXPORT void NativeSecureConnection::stopReading()
{
    return;
}

OS_EXPORT void NativeSecureConnection::stopWriting()
{
    return;
}


/*--------------------------------------------------------------
This function closes the secure socket

Open C APIs used :

SSL_CTX_free(): decrements the reference count of ctx
                 and removes the SSL_CTX object pointed to by ctx
                 and frees up the allocated memory if the reference count has reached 0.

ERR_free_strings(): frees all the loaded error strings

close() : closes the secure socket descriptor

---------------------------------------------------------------*/
int NativeSecureConnection::secureSocketClose()
{
    JELOG2(ESOCKET);
    int retVal;

    SSL_CTX_free(mCtxObj); // No return value


    ERR_free_strings(); // No return value

    retVal = close(mSockDesc);
    if (retVal < 0)
    {
        retVal = -errno;
    }
    return retVal;

}

/*--------------------------------------------------------------
This function gets the peer certificate related information

Open C APIs used

X509_get_serialNumber(): gets the serial number of the certificate

X509_get_subject_name(): get the certificate subject name

X509_get_version(): returns certifcates version

X509_get_notBefore(): get the certificate time after, value is string
                      in the form of yymmddhhmmss (time w.r.t to GMT)

X509_get_notAfter(): get the certificate time after, value is string
                     in the form of yymmddhhmmss (time w.r.t to GMT)

SSL_CIPHER_get_name(): returns the name of the SSL cipher

----------------------------------------------------------------*/
void NativeSecureConnection::getCertificateInformation(X509 *aPeer)
{
    JELOG2(ESOCKET);
    ASN1_INTEGER *x1 = X509_get_serialNumber(aPeer);

    // conversion of serial number into a string
    BIGNUM *bntmp = ASN1_INTEGER_to_BN(x1, NULL);
    char *serial_no = BN_bn2hex(bntmp);


    char subject_name[512];
    X509_NAME_oneline(X509_get_subject_name(aPeer), subject_name,
                      sizeof(subject_name));

    // get the certificate issure name, value is string
    char issuer_name[512];
    X509_NAME_oneline(X509_get_issuer_name(aPeer), issuer_name,
                      sizeof(issuer_name));

    long version = X509_get_version(aPeer);
    char version_string[10];
    sprintf(version_string,"%ld",version);


    ASN1_TIME *cert_time;
    char *time_notbef;
    cert_time = X509_get_notBefore(aPeer);
    time_notbef = (char*) cert_time->data;

    ASN1_TIME *cert_time_after;
    char *time_notafter;
    cert_time_after = X509_get_notAfter(aPeer);
    time_notafter = (char*) cert_time_after->data;

    // get the signature algorithm type, value is string. ex = sha1WithRSAEncryption
    int type = OBJ_obj2nid((aPeer)->sig_alg->algorithm);
    const char *alg_name = OBJ_nid2ln(type);

    //get the cipher name used by this ssl
    const char *cipher_name = SSL_CIPHER_get_name(SSL_get_current_cipher(
                                  mSslObj));

    const char *protocol_name = SSL_get_version(mSslObj);

    PLOG1(ESOCKET, "Peer certificate subject name %s", subject_name);
    PLOG1(ESOCKET, "Peer certificate issuer name %s", issuer_name);
    PLOG1(ESOCKET, "Peer certificate serial number %s", serial_no);
    PLOG1(ESOCKET, "Peer certificate serial number strlen %d", strlen(
              serial_no));
    PLOG1(ESOCKET, "Peer certificate version string  %s", version_string);
    PLOG1(ESOCKET, "Peer certificate get not before time %s", time_notbef);
    PLOG1(ESOCKET, "Peer certificate get not time after %s",
          time_notafter);
    PLOG1(ESOCKET, "alg_name %s", alg_name);
    PLOG1(ESOCKET, "current cipher %s", cipher_name);
    PLOG1(ESOCKET, "current protocol %s", protocol_name);

    // Allocate memory for 9 string as we know that 9 certificate properties will be retrieved
    mResult = (char **) malloc(9 * sizeof(char *));

    mResult[0] = (char *) malloc((strlen(subject_name) + 1) * sizeof(char));
    strcpy(mResult[0], subject_name);

    mResult[1] = (char *) malloc((strlen(issuer_name) + 1) * sizeof(char));
    strcpy(mResult[1], issuer_name);

    mResult[2] = (char *) malloc((strlen(serial_no) + 1) * sizeof(char));
    strcpy(mResult[2], serial_no);

    mResult[3] = (char *) malloc((strlen(time_notbef) + 1) * sizeof(char));
    strcpy(mResult[3], time_notbef);

    mResult[4] = (char *) malloc((strlen(time_notafter) + 1) * sizeof(char));
    strcpy(mResult[4], time_notafter);

    mResult[5] = (char *) malloc((strlen(alg_name) + 1) * sizeof(char));
    strcpy(mResult[5], alg_name);

    mResult[6] = (char *) malloc(20 * sizeof(char));
    strcpy(mResult[6], version_string);

    mResult[7] = (char *) malloc(50 * sizeof(char));
    strcpy(mResult[7], protocol_name);

    mResult[8] = (char *) malloc(50 * sizeof(char));
    strcpy(mResult[8], cipher_name);
}

OS_EXPORT char ** NativeSecureConnection::getSecurityInfo()
{
    JELOG2(ESOCKET);
    return mResult;
}

NativeSecureConnection::~NativeSecureConnection()
{
    JELOG2(ESOCKET);
    if (mSecureSocketBuffer != NULL)
    {
        delete[] mSecureSocketBuffer;
        mSecureSocketBuffer = NULL;
    }
    int i;
    for (i=0; i<9; i++)
    {
        delete[] mResult[i];
    }
    mResult = NULL;
}
