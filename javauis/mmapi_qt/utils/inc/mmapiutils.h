#ifndef MMAPIUTILS_H
#define MMAPIUTILS_H
#include "jni.h"
#include "e32base.h"
#include "BADESCA.H"

class MMAPIUtils
{
public:
    
    static TInt CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer,
                                    jbyteArray aJavaBuffer, TInt aOffset, TInt aLength);  
    static jstring CreateJavaString(JNIEnv& aJni, const TDesC16& aString);

    static jobjectArray CopyToNewJavaStringArrayL(JNIEnv& aJni, const CDesCArray& aNativeArray);
    
    static void AddToJavaStringArrayL(JNIEnv& aJni, jobjectArray& aContainer, TInt aPosition, const TDesC& aString);
};

#endif // MMAPIUTILS_H
