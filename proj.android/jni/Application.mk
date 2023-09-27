# APP_STL := gnustl_static
#APP_STL := c++_shared

APP_STL := c++_static

APP_CPPFLAGS := -frtti -DCC_ENABLE_CHIPMUNK_INTEGRATION=1 -std=c++11 -fsigned-char
APP_LDFLAGS := -latomic


APP_DEBUG := $(strip $(NDK_DEBUG))
ifeq ($(APP_DEBUG),1)
  APP_CPPFLAGS += -DCOCOS2D_DEBUG=1
  APP_OPTIM := debug
else
  APP_CPPFLAGS += -DNDEBUG
  APP_OPTIM := release
endif

# si APP_STL:=++static, hay que poner LIBCXX_FORCE_REBUILD := true 
LIBCXX_FORCE_REBUILD := true  
