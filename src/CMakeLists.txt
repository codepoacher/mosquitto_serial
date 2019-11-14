cmake_minimum_required(VERSION 2.6)

PROJECT(udemod C)
ADD_DEFINITIONS(-Os -Wall -Werror --std=gnu99 -D_GNU_SOURCE -D__USE_XOPEN -Wmissing-declarations)

FILE(GLOB SOURCES *.c)

ADD_EXECUTABLE(udemod ${SOURCES})
TARGET_LINK_LIBRARIES(udemod mosquitto pthread dl)

INSTALL(TARGETS udemod
	RUNTIME DESTINATION sbin
)
