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
* Description:
*
*/


package com.nokia.mj.impl.installer.applicationregistrator;

import java.util.Hashtable;

/**
 * Mapping between ISO language/country codes and Symbian TLanguage.
 */
public final class S60LangMapper
{
    private static Hashtable iIsoToLangMap = new Hashtable();

    static
    {
        iIsoToLangMap.put("en", new Integer(1)); // ELangEnglish, UK English.
        iIsoToLangMap.put("fr", new Integer(2)); // ELangFrench, French.
        iIsoToLangMap.put("de", new Integer(3)); // ELangGerman, German.
        iIsoToLangMap.put("es", new Integer(4)); // ELangSpanish, Spanish.
        iIsoToLangMap.put("it", new Integer(5)); // ELangItalian, Italian.
        iIsoToLangMap.put("sv", new Integer(6)); // ELangSwedish, Swedish.
        iIsoToLangMap.put("da", new Integer(7)); // ELangDanish, Danish.
        iIsoToLangMap.put("no", new Integer(8)); // ELangNorwegian, Norwegian.
        iIsoToLangMap.put("fi", new Integer(9)); // ELangFinnish, Finnish.
        iIsoToLangMap.put("en-us", new Integer(10)); // ELangAmerican, American.
        iIsoToLangMap.put("fr-ch", new Integer(11)); // ELangSwissFrench, Swiss French.
        iIsoToLangMap.put("de-ch", new Integer(12)); // ELangSwissGerman, Swiss German.
        iIsoToLangMap.put("pt", new Integer(13)); // ELangPortuguese, Portuguese.
        iIsoToLangMap.put("tr", new Integer(14)); // ELangTurkish, Turkish.
        iIsoToLangMap.put("is", new Integer(15)); // ELangIcelandic, Icelandic.
        iIsoToLangMap.put("ru", new Integer(16)); // ELangRussian, Russian.
        iIsoToLangMap.put("hu", new Integer(17)); // ELangHungarian, Hungarian.
        iIsoToLangMap.put("nl", new Integer(18)); // ELangDutch, Dutch.
        iIsoToLangMap.put("nl-be", new Integer(19)); // ELangBelgianFlemish, Belgian Flemish.
        iIsoToLangMap.put("en-au", new Integer(20)); // ELangAustralian, Australian English.
        iIsoToLangMap.put("fr-be", new Integer(21)); // ELangBelgianFrench, Belgian French.
        iIsoToLangMap.put("de-at", new Integer(22)); // ELangAustrian, Austrian German.
        iIsoToLangMap.put("en-nz", new Integer(23)); // ELangNewZealand, New Zealand English.
        iIsoToLangMap.put("cs", new Integer(25)); // ELangCzech, Czech.
        iIsoToLangMap.put("sk", new Integer(26)); // ELangSlovak, Slovak.
        iIsoToLangMap.put("pl", new Integer(27)); // ELangPolish, Polish.
        iIsoToLangMap.put("sl", new Integer(28)); // ELangSlovenian, Slovenian.
        iIsoToLangMap.put("zh-tw", new Integer(29)); // ELangTaiwanChinese, Taiwanese Chinese.
        iIsoToLangMap.put("zh-hk", new Integer(30)); // ELangHongKongChinese, Hong Kong Chinese.
        iIsoToLangMap.put("zh", new Integer(31)); // ELangPrcChinese, Peoples Republic of China's Chinese.
        iIsoToLangMap.put("ja", new Integer(32)); // ELangJapanese, Japanese.
        iIsoToLangMap.put("th", new Integer(33)); // ELangThai, Thai.
        iIsoToLangMap.put("af", new Integer(34)); // ELangAfrikaans, Afrikaans.
        iIsoToLangMap.put("sq", new Integer(35)); // ELangAlbanian, Albanian.
        iIsoToLangMap.put("am", new Integer(36)); // ELangAmharic, Amharic.
        iIsoToLangMap.put("ar", new Integer(37)); // ELangArabic, Arabic.
        iIsoToLangMap.put("hy", new Integer(38)); // ELangArmenian, Armenian.
        iIsoToLangMap.put("tl", new Integer(39)); // ELangTagalog, Tagalog.
        iIsoToLangMap.put("be", new Integer(40)); // ELangBelarussian, Belarussian.
        iIsoToLangMap.put("bn", new Integer(41)); // ELangBengali, Bengali.
        iIsoToLangMap.put("bg", new Integer(42)); // ELangBulgarian, Bulgarian.
        iIsoToLangMap.put("my", new Integer(43)); // ELangBurmese, Burmese.
        iIsoToLangMap.put("ca", new Integer(44)); // ELangCatalan, Catalan.
        iIsoToLangMap.put("hr", new Integer(45)); // ELangCroatian, Croatian.
        iIsoToLangMap.put("en-ca", new Integer(46)); // ELangCanadianEnglish, Canadian English.
        iIsoToLangMap.put("en-za", new Integer(48)); // ELangSouthAfricanEnglish, South African English.
        iIsoToLangMap.put("et", new Integer(49)); // ELangEstonian, Estonian.
        iIsoToLangMap.put("fa", new Integer(50)); // ELangFarsi, Farsi.
        iIsoToLangMap.put("fr-ca", new Integer(51)); // ELangCanadianFrench, Canadian French.
        iIsoToLangMap.put("gd", new Integer(52)); // ELangScotsGaelic, Gaelic.
        iIsoToLangMap.put("ka", new Integer(53)); // ELangGeorgian, Georgian.
        iIsoToLangMap.put("el", new Integer(54)); // ELangGreek, Greek.
        iIsoToLangMap.put("el-cy", new Integer(55)); // ELangCyprusGreek, Cyprus Greek.
        iIsoToLangMap.put("gu", new Integer(56)); // ELangGujarati, Gujarati.
        iIsoToLangMap.put("iw", new Integer(57)); // ELangHebrew, Hebrew.
        iIsoToLangMap.put("hi", new Integer(58)); // ELangHindi, Hindi.
        iIsoToLangMap.put("in", new Integer(59)); // ELangIndonesian, Indonesian.
        iIsoToLangMap.put("ga", new Integer(60)); // ELangIrish, Irish.
        iIsoToLangMap.put("it-ch", new Integer(61)); // ELangSwissItalian, Swiss Italian.
        iIsoToLangMap.put("kk", new Integer(63)); // ELangKazakh, Kazakh.
        iIsoToLangMap.put("km", new Integer(64)); // ELangKhmer, Khmer.
        iIsoToLangMap.put("ko", new Integer(65)); // ELangKorean, Korean.
        iIsoToLangMap.put("lo", new Integer(66)); // ELangLao, Lao.
        iIsoToLangMap.put("lv", new Integer(67)); // ELangLatvian, Latvian.
        iIsoToLangMap.put("lt", new Integer(68)); // ELangLithuanian, Lithuanian.
        iIsoToLangMap.put("mk", new Integer(69)); // ELangMacedonian, Macedonian.
        iIsoToLangMap.put("ms", new Integer(70)); // ELangMalay, Malay.
        iIsoToLangMap.put("ml", new Integer(71)); // ELangMalayalam, Malayalam.
        iIsoToLangMap.put("mr", new Integer(72)); // ELangMarathi, Marathi.
        iIsoToLangMap.put("mo", new Integer(73)); // ELangMoldavian, Moldavian.
        iIsoToLangMap.put("mn", new Integer(74)); // ELangMongolian, Mongolian.
        iIsoToLangMap.put("nn", new Integer(75)); // ELangNorwegianNynorsk, Norwegian Nynorsk.
        iIsoToLangMap.put("pt-br", new Integer(76)); // ELangBrazilianPortuguese, Brazilian Portuguese.
        iIsoToLangMap.put("kn", new Integer(62)); // ELangKannada, Kannada.
        iIsoToLangMap.put("pa", new Integer(77)); // ELangPunjabi, Punjabi.
        iIsoToLangMap.put("ro", new Integer(78)); // ELangRomanian, Romanian.
        iIsoToLangMap.put("sr", new Integer(79)); // ELangSerbian, Serbian.
        iIsoToLangMap.put("si", new Integer(80)); // ELangSinhalese, Sinhalese.
        iIsoToLangMap.put("so", new Integer(81)); // ELangSomali, Somali.
        iIsoToLangMap.put("es-us", new Integer(83)); // ELangLatinAmericanSpanish, American Spanish.
        iIsoToLangMap.put("sw", new Integer(84)); // ELangSwahili, Swahili.
        iIsoToLangMap.put("sv-fi", new Integer(85)); // ELangFinlandSwedish, Finland Swedish.
        iIsoToLangMap.put("ta", new Integer(87)); // ELangTamil, Tamil.
        iIsoToLangMap.put("te", new Integer(88)); // ELangTelugu, Telugu.
        iIsoToLangMap.put("bo", new Integer(89)); // ELangTibetan, Tibetan.
        iIsoToLangMap.put("ti", new Integer(90)); // ELangTigrinya, Tigrinya.
        iIsoToLangMap.put("tr-cy", new Integer(91)); // ELangCyprusTurkish, Cyprus Turkish.
        iIsoToLangMap.put("tk", new Integer(92)); // ELangTurkmen, Turkmen.
        iIsoToLangMap.put("uk", new Integer(93)); // ELangUkrainian, Ukrainian.
        iIsoToLangMap.put("ur", new Integer(94)); // ELangUrdu, Urdu.
        iIsoToLangMap.put("vi", new Integer(96)); // ELangVietnamese, Vietnamese.
        iIsoToLangMap.put("cy", new Integer(97)); // ELangWelsh, Welsh.
        iIsoToLangMap.put("zu", new Integer(98)); // ELangZulu, Zulu.

        // No mapping yet for the following:
        // 24 // ELangInternationalFrench, International French.
        // 47 // ELangInternationalEnglish, International English.
        // 82 // ELangInternationalSpanish, International Spanish.
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Maps given ISO language/country code to Symbian TLanguage.
     *
     * @param aLocale ISO language/country code
     * @return Symbian TLanguage value, or -1 if no matching language is found.
     */
    public static int isoToLang(String aLocale)
    {
        // Set locale to lowercase and replace possible underscore with dash.
        aLocale = aLocale.toLowerCase().replace('_', '-');
        Integer lang = (Integer)iIsoToLangMap.get(aLocale);
        if (lang == null)
        {
            // Get language using two first characters.
            lang = (Integer)iIsoToLangMap.get(aLocale.substring(0, 2));
        }
        if (lang == null)
        {
            return -1; // No matching language.
        }
        return lang.intValue();
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Default constructor.
     */
    private S60LangMapper()
    {
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
