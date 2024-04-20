#ifndef VIEWTREE_H
#define VIEWTREE_H

namespace  SKT {

T_SHAREABLE(ViewTreeNode)

class ViewTreeNode
{
public:
    ViewTreeNode();
    ViewTreeNode(QObject* view, const QString& url);
    ~ViewTreeNode();

    ViewTreeNodePtr parent();
    void setParent( const ViewTreeNodePtr& parent );

    QObject* view() const;
    QString url() const;

    void appendChild(const ViewTreeNodePtr& child);
    void removeChild(const ViewTreeNodePtr& child);

    ViewTreeNodePtr child(int row);
    int childCount() const;    
    int row() const;    

    int tick();
    void setTick( int tick );

private:
    QList<ViewTreeNodePtr> mChildren;
    QObject* mView;
    ViewTreeNodePtr mParent;
    QString mUrl;
    int mTick;
};

class ViewTree;
typedef std::function<bool(const ViewTreeNodePtr& node, bool visible, int depth)> ViewTreeNodeVisitor;

struct ViewTreeNodeIterator
{
    ViewTree* viewTree = nullptr;
    ViewTreeNodeVisitor visitor;

    void traverse( const ViewTreeNodePtr& node, int depth = 0 );
};

class ViewTree : public QObject
{
    Q_OBJECT
public:
    ViewTree();

    void addView( QObject* view, QObject* parentView, const QString& source );

    void visitNodes( const ViewTreeNodeVisitor& visitor );

    QObject* findViewByObjectName( const QString& objectName );
    ViewTreeNodePtr findNodeByUrl( const QString& url );

    QList<QList<ViewTreeNodePtr>> visibleDepthList();

    QString debugVisibleDepthList();
    QString debugVisibleNodes();

    static QVariant readProperty( QObject* view, const QString& name );

    void handleViewVisibleChanged( QObject* view );
    void handleViewDestruction( QObject* view );

signals:
    void visibilityChanged();    

private:    
    void removeView( QObject* view );

    void updateVisibleDepth();

    bool isActivatableView( QObject* view );

    QList<ViewTreeNodePtr> mRootNodes;
    QMap<QObject*,ViewTreeNodePtr> mViewNodeMap;
    QList<QList<ViewTreeNodePtr>> mVisibleDepthList;
    int mTick;
};

}

#endif // VIEWTREE_H
