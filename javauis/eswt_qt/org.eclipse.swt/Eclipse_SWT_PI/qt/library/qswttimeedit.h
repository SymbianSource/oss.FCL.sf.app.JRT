/*******************************************************************************
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/

#ifndef QSWTTIMEEDIT_H
#define QSWTTIMEEDIT_H


#include <QDateTimeEdit>


namespace Java { namespace eSWT {


/**
 * QSwtTimeEdit is needed to get cursor position information from
 * QDateTimeEdit
 */
class QSwtTimeEdit
    : public QDateTimeEdit
    {
    Q_OBJECT

public:

	QSwtTimeEdit();

    int textLength() const;
    int cursorPosition() const;
    QLineEdit* editor() const;
    void setTextVisible(bool enable);

protected:
	// From QDateTimeEdit
	virtual QString textFromDateTime( const QDateTime& dateTime ) const;
	bool textEnable;
    };

/**
 * QOffsetTimeEdit implements OFFSET style date editor that can have negative
 * values
 */
class QOffsetTimeEdit
    : public QSwtTimeEdit
    {
    Q_OBJECT

public:
    QOffsetTimeEdit();

    int getOffset();
    void setOffset(int offset);
    void setMinimum(int minimum);
    void setMaximum(int maximum);

    // From QAbstractSpinBox
    virtual void stepBy(int steps);

public slots:
    void timeChanged( const QTime& time);

protected:
    // From QDateTimeEdit
    virtual QString textFromDateTime( const QDateTime& dateTime ) const;

    // From QAbstractSpinBox
    virtual StepEnabled stepEnabled() const;

private:
    void getHMS(int& hour, int& minute, int& second) const;
    void setOffset(int hour, int minute, int second);

    int offset;
    int sign;
    int reset;
    int minimum;
    int maximum;
};

} // namespace eSWT

} // namespace Java

#endif // QSWTTIMEEDIT_H

