/*
 * QBGlobal.hpp
 *
 *  Created on: 27.02.2013
 *      Author: kirill
 */

#ifndef QBGLOBAL_HPP_
#define QBGLOBAL_HPP_

#include <QObject>

#if defined(QB)
#  define QB_EXPORT Q_DECL_EXPORT
#else
#  define QB_EXPORT Q_DECL_IMPORT
#endif

//////////// Static constant definitions ///////////
const QString QBLOX_API_SERVER("http://api.quickblox.com/"); // QBlox server
//const QString QBLOX_APP_ID("1161"); // add here your Application id
//const QString QBLOX_AUTH_KEY("KqAB7pdDnxJgPmx"); // add here your Authorization key
//const QString QBLOX_AUTH_SECRET("J9sepXuXpqbhBgM"); // add here your Authorization secret
//const QString QBLOX_APP_ID("1908"); // add here your Application id
//const QString QBLOX_AUTH_KEY("fcV43YZctc5uC-k"); // add here your Authorization key
//const QString QBLOX_AUTH_SECRET("OfykhKX3AmxDEqz"); // add here your Authorization secret
const QString QBLOX_APP_ID("92"); // add here your Application id
const QString QBLOX_AUTH_KEY("wJHdOcQSxXQGWx5"); // add here your Authorization key
const QString QBLOX_AUTH_SECRET("BTFsj7Rtt27DAmT"); // add here your Authorization secret


#endif /* QBGLOBAL_HPP_ */
