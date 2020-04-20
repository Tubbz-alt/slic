FIND_PATH( HEPPDT_INCLUDE_DIR HepPDT/Version.hh ${HEPPDT_DIR}/include )

FIND_LIBRARY( HEPPDT_LIBRARY HepPDT ${HEPPDT_DIR}/lib )
FIND_LIBRARY( HEPPID_LIBRARY HepPID ${HEPPDT_DIR}/lib )

SET( HEPPDT_LIBRARIES ${HEPPDT_LIBRARY} ${HEPPID_LIBRARY} )

# Report the found libraries, quit with fatal error if any required library has not been found.
FIND_PACKAGE_HANDLE_STANDARD_ARGS( HEPPDT DEFAULT_MSG HEPPDT_LIBRARIES HEPPDT_INCLUDE_DIR )
