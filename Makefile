BOARDS=https://arduino.esp8266.com/stable/package_esp8266com_index.json
FQBN=esp8266:esp8266:d1_mini_clone

SRC_FILES=$(wildcard */*.ino)
LIB_VERSION=$(shell grep -e '^ *version *=' VivariumMonitor/library.properties | sed 's/version=\s*//')

all: $(SRC_FILES)

build:
	mkdir -p build

$(SRC_FILES): build
	$(eval FW_VERSION := $(basename $(notdir $@))-$(LIB_VERSION)-$(shell md5sum < "$@" | awk NF=1))
ifdef DEBUG
	arduino-cli compile \
		--library VivariumMonitor \
		--export-binaries \
		--build-property "build.extra_flags=\"-DFIRMWARE_VERSION=\"debug_$(FW_VERSION)\"\"" \
		--fqbn $(FQBN):lvl=CORE \
		--optimize-for-debug \
		$(dir $@)
	cp $(dir $@)build/$(subst :,.,$(FQBN))/$(notdir $@).bin build/debug_$(FW_VERSION).bin
else
	arduino-cli compile \
		--library VivariumMonitor \
		--export-binaries \
		--build-property "build.extra_flags=\"-DFIRMWARE_VERSION=\"$(FW_VERSION)\"\"" \
		--fqbn $(FQBN) \
		$(dir $@)
	cp $(dir $@)build/$(subst :,.,$(FQBN))/$(notdir $@).bin build/$(FW_VERSION).bin
endif

install_libs:
	arduino-cli core update-index --additional-urls "$(BOARDS)"
	arduino-cli core install esp8266:esp8266 --additional-urls "$(BOARDS)"
	arduino-cli lib install \
		"DallasTemperature" "ESP Telnet" "OneWire" "StreamUtils" "WifiManager"

clean:
	rm -r build
	rm -r */build/

.PHONY: all install_lib clean $(SRC_FILES)
