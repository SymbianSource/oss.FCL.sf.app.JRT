/*******************************************************************************
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef _JNI_UTILS_H_
#define _JNI_UTILS_H_

#include <QObject>
#include <QRect>
#include <QPoint>
#include <QStringList>
#include <QStack>
#include <QPointer>
#include <jni.h>

template<class T>
class QStack;

namespace Java { namespace GFX {
class ImageDataWrapper;
class PaletteDataWrapper;
class Image;
class GfxException;
}}

namespace Java { namespace eSWT {

class ExecExceptionHandler;

typedef QPointer<QObject> QObjectPtr;
typedef QStack<QObjectPtr> PtrStack;
typedef QStack<ExecExceptionHandler*> ExecStack;

class JniUtils : public QObject {
    Q_OBJECT

public:
    /**
     * The constructor.
     * @param aEnv The JNI environment pointer of the Qt GUI thread.
     * @param aDisplay Local ref to the Java Display instance, JniUtils will create a global ref
     */
    JniUtils(JNIEnv* aEnv, jobject aDisplay);

    /**
     * The destructor.
     */
    virtual ~JniUtils();

    /**
     * Calls eventProcess Java method of the active Display instance. 
     * Can only be called in the UI thread. 
     * @param aQObject Target QObject of the QEvent
     * @param aQEventType Type of the QEvent
     * @param aN Event type specific integer arguments
     * @param aString Event type specific string argument
     * @return True if the event should be canceled.
     */
    bool eventProcess(const QObject* aQObject, const int& aQEventType,
                      const int& a1 = 0, const int& a2 = 0, const int& a3 = 0,
                      const int& a4 = 0, const int& a5 = 0,
                      const jstring aString = NULL);

    /**
     * Calls eventProcess Java method based on the given jobject and jmethodID.
     * Can only be called in the UI thread. 
     * @param aObject The Java object instance for the method call.
     * @param aMethodID The Java methodID for the method call.
     * @param ... Parameters to pass to the Java method.
     * @return True if the event should be canceled.
     */
    bool eventProcess(jobject aObject, const jmethodID aMethodID,
            const int& aQObject, const int& aQEventType, const int& a1,
            const int& a2, const int& a3, const int& a4, const int& a5,
            const jstring aString);

    /**
     * Query if the jobject is the Display of the process.
     * Can only be called in the UI thread. 
     * @param aObject jobject to compare to the Display.
     * @return true if given jobject is the Display
     */
    bool isDisplay(jobject aObject);

    /**
     * This must be called before entering an event loop. E.g. dialogs and
     * menus need to call this before calling exec(). This will handle closing
     * the dialog or menu properly in case a Java exception is thrown while in
     * exec(). Call to enterExec must always be paired with leaveExec.
     * @param aObject the object that exec will be called on.
     */
    void enterExec(QObject* aObject);

    /**
     * This must be called after exiting an event loop. It must be preceded by
     * enterExec.
     */
    void leaveExec();

    /**
     * Creates a Java Rectangle from the given QRect.
     * @param aEnv JNI environment for the thread
     * @param aRect The QRect to use.
     * @return A Java Rectangle object reference. Never NULL.
     * @exception std::bad_alloc
     */
    jobject NewJavaRectangle(JNIEnv* aEnv, const QRect& aRect);

    /**
     * Creates a Java Point from the given QSize.
     * @param aEnv JNI environment for the thread
     * @param aSize The QSize object ot use.
     * @return A Java Point object reference.
     * @exception std::bad_alloc
     */
    jobject NewJavaPoint(JNIEnv* aEnv, const QSize& aSize);

    /**
     * Creates a Java Point from the given QPoint.
     * @param aEnv JNI environment for the thread
     * @param aPoint The QPoint object ot use.
     * @return A Java Point object reference.
     * @exception std::bad_alloc
     */
    jobject NewJavaPoint(JNIEnv* aEnv, const QPoint& aPoint);

    /**
     * Creates a QString from the given Java string.
     * @param aEnv JNI environment for the thread
     * @param The Java String to use.
     * @return A QString object.
     * @exception std::bad_alloc
     */
    QString JavaStringToQString(JNIEnv* aEnv, jstring aJavaString);

    /**
     * Creates a Java string from the given QString.
     * @param aEnv JNI environment for the thread
     * @param aQString The QString object to use.
     * @return A Java string.
     * @exception std::bad_alloc
     */
    jstring QStringToJavaString(JNIEnv* aEnv, const QString& aQString);

    /**
     * Creates a new Java integer array from the given native int array.
     * @param aEnv JNI environment for the thread
     * @param aArray Native primitive integer array.
     * @param aItemCount Number of items to read from the array starting from the first item.
     * @return A Java integer array.
     * @exception std::bad_alloc
     */
    jintArray NewJavaIntArray(JNIEnv* aEnv, int* aArray, const int& aItemCount);

    /**
     * Creates a new Java string array from the given QStringList.
     * @param aEnv JNI environment for the thread
     * @param aStrList QStringList.
     * @return A Java object array.
     * @exception std::bad_alloc or std::bad_typeid
     */
    jobjectArray NewJavaStringArray(JNIEnv* aEnv, const QStringList& aStrList);

    /**
     * Creates a QStringList from the given Java string array .
     * @param aEnv JNI environment for the thread
     * @param aObjectArray jobjectArray.
     * @return A QStringList
     */
    QStringList JavaStringArrayToQStringList(JNIEnv* aEnv, jobjectArray aObjectArray);

    /**
     * Creates a new Java byte array from the given QByteArray.
     * @param aEnv JNI environment for the thread
     * @param aArray QByteArray.
     * @return A Java byte array.
     * @exception std::bad_alloc
     */
    jbyteArray NewJavaByteArray(JNIEnv* aEnv, const QByteArray& aArray);

    /**
     * Creates a QByteArray from the given Java byte array.
     * @param aEnv JNI environment for the thread
     * @param Java byte array.
     * @return A QByteArray object.
     * @exception std::bad_alloc
     */
    QByteArray JavaByteArrayToQByteArray(JNIEnv* aEnv, jbyteArray aByteArray);

    /**
     * Throws an SWT error exception from the current JNI call.
     * Note that after this call the JVM is handling the exception and further Java execution is not
     * possible until the exception has been caught by the JVM.
     * @param aEnv JNI environment for the thread
     * @param aError The SWT error code to throw.
     */
    void Throw(JNIEnv* aEnv, const int& aError);
    
    /**
     * Throws a custom Java exception from the current JNI call. 
     * Note that after this call the JVM is handling the exception and further Java execution is not 
     * possible until the exception has been caught by the JVM. 
     * @param aEnv JNI environment for the thread
     * @param aException The GfxException containing error code and error description.
     */
    void Throw(JNIEnv* aEnv, Java::GFX::GfxException& aException);

    /**
     * Returns a Java object instance method ID based of on the given method name and signature.
     * @param aEnv JNI environment for the thread
     * @param aObject The Java object instance.
     * @param aMethodName The name of the Java method.
     * @param aMethodSignature The JNI signature of the Java method.
     * @return JNI method id of the method or NULL if unsuccessful.
     */
    jmethodID FindJavaMethodID(JNIEnv* aEnv, jobject aObject, const char* aMethodName,
            const char* aMethodSignature);

    /**
     * Returns the number of currently active (maybe nested) calls to event 
     * processing functions. I.e. calls to functions that might lead to 
     * callbacks to Java event listeners. Used to defer the desruction of 
     * QApplication to the point where all nested events loops have exited. 
     */
    int javaCallbackCount();

    /**
     * Copies a region of a Java integer array to a pre-allocated primitive int
     * buffer. 
     * @param aEnv JNI environment for the thread
     * @param aArray The source Java array
     * @param aStart The copy operation start index in the source array
     * @param aLength The number of elements to be copied
     * @param aBuffer The pre-allocated destination buffer
     * @exception std::bad_alloc
     */
    void GetJavaIntArrayRegionToIntArray(JNIEnv* aEnv, jintArray aArray, jsize aStart, jsize aLength, int* aBuffer);
    
    /**
     * Copies contents of a primitive integer buffer to a region of a Java 
     * integer array. 
     * @param aEnv JNI environment for the thread
     * @param aArray The target Java array
     * @param aStart The copy operation start index in the target Java array
     * @param aLength The number of elements to be copied
     * @param aBuffer The native primitive type source buffer
     * @exception std::bad_alloc
     */
    void SetJavaIntArrayRegionFromIntArray(JNIEnv* aEnv, jintArray aArray, jsize aStart, jsize aLength, int* aBuffer);
    
    /**
     * Copies a region of a Java byte array to a pre-allocated primitive char
     * buffer. 
     * @param aEnv JNI environment for the thread
     * @param aArray The source Java array
     * @param aStart The copy operation start index in the source array
     * @param aLength The number of elements to be copied
     * @param aBuffer The pre-allocated destination buffer
     * @exception std::bad_alloc
     */
    void GetJavaByteArrayRegionToCharArray(JNIEnv* aEnv, jbyteArray aArray, jsize aStart, jsize aLength, char* aBuffer);
    
    /**
     * Copies contents of a primitive integer buffer to a region of a Java 
     * integer array. 
     * @param aEnv JNI environment for the thread
     * @param aArray The target Java array
     * @param aStart The copy operation start index in the target Java array
     * @param aLength The number of elements to be copied
     * @param aBuffer The native primitive type source buffer
     * @exception std::bad_alloc
     */
    void SetJavaByteArrayRegionFromCharArray(JNIEnv* aEnv, jbyteArray aArray, jsize aStart, jsize aLength, char* aBuffer);
    
    /**
     * Copies a region of a Java short array to a pre-allocated primitive short
     * buffer. 
     * @param aEnv JNI environment for the thread
     * @param aArray The source Java array
     * @param aStart The copy operation start index in the source array
     * @param aLength The number of elements to be copied
     * @param aBuffer The pre-allocated destination buffer
     * @exception std::bad_alloc
     */
    void GetJavaShortArrayRegionToShortArray(JNIEnv* aEnv, jshortArray aArray, jsize aStart, jsize aLength, short* aBuffer);
    
    /**
     * Copies contents of a primitive short buffer to a region of a Java 
     * short array. 
     * @param aEnv JNI environment for the thread
     * @param aArray The target Java array
     * @param aStart The copy operation start index in the target Java array
     * @param aLength The number of elements to be copied
     * @param aBuffer The native primitive type source buffer
     * @exception std::bad_alloc
     */
    void SetJavaShortArrayRegionFromShortArray(JNIEnv* aEnv, jshortArray aArray, jsize aStart, jsize aLength, short* aBuffer);
    
    /**
     * Creates java side image data object based on the given native side 
     * image data object.
     * @param aEnv JNI environment for the thread
     * @param aImageData The native image data object
     * @return Java side image data object
     */
    jobject CreateJavaImageData(JNIEnv* aEnv, Java::GFX::ImageDataWrapper& aImageData);

    /**
     * Creates java side palette data object based on the given native side 
     * palette data object.
     * @param aEnv JNI environment for the thread
     * @param aPaletteData The native palette data object
     * @return Java side palette data object
     */
    jobject CreateJavaPaletteData(JNIEnv* aEnv, Java::GFX::PaletteDataWrapper& aPaletteData);

    /**
     * Creates a native image object based on the given image data object.
     * @param aEnv JNI environment for the thread
     * @param aJavaImageData Java side image data object
     * @return Native image object
     */
    Java::GFX::Image* CreateImage(JNIEnv* aEnv, jobject& aJavaImageData);

    /**
     * Checks if in terms of event processing the object can be deleted. Used 
     * to prevent objects from getting deleted inside their event listeners. 
     * @param aPtrToDelete Pointer to the deletion candidate object. 
     * @return True if the object can be deleted. 
     */
    bool safeToDelete(QObject* aPtrToDelete);

