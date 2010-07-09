#include "swt.h"
#include "os.h"
#include <QAbstractAnimation>
#include <QPropertyAnimation>
#include <QVariant>
#include <QPoint>
#include <QAnimationGroup>
#include <QParallelAnimationGroup>
#include <QSequentialAnimationGroup>

using namespace Java::eSWT;

#define OS_NATIVE(func) Java_org_eclipse_swt_animation_internal_qt_OS_##func

#define POINTER_TO_HANDLE(pointer) reinterpret_cast<jint>(static_cast<QObject*>(pointer))
#define HANDLE_TO_POINTER(type, variable, handle) type variable = qobject_cast<type>(static_cast<type>(reinterpret_cast<QObject*>( handle )))

#define SWT_TRY try
#define SWT_CATCH \
catch(std::bad_alloc const&)\
    {\
    Throw( aJniEnv, ESwtErrorNoHandles );\
    }\
catch(std::exception const&)\
    {\
    Throw( aJniEnv, ESwtErrorUnspecified );\
    }
#define SWT_CATCH_1(err) \
catch(std::exception const&)\
    {\
    Throw( aJniEnv, err );\
    }

#ifdef __cplusplus
extern "C" {
#endif

//TODO: use from jniutils
void Throw( JNIEnv* aJniEnv, const int& aError )
    {
    jclass clazz = aJniEnv->FindClass("org/eclipse/swt/SWT");
    if (clazz != NULL)
        {
        jmethodID id = aJniEnv->GetStaticMethodID(clazz, "error", "(I)V");
        if (id != NULL)
            aJniEnv->CallStaticVoidMethod(clazz, id, aError);
        aJniEnv->DeleteLocalRef(clazz);
        }
    }

JNIEXPORT jint JNICALL OS_NATIVE(QPropertyAnimation_1new)
  (JNIEnv *aJniEnv, jclass, jint aHandle)
  {
	QPropertyAnimation* animation = NULL;
    SWT_TRY
        {
        HANDLE_TO_POINTER( QObject*, parent, aHandle );
        animation = new QPropertyAnimation( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( animation );

  }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractAnimation_1start )
  (JNIEnv *aJniEnv, jclass, jint aHandle)
  {
   SWT_TRY
        {
        HANDLE_TO_POINTER( QAbstractAnimation*, animation, aHandle );
        animation->start();
        }
    SWT_CATCH
  }

JNIEXPORT void JNICALL OS_NATIVE( QAbstractAnimation_1stop )
  (JNIEnv *aJniEnv, jclass, jint aHandle)
  {
  SWT_TRY
        {
        HANDLE_TO_POINTER( QAbstractAnimation*, animation, aHandle );
        animation->stop();
        }
   SWT_CATCH
  }

JNIEXPORT void JNICALL OS_NATIVE (QAbstractAnimation_1setLoopCount)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aLoopCount)
  {
	  SWT_TRY
	        {
	        HANDLE_TO_POINTER( QAbstractAnimation*, animation, aHandle );
	        animation->setLoopCount( aLoopCount );
	        }
	  SWT_CATCH
  }

JNIEXPORT jint JNICALL OS_NATIVE( QAbstractAnimation_1duration)
  (JNIEnv *aJniEnv, jclass, jint aHandle )
  {
	 int duration = 0;
	  SWT_TRY
	        {
	        HANDLE_TO_POINTER( QAbstractAnimation*, animation, aHandle );
	        duration= animation->duration();
	        }
	  SWT_CATCH
	  return duration;
  }

JNIEXPORT void JNICALL OS_NATIVE( QPropertyAnimation_1setPropertyName)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jstring string)
  {
   SWT_TRY
        {
        jboolean isCopy;
    	const jchar* javaChars = aJniEnv->GetStringChars( string, &isCopy );

    	jsize length = aJniEnv->GetStringLength( string );
        QString s = QString::fromUtf16( javaChars, length );
        QByteArray a =s.toAscii();

        HANDLE_TO_POINTER( QPropertyAnimation*, animation, aHandle );

        animation->setPropertyName( a );
        aJniEnv->ReleaseStringChars( string, javaChars );
        }
   SWT_CATCH
  }

JNIEXPORT void JNICALL OS_NATIVE(QPropertyAnimation_1setTargetObject)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aWidgetHandle)
	{
  SWT_TRY
        {
        HANDLE_TO_POINTER( QPropertyAnimation*, animation, aHandle );
        HANDLE_TO_POINTER( QObject*, object, aWidgetHandle );
        animation->setTargetObject( object );
        }
   SWT_CATCH
  	}

