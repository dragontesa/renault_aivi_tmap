/*!
    \class ApplicationMediator
    \brief MVC view 간의 생성 관계에 의한 트리 구조 유지

    생성 관계 트리 구조를 기반으로 visibilty 계층 구조 유지(visibleDepthList)
    visibility 변화시 visibilityChanged 시그널 발생

    복잡성을 피하기 위해 업무로직에 영향받지 않도록 ViewTree 자체는 위와 같은 데이터 구조만을 유지하도록 한다.
*/

#include "ViewTree.h"
#include "CommonConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

#define VIEW_TREE_USE_IN_QML
#define VIEW_TREE_ENABLE_DEBUG

#ifdef VIEW_TREE_ENABLE_DEBUG
    #define ViewTree_debugMsg( msg ) qDebug().noquote() << QString( "[ViewTree]%1" ).arg( msg )
#else
    #define ViewTree_debugMsg( msg )
#endif

ViewTreeNode::ViewTreeNode(QObject* view, const QString& url)
    : mView( view ),
    mUrl( url ),
    mTick( -1 )
{
}

ViewTreeNode::~ViewTreeNode()
{
}

void ViewTreeNode::appendChild(const ViewTreeNodePtr& child)
{
    mChildren.append(child);
}

void ViewTreeNode::removeChild(const ViewTreeNodePtr& child)
{
    int foundIndex = -1;

    for ( int i = 0, count = mChildren.count(); i < count; i++ ) {
        auto item = mChildren[i];
        if ( item->view() == child->view() ) {
            foundIndex = i;
            break;
        }
    }

    mChildren.removeAt( foundIndex );
}

ViewTreeNodePtr ViewTreeNode::child(int row)
{
    return mChildren.value(row);
}

int ViewTreeNode::childCount() const
{
    return mChildren.count();
}

QObject* ViewTreeNode::view() const
{
    return mView;
}

QString ViewTreeNode::url() const
{
    return mUrl;
}

ViewTreeNodePtr ViewTreeNode::parent()
{
    return mParent;
}

void ViewTreeNode::setParent( const ViewTreeNodePtr& parent )
{
    mParent = parent;
}

int ViewTreeNode::row() const
{
    int ret = -1;

    for ( int i = 0, count = mParent->mChildren.count(); i < count; i++ ) {
        auto item = mParent->mChildren[i];
        if ( item->view() == mView ) {
            ret = i;
            break;
        }
    }

    return ret;
}

int ViewTreeNode::tick()
{
    return mTick;
}

void ViewTreeNode::setTick( int tick )
{
    mTick = tick;
}

void ViewTreeNodeIterator::traverse( const ViewTreeNodePtr& node, int depth )
{
    auto visible = ViewTree::readProperty( node->view(), CommonConstants::Visible).toBool();
    auto cont = visitor( node, visible, depth );
    if ( cont ) {
        for ( int i = 0, count = node->childCount(); i < count; i++ ) {
            auto child = node->child(i);
            traverse( child, depth + 1 );
        }
    }
}

ViewTree::ViewTree()
    : mTick( 0 )
{
}

bool ViewTree::isActivatableView( QObject* view )
{
    auto regId = QQmlProperty::read( view, QStringLiteral("regId") ).toInt();
    auto exclude = ViewRegistry::hasTraits( regId,
        ViewRegistry::GuiFrame |
        ViewRegistry::RgPopup |
        ViewRegistry::Overlap |
        //ViewRegistry::Widget |
        ViewRegistry::Test
        );

    return !exclude;
}

void ViewTree::addView( QObject* view, QObject* parentView, const QString& url )
{
    mTick++;

    auto newNode = t_new_shared<ViewTreeNode>( view, url );
    newNode->setTick( mTick );
    QQmlProperty::write(view, CommonConstants::Z, newNode->tick());

    auto parentNode = mViewNodeMap[parentView];
    if ( parentNode ) {
        parentNode->appendChild( newNode );
        newNode->setParent( parentNode );
    }
    else {
        mRootNodes.append( newNode );
    }

    mViewNodeMap[view] = newNode;

    updateVisibleDepth();

    if ( isActivatableView( view ) ) {
        ViewTree_debugMsg( QString( "Added %1\n%2" ).arg( ViewTree::readProperty( view, CommonConstants::ObjectName).toString() ).arg( debugVisibleNodes() ) );
    }

    emit visibilityChanged();
}

void ViewTree::removeView( QObject* view )
{
    mTick++;

    auto node = mViewNodeMap[view];
    auto parentNode = node->parent();
    if ( parentNode ) {
        parentNode->removeChild( node );
    }

    QMutableListIterator<ViewTreeNodePtr> it( mRootNodes );
    while ( it.hasNext() ) {
        auto& node = it.next();
        if ( node->view() == view ) {
            it.remove();
        }
    }

    mViewNodeMap.remove( view );

    // 17.10.27 hskim add children to root nodes
    for ( int i = 0; i < node->childCount(); i++ ) {
        auto childNode = node->child( i );
        childNode->setParent( ViewTreeNodePtr() );
        mRootNodes.append( childNode );
    }

    updateVisibleDepth();

    if ( isActivatableView( view ) ) {
        ViewTree_debugMsg( QString( "Removed %1\n%2" ).arg( ViewTree::readProperty( view, CommonConstants::ObjectName).toString() ).arg( debugVisibleNodes() ) );
    }

    emit visibilityChanged();
}

