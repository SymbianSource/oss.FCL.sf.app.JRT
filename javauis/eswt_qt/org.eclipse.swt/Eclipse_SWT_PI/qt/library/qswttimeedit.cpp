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

#include <QDateTimeEdit>
#include <QLineEdit>
#include "qswttimeedit.h"
#include "swtlog.h"


using namespace Java::eSWT;


#define SWT_POSITIVE_DISPLAY_FORMAT "hh:mm:ss"
#define SWT_NEGATIVE_DISPLAY_FORMAT "-hh:mm:ss"

#define SWT_GETHMS()    \
        int hour = 0;   \
        int minute = 0; \
        int second = 0; \
        getHMS(hour, minute, second);

//
// QSwtTimeEdit
//

QSwtTimeEdit::QSwtTimeEdit()
	: QDateTimeEdit(), textEnable(true)
	{

	}

int QSwtTimeEdit::textLength() const
    {
    SWT_LOG_FUNC_CALL();

    QLineEdit* editor = lineEdit();
    if (editor)
        {
        return editor->text().length();
        }
    return 0;
    }

int QSwtTimeEdit::cursorPosition() const
    {
    SWT_LOG_FUNC_CALL();
    QLineEdit* editor = lineEdit();
    if (editor)
        {
        return editor->cursorPosition();
        }
    return 0;
    }

QLineEdit* QSwtTimeEdit::editor() const
    {
    SWT_LOG_FUNC_CALL();
    return lineEdit();
    }

QString QSwtTimeEdit::textFromDateTime( const QDateTime& dateTime ) const
    {
    SWT_LOG_FUNC_CALL();

    if( textEnable == true)
		{
    	return QDateTimeEdit::textFromDateTime(dateTime);
		}
    else
		{
    	return "";
		}

    }

void QSwtTimeEdit::setTextVisible(bool enable)
	{
	SWT_LOG_FUNC_CALL();
    textEnable = enable;
	}

//
// QOffsetTimeEdit
//
QOffsetTimeEdit::QOffsetTimeEdit()
    : QSwtTimeEdit(),
      offset(0), sign(1), reset(false), minimum(0), maximum(0)
    {
    SWT_LOG_FUNC_CALL();
    setDisplayFormat(SWT_POSITIVE_DISPLAY_FORMAT);
    setTimeSpec(Qt::UTC);
    setDateTime(QDateTime(QDate::currentDate(), QTime(1,1,1), Qt::UTC));
    setTimeRange(QTime(0,0,0), QTime(23,59,59));

    connect( this, SIGNAL(timeChanged(const QTime&)), this, SLOT(timeChanged(const QTime&)));
    }

QAbstractSpinBox::StepEnabled QOffsetTimeEdit::stepEnabled() const
    {
    StepEnabled flags = StepNone;

    if (offset < maximum) flags |= StepUpEnabled;
    if (offset > minimum) flags |= StepDownEnabled;

    return flags;
    }

void QOffsetTimeEdit::getHMS(int& hour, int& minute, int& second) const
    {
    hour = offset / 3600;
    minute = ( offset % 3600 ) / 60;
    second = ( offset % 3600 ) % 60;
    }

int QOffsetTimeEdit::getOffset()
    {
    return offset;
    }

void QOffsetTimeEdit::setOffset(int newOffset)
    {
    offset = newOffset;
    if (offset > maximum) offset = maximum;
    if (offset < minimum) offset = minimum;
    stepBy(0);
    }

void QOffsetTimeEdit::setMinimum(int minimum)
    {
    this->minimum = minimum;
    }

void QOffsetTimeEdit::setMaximum(int maximum)
    {
    this->maximum = maximum;
    }

void QOffsetTimeEdit::setOffset(int hour, int minute, int second)
    {
    offset = (hour * 3600 + minute * 60 + second);
    if (sign < 0 && offset > 0 || sign > 0 && offset < 0)
        {
        offset = -offset;
        }
    if (offset > maximum) offset = maximum;
    if (offset < minimum) offset = minimum;
    }

void QOffsetTimeEdit::timeChanged( const QTime& time)
    {
    SWT_LOG_FUNC_CALL();
    if (reset) return;

    // Set offset according to contents of the UI
    setOffset(time.hour(), time.minute(), time.second());

    // Always keep the internal state of QTimeEdit at (1,1,1)
    reset = true;
    setDateTime(QDateTime(QDate::currentDate(), QTime(1,1,1), Qt::UTC));
    reset = false;
    }

QString QOffsetTimeEdit::textFromDateTime( const QDateTime& /*dateTime*/ ) const
    {
    SWT_LOG_FUNC_CALL();

    // When the widget asks for string representation of the time in the
    // widget, give back a string constructed from the offset
    SWT_GETHMS();

    if( textEnable )
    	{
    	QDateTime offsetTime(QDate::currentDate(), QTime(qAbs(hour), qAbs(minute), qAbs(second)), Qt::UTC);
    	return QDateTimeEdit::textFromDateTime(offsetTime);
    	}
    else
		{
    	return "";
		}
    }

void QOffsetTimeEdit::stepBy(int steps)
    {
    SWT_LOG_FUNC_CALL();
    int index = currentSectionIndex();
    int step = 0;
    bool signChanged = false;

    SWT_GETHMS();

    if (index == 0)
        {
        step = steps*3600;
        hour += steps;
        }
    else if (index == 1)
        {
        step = steps*60;
        minute += steps;
        }
    else if (index == 2)
        {
        step = steps;
        second += steps;
        }

    // Block signals while changing the state of the editor to prevent
    // Java side from generating unwanted events
    blockSignals(true);

    // Check if setting the new value would cause the sign to change
    if ((sign < 0 && offset + step >= 0) || (sign > 0 && offset + step < 0))
        {
        signChanged = true;
        sign = -sign;
        }

    if (steps != 0)
        {
        if (signChanged)
            {
            // When the sign changes, the numbers stay the same, only
            // the sign changes
            setOffset(qAbs(hour), qAbs(minute), qAbs(second));
            }
        else
            {
            // When the sign does not change, going e.g. down to 59 from 60 secs
            // causes hour/minute to change also
            setOffset(hour, minute, second);
            }
        }

    // Call base class implementation to make sure that widget
    // gets redrawn etc.
    QDateTimeEdit::stepBy(0);

    // If offset sign changes because of the step, update the
    // display format of the widget
    if (signChanged)
        {
        setDisplayFormat(sign < 0 ? SWT_NEGATIVE_DISPLAY_FORMAT : SWT_POSITIVE_DISPLAY_FORMAT);
        }

    // Make sure that we didn't cause the active section to change
    setCurrentSection(sectionAt(index));

    // Unblock signals
    blockSignals(false);

    // Emit a signal to notify Java side of the change if the value changes (i.e. steps != 0)
    if (steps != 0)
        {
        reset = true;
        emit dateTimeChanged(QDateTime(QDate::currentDate(), QTime(1,1,1), Qt::UTC));
        reset = false;
        }
    }

