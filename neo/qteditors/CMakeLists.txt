
set(SOURCES
  qteditors_public.h
  precompiled.h
  common/common.cpp  
  dialogs/EfxEditor.h
  dialogs/EfxEditor.cpp  
  dialogs/LightEditor.h
  dialogs/LightEditor.cpp
  widgets/RenderWidget.h
  widgets/RenderWidget.cpp  
  widgets/NumEdit.h
  widgets/NumEdit.cpp
  widgets/SliderNumEdit.h
  widgets/SliderNumEdit.cpp  
  widgets/ColorEdit.h
  widgets/ColorEdit.cpp  
  widgets/Vec3Edit.h
  widgets/Vec3Edit.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_force_include(precompiled.h SOURCES)
add_msvc_precompiled_header(precompiled.h SOURCES)

set(TARGET_NAME "qteditors")
add_definitions(-DGLEW_STATIC -D__DOOM__ -D__DOOM_DLL__)
add_library(${TARGET_NAME} STATIC ${SOURCES})
if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND ID_ENFORCE32BIT=OFF)
  add_dependencies(${TARGET_NAME} fhGame-x64 fhGame-roe-x64)
else()
  add_dependencies(${TARGET_NAME} fhGame-x86 fhGame-roe-x86)
endif()
qt5_use_modules(${TARGET_NAME} Widgets)

if(MSVC)
  set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER libs)

  source_group(common common/.*)
  source_group(dialogs dialogs/.*)
  source_group(widgets widgets/.*)
endif()

set_cpu_arch(${TARGET_NAME}) 