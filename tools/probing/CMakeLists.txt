PROJECT(PROBINGS)

SET(PROBINGS
    newitqlsh_probing
    laitqlsh_rank
    laitqlsh_topkquery
    lapcalsh_topkquery
    lashlsh_topkquery
    lasphlsh_topkquery
    hasher
)

FOREACH(PROBING ${PROBINGS})
    ADD_EXECUTABLE(${PROBING} ${PROBING}.cpp)
ENDFOREACH(PROBING)

# SET(EXECUTABLE_OUTPUT_PATH ${LSHBOX_BINARY_DIR}/bin/${SAVE_CLASS})
SET(EXECUTABLE_OUTPUT_PATH ${LSHBOX_BINARY_DIR}/bin)
