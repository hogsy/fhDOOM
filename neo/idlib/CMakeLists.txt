
SET(SOURCES
  Base64.cpp
  Base64.h
  BitMsg.cpp
  BitMsg.h
  bv/Bounds.cpp
  bv/Bounds.h
  bv/Box.cpp
  bv/Box.h
  bv/Frustum.cpp
  bv/Frustum.h
  bv/Frustum_gcc.cpp
  bv/Sphere.cpp
  bv/Sphere.h
  CmdArgs.cpp
  CmdArgs.h
  containers/ArrayRef.h
  containers/BinSearch.h
  containers/BTree.h
  containers/HashIndex.cpp
  containers/HashIndex.h
  containers/HashTable.h
  containers/Hierarchy.h
  containers/LinkList.h
  containers/List.h
  containers/PlaneSet.h
  containers/Queue.h
  containers/Sort.h
  containers/Stack.h
  containers/StaticList.h
  containers/StrList.h
  containers/StrPool.h
  containers/VectorSet.h
  Dict.cpp
  Dict.h
  geometry/DrawVert.cpp
  geometry/DrawVert.h
  geometry/JointTransform.cpp
  geometry/JointTransform.h
  geometry/Surface.cpp
  geometry/Surface.h
  geometry/Surface_Patch.cpp
  geometry/Surface_Patch.h
  geometry/Surface_Polytope.cpp
  geometry/Surface_Polytope.h
  geometry/Surface_SweptSpline.cpp
  geometry/Surface_SweptSpline.h
  geometry/TraceModel.cpp
  geometry/TraceModel.h
  geometry/Winding.cpp
  geometry/Winding.h
  geometry/Winding2D.cpp
  geometry/Winding2D.h
  hashing/CRC32.cpp
  hashing/CRC32.h
  hashing/MD4.cpp
  hashing/MD4.h
  hashing/MD5.cpp
  hashing/MD5.h
  Heap.cpp
  Heap.h
  LangDict.cpp
  LangDict.h
  Lexer.cpp
  Lexer.h
  Lib.cpp
  Lib.h
  MapFile.cpp
  MapFile.h
  math/Angles.cpp
  math/Angles.h
  math/Complex.cpp
  math/Complex.h
  math/Curve.h
  math/Extrapolate.h
  math/Interpolate.h
  math/Lcp.cpp
  math/Lcp.h
  math/Math.cpp
  math/Math.h
  math/Matrix.cpp
  math/Matrix.h
  math/Ode.cpp
  math/Ode.h
  math/Plane.cpp
  math/Plane.h
  math/Pluecker.cpp
  math/Pluecker.h
  math/Polynomial.cpp
  math/Polynomial.h
  math/Quat.cpp
  math/Quat.h
  math/Random.h
  math/Rotation.cpp
  math/Rotation.h
  math/Simd.cpp
  math/Simd.h
  #math/Simd_3DNow.cpp
  #math/Simd_3DNow.h
  #math/Simd_AltiVec.cpp
  #math/Simd_AltiVec.h
  math/Simd_Generic.cpp
  math/Simd_Generic.h
  #math/Simd_MMX.cpp
  #math/Simd_MMX.h
  math/Simd_SSE.cpp
  math/Simd_SSE.h
  #math/Simd_SSE2.cpp
  #math/Simd_SSE2.h
  #math/Simd_SSE3.cpp
  #math/Simd_SSE3.h
  math/Vector.cpp
  math/Vector.h
  Parser.cpp
  Parser.h
  #precompiled.cpp
  precompiled.h
  Str.cpp
  Str.h
  StrRef.h
  Timer.cpp
  Timer.h
  Token.cpp
  Token.h
)

source_group(idlib .*)  
source_group(idlib\\BV bv/.*)  
source_group(idlib\\Containers containers/.*)  
source_group(idlib\\Geometry geometry/.*)  
source_group(idlib\\Hashing hashing/.*)  
source_group(idlib\\Math math/.*) 


include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_force_include(precompiled.h SOURCES)
add_msvc_precompiled_header(precompiled.h SOURCES)

add_library(idlib ${SOURCES})
SET_PROPERTY(TARGET idlib                PROPERTY FOLDER libs)
set_cpu_arch(idlib)
