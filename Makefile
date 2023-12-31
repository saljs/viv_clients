BOARDS=https://arduino.esp8266.com/stable/package_esp8266com_index.json
FQBN=esp8266:esp8266:d1_mini_clone
DEBUG_LVL=

SRC_FILES=$(wildcard */*.ino)
LIB_VERSION=$(shell grep -e '^ *version *=' VivariumMonitor/library.properties | sed 's/version=\s*//')

all: $(SRC_FILES)

debug: DEBUG_LVL=:lvl=CORE
debug: $(SRC_FILES)

build:
	mkdir -p build

$(SRC_FILES): build
	$(eval FW_VERSION := $(basename $(notdir $@))-$(LIB_VERSION)-$(shell md5sum < "$@" | awk NF=1))
	arduino-cli compile \
		--library VivariumMonitor \
		--export-binaries \
		--build-property "build.extra_flags=\"-DFIRMWARE_VERSION=\"$(FW_VERSION)\"\"" \
		--fqbn $(FQBN)$(DEBUG_LVL) \
		$(dir $@)
	cp $(dir $@)build/$(subst :,.,$(FQBN))/$(notdir $@).bin build/$(FW_VERSION).bin

install_libs:
	arduino-cli core update-index --additional-urls "$(BOARDS)"
	arduino-cli core install esp8266:esp8266 --additional-urls "$(BOARDS)"
	arduino-cli lib install \
		"DallasTemperature" "ESP Telnet" "OneWire" "StreamUtils" "WifiManager"

clean:
	rm -r build
	rm -r */build/

.PHONY: all install_lib clean $(SRC_FILES)
