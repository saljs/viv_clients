BOARDS=https://arduino.esp8266.com/stable/package_esp8266com_index.json
FQBN=esp8266:esp8266:d1_mini_clone
LIB_VERSION=1.1.0
LIB_URL=https://github.com/saljs/VivariumMonitor.git

BUILD_CFG=arduino-cli.yaml
SRC_FILES=$(wildcard */*.ino)

all: $(SRC_FILES)

build:
	mkdir -p build

$(SRC_FILES): build
	$(eval FW_VERSION := $(basename $(notdir $@))-$(LIB_VERSION)-$(shell md5sum < "$@" | awk NF=1))
	arduino-cli compile \
		--config-file "$(BUILD_CFG)" \
		--build-property "build.extra_flags=\"-DFIRMWARE_VERSION=\"\$(FW_VERSION)\"\"" \
		--fqbn $(FQBN) \
		$(dir $@)
	cp $(dir $@)build/$(subst :,.,$(FQBN))/$(notdir $@).bin build/$(FW_VERSION).bin

install_libs:
	arduino-cli core update-index --config-file "$(BUILD_CFG)"
	arduino-cli lib install --git-url "$(LIB_URL)" --config-file "$(BUILD_CFG)"
	arduino-cli lib install \
		"DallasTemperature" "ESP Telnet" "OneWire" "StreamUtils" "WifiManager" \
		--config-file "$(BUILD_CFG)"
clean:
	rm -r build
	rm -r */build/

.PHONY: all install_lib clean $(SRC_FILES)
