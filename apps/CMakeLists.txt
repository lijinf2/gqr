PROJECT(APPS)

SET(APPS
    sample_queries
    sample_remove_queries
    cal_groundtruth
    bin_to_fvecs
    doublebin_to_fvecs
    benchhasher
    search
    opq_evaluate
    test
)

FOREACH(APP ${APPS})
    ADD_EXECUTABLE(${APP} ${APP}.cpp)
ENDFOREACH(APP)

SET(EXECUTABLE_OUTPUT_PATH ${LSHBOX_BINARY_DIR}/bin/${SAVE_CLASS})

ADD_SUBDIRECTORY(mips)
