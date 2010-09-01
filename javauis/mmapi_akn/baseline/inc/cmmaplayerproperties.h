/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for storing and parsing properties
*
*/


#ifndef CMMAPLAYERPROPERTIES_H
#define CMMAPLAYERPROPERTIES_H

//  INCLUDES
#include <e32base.h>

// STRUCTS

/**
 * These structs are used for storing array of strings
 */
NONSHARABLE_STRUCT(TMMAStaticStr)
{
    TInt iLength;
    const TText* iStr;

    inline const TPtrC operator()() const
    {
        return TPtrC(iStr, iLength);
    }
};

NONSHARABLE_STRUCT(TMMAStaticStrArray)
{
    const TMMAStaticStr* iStr;
    inline const TPtrC operator()() const
    {
        return TPtrC(iStr->iStr, iStr->iLength);
    }
};

#define MMA_PARAMETER_ARRAY(name) const TMMAStaticStrArray name[] =
#define MMA_PARAMETER_STR(name, s) static const TMMAStaticStr name = {sizeof(L##s)/sizeof(TText)-1, (TText*)L##s}
#define MMA_PARAMETER_ARRAY_SIZE(name) (sizeof(name)/sizeof((name)[0]))

//  CLASS DECLARATION
/**
*   This class is base class for storing and validating
*/

NONSHARABLE_CLASS(MMMAParameterRule)
{
public:
    /**
     * Validates key-value pair based on rule. If derived class is leaf with specified key
     * then it must store value if it is valid.
     */
    virtual TBool ValidateAndStoreL(const TDesC& aKey, const TDesC& aValue) = 0;

    /**
     * Compares key-value pair based on rule and stored value.
     */
    virtual TBool Compare(const TDesC& aKey, const TDesC& aValue) = 0;
    virtual TBool Compare(const TDesC& aKey, const TInt aValue) = 0;

    /**
     * Gets value for specified key, returns EFalse if specified key is not found
     */
    virtual TBool FindProperty(const TDesC& aKey, TPtrC& aValue) = 0;
    virtual TBool FindProperty(const TDesC& aKey, TInt& aValue) = 0;
};

//  CLASS DECLARATION
/**
*   This class is used for storing and parsing properties
*   The property string is in format "name=value&name=value&..."
*
*/

NONSHARABLE_CLASS(CMMAPlayerProperties): public CBase
{
public:// Construction and destruction
    /**
    * Two-phased constructor.
    */
    static CMMAPlayerProperties* NewL(const TDesC& aProperties,
    MMMAParameterRule& aRule);
    /**
    * Destructor.
    */
    ~CMMAPlayerProperties();

protected: // default constructor and second phase construction
    CMMAPlayerProperties(const TDesC& aProperties, MMMAParameterRule& aRule);

public: // new methods
    /**
     * Gets property for given key. Returns EFalse if key cannot
     * be found
     */
    TBool GetProperty(const TDesC& aKey, TInt& aValue) const;
    TBool GetProperty(const TDesC& aKey, TPtrC& aValue) const;

    /**
     * Compares that given key and value pair can be found in
     * given properties string. Notice that this returns EFalse also
     * if given key is not found.
     */
    TBool Compare(const TDesC& aKey, const TInt& aValue) const;
    TBool Compare(const TDesC& aKey, const TDesC& aValue) const;

    /**
     * Validates given properties with given rule(s), leaves with
     * KErrArgument if properties was not valid.
     */

    void ValidateL() const;

private: //data
    // properties
    const TDesC& iProperties;
    // rule composition
    MMMAParameterRule& iRule;
};



//  CLASS DECLARATION
/**
*   Composite class for storing rule set
*/

NONSHARABLE_CLASS(CMMAParameterRuleSet): public CBase, public MMMAParameterRule
{
public: // Construction and destruction
    /**
    * Two-phased constructor.
    */
    static CMMAParameterRuleSet* NewLC();

    /**
    * Destructor.
    */
    ~CMMAParameterRuleSet();

protected: // default constructor and second phase construction
    CMMAParameterRuleSet();
    virtual void ConstructL();

public: // From MMMAParameterRule
    TBool ValidateAndStoreL(const TDesC& aKey, const TDesC& aValue);
    TBool Compare(const TDesC& aKey, const TDesC& aValue);
    TBool Compare(const TDesC& aKey, const TInt aValue);
    TBool FindProperty(const TDesC& aKey, TPtrC& aValue);
    TBool FindProperty(const TDesC& aKey, TInt& aValue);

public: // new methods
    void AppendRuleL(MMMAParameterRule* aRule);

private: // data
    // list of rules in this composite, owned
    CArrayPtrSeg< MMMAParameterRule >* iRules;
};

//  CLASS DECLARATION
/**
*   Base class for property rules
*/
NONSHARABLE_CLASS(TMMAParameterRuleBase): public MMMAParameterRule
{
public: // constructor
    TMMAParameterRuleBase(const TDesC& aKey);

public: // From MMMAParameterRule
    TBool ValidateAndStoreL(const TDesC& aKey, const TDesC& aValue);
    TBool Compare(const TDesC& aKey, const TDesC& aValue);
    TBool Compare(const TDesC& aKey, const TInt aValue);
    TBool FindProperty(const TDesC& aKey, TPtrC& aValue);
    TBool FindProperty(const TDesC& aKey, TInt& aValue);

protected: // new methods
    /**
     * Returns EFalse if value is not valid for this rule
     * Derived class must also store value if it is accepted
     */
    virtual TBool ValidateValueL(const TDesC& aValue) = 0;

protected: //data
    const TDesC& iKey;
    TBool iAssigned;
};

//  CLASS DECLARATION
/**
*   Rule for TInt
*/
NONSHARABLE_CLASS(TMMAParameterRuleInt): public TMMAParameterRuleBase
{
public: // constructors
    TMMAParameterRuleInt(const TDesC& aKey);

    TMMAParameterRuleInt(const TDesC& aKey,
                         const TInt aLowerLimit);

    TMMAParameterRuleInt(const TDesC& aKey,
                         const TInt aLowerLimit,
                         const TInt aUpperLimit);

public: // From MMMAParameterRule
    TBool ValidateValueL(const TDesC& aValue);
    TBool Compare(const TDesC& aKey, const TInt aValue);
    TBool FindProperty(const TDesC& aKey, TInt& aValue);



private: // data
    TInt iValue;
    const TInt iUpperLimit;
    const TInt iLowerLimit;
};

//  CLASS DECLARATION
/**
*   Rule for TDesC
*/
NONSHARABLE_CLASS(TMMAParameterRuleDes): public TMMAParameterRuleBase
{
public: // constructors
    TMMAParameterRuleDes(const TDesC& aKey);

    TMMAParameterRuleDes(const TDesC& aKey,
                         const TMMAStaticStrArray* aValidValues,
                         const TInt aArraySize);

public: // From MMMAParameterRule
    TBool ValidateValueL(const TDesC& aValue);
    TBool Compare(const TDesC& aKey, const TDesC& aValue);
    TBool FindProperty(const TDesC& aKey, TPtrC& aValue);


private: // data
    TPtrC iValue;
    // not owned
    const TMMAStaticStrArray* iValidValues;
    const TInt iArraySize;
};
#endif // CMMAPLAYERPROPERTIES_H