QObject* ViewTree::findViewByObjectName( const QString& objectName )
{
    QObject* ret = nullptr;

    visitNodes( [&](const ViewTreeNodePtr& node, bool visible, int depth) {
        auto objName = ViewTree::readProperty( node->view(), CommonConstants::ObjectName).toString();
        if ( objName == objectName ) {
            ret = node->view();
            return false;
        }

        return true;
    });

    return ret;
}

ViewTreeNodePtr ViewTree::findNodeByUrl( const QString& url )
{
    ViewTreeNodePtr ret;

    QFileInfo fi( url );
    auto absSrcPath = fi.absoluteFilePath();

    visitNodes( [&](const ViewTreeNodePtr& node, bool visible, int depth) {
        if ( node->url() == absSrcPath ) {
            ret = node;
            return false;
        }

        return true;
    });

    return ret;
}

QList<QList<ViewTreeNodePtr>> ViewTree::visibleDepthList()
{
    return mVisibleDepthList;
}

void ViewTree::handleViewVisibleChanged( QObject* view )
{  
    mTick++;

    auto node = mViewNodeMap[view];
    auto visible = ViewTree::readProperty( view, CommonConstants::Visible).toBool();
    if ( visible ) {
        node->setTick( mTick );
        QQmlProperty::write(view, CommonConstants::Z, node->tick());

        auto visitor = [&](const ViewTreeNodePtr& node, bool visible, int depth) {
            if ( !visible ) {
                return false;
            }
            node->setTick( mTick );

            return true;
        };

        ViewTreeNodeIterator nodeIt;
        nodeIt.viewTree = this;
        nodeIt.visitor = visitor;

        nodeIt.traverse( node );
    }

    updateVisibleDepth();

    if ( isActivatableView( view ) ) {
        ViewTree_debugMsg( QString( "VisibleChanged %1 visible: %2\n%3" ).arg( ViewTree::readProperty( view, CommonConstants::ObjectName).toString() ).arg( visible ).arg( debugVisibleNodes() ) );
    }

    emit visibilityChanged();
}

void ViewTree::handleViewDestruction( QObject* view )
{
    removeView( view );
}

QVariant ViewTree::readProperty( QObject* view, const QString& name )
{
#ifdef  VIEW_TREE_USE_IN_QML
    return QQmlProperty::read( view, name);
#else
    return view->property( name );
#endif
}

QString ViewTree::debugVisibleNodes()
{
    QString buf;

#ifdef VIEW_TREE_ENABLE_DEBUG
    visitNodes( [&](const ViewTreeNodePtr& node, bool visible, int depth) {
        auto nodeChar = QString().fill( QChar( '-' ), depth );
        buf.append( QString( "%1 %2" )
            .arg( nodeChar )
            .arg( ViewTree::readProperty( node->view(), CommonConstants::ObjectName).toString() )
        );
        auto z = ViewTree::readProperty( node->view(), CommonConstants::Z ).toInt();
        buf.append( QString( "(T: %1 Z: %2)" ).arg( node->tick() ).arg( z ) );
        if ( visible ) {
            buf.append( " v" );
        }
        buf.append( "\n" );
        return true;
    });
#endif

    return buf;
}

QString ViewTree::debugVisibleDepthList()
{
    QString buf;

#ifdef VIEW_TREE_ENABLE_DEBUG
    for ( int depth = mVisibleDepthList.count() - 1; depth >= 0; depth-- ) {
        auto& nodeList = mVisibleDepthList[depth];
        buf.append( QString( "%1: " ).arg( depth ) );
        for ( auto node : nodeList ) {
            buf.append( ViewTree::readProperty( node->view(), CommonConstants::ObjectName).toString() );
            buf.append( QString( "(%1)" ).arg( node->tick() ) );
            buf.append( " " );
        }
        buf.append( "\n" );
    }
#endif

    return buf;
}

void ViewTree::updateVisibleDepth()
{
    mVisibleDepthList.clear();

    visitNodes( [&](const ViewTreeNodePtr& node, bool visible, int depth) {
        if ( !visible ) {
            return false;
        }

        auto visDepth = mVisibleDepthList.count();
        if ( visDepth <= depth ) {
            mVisibleDepthList.append(QList<ViewTreeNodePtr>());
        }

        QList<ViewTreeNodePtr>& viewList = mVisibleDepthList[depth];
        viewList.append( node );

        return true;
    });
}

void ViewTree::visitNodes( const ViewTreeNodeVisitor& visitor )
{
    ViewTreeNodeIterator nodeIt;
    nodeIt.viewTree = this;
    nodeIt.visitor = visitor;

    for ( auto rootNode :  mRootNodes ) {
        nodeIt.traverse( rootNode );
    }
}
