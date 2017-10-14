PROJECT(APPS)

SET(APPS
    sample_queries
    cal_groundtruth
    hasher
    benchhasher
    search
)

FOREACH(APP ${APPS})
    ADD_EXECUTABLE(${APP} ${APP}.cpp)
ENDFOREACH(APP)

SET(EXECUTABLE_OUTPUT_PATH ${LSHBOX_BINARY_DIR}/bin/${SAVE_CLASS})
