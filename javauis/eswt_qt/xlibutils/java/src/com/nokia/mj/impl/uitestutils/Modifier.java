package com.nokia.mj.impl.uitestutils;

public final class Modifier {

    // Bitmask for sending modifiers 
    // Values taken from /usr/include/X11/X.h
    public static final int Shift                           = (1<<0);
    public static final int CapsLock                        = (1<<1);
    public static final int Control                         = (1<<2);
    public static final int Alt                             = (1<<3);
    public static final int NumLock                         = (1<<4);
    public static final int ScrllLock                       = (1<<4);
    public static final int AltGr                           = (1<<7);
}
