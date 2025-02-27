cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)
project(WebF)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
endif()

if (${ENABLE_PROFILE})
  add_definitions(-DENABLE_PROFILE=1)
else ()
  add_definitions(-DENABLE_PROFILE=0)
endif ()

execute_process(
  COMMAND bash "-c" "if [ ! -d \"node_modules\" ]; then npm install; fi"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/code_generator
) # install code_generator deps

execute_process(
  COMMAND bash "-c" "npm run build"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/code_generator
) # g

execute_process(
  COMMAND bash "-c" "node bin/code_generator -s ../../bindings/qjs/dom/elements -d ../../bindings/qjs/dom/elements/.gen"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/code_generator
) # generate elements code

execute_process(
  COMMAND bash "-c" "node bin/code_generator -s ../../bindings/qjs/dom/events -d ../../bindings/qjs/dom/events/.gen"
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/scripts/code_generator
) # generate events code

execute_process(
  COMMAND bash "-c" "read dart_sdk < <(type -p dart) && echo $\{dart_sdk%/*\}/cache/dart-sdk/include | xargs"
  OUTPUT_VARIABLE DART_SDK
)
string(REGEX REPLACE "\n$" "" DART_SDK "${DART_SDK}")

list(APPEND WEBF_PUBLIC_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/webf_bridge.h
)

set(QUICKJS_PUBLIC_HEADERS
  third_party/quickjs/cutils.h
  third_party/quickjs/libregexp.h
  third_party/quickjs/libregexp-opcode.h
  third_party/quickjs/libunicode.h
  third_party/quickjs/libunicode-table.h
  third_party/quickjs/list.h
  third_party/quickjs/quickjs.h
  third_party/quickjs/quickjs-atom.h
  third_party/quickjs/quickjs-opcode.h
)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  add_compile_options(-fPIC)
endif()

if (ENABLE_ASAN)
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer -O1)
  add_link_options(-fsanitize=address -fno-omit-frame-pointer)
endif ()

if (DEFINED PLATFORM)
  if (${PLATFORM} STREQUAL "OS")
    add_compile_options(-fno-aligned-allocation)
  endif()
endif()

list(APPEND BRIDGE_SOURCE
        webf_bridge.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/include/webf_bridge.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/webf_foundation.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/dart_methods.h
  foundation/logging.cc
  foundation/logging.h
  foundation/colors.h
  foundation/ref_counted_internal.h
  foundation/ref_counter.h
  foundation/ref_ptr.h
  foundation/ref_ptr_internal.h
  foundation/ui_task_queue.h
  foundation/ui_task_queue.cc
  foundation/inspector_task_queue.h
  foundation/inspector_task_queue.cc
  foundation/task_queue.cc
  foundation/task_queue.h
  foundation/ui_command_buffer.cc
  foundation/ui_command_buffer.h
  foundation/ui_command_callback_queue.cc
  foundation/closure.h
  dart_methods.cc
  polyfill/dist/polyfill.cc
  )

list(APPEND GUMBO_PARSER
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/attribute.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/char_ref.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/error.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/parser.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/string_buffer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/string_piece.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/tag.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/string_piece.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/tokenizer.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/utf8.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/util.c
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party/gumbo-parser/src/vector.c
  )

list(APPEND BRIDGE_INCLUDE
  ${CMAKE_CURRENT_LIST_DIR}/foundation
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/include
  ${CMAKE_CURRENT_LIST_DIR}/polyfill/dist
  ${DART_SDK}
  ${ADDITIONAL_INCLUDE_DIRS}
  )

