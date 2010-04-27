/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "javajniutils.h"
#include "com_nokia_mj_impl_security_midp_authorization_ClassInstantiator.h"

using namespace std;

// forward declarations
void handleException(JNIEnv*, const char *);

JNIEXPORT
jobject JNICALL Java_com_nokia_mj_impl_security_midp_authorization_ClassInstantiator__1newInstance(
    JNIEnv* env, jclass /*peer*/, jstring className, jobjectArray args)
{
    //It is expected that class name is formatted in Java side to
    //format com/xxx/yyy

    jboolean isCopy;
    //Get the class name in utf8 format.
    const char* utf8ClassName(env->GetStringUTFChars(className, &isCopy));

    //Find the class
    jclass cls = env->FindClass(utf8ClassName);
    handleException(env,"Permission class not found");

    //Object to be returned
    jobject obj = 0;
    if (cls != 0)
    {
        //Create the signature. All the arguments are expected to be Strings.
        std::string signature = "(";
        int argCount = env->GetArrayLength(args); //How many arguments there are
        jvalue* jargs = new jvalue[argCount]; //Argument array for NewObjectA()
        for (int i = 0; i < argCount; i++)
        {
            signature += "Ljava/lang/String;"; //Append signature
            jobject o = env->GetObjectArrayElement(args, i); //Get the argument object
            jargs[i].l = o; //Store it to array
        }
        signature += ")V"; //Append the return type

        //Try to find the method id.
        jmethodID ctor = env->GetMethodID(cls,"<init>", signature.c_str());
        handleException(env, "No constructor found to match the supplied parameters");

        if (ctor)
        {
            //Construct the object
            obj = env->NewObjectA(cls, ctor, jargs);
            handleException(env, "Error instantiating a class with the supplied parameters");
        }
        delete[] jargs;
    }

    //Releasing the allocated class name
    env->ReleaseStringUTFChars(className, utf8ClassName);
    return obj;
}

void handleException(JNIEnv* env, const char * error_msg)
{
    jthrowable ex = env->ExceptionOccurred();
    if (ex)
    {
        env->ExceptionClear();
        jclass newExcClass;
        newExcClass = env->FindClass("java/lang/InstantiationException");
        if (newExcClass != 0)
        {
            env->ThrowNew(newExcClass, error_msg);
        }
    }
}

