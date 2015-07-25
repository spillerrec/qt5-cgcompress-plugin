#include <QImageIOHandler>
#include <QImageIOPlugin>

class OraPlugin: public QImageIOPlugin{
	Q_OBJECT
	
#if QT_VERSION >= 0x050000
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "ora.json")
#endif
	
	public:
		QStringList keys() const;
		Capabilities capabilities( QIODevice *device, const QByteArray &format ) const;
		QImageIOHandler* create( QIODevice *device, const QByteArray &format = QByteArray() ) const;
};