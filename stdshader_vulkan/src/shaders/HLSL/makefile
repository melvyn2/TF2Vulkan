SAMPLER_OFFSET:=0 #100
TEXTURE_OFFSET:=0 #200

# DirectXShaderCompiler
# SHADER_COMPILER:=dxc_git.exe
# COMPILER_ARGS:=-spirv -E main -WX -fspv-debug=line -fspv-reflect -No -fvk-s-shift $(SAMPLER_OFFSET) 0 -fvk-t-shift $(TEXTURE_OFFSET) 0
# COMPILER_ARGS_VERT:=-T vs_6_0 -fvk-invert-y
# COMPILER_ARGS_FRAG:=-T ps_6_0
# COMPILER_ARGS_TARGET:=-Fo

# glslc
# SHADER_COMPILER:=glslc.exe
# COMPILER_ARGS:= -fhlsl-functionality1 -fsampler-binding-base $(SAMPLER_OFFSET) -ftexture-binding-base $(TEXTURE_OFFSET -g -DINVERT_Y
# COMPILER_ARGS_VERT:= -fshader-stage=vert
# COMPILER_ARGS_FRAG:= -fshader-stage=frag
# COMPILER_ARGS_TARGET:= -o

# glslangValidator
SHADER_COMPILER:=glslangValidator
COMPILER_ARGS:= -fhlsl_functionality1 --shift-sampler-binding $(SAMPLER_OFFSET) --shift-texture-binding $(TEXTURE_OFFSET) --invert-y -V -e main
COMPILER_ARGS_VERT:=
COMPILER_ARGS_FRAG:=
COMPILER_ARGS_TARGET:= -o

SPIRV_VAL:=spirv-val
SPIRV_DIS:=spirv-dis

SOURCE_DIR:=
TARGET_DIR:=../Generated/

SOURCE_FILES:=$(wildcard *.hlsl)
INCLUDE_FILES:=$(wildcard *.hlsli) ../../../include/stdshader_vulkan/ShaderDataShared.h
OUTPUT_FILES_HEADER:=$(patsubst $(SOURCE_DIR)%.hlsl, $(TARGET_DIR)%.h, $(SOURCE_FILES))
OUTPUT_FILES_SPIRV:=$(patsubst $(SOURCE_DIR)%.hlsl, $(TARGET_DIR)%.spirv, $(SOURCE_FILES))
OUTPUT_FILES_SPIRV_DIS:=$(patsubst $(SOURCE_DIR)%.hlsl, $(TARGET_DIR)%.spirv_dis, $(SOURCE_FILES))

marker.tlog : $(OUTPUT_FILES_SPIRV) $(OUTPUT_FILES_SPIRV_DIS) $(OUTPUT_FILES_HEADER)
	$(SHADER_COMPILER) --version
	touch marker.tlog

clean :
	rm -rf $(TARGET_DIR)*

$(TARGET_DIR)%.vert.spirv : %.vert.hlsl $(INCLUDE_FILES) makefile
	$(SHADER_COMPILER) $(COMPILER_ARGS) $(COMPILER_ARGS_VERT) $< $(COMPILER_ARGS_TARGET) $@
$(TARGET_DIR)%.frag.spirv : %.frag.hlsl $(INCLUDE_FILES) makefile
	$(SHADER_COMPILER) $(COMPILER_ARGS) $(COMPILER_ARGS_FRAG) $< $(COMPILER_ARGS_TARGET) $@

$(TARGET_DIR)%.h : $(TARGET_DIR)%.spirv $(TARGET_DIR)%.spirv_val
	cd $(TARGET_DIR) && xxd -i -u $(notdir $<) $(notdir $@)

$(TARGET_DIR)%.spirv_dis : $(TARGET_DIR)%.spirv
	$(SPIRV_DIS) -o $@ $<

$(TARGET_DIR)%.spirv_val : $(TARGET_DIR)%.spirv
	$(SPIRV_VAL) $< && touch $@
