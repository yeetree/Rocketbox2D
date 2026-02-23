# bin2h.cmake
file(READ ${INPUT_FILE} hex_content HEX)

string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " array_content ${hex_content})

set(header_text "
#pragma once
const unsigned char ${VAR_NAME}[] = { ${array_content} };
const unsigned int ${VAR_NAME}_len = sizeof(${VAR_NAME});
")

file(WRITE ${OUTPUT_FILE} "${header_text}")