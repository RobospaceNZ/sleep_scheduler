#
# Copyright (c) 2025 Hubert Jetschko
#
# SPDX-License-Identifier: MIT
#

# Header only, there are no sources to build
if(CONFIG_SLEEP_SCHEDULER)
  zephyr_include_directories(.)
endif()