JNIEXPORT void JNICALL OS_NATIVE( QVariantAnimation_1setDuration )
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aDuration)
  {
  SWT_TRY
        {
        HANDLE_TO_POINTER( QVariantAnimation*, animation, aHandle );
        animation->setDuration( aDuration );
        }
   SWT_CATCH
  }


JNIEXPORT void JNICALL OS_NATIVE( QVariantAnimation_1setEasingCurve)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aEasing )
  {
  SWT_TRY
        {
        HANDLE_TO_POINTER( QVariantAnimation*, animation, aHandle );
        QEasingCurve* easing = static_cast<QEasingCurve*>(reinterpret_cast<QEasingCurve*>( aEasing ));
        animation->setEasingCurve( *easing );
        }
   SWT_CATCH
  }

JNIEXPORT void JNICALL OS_NATIVE(QVariantAnimation_1swt_1setEndValuePoint)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aX, jint aY)
	{
	  SWT_TRY
	        {
	        HANDLE_TO_POINTER( QVariantAnimation*, animation, aHandle );
	        QPoint point(static_cast<int>(aX), static_cast<int>(aY));
	        animation->setEndValue(QVariant(point));
	        }
	   SWT_CATCH
	}

JNIEXPORT void JNICALL OS_NATIVE(QVariantAnimation_1swt_1setStartValuePoint)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aX, jint aY)
	{
	  SWT_TRY
	        {
	        HANDLE_TO_POINTER( QVariantAnimation*, animation, aHandle );
	        QPoint point(static_cast<int>(aX), static_cast<int>(aY));
	        animation->setStartValue(QVariant(point));
	        }
	   SWT_CATCH
	}

JNIEXPORT void JNICALL OS_NATIVE (QVariantAnimation_1swt_1setEndValueDouble)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jdouble aDouble)
  {
	  SWT_TRY
	        {
	        HANDLE_TO_POINTER( QVariantAnimation*, animation, aHandle );
	        animation->setEndValue(QVariant(static_cast<double>(aDouble)));
	        }
	   SWT_CATCH
  }


JNIEXPORT void JNICALL OS_NATIVE (QVariantAnimation_1swt_1setStartValueDouble )
  (JNIEnv *aJniEnv, jclass, jint aHandle, jdouble aDouble)
	{
	  SWT_TRY
	        {
	        HANDLE_TO_POINTER( QVariantAnimation*, animation, aHandle );
	        animation->setStartValue(QVariant(static_cast<double>(aDouble)));
	        }
	   SWT_CATCH
	}

JNIEXPORT void JNICALL OS_NATIVE(QAnimationGroup_1addAnimation)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aAnimation)
  {
	  SWT_TRY
	        {
	        HANDLE_TO_POINTER( QAnimationGroup*, group, aHandle );
	        HANDLE_TO_POINTER( QAbstractAnimation*, animation, aAnimation );
	        group->addAnimation(animation);
	        }
	   SWT_CATCH
  }

JNIEXPORT jint JNICALL OS_NATIVE( QSequentialAnimationGroup_1new)
  (JNIEnv *aJniEnv, jclass, jint aHandle)
  {
	QSequentialAnimationGroup* animation = NULL;
    SWT_TRY
        {
        HANDLE_TO_POINTER( QObject*, parent, aHandle );
        animation = new QSequentialAnimationGroup ( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( animation );
  }


JNIEXPORT void OS_NATIVE( QSequentialAnimationGroup_1addPause)
  (JNIEnv *aJniEnv, jclass, jint aHandle, jint aMsec)
  {
  SWT_TRY
	  {
		HANDLE_TO_POINTER( QSequentialAnimationGroup*, animation, aHandle );
		animation->addPause(aMsec);
	  }
  SWT_CATCH
}

JNIEXPORT jint OS_NATIVE( QParallelAnimationGroup_1new)
  (JNIEnv *aJniEnv, jclass, jint aHandle)
  {
	QParallelAnimationGroup* animation = NULL;
    SWT_TRY
        {
        HANDLE_TO_POINTER( QObject*, parent, aHandle );
        animation = new QParallelAnimationGroup( parent );
        }
    SWT_CATCH
    return POINTER_TO_HANDLE( animation );
  }

JNIEXPORT jint OS_NATIVE( QEasingCurve_1new)
  (JNIEnv *aJniEnv, jclass, jint aType)
  {
	QEasingCurve* easing = NULL;
    SWT_TRY
        {
        easing = new QEasingCurve( static_cast<QEasingCurve::Type>(aType) );
        }
    SWT_CATCH
    return reinterpret_cast<jint>(easing);

  }

#ifdef __cplusplus
}
#endif

