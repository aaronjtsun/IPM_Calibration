##############################
PROJECT( ipm )
##############################
cmake_minimum_required (VERSION 2.6)

find_package ( OpenCV REQUIRED )

set(SOURCE_FILES 
	IPM_Calibration.cpp
)


# Add executable and target link libraries
ADD_EXECUTABLE( ipm ${SOURCE_FILES})

target_link_libraries( ipm ${OpenCV_LIBS} )
