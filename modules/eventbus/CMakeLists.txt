add_library(eventbus INTERFACE include/event/event.h include/event/event_scope.h include/event/forward.h)

#set_property(TARGET util PROPERTY CXX_STANDARD 20)

target_include_directories(eventbus INTERFACE include)
#target_include_directories(eventbus PRIVATE src)

target_link_libraries(eventbus INTERFACE util common)
#target_link_libraries(eventbus PRIVATE logger)