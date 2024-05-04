# Place .c/.h files in the ./src directory
# Place assets in the ./assets directory
# Run make to compile the app into the ./output directory
# Set CONFIG_ARM7=1 to compile for the RG35XX

ifeq ($(CONFIG_ARM7), 1)
include ./config/configArm7.mk
else
include ./config/configX86.mk
endif

SRC := $(wildcard $(SRC_DIR)/*.cpp)
_OBJ = $(SRC:.cpp=.o)
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(OUTPUT_DIR)/bin/app: $(OBJ)
	@echo "Compiling App: $@"
	mkdir -p $(@D)
	@echo Copy assets to bin
	cp -r $(ASSETS_DIR) $(OUTPUT_DIR)/bin
	$(CC) -o $@ $^ $(LDFLAGS)
	
# Create the object files in a separate folder
$(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling: $<"
	@echo "OBJ: $@"
	mkdir -p $(@D)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $^ -o $@