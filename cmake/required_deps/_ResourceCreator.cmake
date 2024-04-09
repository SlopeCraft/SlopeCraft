
message(STATUS "Downloading ResourceCreator.cmake...")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/3rdParty/")
set(SC_RC_location "${CMAKE_BINARY_DIR}/3rdParty/ResourceCreator.cmake")
file(DOWNLOAD "https://github.com/SlopeCraft/ResourceCreator.cmake/releases/download/v0.0.0/ResourceCreator.cmake"
    ${SC_RC_location}
    EXPECTED_HASH SHA512=241799A7BCC3A0AF1BD32FFD350EEEF0751D4D86DA7F11CFA6BD8F979E150F1585E9F55CBBBBAAB5C59C7379F5F93C27BF8E414BB0A603DFC5D07136C37EC6FA
)

include(${SC_RC_location})