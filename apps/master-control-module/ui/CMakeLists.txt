# EvH.MasterControl.UI — hardware-style controls used by the master control panel.
# Pure QML; built as a static QML module linked into the application.

qt_add_library(master-control-module-ui STATIC)

# Implementation detail of the controls; not usable from outside the module
set_source_files_properties(GradientEllipse.qml PROPERTIES QT_QML_INTERNAL_TYPE TRUE)

qt_add_qml_module(master-control-module-ui
    URI EvH.MasterControl.UI
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    # Imported from main.cpp via Q_IMPORT_QML_PLUGIN
    CLASS_NAME EvH_MasterControl_UIPlugin
    QML_FILES
        FlipSwitch.qml
        GradientEllipse.qml
        ImageToggleSwitch.qml
        LedIndicator.qml
    DEPENDENCIES
        QtQuick
)
