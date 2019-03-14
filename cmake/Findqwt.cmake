if(${TARGET_SELECTED_ARCH} STREQUAL "buildroot-imx" )
    include_directories(/usr/local/qwt-release/include )
    link_directories(/usr/local/qwt-release/lib/ )
else ()
    include_directories(/usr/local/qwt-6.1.2/include )
    link_directories(/usr/local/qwt-6.1.2/lib/ )
endif()

