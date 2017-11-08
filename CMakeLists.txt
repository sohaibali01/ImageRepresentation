cmake_minimum_required(VERSION 2.8)

project( Representation )

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIRS} )

SET(RepresentationSrcs imageRepresentation.cpp colourHistogram.cpp main.cpp RandomPointsRepresentationColorHistogram.cpp RandomPointsRepresentationMultiDimensionalHistogram.cpp)  
SET(RepresentationHeaders imageRepresentation.h colourHistogram.h RandomPointsRepresentationColorHistogram.h RandomPointsRepresentationMultiDimensionalHistogram.h)

add_executable(Representation ${RepresentationSrcs} ${RepresentationHeaders} )

target_link_libraries( Representation ${OpenCV_LIBS} )