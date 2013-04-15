/*
 * QBUtils.hpp
 *
 *  Created on: 01.03.2013
 *      Author: kirill
 */

#ifndef QBUTILS_HPP_
#define QBUTILS_HPP_

#include "QBGlobal.hpp"
#include <QObject>
#include <QDateTime>

class QB_EXPORT QBUtils {
public:
	static QDateTime getCurrentTimeUTC();
	static QString getTimestampUTC();
	static uint getTimestampUTCMs();
};

#endif /* QBUTILS_HPP_ */
