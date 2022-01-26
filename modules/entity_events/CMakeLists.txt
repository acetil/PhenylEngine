add_library(entity_events INTERFACE include/event/events)

#set_property(TARGET util PROPERTY CXX_STANDARD 20)

target_include_directories(entity_events INTERFACE include)
#target_include_directories(eventbus PRIVATE src)

#target_link_libraries(eventbus PRIVATE logger)