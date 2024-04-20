#ifndef DUALCENTERPOLICYFETCH_H
#define DUALCENTERPOLICYFETCH_H
#include <QObject>

namespace SKT {

class PolicyFetcher;
class ServerPolicyFetchCreator
{
public:
    ServerPolicyFetchCreator();
    PolicyFetcher* create();
};

class PolicyFetcher: public QObject
{
    Q_OBJECT
    friend class ServerPolicyFetchCreator;
private:
    PolicyFetcher(QObject* parent=nullptr);
public:
    virtual ~PolicyFetcher();

public:
    void fetch();

private:
    void fetchDualcenterPolicy(const QString& policy);
    void fetchReroutePolicy(const QString& policy);
    void tryFetch();

public slots:
    void  free();

private slots:
    void  handleNddsMessage(int msg, long lParam1, long lParam2, void *lpParam);

signals:
     void fetchedDualcenterPolicy(const QString& domainName, int portNum);
     void fetchedReroutePolicy(const QString& policy);
     void finished();

private:
     int mFailCnt; // 재시도 횟수 최대 3회면 로컬로 전환

};

}

#endif // DUALCENTERPOLICYFETCH_H
