/*
 * QBUtils.cpp
 *
 *  Created on: 01.03.2013
 *      Author: kirill
 */

#include "QBUtils.hpp"

QDateTime QBUtils::getCurrentTime()
{
	return QDateTime::currentDateTime();
}

QDateTime QBUtils::getCurrentTimeUTC()
{
#if QT_VERSION >= 0x040700
	return QDateTime::currentDateTimeUtc();
#else
	return QDateTime::currentDateTime().toUTC();
#endif
}

QString QBUtils::getTimestampUTC() {
#if QT_VERSION >= 0x040700
	return QDateTime::currentDateTimeUtc().toString();
#else
	return QDateTime::currentDateTime().toUTC().toString();
#endif
}

uint QBUtils::getTimestampUTCMs() {
#if QT_VERSION >= 0x040700
	return QDateTime::currentDateTimeUtc().toTime_t();
#else
	return QDateTime::currentDateTime().toUTC().toTime_t();
#endif
}