private:
    JniUtils() {};

    /**
     * Creates a native palette data object based on the given palette data object.
     * @param aEnv JNI environment
     * @param aJavaPaletteData Java side image data object
     * @return Native image object
     */
    Java::GFX::PaletteDataWrapper* CreatePaletteData(JNIEnv* aEnv, jobject& aJavaPaletteData);
    
    /**
     * Converts GfxException error code to java exception class string.
     * @param aErrorCode The Gfx error code to throw. 
     */
    const char* resolveException(int aErrorCode);
    
    /**
     * Checks recursively if the object is a child of the other object. 
     * @param aChild The child candidate.
     * @param aParent The candidate parent object whose children are searched. 
     * @returns True if aParent equals aChild or aChild is one of the children. 
     */
    bool isSameOrChild(const QObject* aChild, const QObject* aParent) const;

    /**
     * Private helper class that can be created on stack to safely update the 
     * number of recursive callbacks and the list of objects currently in
     * their event or signal handler. 
     */
    class JavaCallbackCounter
    {
    public:
        JavaCallbackCounter(PtrStack& aStack, QObject* aObject)
            : mStack(aStack)
        { 
            QObjectPtr ptr = aObject;
            mStack.push(ptr);
        }
        virtual ~JavaCallbackCounter()
        { 
            mStack.pop();
        }
    private:
        PtrStack& mStack;
    };

    JNIEnv*     mUIThreadJniEnv;
    jobject     mDisplay;
    jclass*     mJclasses;
    jmethodID*  mJmethodIds;
    ExecStack   mExecStack;
    PtrStack    mObjectsBeingHandled;
};

}}

#endif // _JNI_UTILS_H_
