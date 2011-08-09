file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/_gen")

tpqt4_setup_dbus_test_environment()

if(ENABLE_TP_GLIB_TESTS)
    include_directories(${TELEPATHY_GLIB_INCLUDE_DIR}
                        ${GLIB2_INCLUDE_DIR}
                        ${DBUS_INCLUDE_DIR})

    add_definitions(-DQT_NO_KEYWORDS)

    tpqt4_add_dbus_unit_test(CallChannel call-chan tp-glib-tests tp-qt4-yell-tests tp-qt4-tests-glib-helpers)
    tpqt4_add_dbus_unit_test(AccountsModelBasics accounts-model-basics tp-glib-tests tp-qt4-yell-tests
                                                                       tp-qt4-tests-glib-helpers
                                                                       telepathy-qt4-yell-models)
                                                                       #${GLIB2_LIBRARY}
                                                                       #example-cm-echo2
                                                                       #${PKG_GOBJECT2_LIBRARIES}
endif(ENABLE_TP_GLIB_TESTS)

# Make check target. In case of check, output on failure and put it into a log
# This target has to stay here for catching all of the tests
add_custom_target(check ctest --output-on-failure -O test.log
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
add_dependencies(check check-local ${tpqt4_test_cases})
