# Build
NAME := Skidspår_test
RELEASE_NAME := Spaar
AUTHOR := Längdskidsmunken
VERSION := 0.0.0
OUTPUT_DIRECTORY := debug
PACKAGE_DIRECTORY := package
DEBUG := -Wextra -Wpedantic -Werror -DDEBUG # Komentera för icke-debug
STD := c++23

# Config
VULKAN_VERSION := 1.4.350.0
VULKAN_SDK_PATH := ~/VulkanSDK/$(VULKAN_VERSION)/macOS

# Source
SOURCE_DIRECTORIES := source
MAIN_TEST_DIRECTORY := example

# Libraries
LIBRARY_SEARCH_PATHS := /Library/Binaries
LIBRARIES := MoltenVK SDL3.0
A_SEARCH_PATHS :=
A :=

# Includes
INCLUDE_SEARCH_PATHS := source $(VULKAN_SDK_PATH)/include/ /Library/Headers

# Frameworks
FRAMEWORK_SEARCH_PATHS :=
FRAMEWORKS := Cocoa

# Commands
compile:
	mkdir -pv $(OUTPUT_DIRECTORY)/$(VERSION)/
	g++ -o $(OUTPUT_DIRECTORY)/$(VERSION)/$(NAME)																		\
		$(foreach source,$(SOURCE_DIRECTORIES),$(wildcard $(source)/*.cpp)) $(wildcard $(MAIN_TEST_DIRECTORY)/*.cpp)	\
		$(foreach include,$(INCLUDE_SEARCH_PATHS),-I$(include))															\
		$(foreach library_paths,$(LIBRARY_SEARCH_PATHS),-L$(library_paths))												\
			$(foreach lib,$(LIBRARIES),-l$(lib))																		\
		$(foreach a_paths,$(A_SEARCH_PATHS),-L$(a_paths))																\
			$(foreach a,$(A),-l$(a))																					\
		$(foreach framework_paths,$(FRAMEWORK_SEARCH_PATHS),-F$(framework_paths))										\
			$(foreach framework,$(FRAMEWORKS),-framework $(framework))													\
		-Wl,-rpath,/Library/Binaries/ -Wall $(DEBUG) -Og -std=$(STD)

run:
	clear
	rm -rf $(wildcard $(OUTPUT_DIRECTORY)/log/*.txt)
	$(OUTPUT_DIRECTORY)/$(VERSION)/$(NAME) > $(OUTPUT_DIRECTORY)/log/$(shell date '+%Y-%m-%d-%H:%M:%S').txt
	make open
open:
	code $(wildcard $(OUTPUT_DIRECTORY)/log/*.txt)

app:
	g++ -o $(PACKAGE_DIRECTORY)/$(NAME)-$(VERSION).app/Contents/MacOS/$(NAME)-$(VERSION)								\
		$(foreach source,$(SOURCE_DIRECTORIES),$(wildcard $(source)/*.cpp)) $(wildcard $(MAIN_TEST_DIRECTORY)/*.cpp)	\
		$(foreach include,$(INCLUDE_SEARCH_PATHS),-I$(include))															\
		$(foreach library_paths,$(LIBRARY_SEARCH_PATHS),-L$(library_paths))												\
			$(foreach lib,$(LIBRARIES),-l$(lib))																		\
		$(foreach a_paths,$(A_SEARCH_PATHS),-L$(a_paths))																\
			$(foreach a,$(A),-l$(a))																					\
		$(foreach framework_paths,$(FRAMEWORK_SEARCH_PATHS),-F$(framework_paths))										\
			$(foreach framework,$(FRAMEWORKS),-framework $(framework))													\
		-Wl,-rpath,@executable_path/../Frameworks/ -Wall -Ofast -std=$(STD)
bundle:
	mkdir -pv $(PACKAGE_DIRECTORY)/$(NAME)-$(VERSION).app/Contents
	mkdir -pv $(PACKAGE_DIRECTORY)/$(NAME)-$(VERSION).app/Contents/MacOS
	cat resources/Info.plist																	\
		| sed s/#1/$(NAME)/g																	\
		| sed s/#2/$(VERSION)/g																	\
		| sed s/#3/$(AUTHOR)/g																	\
		> $(PACKAGE_DIRECTORY)/$(NAME)-$(VERSION).app/Contents/Info.plist
	cp -rf $(foreach lib,$(LIBRARIES),/Library/Binaries/lib$(lib).dylib) $(PACKAGE_DIRECTORY)/$(NAME)-$(VERSION).app/Contents/Frameworks/
	open $(PACKAGE_DIRECTORY)
	cp resources/image/Spår_icon.icns $(PACKAGE_DIRECTORY)/$(NAME)-$(VERSION).app/Contents/Resources/AppIcon.icns
	codesign --force --deep --sign - $(PACKAGE_DIRECTORY)/$(NAME)-$(VERSION).app 
dmg:
	rm -f $(PACKAGE_DIRECTORY)/$(NAME).dmg
	create-dmg --volname "Skidspår Test" --volicon "resources/image/Spår_icon.icns" $(PACKAGE_DIRECTORY)/$(NAME).dmg "$(PACKAGE_DIRECTORY)"/
	open $(PACKAGE_DIRECTORY)

# Shortcuts
cr: compile run
ab: app bundle
prog:
	cloc source/ example/

# Shaders
shaders:
	$(foreach vert,$(wildcard test/shaders/*.vert),$(VULKAN_SDK_PATH)/bin/glslc $(vert) -o $(vert).spv)
	$(foreach frag,$(wildcard test/shaders/*.frag),$(VULKAN_SDK_PATH)/bin/glslc $(frag) -o $(frag).spv)

# Unit tests
units:
	rm -f tests/log/results.txt
	g++ -o tests/bin/test_bin																	\
		$(foreach source,$(SOURCE_DIRECTORIES),$(wildcard $(source)/*.cpp))						\
		$(wildcard tests/*.cpp) tests/catch2/catch_amalgamated.cpp								\
		$(foreach include,$(INCLUDE_SEARCH_PATHS),-I$(include)) -Itests/						\
		$(foreach library_paths,$(LIBRARY_SEARCH_PATHS),-L$(library_paths))						\
			$(foreach lib,$(LIBRARIES),-l$(lib))												\
		$(foreach a_paths,$(A_SEARCH_PATHS),-L$(a_paths))										\
			$(foreach a,$(A),-l$(a))															\
		$(foreach framework_paths,$(FRAMEWORK_SEARCH_PATHS),-F$(framework_paths))				\
			$(foreach framework,$(FRAMEWORKS),-framework $(framework))							\
		-Wl,-rpath,/Library/Binaries/ -Wall $(DEBUG) -Og -std=$(STD)
	./tests/bin/test_bin > tests/log/results.txt
	code tests/log/results.txt 

# Check syntax
syntax:
	g++ -fsyntax-only																			\
		$(foreach source,$(SOURCE_DIRECTORIES),$(wildcard $(source)/*.cpp))						\
		$(wildcard tests/*.cpp) tests/catch2/catch_amalgamated.cpp								\
		$(foreach include,$(INCLUDE_SEARCH_PATHS),-I$(include)) -Itests/						\
		-Wall $(DEBUG) -Og -std=$(STD)

# Actual release
RELEASE_DIRECTORY := release
final:
	g++ -dynamiclib -o $(RELEASE_DIRECTORY)/lib$(RELEASE_NAME).dylib 					\
		$(foreach source,$(SOURCE_DIRECTORIES),$(wildcard $(source)/*.cpp))				\
		$(foreach include,$(INCLUDE_SEARCH_PATHS),-I$(include))							\
		$(foreach library_paths,$(LIBRARY_SEARCH_PATHS),-L$(library_paths))				\
			$(foreach lib,$(LIBRARIES),-l$(lib))										\
		$(foreach a_paths,$(A_SEARCH_PATHS),-L$(a_paths))								\
			$(foreach a,$(A),-l$(a))													\
		$(foreach framework_paths,$(FRAMEWORK_SEARCH_PATHS),-F$(framework_paths))		\
			$(foreach framework,$(FRAMEWORKS),-framework $(framework))					\
		-Wall -Ofast -std=$(STD)
	rm -f $(RELEASE_DIRECTORY)/spaar.zip 
	zip -vj $(RELEASE_DIRECTORY)/spaar.zip $(foreach source,$(SOURCE_DIRECTORIES),$(wildcard $(source)/*.hpp)) -X
test_dylib:
	g++ -o $(OUTPUT_DIRECTORY)/$(VERSION)/$(NAME)																		\
		$(wildcard $(MAIN_TEST_DIRECTORY)/*.cpp)																		\
		$(foreach include,$(INCLUDE_SEARCH_PATHS),-I$(include))															\
		$(foreach library_paths,$(LIBRARY_SEARCH_PATHS),-L$(library_paths))												\
			$(foreach lib,$(LIBRARIES),-l$(lib))																		\
		-Lrelease -lSpaar																							\
		$(foreach a_paths,$(A_SEARCH_PATHS),-L$(a_paths))																\
			$(foreach a,$(A),-l$(a))																					\
		$(foreach framework_paths,$(FRAMEWORK_SEARCH_PATHS),-F$(framework_paths))										\
			$(foreach framework,$(FRAMEWORKS),-framework $(framework))													\
		-Wl,-rpath,/Library/Binaries/ -Wall $(DEBUG) -Og -std=$(STD)
	make run