# SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
# SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0

cmake_minimum_required(VERSION 3.16)

list(APPEND SOURCES src/tinybus.cpp src/tb_config.cpp)

idf_component_register(
    SRCS ${SOURCES}
    INCLUDE_DIRS include
    PRIV_INCLUDE_DIRS src
)
