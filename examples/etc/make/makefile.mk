AWK := awk
AWK_FLAGS :=

# Define commands based on the operating system
ifeq ($(OS),Windows_NT)
  RM = del /f /q
  RMDIR = rmdir /s /q
else
  RM = rm -f
  RMDIR = rm -rf
endif

mydir := $(dir $(lastword $(MAKEFILE_LIST)))

.PHONY: help
help:
	@$(AWK) $(AWK_FLAGS) \
		-f ${mydir}/makefile-doc.awk $(MAKEFILE_LIST)
