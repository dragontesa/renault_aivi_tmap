#ifndef LAYOUTMANANGER_H
#define LAYOUTMANANGER_H

namespace SKT {

class LayoutManager {
public:
    LayoutManager();
    ~LayoutManager();
	
    int layout();
    void setLayout( int layout );
    int defaultLayout();
    void setDefaultLayout( int layout );

    QString layoutName();

    QString layoutResDir( int layout );

    QString layoutGlobalConstantsJs( int layout );

    QString mainWindowPath();

private:	
    int mLayout;
    int mDefaultLayout;
};

}

#endif
