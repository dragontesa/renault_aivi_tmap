#ifndef SEARCHMANAGER_H
#define SEARCHMANAGER_H

#include <QObject>
#include "searchrequest.h"
#include "searchmanagerinterface.h"

namespace SKT {

class SearchWorker;
class ServerSearch;

class SearchReplyBase : public QObject, public SearchReplyBaseInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchReplyBaseInterface)
public:
    SearchReplyBase(const SearchRequestBasePtr &req, QObject *parent)
        : QObject(parent)
        , m_req(req)
        , m_error(false)
        , m_errorCause(0) {}

    virtual ~SearchReplyBase() {}

    QObject* qobject() override { return this; }
    SearchRequestBasePtr request() override { return m_req; }

    bool error() const override { return m_error; }
    int errorCause() const override { return m_errorCause; }

signals:
    void finished();

protected:
    SearchRequestBasePtr m_req;
    bool m_error;
    int m_errorCause;

    friend class SearchWorker;
    friend class ServerSearch;
    friend class SimpleSearch;
};

class SearchReply : public SearchReplyBase, public SearchReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchReplyInterface)
public:
    explicit SearchReply(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0) {}
    virtual ~SearchReply() {}

    const TPOI_SEARCH_LIST* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_totalCount; }

private:
    QScopedArrayPointer<TPOI_SEARCH_LIST> m_results;
    int m_resultCount;
    int m_totalCount;

    friend class SearchWorker;
    friend class SimpleSearch;
    friend class ServerSearch;
};

class SearchReplyPoiDetail : public SearchReplyBase, public SearchPoiReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchPoiReplyInterface)
public:
    explicit SearchReplyPoiDetail(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0) {}
    virtual ~SearchReplyPoiDetail() { }

    const TPOI_DETAIL_INFO* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_resultCount; }

private:
    QScopedArrayPointer<TPOI_DETAIL_INFO> m_results;
    int m_resultCount;

    friend class SearchWorker;
    friend class SimpleSearch;
    friend class ServerSearch;
};


class SearchReplyEvPoiDetail : public SearchReplyBase, public SearchEvPoiReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchEvPoiReplyInterface)
public:
    explicit SearchReplyEvPoiDetail(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0) {}
    virtual ~SearchReplyEvPoiDetail() { }

    const TPOI_EvDETAIL_INFO* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_resultCount; }

private:
    QScopedArrayPointer<TPOI_EvDETAIL_INFO> m_results;
    int m_resultCount;

    friend class SearchWorker;
    friend class SimpleSearch;
    friend class ServerSearch;
};

class SearchReplyPoiStation : public SearchReplyBase, public SearchPoiStationReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchPoiStationReplyInterface)
public:
    explicit SearchReplyPoiStation(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0) {}
    virtual ~SearchReplyPoiStation() { }

    const TPOI_STATION_INFO* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_resultCount; }

private:
    QScopedArrayPointer<TPOI_STATION_INFO> m_results;
    int m_resultCount;

    friend class SearchWorker;
    friend class SimpleSearch;
    friend class ServerSearch;
};

class SearchNextAddrReply : public SearchReplyBase, public SearchNextAddrReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchNextAddrReplyInterface)
public:
    explicit SearchNextAddrReply(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0)
        , m_totalCount(0) {}
    virtual ~SearchNextAddrReply() { }

    const TS_RESULT_ADDR* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_totalCount; }

private:
    QScopedArrayPointer<TS_RESULT_ADDR> m_results;
    int m_resultCount;
    int m_totalCount;

    friend class SearchWorker;
    friend class ServerSearch;
    friend class SimpleSearch;
};

class SearchNextCateReply : public SearchReplyBase, public SearchNextCateReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchNextCateReplyInterface)
public:
    explicit SearchNextCateReply(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0)
        , m_totalCount(0) {}
    virtual ~SearchNextCateReply() { }

    const TS_RESULT_CATE* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_totalCount; }

private:
    QScopedArrayPointer<TS_RESULT_CATE> m_results;
    int m_resultCount;
    int m_totalCount;

    friend class SearchWorker;
    friend class ServerSearch;
    friend class SimpleSearch;
};

class SearchFavoriteReply : public SearchReplyBase, public SearchFavoriteReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchFavoriteReplyInterface)
public:
    explicit SearchFavoriteReply(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0)
        , m_totalCount(0) {}
    virtual ~SearchFavoriteReply() { }

    const TPOI_FAVORITE_INFO* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_totalCount; }

private:
    QScopedArrayPointer<TPOI_FAVORITE_INFO> m_results;
    int m_resultCount;
    int m_totalCount;

    friend class SearchWorker;
    friend class ServerSearch;
    friend class SimpleSearch;
};


class SearchMyHomeOfficeReply: public SearchReplyBase, public SearchMyHomeOfficeReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchMyHomeOfficeReplyInterface)
public:
    explicit SearchMyHomeOfficeReply(const SearchRequestBasePtr &req, QObject* parent )
    : SearchReplyBase(req, parent) {}
    virtual ~SearchMyHomeOfficeReply() {}

    const TPOI_MYHOMEOFFICE_INFO* results() const override { return m_results.data(); }

private:
    QScopedArrayPointer<TPOI_MYHOMEOFFICE_INFO> m_results;
    friend class SearchWorker;
    friend class ServerSearch;
    friend class SimpleSearch;
};

