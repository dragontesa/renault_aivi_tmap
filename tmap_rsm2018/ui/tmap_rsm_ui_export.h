#ifndef TMAP_RSM_UI_EXPORT_H
#define TMAP_RSM_UI_EXPORT_H

#ifndef TMAP_RSM_UI_STATIC
    #if defined(TMAP_RSM_UI_DLL_LIBRARY)
        #define TMAP_RSM_UI_DLLSHARED_EXPORT Q_DECL_EXPORT
    #else
        #define TMAP_RSM_UI_DLLSHARED_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define TMAP_RSM_UI_DLLSHARED_EXPORT
#endif

#endif // TMAP_RSM_UI_EXPORT_H