if ($ENV{WEBF_JS_ENGINE} MATCHES "quickjs")
  add_compile_options(-DWEBF_QUICK_JS_ENGINE=1)

  execute_process(
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/third_party/quickjs/VERSION
    OUTPUT_VARIABLE QUICKJS_VERSION
  )

  list(APPEND QUICK_JS_SOURCE
    third_party/quickjs/src/libbf.c
    third_party/quickjs/src/cutils.c
    third_party/quickjs/src/libregexp.c
    third_party/quickjs/src/libunicode.c
    third_party/quickjs/src/core/string.c
    third_party/quickjs/src/core/function.c
    third_party/quickjs/src/core/memory.c
    third_party/quickjs/src/core/bytecode.c
    third_party/quickjs/src/core/object.c
    third_party/quickjs/src/core/exception.c
    third_party/quickjs/src/core/gc.c
    third_party/quickjs/src/core/malloc.c
    third_party/quickjs/src/core/shape.c
    third_party/quickjs/src/core/parser.c
    third_party/quickjs/src/core/convertion.c
    third_party/quickjs/src/core/runtime.c
    third_party/quickjs/src/core/module.c
    third_party/quickjs/src/core/builtins/js-array.c
    third_party/quickjs/src/core/builtins/js-async-function.c
    third_party/quickjs/src/core/builtins/js-async-generator.c
    third_party/quickjs/src/core/builtins/js-atomics.c
    third_party/quickjs/src/core/builtins/js-big-num.c
    third_party/quickjs/src/core/builtins/js-boolean.c
    third_party/quickjs/src/core/builtins/js-date.c
    third_party/quickjs/src/core/builtins/js-function.c
    third_party/quickjs/src/core/builtins/js-generator.c
    third_party/quickjs/src/core/builtins/js-json.c
    third_party/quickjs/src/core/builtins/js-map.c
    third_party/quickjs/src/core/builtins/js-math.c
    third_party/quickjs/src/core/builtins/js-number.c
    third_party/quickjs/src/core/builtins/js-object.c
    third_party/quickjs/src/core/builtins/js-closures.c
    third_party/quickjs/src/core/builtins/js-operator.c
    third_party/quickjs/src/core/builtins/js-promise.c
    third_party/quickjs/src/core/builtins/js-proxy.c
    third_party/quickjs/src/core/builtins/js-reflect.c
    third_party/quickjs/src/core/builtins/js-regexp.c
    third_party/quickjs/src/core/builtins/js-string.c
    third_party/quickjs/src/core/builtins/js-symbol.c
    third_party/quickjs/src/core/builtins/js-typed-array.c
  )
  if(${STATIC_QUICKJS})
    add_library(quickjs STATIC ${QUICK_JS_SOURCE})
  else()
    add_library(quickjs SHARED ${QUICK_JS_SOURCE})
  endif()

  target_include_directories(quickjs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/quickjs/include)

  list(APPEND BRIDGE_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/third_party)
  list(APPEND BRIDGE_LINK_LIBS quickjs)

  list(APPEND BRIDGE_SOURCE
    page.cc
    page.h
    bindings/qjs/garbage_collected.h
    bindings/qjs/executing_context.cc
    bindings/qjs/executing_context.h
    bindings/qjs/heap_hashmap.h
    bindings/qjs/native_value.cc
    bindings/qjs/native_value.h
    bindings/qjs/host_object.h
    bindings/qjs/host_object.cc
    bindings/qjs/host_class.h
    bindings/qjs/qjs_patch.c
    bindings/qjs/qjs_patch.h
    bindings/qjs/rejected_promises.cc
    bindings/qjs/rejected_promises.h
    bindings/qjs/module_manager.cc
    bindings/qjs/module_manager.h
    bindings/qjs/html_parser.cc
    bindings/qjs/html_parser.h
    bindings/qjs/bom/console.cc
    bindings/qjs/bom/console.h
    bindings/qjs/bom/screen.cc
    bindings/qjs/bom/screen.h
    bindings/qjs/bom/timer.cc
    bindings/qjs/bom/timer.h
    bindings/qjs/bom/dom_timer_coordinator.cc
    bindings/qjs/bom/dom_timer_coordinator.h
    bindings/qjs/dom/frame_request_callback_collection.cc
    bindings/qjs/dom/frame_request_callback_collection.h
    bindings/qjs/dom/event_listener_map.cc
    bindings/qjs/dom/event_listener_map.h
    bindings/qjs/dom/script_animation_controller.cc
    bindings/qjs/dom/script_animation_controller.h
    bindings/qjs/dom/event_target.cc
    bindings/qjs/dom/event_target.h
    bindings/qjs/dom/event.cc
    bindings/qjs/dom/event.h
    bindings/qjs/dom/node.h
    bindings/qjs/dom/node.cc
    bindings/qjs/dom/element.cc
    bindings/qjs/dom/element.h
    bindings/qjs/dom/document.cc
    bindings/qjs/dom/document.h
    bindings/qjs/dom/text_node.cc
    bindings/qjs/dom/text_node.h
    bindings/qjs/dom/event_type_names.h
    bindings/qjs/dom/event_type_names.cc
    bindings/qjs/dom/comment_node.cc
    bindings/qjs/dom/comment_node.h
    bindings/qjs/dom/document_fragment.cc
    bindings/qjs/dom/document_fragment.h
    bindings/qjs/dom/style_declaration.cc
    bindings/qjs/dom/style_declaration.h
    bindings/qjs/dom/css_property_list.h
    bindings/qjs/dom/elements/.gen/canvas_element.cc
    bindings/qjs/dom/elements/.gen/canvas_element.h
    bindings/qjs/dom/elements/image_element.cc
    bindings/qjs/dom/elements/image_element.h
    bindings/qjs/dom/elements/.gen/input_element.cc
    bindings/qjs/dom/elements/.gen/input_element.h
    bindings/qjs/dom/elements/.gen/textarea_element.cc
    bindings/qjs/dom/elements/.gen/textarea_element.h
    bindings/qjs/dom/elements/.gen/anchor_element.cc
    bindings/qjs/dom/elements/.gen/anchor_element.h
    bindings/qjs/dom/elements/.gen/object_element.cc
    bindings/qjs/dom/elements/.gen/object_element.h
    bindings/qjs/dom/elements/.gen/script_element.cc
    bindings/qjs/dom/elements/.gen/script_element.h
    bindings/qjs/dom/elements/template_element.cc
    bindings/qjs/dom/elements/template_element.h
    bindings/qjs/dom/events/.gen/close_event.h
    bindings/qjs/dom/events/.gen/close_event.cc
    bindings/qjs/dom/events/.gen/gesture_event.cc
    bindings/qjs/dom/events/.gen/gesture_event.h
    bindings/qjs/dom/events/.gen/input_event.cc
    bindings/qjs/dom/events/.gen/input_event.h
    bindings/qjs/dom/events/.gen/popstate_event.cc
    bindings/qjs/dom/events/.gen/popstate_event.h
    bindings/qjs/dom/events/.gen/intersection_change.cc
    bindings/qjs/dom/events/.gen/intersection_change.h
    bindings/qjs/dom/events/.gen/media_error_event.cc
    bindings/qjs/dom/events/.gen/media_error_event.h
    bindings/qjs/dom/events/.gen/mouse_event.cc
    bindings/qjs/dom/events/.gen/mouse_event.h
    bindings/qjs/dom/events/.gen/message_event.h
    bindings/qjs/dom/events/.gen/message_event.cc
    bindings/qjs/dom/events/touch_event.cc
    bindings/qjs/dom/events/touch_event.h
    bindings/qjs/bom/blob.cc
    bindings/qjs/bom/blob.h
    bindings/qjs/bom/location.h
    bindings/qjs/bom/location.cc
    bindings/qjs/bom/window.cc
    bindings/qjs/bom/window.h
    bindings/qjs/bom/performance.cc
    bindings/qjs/bom/performance.h
    bindings/qjs/dom/custom_event.cc
    bindings/qjs/dom/custom_event.h
    bindings/qjs/dom/all_collection.cc
    bindings/qjs/dom/all_collection.h
    )

  # Quickjs use __builtin_frame_address() to get stack pointer, we should add follow options to get it work with -O2
  # https://stackoverflow.com/questions/14735010/how-do-you-get-gccs-builtin-frame-address-to-work-with-o2
  add_compile_options(-fno-optimize-sibling-calls -fno-omit-frame-pointer)
  target_compile_options(quickjs PUBLIC -DCONFIG_VERSION=${\"QUICKJS_VERSION\"})

endif ()

list(APPEND PUBLIC_HEADER
  include/webf_bridge.h
  )

add_library(webf SHARED ${BRIDGE_SOURCE})
add_library(webf_static STATIC ${BRIDGE_SOURCE})

target_compile_definitions(webf PUBLIC -DFLUTTER_BACKEND=1)

add_library(gumbo_parse_static STATIC ${GUMBO_PARSER})
list(APPEND BRIDGE_LINK_LIBS gumbo_parse_static)

if (${IS_ANDROID})
  find_library(log-lib log)

  if (${ANDROID_ABI} MATCHES "armeabi-v7a" OR ${ANDROID_ABI} MATCHES "x86")
    add_definitions(-DANDROID_32_BIT=1)
  endif()

  add_definitions(-DIS_ANDROID=1)
  list(APPEND BRIDGE_LINK_LIBS ${log-lib})
elseif(${IS_IOS})
  add_definitions(-DIS_IOS=1)
endif()

### webf
target_include_directories(webf PRIVATE
  ${BRIDGE_INCLUDE}
  ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ./include)
target_link_libraries(webf PRIVATE ${BRIDGE_LINK_LIBS})

if ($ENV{WEBF_JS_ENGINE} MATCHES "quickjs")
  if (${CMAKE_BUILD_TYPE} STREQUAL "Release" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
    target_compile_options(webf PRIVATE -fno-exceptions -fvisibility=hidden -fno-rtti)
  else ()
    ### remove dynamic_cast and exceptions
    target_compile_options(webf PRIVATE -fno-exceptions -fno-rtti)
  endif ()
endif ()

### webfStatic
target_include_directories(webf_static PRIVATE
  ${BRIDGE_INCLUDE}
  ${CMAKE_CURRENT_SOURCE_DIR} PUBLIC ./include)
target_link_libraries(webf_static ${BRIDGE_LINK_LIBS})

execute_process(
  COMMAND grep version: ./pubspec.yaml
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../webf
  OUTPUT_VARIABLE APP_VER
)
string(SUBSTRING ${APP_VER} 9 30 APP_VER)
string(REGEX REPLACE "\n$" "" APP_VER "${APP_VER}")

string(REPLACE \n "" APP_VER ${APP_VER}) # Remove last \n
add_definitions(-DAPP_VERSION="${APP_VER}") # Read from dartfm version
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HEAD
)
string(REPLACE \n "" GIT_HEAD ${GIT_HEAD}) # Remove last \n
add_definitions(-DAPP_REV="${GIT_HEAD}") # Read from git head sha1

if (${ENABLE_TEST})
  add_compile_definitions(IS_TEST=true)
  include(./test/test.cmake)
endif ()

if (DEFINED ENV{LIBRARY_OUTPUT_DIR})
  set_target_properties(webf
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "$ENV{LIBRARY_OUTPUT_DIR}"
    )
  set_target_properties(webf_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "$ENV{LIBRARY_OUTPUT_DIR}")

  if ($ENV{WEBF_JS_ENGINE} MATCHES "quickjs")
    set_target_properties(quickjs PROPERTIES LIBRARY_OUTPUT_DIRECTORY "$ENV{LIBRARY_OUTPUT_DIR}")
  endif ()
elseif (IS_ANDROID)
  # android do nothing
endif ()

if (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
  set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

  set_target_properties(webf PROPERTIES
    OUTPUT_NAME webf_bridge
    FRAMEWORK TRUE
    FRAMEWORK_VERSION C
    MACOSX_FRAMEWORK_IDENTIFIER com.openwebf.webf-bridge
    MACOSX_FRAMEWORK_BUNDLE_VERSION 1.0
    MACOSX_FRAMEWORK_SHORT_VERSION_STRING 1.0
    PUBLIC_HEADER ${WEBF_PUBLIC_HEADERS}
    )

  # If quickjs is static, there will be no quickjs.framework any more.
  if(NOT DEFINED STATIC_QUICKJS)
    set_target_properties(quickjs PROPERTIES
      OUTPUT_NAME quickjs
      FRAMEWORK TRUE
      FRAMEWORK_VERSION C
      MACOSX_FRAMEWORK_IDENTIFIER com.openwebf.quickjs
      MACOSX_FRAMEWORK_BUNDLE_VERSION 1.0
      MACOSX_FRAMEWORK_SHORT_VERSION_STRING 1.0
      PUBLIC_HEADER ${QUICKJS_PUBLIC_HEADERS}
      )
  endif()
endif ()
