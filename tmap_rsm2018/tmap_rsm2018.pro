TEMPLATE = subdirs
include(global_rsm2018.pri)

tmap_app.file = tmap_rsm_app.pro
tmap_ui.file = ui/tmap_rsm_ui.pro
tmap_ndds.file = ndds/tmap_rsm_ndds.pro
vsmsdk.file = vsmsdk/build/qtcreator_vsm_sdk/build_navi.pro
tmap_ui.depends = vsmsdk
tmap_app.depends = tmap_ui tmap_ndds

SUBDIRS += tmap_app
SUBDIRS += tmap_ndds
SUBDIRS += tmap_ui
SUBDIRS += vsmsdk

TRANSLATIONS += ui/res/lang/tmap_rsm_ui_ko.ts ui/res/lang/tmap_rsm_ui_en.ts
