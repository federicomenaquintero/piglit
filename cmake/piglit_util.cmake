# Copyright 2012 Intel Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

# function piglit_include_target_api
#
# If the file CMakeLists.${piglit_target_api}.txt exists in the current source
# directory, then include it.
#
function(piglit_include_target_api)

    # Verify that the value of `piglit_target_api` is valid.
    set(valid_api FALSE)

    foreach(api "gl" "gles1" "gles2" "no_api")
        if(piglit_target_api STREQUAL ${api})
            set(valid_api TRUE)
            break()
        endif(piglit_target_api STREQUAL ${api})
    endforeach(api)

    if(NOT valid_api)
        message(FATAL_ERROR "Invalid value for piglit_target_api: ${piglit_target_api}")
    endif(NOT valid_api)

    # Include CMakeLists.${piglit_target_api}.txt` if it exists.
    set(api_file ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.${piglit_target_api}.txt)
    if(EXISTS ${api_file})
        include(${api_file})
    endif(EXISTS ${api_file})

endfunction(piglit_include_target_api)