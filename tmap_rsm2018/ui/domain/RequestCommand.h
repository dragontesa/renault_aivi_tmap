#ifndef REQUESTCOMMAND_H
#define REQUESTCOMMAND_H

#include <QObject>
namespace SKT {

T_SHAREABLE( RequestCommand );

class RequestCommand : public QObject , public mvc::TObserver
{
    Q_OBJECT
public:
    explicit RequestCommand(const mvc::IObserver::NotifyMethod& notifyMethod,
                            const QObjectPtr& notifyContext,
                            const QVariantMap reqCommand);

    ~RequestCommand();

    QVariantMap getReqData();
    QVariantMap getResData();
    void setResData(const QVariantMap &data);
    void setReqData(const QVariantMap &data);

signals:

public slots:

private:
    QVariantMap reqData;
    QVariantMap resData;
};

}

#endif // REQUESTCOMMAND_H
