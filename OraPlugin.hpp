#include <QImageIOHandler>
#include <QImageIOPlugin>

class OraPlugin: public QImageIOPlugin{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "ora.json")
	
	public:
		QStringList keys() const;
		Capabilities capabilities( QIODevice *device, const QByteArray &format ) const;
		QImageIOHandler* create( QIODevice *device, const QByteArray &format = QByteArray() ) const;
};