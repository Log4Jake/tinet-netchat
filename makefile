# ----------------------------
# Makefile Options
# ----------------------------

NAME = TINET
ICON = icon.png
DESCRIPTION = "TINET Client"
COMPRESSED = YES
ARCHIVED = YES

CFLAGS = -Wall -Wextra -Oz -ferror-limit=100
CXXFLAGS = -Wall -Wextra -Oz -ferror-limit=100

# ----------------------------

include $(shell cedev-config --makefile)
