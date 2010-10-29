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

#ifndef NATIVESECURECONNECTION_H
#define NATIVESECURECONNECTION_H

#include <logger.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <openssl/x509v3.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/dsa.h>      // for DSA functions
#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <time.h>

#include "nativesocketconnection.h"
#include "javaosheaders.h"

namespace java
{

/**
 * This is the base class for Native Peer of secure sockets.
 * It derives from NativeSocketConnection native class, to reuse the socket
 * methods like getsocketoption, setsocketoption etc.
 * SecureConnectionImpl in java will make calls to this class to create a carry
 * out the all secure socket operations.
 * StreamConnectionBase in java will also call the readBytes() and writeBytes()
 * methods of this class to get/send data from the native secure socket buffer.
 * NativeStreamBase will have stored a handle to the Object and uses it to call
 * these methods.
 */

class NativeSecureConnection: public NativeSocketConnection
{
public:

    NativeSecureConnection(const char* aName, int aMode, const char* aHost,
                           int aPort);

    //Methods inherited from NativeStreamBase

    /**
     * ReadBytes is called by StreamConnectionBase when the buffer at input
     * stream buffer runs out of data.
     * @param[in]  aJavaBuffer: Byte array to which the data read is copied into
     * @return the number of bytes read. If error, then the
     * value of Ssl_get_error should be negated and returned. (return -error)
     */
    OS_IMPORT int readBytes(JNIEnv& aJni, jbyteArray aJavaBuffer);

    /**
     * writeBytes is called by StreamConnectionBase when the output stream
     * buffer in java overflows
     * It is also called if flush() is called at the Java side.
     * @param[in]  aJavaBuffer: A byte array of length aLength
     * @return the number of bytes written. If error, then the
     *    value of Ssl_get_error should be negated and returned. (return -error)
     */
    OS_IMPORT int writeBytes(JNIEnv& aJni, jbyteArray aJavaBuffer, int aOffset,
                             int aLength);

    /**
     * StopReading is called by StreamConnectionBase once InputStream is closed
     * from the Application.
     */
    OS_IMPORT void stopReading();

    /**
     * StopWriting is called by StreamConnectionBase OutputStream is closed
     * from the Application.
     */
    OS_IMPORT void stopWriting();

    //Internal methods


    /**
     * InitialiseSslConnection is internal function
     * initializes Ssl library
     * creates Ssl context object and Ssl obejct
     */
    void initialiseSslConnection();

    /**
     * SecureHandshake is internal function - performs securehandshake
     * returns the open C API SSL_connect() return value
     * If error, then the value os Ssl_get_error should be negated and returned
     * (return -error)
     */
    int secureHandshake();

    /**
     * doHandshake is internal function to open a socket and secure it
     * returns the open C API SSL_connect() return value
     * If error, then the value os Ssl_get_error should be negated and returned
     * (return -error)
     */
    int doHandshake(int aSocket, int aType, int aApn, int *err1, int * err2);

    //   security info methods
    void getCertificateInformation(X509 *);

    OS_IMPORT char ** getSecurityInfo();

    /**
     * securesocketWrite is internal function called by writeBytes() method.
     * @param[in]  aWriteBuf: A character array of length len holding the data
     *                        to be written.
     * @return the number of bytes written. If error, then the
     *   value of Ssl_get_error should be negated and returned. (return -errror)
     */
    int secureSocketWrite(char *aWriteBuf, int aLen);

    /**
     * socketRead is internal function called by readBytes() method.
     * @param[in]  aReadBuf: A character array of length len.
     * @return the number of bytes read. If error, then the
     *         value of Ssl_get_error should be negated and returned. (return -error)
     */
    int secureSocketRead(char *aReadBuf, int aLen);

    /**
     * SecureSocketClose is called by SocketConnectionImpl to close the socket
     * connection.
     * @return 0 on successful completion. If error, then the
     *         value of errno should be negated and returned. (return -errno)
     */
    int secureSocketClose();

    X509* getCertificate();

    ~NativeSecureConnection();

private:
    char **mResult;
    int mMode;
    int mPort;
    BIO * mBio;
    SSL * mSslObj;
    SSL_CTX * mCtxObj;
    char* mSecureSocketBuffer;
    bool mIsSecureConnected;
    int mBytesRead;

};

}
#endif // NATIVESECURECONNECTION_H