class SearchRecentDestinationReply: public SearchReplyBase, public SearchRecentDestinationReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchRecentDestinationReplyInterface)
public:
    explicit SearchRecentDestinationReply(const SearchRequestBasePtr &req, QObject* parent)
    : SearchReplyBase(req, parent)
    , m_resultCount(0)
    , m_totalCount(0) {}
    virtual ~SearchRecentDestinationReply() {}

    const TPOI_RECENTDESTINATION_INFO* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_totalCount; }
private:
    QScopedArrayPointer<TPOI_RECENTDESTINATION_INFO> m_results;
    int m_resultCount;
    int m_totalCount;

    friend class SearchWorker;
    friend class ServerSearch;
    friend class SimpleSearch;
};


class SearchSendToCarReply : public SearchReplyBase, public SearchSendToCarReplyInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchSendToCarReplyInterface)
public:
    explicit SearchSendToCarReply(const SearchRequestBasePtr &req, QObject *parent)
        : SearchReplyBase(req, parent)
        , m_resultCount(0)
        , m_totalCount(0) {}
    virtual ~SearchSendToCarReply() { }

    const TPOI_SENDTOCAR_INFO* results() const override { return m_results.data(); }
    int resultCount() const override { return m_resultCount; }
    int totalCount() const override { return m_totalCount; }

private:
    QScopedArrayPointer<TPOI_SENDTOCAR_INFO> m_results;
    int m_resultCount;
    int m_totalCount;

    friend class SearchWorker;
    friend class ServerSearch;
    friend class SimpleSearch;
};

class SearchSisulNamePredictionReply: public SearchReplyBase,
                                      public SearchSisulNanmePredictionReplyInterface
 {
    Q_OBJECT
    Q_INTERFACES(SKT::SearchSisulNanmePredictionReplyInterface)
     public:
        SearchSisulNamePredictionReply(const SearchRequestBasePtr& req, QObject* parent)
        : SearchReplyBase(req,parent)
        , m_resultCount(0)
        , m_totalCount(0) {}
        const TSisulNamePrediction* results() const override { return m_results.data();}
        int resultCount() const { return m_resultCount;}
        int totalCount() const { return m_totalCount;}

     private:
     QScopedArrayPointer<TSisulNamePrediction> m_results;
     int m_resultCount;
     int m_totalCount;

    friend class SearchWorker;
    friend class ServerSearch;
 };


 class SearchGeoCodingReply: public SearchReplyBase,
                             public SearchGeoCodingReplyInterface
 {
     Q_OBJECT
     Q_INTERFACES(SKT::SearchGeoCodingReplyInterface)
     public:
        explicit SearchGeoCodingReply(const SearchRequestBasePtr& req, QObject* parent)
        : SearchReplyBase(req, parent){}
        const TGeoCoding* results() const override { return m_results.data();}
        
     private:
     QScopedArrayPointer<TGeoCoding> m_results;

    friend class SearchWorker;
    friend class ServerSearch;
 };


class SearchComplexImageReply: public SearchReplyBase,
                               public SearchComplexImageReplyInterface
 {
    Q_OBJECT
    Q_INTERFACES(SKT::SearchComplexImageReplyInterface)
     public:
        SearchComplexImageReply(const SearchRequestBasePtr& req, QObject* parent)
        : SearchReplyBase(req, parent) {}
        const TComplexImage* results() const override {
            if (m_results.isNull()) {
                return 0;
            }
            else {
                return m_results.data();
            }
        }
        
     private:
     QScopedArrayPointer<TComplexImage> m_results;

    friend class SearchWorker;
    friend class ServerSearch;
 };

class SearchAqiReply: public SearchReplyBase,
                      public SearchAqiReplyInterface
 {
    Q_OBJECT
    Q_INTERFACES(SKT::SearchAqiReplyInterface)
     public:
        SearchAqiReply(const SearchRequestBasePtr& req, QObject* parent)
        : SearchReplyBase(req, parent) {}
        const TAqi* results() const override { return m_results.data();}
        
     private:
     QScopedArrayPointer<TAqi> m_results;

    friend class SearchWorker;
    friend class ServerSearch;
 };


// Upload Reply Interfaces
class UploadReplyBase : public SearchReplyBase, public UploadReplyBaseInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::UploadReplyBaseInterface)
public:
    UploadReplyBase(const SearchRequestBasePtr &req, QObject* parent):
      SearchReplyBase(req, parent),
      m_uploadedCount(0) 
      {
      }
      
    virtual ~UploadReplyBase() {}

    int uploadedCount() const { return m_uploadedCount; }

private:
    int m_uploadedCount;
    friend class SearchWorker;
    friend class ServerSearch;
};

class SearchManager : public QObject, public SearchManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(SKT::SearchManagerInterface)
public:
    explicit SearchManager();
    virtual ~SearchManager();

public slots:
    SearchReplyBaseInterface* request(const SearchRequestBasePtr &req) override;
    void clear();
    void setOwner(const QObject *owner);

private:
    Q_DISABLE_COPY(SearchManager)

    static SearchReplyBase* createReply(const SearchRequestBasePtr &req, QObject *parent);

protected:
    SearchWorker *m_worker;
};

}

#endif // SEARCHMANAGER_H
