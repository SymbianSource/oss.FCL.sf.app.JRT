/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#include <QRect>
#include <QStringList>
#include <QByteArray>
#include <QStack>

#include "jniutils.h"
#include "swtlog.h"
#include "autorelease.h"
#include "swtapplication.h"
#include "execexceptionhandler.h"
#include "graphics.h"

namespace Java { namespace eSWT {

using namespace Java::GFX;

// Ids for cached jmethodIDs
typedef enum
{
    // [CLASS]_[METHOD]_[PARAMS]_[RETURN]
    DISPLAY_EVENTPROCESS_IIIIIIIISTRING_Z = 0,
    RECTANGLE_INIT_IIII_V,
    POINT_INIT_II_V,
    SWT_ERROR_I_V,

    NUM_SWT_JMETHODIDS
} SwtJMethodId;

// Ids for cached jclasses
typedef enum
{
    // [CLASS]
    RECTANGLE = 0,
    POINT,
    SWT,
    STRING,
    DISPLAY,

    NUM_SWT_JCLASSES
} SwtJClass;

JniUtils::JniUtils(JNIEnv* aEnv) : mUIThreadJniEnv(aEnv)
{
    SWT_LOG_FUNC_CALL();

    // Get local class refs. These classes are needed by Display so they should
    // be already loaded at this point. This doesn't cause any additional
    // classes, that wouldn't potentially be needed, to be loaded.
    mJclasses = new jclass[NUM_SWT_JCLASSES];
    ::memset( mJclasses, 0, sizeof(jclass)*NUM_SWT_JCLASSES );
    mJclasses[RECTANGLE] = mUIThreadJniEnv->FindClass("org/eclipse/swt/graphics/Rectangle");
    mJclasses[POINT] = mUIThreadJniEnv->FindClass("org/eclipse/swt/graphics/Point");
    mJclasses[SWT] = mUIThreadJniEnv->FindClass("org/eclipse/swt/SWT");
    mJclasses[STRING] = mUIThreadJniEnv->FindClass("java/lang/String");
    mJclasses[DISPLAY] = mUIThreadJniEnv->FindClass("org/eclipse/swt/widgets/Display");

    // Check that local class refs ok
    for (int i = 0; i < NUM_SWT_JCLASSES; ++i)
    {
        if (!mJclasses[i])
        {
            throw std::bad_alloc();
        }
    }

    // Create global class refs for caching. Global refs can be used across 
    // threads. Keeping global class refs ensures that the classes won't get
    // unloaded. 
    for (int i = 0; i < NUM_SWT_JCLASSES; ++i)
    {
        jclass globalRef = static_cast<jclass>(mUIThreadJniEnv->NewGlobalRef( mJclasses[i] ));
        mUIThreadJniEnv->DeleteLocalRef( mJclasses[i] );
        mJclasses[i] = globalRef;
    }

    // Check that all global class refs ok
    for (int i = 0; i < NUM_SWT_JCLASSES; ++i)
    {
        if (!mJclasses[i])
        {
            // Free the ones already allocated before throwing
            for (int j = 0; j < NUM_SWT_JCLASSES; ++j)
            {
                if( mJclasses[j] )
                {
                    mUIThreadJniEnv->DeleteGlobalRef(mJclasses[j]);
                    mJclasses[j] = NULL;
                }
            }
            throw std::bad_alloc();
        }
    }

    // Compute methodIDs for caching. MethodIDs have the same values for all 
    // threads so the cached values may be used by any thread. MethodIDs are 
    // valid until objects are garbage collected or classes unloaded. 
    mJmethodIds = new jmethodID[NUM_SWT_JMETHODIDS];
    ::memset( mJmethodIds, 0, sizeof(jmethodID)*NUM_SWT_JMETHODIDS );
    mJmethodIds[DISPLAY_EVENTPROCESS_IIIIIIIISTRING_Z] = mUIThreadJniEnv->GetStaticMethodID(mJclasses[DISPLAY], "eventProcess", "(IIIIIIIILjava/lang/String;)Z");
    mJmethodIds[RECTANGLE_INIT_IIII_V] = mUIThreadJniEnv->GetMethodID(mJclasses[RECTANGLE], "<init>", "(IIII)V");
    mJmethodIds[POINT_INIT_II_V] = mUIThreadJniEnv->GetMethodID(mJclasses[POINT], "<init>", "(II)V");
    mJmethodIds[SWT_ERROR_I_V] = mUIThreadJniEnv->GetStaticMethodID(mJclasses[SWT], "error", "(I)V");

    // Check that methodIDs are ok
    for (int i = 0; i < NUM_SWT_JMETHODIDS; ++i)
    {
        if (!mJmethodIds[i])
        {
            throw std::bad_alloc();
        }
    }
}

JniUtils::~JniUtils()
{
    SWT_LOG_FUNC_CALL();

    for (int j = 0; j < NUM_SWT_JCLASSES; ++j)
    {
        if (mJclasses[j])
        {
            mUIThreadJniEnv->DeleteGlobalRef(mJclasses[j]);
            mJclasses[j] = NULL;
        }
    }
    delete mJclasses;
    mJclasses = NULL;

    delete mJmethodIds;
    mJmethodIds = NULL;
}

bool JniUtils::eventProcess(const QObject* aQObject, const int& aQEventType,
                            const int& a1, const int& a2, const int& a3,
                            const int& a4, const int& a5,
                            const jstring aString)
{
    SWT_LOG_FUNC_CALL();
    return eventProcess(
            NULL, 
            mJclasses[DISPLAY], 
            mJmethodIds[DISPLAY_EVENTPROCESS_IIIIIIIISTRING_Z], 
            reinterpret_cast<int>(aQObject),
            aQEventType, a1, a2, a3, a4, a5, aString);
}

bool JniUtils::eventProcess(jobject aObject, const jmethodID aMethodID,
        const QObject* aQObject, const int& aQEventType, const int& a1,
        const int& a2, const int& a3, const int& a4, const int& a5,
        const jstring aString)
{
    return eventProcess(
            aObject, 
            NULL, 
            aMethodID, 
            reinterpret_cast<int>(aQObject),
            aQEventType, a1, a2, a3, a4, a5, aString);
}

bool JniUtils::eventProcess(jclass aClazz, const jmethodID aMethodID,
        const QObject* aQObject, const int& aQEventType, const int& a1,
        const int& a2, const int& a3, const int& a4, const int& a5,
        const jstring aString)
{
    return eventProcess(
            NULL, 
            aClazz, 
            aMethodID, 
            reinterpret_cast<int>(aQObject),
            aQEventType, a1, a2, a3, a4, a5, aString);
}

bool JniUtils::eventProcess(jobject aObject, jclass aClazz, const jmethodID aMethodID,
        const int& aQObject, const int& aQEventType, const int& a1,
        const int& a2, const int& a3, const int& a4, const int& a5,
        const jstring aString)
{
    SWT_LOG_FUNC_CALL();

    // Can't attempt callback if JVM is processing an exception. Event is
    // possibly lost by Java listener.
    if (mUIThreadJniEnv->ExceptionCheck() == JNI_TRUE)
    {
        return false;
    }

    if (aMethodID != NULL)
    {
        // Add object to the list of objects being handled. It's not safe to 
        // delete an object in its event or signal handler because that could
        // result in freeing memory that is still going to be accessed as we
        // return back through the call stack. This list of objects can be used
        // to determine if deletion of an object is safe in these terms. 
        JavaCallbackCounter counter(mObjectsBeingHandled, reinterpret_cast<QObject*>(aQObject));

        jboolean result;

        // Callback Java
        if(aObject == NULL)
        {
            result = mUIThreadJniEnv->CallStaticBooleanMethod(aClazz, aMethodID,
                    aQObject, aQEventType, swtApp->eventTime(), a1, a2, a3, a4, a5, aString);        
        }
        else
        {
            result = mUIThreadJniEnv->CallBooleanMethod(aObject, aMethodID,
                    aQObject, aQEventType, swtApp->eventTime(), a1, a2, a3, a4, a5, aString);
        }
        
        // If an exception has occurred then any native eventloop we have
        // started, e.g. when opening a QDialog, must exit to allow Java stack
        // unwinding to continue.
        if (mUIThreadJniEnv->ExceptionCheck() == JNI_TRUE)
        {
            if(!mExecStack.isEmpty())
            {
                ExecExceptionHandler* exec = mExecStack.top();
                if( !exec->isTerminated() )
                {
                    exec->terminate();
                }
            }
        }

        return (result == JNI_TRUE ? true : false);
    }
    return false;
}

void JniUtils::enterExec(QObject* aObject)
{
    QDialog* dialog = qobject_cast<QDialog*>( aObject );
    if( dialog )
    {
        mExecStack.push( new DialogExecExceptionHandler( dialog ) );
        return;
    }
    QMenu* menu = qobject_cast<QMenu*>( aObject );
    if( menu )
    {
        mExecStack.push( new MenuExecExceptionHandler( menu ) );
        return;
    }
    Q_ASSERT(false); // Something was given we don't know how to handle
}

void JniUtils::leaveExec()
{
    mExecStack.pop();
}

jobject JniUtils::NewJavaRectangle(JNIEnv* aEnv, const QRect& aRect)
{
    SWT_LOG_FUNC_CALL();

    jobject result = aEnv->NewObject(mJclasses[RECTANGLE],
            mJmethodIds[RECTANGLE_INIT_IIII_V], aRect.x(), aRect.y(),
            aRect.width(), aRect.height());
    if (!result)
    {
        throw std::bad_alloc();
    }
    return result;
}

jobject JniUtils::NewJavaPoint(JNIEnv* aEnv, const QSize& aSize)
{
    SWT_LOG_FUNC_CALL();

    jobject result = aEnv->NewObject(mJclasses[POINT],
            mJmethodIds[POINT_INIT_II_V], aSize.width(), aSize.height());
    if (!result)
    {
        throw std::bad_alloc();
    }
    return result;
}

jobject JniUtils::NewJavaPoint(JNIEnv* aEnv, const QPoint& aPoint)
{
    SWT_LOG_FUNC_CALL();
    jobject result = aEnv->NewObject(mJclasses[POINT],
            mJmethodIds[POINT_INIT_II_V], aPoint.x(), aPoint.y());
    if (!result)
    {
        throw std::bad_alloc();
    }
    return result;
}

SWTQT_EXPORT QString JniUtils::JavaStringToQString(JNIEnv* aEnv, jstring aJavaString)
{
    SWT_LOG_FUNC_CALL();
    if (aJavaString == NULL)
    {
        return QString();
    }
    jboolean isCopy;
    const jchar* javaChars = aEnv->GetStringChars(aJavaString, &isCopy);
    if (javaChars)
    {
        AutoReleaseStringChars cleaner(aEnv, aJavaString, javaChars);

        // This conversion should be ok for UCS-2 and UTF-16
        jsize length = aEnv->GetStringLength(aJavaString);
        return QString::fromUtf16(javaChars, length);
    }
    else
    {
        throw std::bad_alloc();
    }
 }

SWTQT_EXPORT jstring JniUtils::QStringToJavaString(JNIEnv* aEnv, const QString& aQString)
{
    SWT_LOG_FUNC_CALL();

    jstring result = aEnv->NewString(aQString.utf16(), aQString.size());
    if (!result)
    {
        throw std::bad_alloc();
    }
    return result;
}

jintArray JniUtils::NewJavaIntArray(JNIEnv* aEnv, int* aArray, const int& aItemCount)
{
    SWT_LOG_FUNC_CALL();

    jintArray result = aEnv->NewIntArray(aItemCount);
    if (result != NULL)
    {
        if (sizeof(int) != sizeof(jint))
        {
            jint* tmp = new jint[aItemCount];
            for (int i = 0; i < aItemCount; i++)
            {
                tmp[i] = static_cast<jint> (aArray[i]);
            }
            aEnv->SetIntArrayRegion(result, 0, aItemCount, tmp);
            delete[] tmp;
        }
        else
        {
            // This reinterpret_cast is safe now because we know that jint and int are same size.
            aEnv->SetIntArrayRegion(result, 0, aItemCount,
                    reinterpret_cast<jint*> (aArray));
        }
    }
    if (!result)
    {
        throw std::bad_alloc();
    }
    return result;
}

jobjectArray JniUtils::NewJavaStringArray(JNIEnv* aEnv, const QStringList& aStrList)
{
    SWT_LOG_FUNC_CALL();

    const int count = aStrList.size();
    jobjectArray result = aEnv->NewObjectArray(count, mJclasses[STRING],
            NULL);
    if (!result)
    {
        throw std::bad_alloc();
    }

    jstring javaString;
    for (int i = 0; i < count; ++i)
    {
        javaString = QStringToJavaString(aEnv, aStrList.at(i));
        aEnv->SetObjectArrayElement(result, i, javaString);
        aEnv->DeleteLocalRef(javaString);
    }

    return result;
}

QStringList JniUtils::JavaStringArrayToQStringList(JNIEnv* aEnv, jobjectArray aObjectArray)
{
    SWT_LOG_FUNC_CALL();
    jsize len = aEnv->GetArrayLength(aObjectArray);
    QStringList result;
    QString qtString;
    for (int i = 0; i < len; ++i)
    {
        qtString = JavaStringToQString(aEnv, 
                (jstring) aEnv->GetObjectArrayElement(aObjectArray, i));
        result.insert(i, qtString);
    }
    return result;
}

jbyteArray JniUtils::NewJavaByteArray(JNIEnv* aEnv, const QByteArray& aArray)
{
    SWT_LOG_FUNC_CALL();
    const int count = aArray.count();
    char* data = const_cast<char*> (aArray.data());
    jbyteArray result = aEnv->NewByteArray(count);
    if (result != NULL)
    {
        if (sizeof(char) != sizeof(jbyte))
        {
            jbyte* tmp = new jbyte[count];
            for (int i = 0; i < count; i++)
            {
                tmp[i] = static_cast<jbyte> (data[i]);
            }
            aEnv->SetByteArrayRegion(result, 0, count, tmp);
            delete[] tmp;
        }
        else
        {
            aEnv->SetByteArrayRegion(result, 0, count,
                    reinterpret_cast<jbyte*> (data));
        }
    }
    else
    {
        throw std::bad_alloc();
    }
    return result;
}

QByteArray JniUtils::JavaByteArrayToQByteArray(JNIEnv* aEnv, jbyteArray aByteArray)
{
    SWT_LOG_FUNC_CALL();
    jsize len = aEnv->GetArrayLength(aByteArray);
    QByteArray result = QByteArray(len,0);
    aEnv->GetByteArrayRegion(aByteArray, 0, len, reinterpret_cast<jbyte*>(result.data()));
    return result;
}

SWTQT_EXPORT void JniUtils::Throw(JNIEnv* aEnv, const int& aError)
{
    SWT_LOG_FUNC_CALL();
    aEnv->CallStaticVoidMethod(mJclasses[SWT], mJmethodIds[SWT_ERROR_I_V],
            aError);
}

void JniUtils::Throw(JNIEnv* aEnv, GfxException& aException)
{
    SWT_LOG_FUNC_CALL();
    
    jclass javaException = aEnv->FindClass(resolveException(aException.getErrorCode()));
    if( javaException )
    {
        aEnv->ThrowNew(javaException, aException.getMsg());
    }
}

jmethodID JniUtils::FindJavaMethodID(JNIEnv* aEnv, jobject aObject, const char* aMethodName,
        const char* aMethodSignature)
{
    SWT_LOG_FUNC_CALL();

    jclass clazz = aEnv->GetObjectClass(aObject);
    if (clazz)
    {
        jmethodID methodID = aEnv->GetMethodID(clazz, aMethodName,
                aMethodSignature);
        aEnv->DeleteLocalRef(clazz);SWT_LOG_DATA_3("MethodId for %s %s is %d", aMethodName, aMethodSignature, methodID);
        return methodID;
    }

    return NULL;
}

int JniUtils::javaCallbackCount()
{ 
    return mObjectsBeingHandled.size();
}

void JniUtils::GetJavaIntArrayRegionToIntArray(JNIEnv* aEnv, jintArray aArray, jsize aStart, jsize aLength, int* aBuffer)
{
    SWT_LOG_FUNC_CALL();
    if(sizeof(jint) == sizeof(int))
    {
        aEnv->GetIntArrayRegion(aArray, aStart, aLength, static_cast<jint*>(aBuffer));
    }
    else
    {
        jint* tmpBuffer = new jint[aLength];
        if(tmpBuffer)
        {
            aEnv->GetIntArrayRegion(aArray, aStart, aLength, tmpBuffer);
            for(int i = 0; i < aLength; ++i)
            {
                aBuffer[i] = tmpBuffer[i];
            }
            delete[] tmpBuffer;
            tmpBuffer = NULL;
        }
    }
}

void JniUtils::SetJavaIntArrayRegionFromIntArray(JNIEnv* aEnv, jintArray aArray, jsize aStart, jsize aLength, int* aBuffer)
{
    SWT_LOG_FUNC_CALL();
    if(sizeof(jint) == sizeof(int))
    {
        aEnv->SetIntArrayRegion(aArray, aStart, aLength, reinterpret_cast<jint*>(aBuffer));
    }
    else
    {
        jint* tmpBuffer = new jint[aLength];
        if(tmpBuffer)
        {
            for(int i = 0; i < aLength; ++i)
            {
                 tmpBuffer[i] = aBuffer[i];
            }
            aEnv->SetIntArrayRegion(aArray, aStart, aLength, tmpBuffer);
            delete[] tmpBuffer;
            tmpBuffer = NULL;
        }
    }
}

void JniUtils::GetJavaByteArrayRegionToCharArray(JNIEnv* aEnv, jbyteArray aArray, jsize aStart, jsize aLength, char* aBuffer)
{
    SWT_LOG_FUNC_CALL();
    if(sizeof(jbyte) == sizeof(char))
    {
        aEnv->GetByteArrayRegion(aArray, aStart, aLength, reinterpret_cast<jbyte*>(aBuffer));
    }
    else
    {
        jbyte* tmpBuffer = new jbyte[aLength];
        if(tmpBuffer)
        {
            aEnv->GetByteArrayRegion(aArray, aStart, aLength, tmpBuffer);
            for(int i = 0; i < aLength; ++i)
            {
                aBuffer[i] = tmpBuffer[i];
            }
            delete[] tmpBuffer;
            tmpBuffer = NULL;
        }
    }
}

void JniUtils::SetJavaByteArrayRegionFromCharArray(JNIEnv* aEnv, jbyteArray aArray, jsize aStart, jsize aLength, char* aBuffer)
{
    SWT_LOG_FUNC_CALL();
    if(sizeof(jbyte) == sizeof(char))
    {
        aEnv->SetByteArrayRegion(aArray, aStart, aLength, reinterpret_cast<jbyte*>(aBuffer));
    }
    else
    {
        jbyte* tmpBuffer = new jbyte[aLength];
        if(tmpBuffer)
        {
            for(int i = 0; i < aLength; ++i)
            {
                 tmpBuffer[i] = aBuffer[i];
            }
            aEnv->SetByteArrayRegion(aArray, aStart, aLength, tmpBuffer);
            delete[] tmpBuffer;
            tmpBuffer = NULL;
        }
    }
}

void JniUtils::GetJavaShortArrayRegionToShortArray(JNIEnv* aEnv, jshortArray aArray, jsize aStart, jsize aLength, short* aBuffer)
{
    SWT_LOG_FUNC_CALL();
    if(sizeof(jshort) == sizeof(short))
    {
        aEnv->GetShortArrayRegion(aArray, aStart, aLength, reinterpret_cast<jshort*>(aBuffer));
    }
    else
    {
        jshort* tmpBuffer = new jshort[aLength];
        if(tmpBuffer)
        {
            aEnv->GetShortArrayRegion(aArray, aStart, aLength, tmpBuffer);
            for(int i = 0; i < aLength; ++i)
            {
                aBuffer[i] = tmpBuffer[i];
            }
            delete[] tmpBuffer;
            tmpBuffer = NULL;
        }
    }
}

void JniUtils::SetJavaShortArrayRegionFromShortArray(JNIEnv* aEnv, jshortArray aArray, jsize aStart, jsize aLength, short* aBuffer)
{
    SWT_LOG_FUNC_CALL();
    if(sizeof(jshort) == sizeof(short))
    {
        aEnv->SetShortArrayRegion(aArray, aStart, aLength, reinterpret_cast<jshort*>(aBuffer));
    }
    else
    {
        jshort* tmpBuffer = new jshort[aLength];
        if(tmpBuffer)
        {
            for(int i = 0; i < aLength; ++i)
            {
                 tmpBuffer[i] = aBuffer[i];
            }
            aEnv->SetShortArrayRegion(aArray, aStart, aLength, tmpBuffer);
            delete[] tmpBuffer;
            tmpBuffer = NULL;
        }
    }
}

jobject JniUtils::CreateJavaImageData(JNIEnv* aEnv, ImageDataWrapper& aImageData)
{
    SWT_LOG_FUNC_CALL();
    
    jobject imageDataObj = 0;
    jclass imageDataClass = aEnv->FindClass("org/eclipse/swt/graphics/ImageData");
    if (imageDataClass)
    {
        jmethodID id = aEnv->GetMethodID(
            imageDataClass, 
            "<init>", 
            "(IIILorg/eclipse/swt/graphics/PaletteData;I[BI[B[BIIIIIII)V");
        if (id)
        {
            // Create palette data object
            jobject palette = CreateJavaPaletteData(aEnv, *aImageData.getPaletteData());
            // Create pixel array
            jbyteArray pixelData = NewJavaByteArray( aEnv, 
                QByteArray::fromRawData(
                    aImageData.getData(ImageDataWrapper::EPixelData), 
                    aImageData.getDataSize(ImageDataWrapper::EPixelData)
                )
            );
            
            jbyteArray maskData = 0;
            jbyteArray alphaData = 0;               
                
            // Create mask array
            if (aImageData.getDataSize(ImageDataWrapper::EMaskData))
            {
                 maskData = NewJavaByteArray( aEnv, 
                         QByteArray::fromRawData(
                            aImageData.getData(ImageDataWrapper::EMaskData), 
                            aImageData.getDataSize(ImageDataWrapper::EMaskData)
                         )
                 );
            }
            // Create alpha array
            if (aImageData.getDataSize(ImageDataWrapper::EAlphaData))
            {
                alphaData = NewJavaByteArray( aEnv, 
                        QByteArray::fromRawData(
                            aImageData.getData(ImageDataWrapper::EAlphaData), 
                            aImageData.getDataSize(ImageDataWrapper::EAlphaData)
                        )    
                );
            }
            
            if (palette && pixelData)
            {
                // Create image data object
                imageDataObj = aEnv->NewObject(
                    imageDataClass, id, aImageData.getWidth(), aImageData.getHeight(), 
                    aImageData.getDepth(), palette, aImageData.getScanlinePad(),
                    pixelData, aImageData.getMaskPad(), maskData,
                    alphaData, aImageData.getAlpha(), aImageData.getTransparentPixel(), 
                    aImageData.getImageType(), 
                    aImageData.getTopLeftX(), aImageData.getTopLeftY(), 
                    aImageData.getDisposalMethod(), aImageData.getDelayTime());
            }

            aEnv->DeleteLocalRef(palette);
            aEnv->DeleteLocalRef(pixelData);
            aEnv->DeleteLocalRef(maskData);
            aEnv->DeleteLocalRef(alphaData);
        }
        aEnv->DeleteLocalRef(imageDataClass);
    }

    return imageDataObj;
}

jobject JniUtils::CreateJavaPaletteData(JNIEnv* aEnv, PaletteDataWrapper& aPaletteData)
{
    SWT_LOG_FUNC_CALL();
    
    jobject result = 0;
    jclass paletteDataClass = aEnv->FindClass("org/eclipse/swt/graphics/PaletteData");
    if (paletteDataClass)
    {
        // Checks if direct palette
        if (aPaletteData.isDirect())
        {
            jmethodID id = aEnv->GetMethodID(paletteDataClass, "<init>", "(III)V");
            if (id)
            {
                // New direct palette data object is created
                PaletteDataWrapper::TDirectData* directData = aPaletteData.getDirectData();
                result = aEnv->NewObject(
                    paletteDataClass, 
                    id, directData->mRedMask, directData->mGreenMask, directData->mBlueMask);
            }
        }
        // Checks if indexed palette
        else
        {
            // Creates rgb array object
            jclass rgbClazz = aEnv->FindClass("org/eclipse/swt/graphics/RGB");
            if (rgbClazz)
            {
                const int count = aPaletteData.getIndexedRgbCount();
                jobjectArray rgbArray = aEnv->NewObjectArray(count, rgbClazz, 0);
                if (rgbArray)
                {
                    jmethodID rgbId = aEnv->GetMethodID(rgbClazz, "<init>", "(III)V");
                    if (rgbId)
                    {
                        // Fills the rgb array
                        PaletteDataWrapper::TIndexedRgb rgb;
                        for (int i = 0; i < count; ++i)
                        {
                            rgb = aPaletteData.getIndexedRgb(i);
                            jobject rgbObj = aEnv->NewObject(
                                rgbClazz, rgbId, rgb.mRed , rgb.mGreen, rgb.mBlue);
                            aEnv->SetObjectArrayElement(rgbArray, i, rgbObj);
                            aEnv->DeleteLocalRef(rgbObj);
                        }
                    }
                    // New indexed palette data object is created
                    jmethodID paletteId = aEnv->GetMethodID(
                        paletteDataClass, 
                        "<init>", 
                        "([Lorg/eclipse/swt/graphics/RGB;)V");
                    if (paletteId)
                    {
                        result = aEnv->NewObject(paletteDataClass, paletteId, rgbArray);
                    }
                    aEnv->DeleteLocalRef(rgbArray);
                }
                aEnv->DeleteLocalRef(rgbClazz);
            }
        }
        aEnv->DeleteLocalRef(paletteDataClass);
    }
    return result;
}

Image* JniUtils::CreateImage(JNIEnv* aEnv, jobject& aImageDataObj)
{
    SWT_LOG_FUNC_CALL();
    
    Image* result = NULL;
    ImageDataWrapper* imageData = GraphicsFactory::createImageData(0);
    jclass imageDataClazz = aEnv->GetObjectClass(aImageDataObj);
    // width
    jfieldID fid = aEnv->GetFieldID(imageDataClazz, "width", "I");
    int width = static_cast<int>(aEnv->GetIntField(aImageDataObj, fid));
    // height
    fid = aEnv->GetFieldID(imageDataClazz, "height", "I");
    imageData->setSize(width, static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // depth
    fid = aEnv->GetFieldID(imageDataClazz, "depth", "I");
    imageData->setDepth(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // scanlinePad
    fid = aEnv->GetFieldID(imageDataClazz, "scanlinePad", "I");
    imageData->setScanlinePad(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // bytesPerLine
    fid = aEnv->GetFieldID(imageDataClazz, "bytesPerLine", "I");
    imageData->setBytesPerLine(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // transparentPixel
    fid = aEnv->GetFieldID(imageDataClazz, "transparentPixel", "I");
    imageData->setTransparentPixel(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // maskPad
    fid = aEnv->GetFieldID(imageDataClazz, "maskPad", "I");
    imageData->setMaskPad(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // alpha
    fid = aEnv->GetFieldID(imageDataClazz, "alpha", "I");
    imageData->setAlpha(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // type
    fid = aEnv->GetFieldID(imageDataClazz, "type", "I");
    imageData->setImageType(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // x
    fid = aEnv->GetFieldID(imageDataClazz, "x", "I");
    int x = static_cast<int>(aEnv->GetIntField(aImageDataObj, fid));
    // y
    fid = aEnv->GetFieldID(imageDataClazz, "y", "I");
    imageData->setTopLeftCorner(x, static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // disposalMethod
    fid = aEnv->GetFieldID(imageDataClazz, "disposalMethod", "I");
    imageData->setDisposalMethod(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // delayTime
    fid = aEnv->GetFieldID(imageDataClazz, "delayTime", "I");
    imageData->setDelayTime(static_cast<int>(aEnv->GetIntField(aImageDataObj, fid)));
    // pixel data
    fid = aEnv->GetFieldID(imageDataClazz, "data", "[B");
    jbyteArray pixelData = static_cast<jbyteArray>(aEnv->GetObjectField(aImageDataObj, fid));
    if (pixelData)
    {
        imageData->setData(ImageDataWrapper::EPixelData, 
                JavaByteArrayToQByteArray(aEnv, pixelData));
    }
    else
    {
        imageData->setData(ImageDataWrapper::EPixelData, QByteArray());
    }
    // alpha data
    fid = aEnv->GetFieldID(imageDataClazz, "alphaData", "[B");
    jbyteArray alphaData = static_cast<jbyteArray>(aEnv->GetObjectField(aImageDataObj, fid));
    if (alphaData)
    {
        imageData->setData(ImageDataWrapper::EAlphaData, 
                JavaByteArrayToQByteArray(aEnv, alphaData));
    }   
    else
    {
        imageData->setData(ImageDataWrapper::EAlphaData, QByteArray());
    }
    // mask data
    fid = aEnv->GetFieldID(imageDataClazz, "maskData", "[B");
    jbyteArray maskData = static_cast<jbyteArray>(aEnv->GetObjectField(aImageDataObj, fid));
    if (maskData)
    {
        imageData->setData(ImageDataWrapper::EMaskData, 
                JavaByteArrayToQByteArray(aEnv, maskData));
    }
    else
    {
        imageData->setData(ImageDataWrapper::EMaskData, QByteArray());
    }
    // palette data
    fid = aEnv->GetFieldID(imageDataClazz, "palette", "Lorg/eclipse/swt/graphics/PaletteData;");
    jobject paletteData = aEnv->GetObjectField(aImageDataObj, fid);
    imageData->setPaletteData(CreatePaletteData(aEnv, paletteData));
    result = GraphicsFactory::createImage(imageData);

    delete imageData;
    if (paletteData)
    {
        aEnv->DeleteLocalRef(paletteData);
    }
    if (pixelData)
    {
        aEnv->DeleteLocalRef(pixelData);
    }
    if (alphaData)
    {
        aEnv->DeleteLocalRef(alphaData);
    }
    if (maskData)
    {
        aEnv->DeleteLocalRef(maskData);
    }
    aEnv->DeleteLocalRef(imageDataClazz);
    return result;
}

bool JniUtils::safeToDelete(QObject* aPtrToDelete)
    {
    SWT_LOG_FUNC_CALL();

    // In the case of an QApplication signal like focusChanged it's not 
    // possible to reliably know which object caused the signal and should be
    // protected from deletion. Therefore, all deletion is deferred while
    // inside this kind of an event handler. 
    if(mObjectsBeingHandled.contains(static_cast<QObject*>(qApp)))
        {
        return false;
        }

    // If deletion candidate object or any of its children is currently
    // in its event handler then it's not safe to delete the object.

    // Check against each object in the list of objects being handled currently
    const int size = mObjectsBeingHandled.size();
    for(int i = 0; i < size; ++i)
        {
        const QObjectPtr& handledObject = mObjectsBeingHandled.at(i);
        if(handledObject)
            {
            QObject* handledPtr = handledObject.data();

            // Recursively compare to all the offspring
            if(isSameOrChild(handledPtr, aPtrToDelete))
                {
                return false;
                }
            }
        }
    return true;
    }

PaletteDataWrapper* JniUtils::CreatePaletteData(JNIEnv* aEnv, jobject& aJavaPaletteData)
{
    SWT_LOG_FUNC_CALL();
    
    jclass paletteDataClazz = aEnv->GetObjectClass(aJavaPaletteData);
    jfieldID fid = aEnv->GetFieldID(paletteDataClazz, "isDirect", "Z");
    PaletteDataWrapper* result = 0;
    if(static_cast<bool>(aEnv->GetBooleanField(aJavaPaletteData, fid)))
    {
        PaletteDataWrapper::TDirectData directData;
        fid = aEnv->GetFieldID(paletteDataClazz, "redMask", "I");
        directData.mRedMask = static_cast<int>(aEnv->GetIntField(aJavaPaletteData, fid));
        fid = aEnv->GetFieldID(paletteDataClazz, "greenMask", "I");
        directData.mGreenMask = static_cast<int>(aEnv->GetIntField(aJavaPaletteData, fid));
        fid = aEnv->GetFieldID(paletteDataClazz, "blueMask", "I");
        directData.mBlueMask = static_cast<int>(aEnv->GetIntField(aJavaPaletteData, fid));
        fid = aEnv->GetFieldID(paletteDataClazz, "redShift", "I");
        directData.mRedShift = static_cast<int>(aEnv->GetIntField(aJavaPaletteData, fid));
        fid = aEnv->GetFieldID(paletteDataClazz, "greenShift", "I");
        directData.mGreenShift = static_cast<int>(aEnv->GetIntField(aJavaPaletteData, fid));
        fid = aEnv->GetFieldID(paletteDataClazz, "blueShift", "I");
        directData.mBlueShift = static_cast<int>(aEnv->GetIntField(aJavaPaletteData, fid));
        result = GraphicsFactory::createPaletteData(directData);
    }
    else
    {
        fid = aEnv->GetFieldID(paletteDataClazz, "colors", "[Lorg/eclipse/swt/graphics/RGB;");
        jobjectArray colors = static_cast<jobjectArray>(aEnv->GetObjectField(aJavaPaletteData, fid));
        if (colors)
        {
            // Allocate the palette object
            int sz = aEnv->GetArrayLength(colors);
            // Read the colour values
            jclass rgbClazz = aEnv->FindClass("org/eclipse/swt/graphics/RGB");
            if (rgbClazz)
            {
                QVector<QRgb> indexedData;
                for (int index = 0; index < sz; index++)
                {
                    jobject colorsObject = aEnv->GetObjectArrayElement(colors, index);
                    fid = aEnv->GetFieldID(rgbClazz, "red", "I");
                    int red = static_cast<int>(aEnv->GetIntField(colorsObject, fid));
                    fid = aEnv->GetFieldID(rgbClazz, "green", "I");
                    int green = static_cast<int>(aEnv->GetIntField(colorsObject, fid));
                    fid = aEnv->GetFieldID(rgbClazz, "blue", "I");
                    int blue = static_cast<int>(aEnv->GetIntField(colorsObject, fid));
                    indexedData.append(qRgb(red, green, blue));
                    aEnv->DeleteLocalRef(colorsObject);
                }
                aEnv->DeleteLocalRef(rgbClazz);
                result = GraphicsFactory::createPaletteData(indexedData);
            }
        }
    }
    aEnv->DeleteLocalRef(paletteDataClazz);
    return result;
}

const char* JniUtils::resolveException(int aErrorCode) 
{
    SWT_LOG_FUNC_CALL();
    
    switch (aErrorCode)
    {
    case EGfxErrorNone:
        return 0;
    case EGfxErrorNoMemory:
        return "java/lang/OutOfMemoryError";
    case EGfxErrorIO:
        return "java/io/IOException";
    case EGfxErrorIllegalArgument:
        return "java/lang/IllegalArgumentException";
    case EGfxErrorIllegalState:
        return "java/lang/IllegalStateException";
    case EGfxErrorArrayIndexOutOfBounds:
        return "java/lang/ArrayIndexOutOfBoundsException";
    default:
        return "java/lang/IllegalArgumentException";
    }
}

bool JniUtils::isSameOrChild(const QObject* aChild, const QObject* aParent) const
    {
    SWT_LOG_FUNC_CALL();
    if(aParent == aChild)
        {
        return true;
        }
    const QObjectList& children = aParent->children();
    for(int i = 0; i < children.size(); ++i)
        {
        if(isSameOrChild(aChild, children.at(i)))
            {
            return true;
            }
        }
    return false;
    }

}}
