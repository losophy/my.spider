#?=的意思是如果變量沒有，則定義，如果有則用原來的。
PLAT ?= none
#可見skynet可以在這三個平台編譯。
PLATS = linux freebsd macosx
#編譯器用的是gcc
CC ?= gcc

#.PHONY是一個固定用法。：後面的代表的是偽目標（標籤）。偽目標是相對於目標而言的。$()則是引用變量。
.PHONY : none $(PLATS) clean all cleanall

#如果$(PLAT)不等於none，意思就是其他地方定義了PLAT，則定義偽目標default，來執行命令$(MAKE) $(PLAT)。$(MAKE)應該就是make。
#ifneq ($(PLAT), none)

.PHONY : default

default :
	$(MAKE) $(PLAT)

#endif

none :
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

#:=是變量賦值，=也是賦值。兩者的區別是，使用=變量的推導可以在後面，這樣可能會出現無限推導，造成報錯。使用:=使得前面的變量不能使用後面的變量。
SKYNET_LIBS := -lpthread -lm
SHARED := -fPIC --shared
EXPORT := -Wl,-E

linux : PLAT = linux
macosx : PLAT = macosx
freebsd : PLAT = freebsd

#其中+=是變量賦值追加的意思。
macosx : SHARED := -fPIC -dynamiclib -Wl,-undefined,dynamic_lookup
macosx : EXPORT :=
macosx linux : SKYNET_LIBS += -ldl
linux freebsd : SKYNET_LIBS += -lrt

# Turn off jemalloc and malloc hook on macosx

macosx : MALLOC_STATICLIB :=
macosx : SKYNET_DEFINES :=-DNOUSE_JEMALLOC

#$@比較特殊，是一個自動化變量，代表所有目標集。這裏應該是對應相應的plat，linux就是linux，macosx就是macosx。
linux macosx freebsd :
	$(MAKE) all PLAT=$@ SKYNET_LIBS="$(SKYNET_LIBS)" SHARED="$(SHARED)" EXPORT="$(EXPORT)" MALLOC_STATICLIB="$(MALLOC_STATICLIB)" SKYNET_DEFINES="$(SKYNET_DEFINES)"
