#pragma once

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT TApplication : public QApplication
{
public:
    TApplication(int& argc, char ** argv);

    QString path( const QString& path, const QString& path2 = QString(),
        const QString& path3 = QString(), const QString& path4 = QString() ) const;
};

}

