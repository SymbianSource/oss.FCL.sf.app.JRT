/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: formatterqtnative
*
*/


#ifdef RD_JAVA_UI_QT
#include <qcoreapplication.h>
#include <qlibraryinfo.h>
#include <qlocale.h>
#include <qtranslator.h>
#ifdef __SYMBIAN32__
#include <hbparameterlengthlimiter.h>
#include <hbstringutil.h>
#else // __SYMBIAN32__
// If HbParameterLengthLimiter is not available, define it as empty macro.
#define HbParameterLengthLimiter(text) (text)
#endif // __SYMBIAN32__
#endif // RD_JAVA_UI_QT

#include "com_nokia_mj_impl_utils_FormatterQt.h"
#include "com_nokia_mj_impl_utils_ResourceLoader.h"
#include "com_nokia_mj_impl_utils_ResourceLoaderQt.h"
#include "javajniutils.h"
#include "logger.h"

#ifdef __SYMBIAN32__
#include "s60commonutils.h"
#else // __SYMBIAN32__
#define KErrNone (0)
#define KErrNotFound (-1)
#endif // __SYMBIAN32__

using namespace java::util;

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_ResourceLoader__1getLocaleIdQt
(JNIEnv *env, jclass)
{
#ifdef RD_JAVA_UI_QT
    QString localeName = QLocale::system().name();
    jstring loc = env->NewString(localeName.utf16(), localeName.size());
    return loc;
#else // RD_JAVA_UI_QT
    (void)env;     // just to suppress a warning
    return NULL;
#endif // RD_JAVA_UI_QT
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_utils_ResourceLoaderQt__1createTranslator
(JNIEnv *aEnv, jclass, jstring aResourceName)
{
#ifdef RD_JAVA_UI_QT
    QString resourceName = QString::fromStdWString(
        JniUtils::jstringToWstring(aEnv, aResourceName)) +
        "_" + QLocale::system().name();

    if (qApp == 0)
    {
        ELOG1(EUtils,
              "ResourceLoaderQt__1createTranslator: QApplication "
              "does not exist. Loading failed for %S",
             resourceName.toStdWString().c_str());
        return KErrNotFound;
    }

    QTranslator* translator = new QTranslator();
    bool translatorLoaded = translator->load(
        resourceName, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    if (!translatorLoaded)
    {
        translatorLoaded = translator->load(
            resourceName,
            "C:" + QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    }
    if (!translatorLoaded)
    {
        translatorLoaded = translator->load(
            resourceName,
            "Z:" + QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    }
    if (translatorLoaded)
    {
        qApp->installTranslator(translator);
    }
    else
    {
        ILOG1(EUtils, "__1createTranslator: loading resource %S failed",
              resourceName.toStdWString().c_str());
        delete translator;
        return KErrNotFound;
    }
    // Return handle to translator.
    return reinterpret_cast<unsigned int>(translator)>>2;
#else // RD_JAVA_UI_QT
    (void)aEnv; // just to suppress a warning
    (void)aResourceName; // just to suppress a warning
    return KErrNone;
#endif // RD_JAVA_UI_QT
}

JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_utils_ResourceLoaderQt__1deleteTranslator
(JNIEnv *, jclass, jint aHandle)
{
#ifdef RD_JAVA_UI_QT
    QTranslator *pTranslator = reinterpret_cast<QTranslator*>(aHandle<<2);
    QCoreApplication::removeTranslator(pTranslator);
    delete pTranslator;
    return KErrNone;
#else // RD_JAVA_UI_QT
    (void)aHandle; // just to suppress a warning
    return KErrNone;
#endif // RD_JAVA_UI_QT
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_ResourceLoaderQt__1translate
(JNIEnv *aEnv, jclass, jint aHandle, jstring aId, jint aN)
{
#ifdef RD_JAVA_UI_QT
    (void)aHandle; // just to suppress a warning
    QString id = QString::fromStdWString(JniUtils::jstringToWstring(aEnv, aId));
    QString result = QString(qtTrId(id.toUtf8().constData(), aN));
    return aEnv->NewString(result.utf16(), result.size());
#else // RD_JAVA_UI_QT
    (void)aEnv; // just to suppress a warning
    (void)aHandle; // just to suppress a warning
    (void)aId; // just to suppress a warning
    (void)aN; // just to suppress a warning
    return aId;
#endif // RD_JAVA_UI_QT
}

#ifdef RD_JAVA_UI_QT
QString FormatTextArg(JNIEnv *aEnv, QString aText, jobjectArray aArgs, int aI)
{
    // Check the type of aArgs[aI] and call correct aText.arg() method.
    jobject obj = aEnv->GetObjectArrayElement(aArgs, aI);
    jclass clazz = aEnv->FindClass("java/lang/String");
    if (aEnv->IsInstanceOf(obj, clazz))
    {
        return HbParameterLengthLimiter(aText).arg(
            QString::fromStdWString(
                JniUtils::jstringToWstring(aEnv, (jstring)obj)));
    }
    clazz = aEnv->FindClass("java/lang/Integer");
    if (aEnv->IsInstanceOf(obj, clazz))
    {
        jmethodID method = aEnv->GetMethodID(clazz, "intValue", "()I");
        return HbParameterLengthLimiter(aText).arg(
            aEnv->CallIntMethod(obj, method));
    }
    clazz = aEnv->FindClass("java/lang/Character");
    if (aEnv->IsInstanceOf(obj, clazz))
    {
        jmethodID method = aEnv->GetMethodID(clazz, "charValue", "()C");
        return HbParameterLengthLimiter(aText).arg(
            (char)aEnv->CallCharMethod(obj, method));
    }
    clazz = aEnv->FindClass("java/util/Calendar");
    if (aEnv->IsInstanceOf(obj, clazz))
    {
        jmethodID method = aEnv->GetMethodID(clazz, "get", "(I)I");
        int year = aEnv->CallIntMethod(obj, method, 1); // Calendar.YEAR
        int month = aEnv->CallIntMethod(obj, method, 2) + 1; // Calendar.MONTH
        int day = aEnv->CallIntMethod(obj, method, 5); // Calendar.DAY_OF_MONTH
        QDate date(year, month, day);
        QLocale locale;
        return HbParameterLengthLimiter(aText).arg(
            locale.toString(date, QLocale::ShortFormat));
    }
    return aText;
}
#endif // RD_JAVA_UI_QT

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_FormatterQt__1formatParameters
(JNIEnv *aEnv, jclass, jstring aText, jobjectArray aArgs)
{
#ifdef RD_JAVA_UI_QT
    std::wstring wsText = JniUtils::jstringToWstring(aEnv, aText);
    QString text = QString::fromStdWString(wsText);
    int argsLen = aEnv->GetArrayLength(aArgs);
    for (int i = 0; i < argsLen; i++)
    {
        text = FormatTextArg(aEnv, text, aArgs, i);
    }
    return aEnv->NewString(text.utf16(), text.size());
#else // RD_JAVA_UI_QT
    (void)aEnv; // just to suppress a warning
    (void)aText; // just to suppress a warning
    (void)aArgs; // just to suppress a warning
    return aText;
#endif // RD_JAVA_UI_QT
}

JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_utils_FormatterQt__1formatDigits
(JNIEnv * aEnv, jclass, jstring aText)
{
    jstring ret = aText;
#if defined(RD_JAVA_UI_QT) && defined(__SYMBIAN32__)
    QString text = QString::fromStdWString(
        JniUtils::jstringToWstring(aEnv, aText));
    text = HbStringUtil::convertDigits(text);
    ret = aEnv->NewString(text.utf16(), text.size());
#else // RD_JAVA_UI_QT && __SYMBIAN32__
    (void)aEnv; // just to suppress a warning
#endif // RD_JAVA_UI_QT && __SYMBIAN32__
    return ret;
}
