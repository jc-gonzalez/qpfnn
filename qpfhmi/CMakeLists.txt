#======================================================================
# CMakeLists.txt
# QPF - Prototype of QLA Processing Framework
# qpfgui - QPF HMI Application
#======================================================================
# Author: J C Gonzalez - 2015,2016,2017
# Copyright (C) 2015,2016,2017  Euclid SOC Team at ESAC
#======================================================================
include (../common.cmake)

#===== Projec dir. =======
project (qpfGui)

find_package (Qt5Widgets)
find_package (Qt5Sql)

set (qpfGuiLib_src
  browser.cpp
  configtool.cpp
  connectionwidget.cpp
  dbbrowser.cpp
  dbmng.cpp
  dbtblmodel.cpp
  dbtreemodel.cpp
  dlgalert.cpp
  dlgdataing.cpp
  dlgshowtaskinfo.cpp
  exttooledit.cpp
  exttoolsdef.cpp
  frm_filter.cpp
  frm_filterset.cpp
  frmagentstatus.cpp
  frmhostinfo.cpp
  hmipxy.cpp
  logframe.cpp
  logwatcher.cpp
  mainwindow.cpp
  procfmkmonit.cpp
  procalertmodel.cpp
  proctskstatmodel.cpp
  productsmodel.cpp
  progbardlg.cpp
  qjsonitem.cpp
  qjsonmodel.cpp
  qsimpletickergraph.cpp
  sysalertmodel.cpp
  textview.cpp
  txtblmodel.cpp
  types.cpp
  verbleveldlg.cpp
  txitem.cpp
  txitemdlg.cpp
  xmlsyntaxhighlight.cpp)

set (qpfGuiLib_hdr
  browser.h
  configtool.h
  connectionwidget.h
  dbbrowser.h
  dbmng.h
  dbtblmodel.h
  dbtreemodel.h
  dlgalert.h
  dlgdataing.h
  dlgshowtaskinfo.h
  exttooledit.h
  exttoolsdef.h
  filters.h
  frm_filter.h
  frm_filterset.h
  frmagentstatus.h
  frmhostinfo.h
  hmipxy.h
  hmitypes.h
  init.h
  logframe.h
  logwatcher.h
  mainwindow.h
  procfmkmonit.h
  procalertmodel.h
  proctskstatmodel.h
  productsmodel.h
  progbardlg.h
  qjsonitem.h
  qjsonmodel.h
  qsimpletickergraph.h
  sysalertmodel.h
  textview.h
  txtblmodel.h
  types.h
  verbleveldlg.h
  version.h
  txitem.h
  txitemdlg.h
  xmlsyntaxhighlight.h)

set (qpfGuiLib_ui
  mainwindow.ui
  dlgdataing.ui
  dlgshowtaskinfo.ui
  configtool.ui
  browserwidget.ui
  dbbrowser.ui
  dlgalert.ui
  exttooledit.ui
  exttoolsdef.ui
  frm_filterset.ui
  frm_filter.ui
  frmagentstatus.ui
  frmhostinfo.ui
  verbleveldlg.ui
  logframe.ui
  txitem.ui)

set (qpfGuiLib_qrc
  configtool.qrc
  jsontree.qrc
  icon.qrc
  tx.qrc)

set (qpfGuiBin_src main.cpp)

qt5_wrap_cpp(qpfGuiLib_hdr_moc ${qpfGuiLib_hdr})
qt5_wrap_ui (qpfGuiLib_ui_moc  ${qpfGuiLib_ui})
qt5_add_resources(qpfGuiLib_ui_qrc ${qpfGuiLib_qrc})

include_directories (${PROJECT_SOURCE_DIR})
include_directories (${PROJECT_BINARY_DIR})
link_directories (${PSQLDIR}/lib ${ZMQDIR}/lib ${CURLDIR}/lib
                  ${UUIDDIR}/lib ${PCRE2DIR}/lib)

add_library (qpfGuiLib SHARED
  ${qpfGuiLib_src}
  ${qpfGuiLib_hdr_moc}
  ${qpfGuiLib_ui_moc}
  ${qpfGuiLib_ui_qrc})
#target_link_libraries (qpfGuiLib Qt5::Widgets Qt5::Sql)
qt5_use_modules(qpfGuiLib Widgets Sql Network Xml)
target_include_directories (qpfGuiLib PUBLIC .
  ${PSQLINCDIR}
  ${NNMSG_ROOT_DIR}/include
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${NNCOMM_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${PCRE2LIBDIR}
  ${PSQLLIBDIR})

add_executable(qpfgui ${qpfGuiBin_src})
add_dependencies (qpfgui qpfGuiLib)
target_include_directories (qpfgui PUBLIC .
    ${PSQLINCDIR}
  ${NNMSG_ROOT_DIR}/include
  ${JSON_ROOT_DIR}
  ${INFIX_ROOT_DIR}
  ${NNCOMM_ROOT_DIR}
  ${TOOLS_ROOT_DIR}
  ${STR_ROOT_DIR}
  ${LOG_ROOT_DIR}
  ${UUID_ROOT_DIR}
  ${FMK_ROOT_DIR}
  ${PCRE2LIBDIR}
  ${PSQLLIBDIR})
target_link_libraries (qpfgui qpfGuiLib
  json infix str log tools uuid fmk nncomm
  nanomsg ${PCRE2LIB} ${PSQLLIB} pthread)
set_target_properties (qpfgui PROPERTIES LINKER_LANGUAGE CXX)

install (TARGETS qpfGuiLib qpfgui
         RUNTIME DESTINATION bin
         ARCHIVE DESTINATION lib
         LIBRARY DESTINATION lib)
