set(SOURCES MDNSService.cpp)

idf_component_register( SRCS ${SOURCES}
                        INCLUDE_DIRS .
                        REQUIRES mdns)