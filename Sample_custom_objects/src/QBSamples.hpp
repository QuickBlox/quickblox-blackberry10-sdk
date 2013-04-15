// Default empty project template
#ifndef QBSamples_HPP_
#define QBSamples_HPP_

#include <QObject>

namespace bb {
namespace cascades {
class Application;
}
}

class QBCustomObject;

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class QBSamples : public QObject
{
    Q_OBJECT
public:
    QBSamples(bb::cascades::Application *app);
    virtual ~QBSamples();

public slots:
	int displayWidth();
	int displayHeight();
	QBCustomObject* getNote(QString note, QString comment);
private:
};


#endif /* QBSamples_HPP_ */
