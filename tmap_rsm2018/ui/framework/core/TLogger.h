#pragma once

namespace SKT
{

class TMAP_RSM_UI_DLLSHARED_EXPORT TLogger
{
public:
    TLogger(void);
    ~TLogger(void);

    static TLogger* instance();

    void setSavePath( const QString& path );

    void write( const QString& msg );

private:   
    bool mSaveLog;
    QString mSavePath;
};

}
