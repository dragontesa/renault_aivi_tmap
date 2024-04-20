TEMPLATE = subdirs
include(global_jlr2019.pri)

tmap_app.file = tmap_jlr2019_app.pro
tmap_ui.file = ui/tmap_jlr2019_ui.pro
vsmsdk.file = vsmsdk/build/qtcreator_vsm_sdk/build_navi.pro
tmap_ui.depends = vsmsdk
tmap_app.depends = tmap_ui

SUBDIRS += tmap_app
SUBDIRS += tmap_ui

ignore_mnssdk {
    # no-op
} else {
    tmap_ndds.file = ndds/tmap_jlr_ndds.pro
    tmap_app.depends += tmap_ndds
    SUBDIRS += tmap_ndds
}
SUBDIRS += vsmsdk
TRANSLATIONS += ui/res/lang/tmap_rsm_ui_ko.ts ui/res/lang/tmap_rsm_ui_en.ts
