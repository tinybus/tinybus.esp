#  SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
#  SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0

## Select the target to build (esp32 / esp32s3 / exp32c6 / ....)
TARGET := esp32s3

# import settings for the used ESP-IDF
include ../make/makefile-524.mk

# targets
# ---------------------------------------------------------------------------
.PHONY: cmake all build flash clean

build: ## (re)compile for the selected TARGET
	${IDFPY} -DIDF_TARGET=${TARGET} build size -l

flash: build ## Flash the firmware to ESP32
	${IDFPY} flash

## Delete build directory and sdkconfig
clean:
	$(RMDIR) build && $(RM) sdkconfig

## Build the firmware for host execution (Linux only)
## This will start the monitor
linux:
	${IDFPY} --preview set-target linux build monitor
