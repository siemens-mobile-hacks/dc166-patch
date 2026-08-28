AS      ?= as
LD      ?= ld
OBJCOPY ?= objcopy
PERL    ?= perl

WORK ?= $(CURDIR)/build/work
RUNTIME_WORK ?= $(CURDIR)/build/runtime

ORIGINAL_INSTALLER := inputs/Tasking-C166-8.6r1-original.exe
C166_ORIGINAL := $(RUNTIME_WORK)/extracted/SourceFiles/bin/c166.exe
L166_ORIGINAL := $(RUNTIME_WORK)/extracted/SourceFiles/bin/l166.exe
XFW166_ORIGINAL := $(RUNTIME_WORK)/extracted/SourceFiles/bin/xfw166.exe

OBJECT := $(WORK)/fp_emitter.o
ELF := $(WORK)/fp_emitter.elf
PAYLOAD := $(WORK)/fp_emitter.bin
C166_BASE := $(WORK)/c166-base.exe
C166_PATCHED := $(WORK)/c166-patched.exe
L166_PATCHED := $(WORK)/l166-patched.exe
XFW166_PATCHED := $(WORK)/xfw166-patched.exe

FINAL_C166_PATCH := patches/final/c166.vkp
FINAL_L166_PATCH := patches/final/l166.vkp
FINAL_XFW166_PATCH := patches/final/xfw166.vkp
FINAL_INSTALLER := dist/Tasking-C166-8.6r1-patched.exe

RUNTIME_EXTRACTED := $(RUNTIME_WORK)/extracted/.stamp
RUNTIME_VARIANTS := ext ext2
MEMORY_MODELS := t m l h
RUNTIME_LIBS := $(foreach variant,$(RUNTIME_VARIANTS),$(foreach model,$(MEMORY_MODELS),\
	lib/$(variant)/c166$(model).lib lib/$(variant)/fp166$(model).lib \
	lib/$(variant)/rt166$(model).lib))
C166_GENERATED_MEMBERS := $(file <runtime/manifests/c166l-generated.modules)
C166_TINY_EXCLUDED_MEMBERS := $(file <runtime/manifests/c166t-excluded.modules)
C166_GENERATED_MEMBERS_t := $(filter-out $(C166_TINY_EXCLUDED_MEMBERS),$(C166_GENERATED_MEMBERS))
C166_GENERATED_MEMBERS_m := $(C166_GENERATED_MEMBERS)
C166_GENERATED_MEMBERS_l := $(C166_GENERATED_MEMBERS)
C166_GENERATED_MEMBERS_h := $(C166_GENERATED_MEMBERS)
C166_SOURCES := $(wildcard runtime/c166l/*.c runtime/c166l/*.asm)
C166_HEADERS := $(wildcard runtime/c166l/*.h)

.PHONY: all rebuild installer libraries verify clean
.SECONDARY:

all: $(FINAL_C166_PATCH) $(FINAL_L166_PATCH) $(FINAL_XFW166_PATCH) \
	$(RUNTIME_LIBS) $(FINAL_INSTALLER)

rebuild:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) verify

installer: $(FINAL_INSTALLER)

libraries: $(RUNTIME_LIBS)

$(WORK) patches/final dist lib/ext lib/ext2:
	mkdir -p $@

$(RUNTIME_EXTRACTED): $(ORIGINAL_INSTALLER) tools/extract-original
	tools/extract-original $(ORIGINAL_INSTALLER) $(RUNTIME_WORK)/extracted
	touch $@

$(C166_ORIGINAL) $(L166_ORIGINAL) $(XFW166_ORIGINAL): $(RUNTIME_EXTRACTED)
	@test -f $@

define MODEL_RULES
C166_GENERATED_OBJECTS_$(1) := $$(addprefix $$(RUNTIME_WORK)/c166$(1)-generated/,$$(C166_GENERATED_MEMBERS_$(1)))

$$(RUNTIME_WORK)/c166$(1)-generated/.stamp: runtime/build_c166l_objects.sh runtime/render_model_asm.pl \
		$$(C166_SOURCES) $$(C166_HEADERS) runtime/manifests/c166l-generated.modules \
		runtime/manifests/c166t-excluded.modules \
		$$(C166_PATCHED) $$(RUNTIME_EXTRACTED)
	runtime/build_c166l_objects.sh $(1) $$(C166_PATCHED) \
		$$(RUNTIME_WORK)/extracted/SourceFiles/bin/a166.exe \
		$$(RUNTIME_WORK)/extracted/SourceFiles/etc/reg.def \
		$$(RUNTIME_WORK)/extracted/SourceFiles/include runtime/c166l \
		runtime/manifests/c166l-generated.modules \
		$$(RUNTIME_WORK)/c166$(1)-generated $$@

define VARIANT_RULES_$(1)
lib/$$(1)/c166$(1).lib: runtime/build_lib.pl runtime/build_near_lib.sh runtime/append_lib.pl \
		runtime/manifests/c166l.modules $$(RUNTIME_WORK)/c166$(1)-generated/.stamp $$(RUNTIME_EXTRACTED) | lib/$$(1)
	@if [ "$(1)" = t ] || [ "$(1)" = m ]; then \
		runtime/build_near_lib.sh $$(RUNTIME_WORK)/extracted/SourceFiles/lib/$$(1)/c166s.lib \
			runtime/manifests/c166l.modules $(1) \
			$$(RUNTIME_WORK)/extracted/SourceFiles/bin/a166.exe \
			$$(RUNTIME_WORK)/extracted/SourceFiles/etc/reg.def \
			$$(RUNTIME_WORK)/$$(1)-c166$(1)-imported \
			$$(RUNTIME_WORK)/$$(1)-c166$(1)-imported.lib; \
	else \
		$$(PERL) runtime/build_lib.pl $$(RUNTIME_WORK)/extracted/SourceFiles/lib/$$(1)/c166s.lib \
			runtime/manifests/c166l.modules $(1) $$(RUNTIME_WORK)/$$(1)-c166$(1)-imported.lib; \
	fi
	@$$(PERL) runtime/append_lib.pl $$(RUNTIME_WORK)/$$(1)-c166$(1)-imported.lib $$$$@ $(1) \
		$$(C166_GENERATED_OBJECTS_$(1))

lib/$$(1)/fp166$(1).lib: runtime/build_lib.pl runtime/build_near_lib.sh runtime/manifests/fp166l.modules $$(RUNTIME_EXTRACTED) | lib/$$(1)
	@if [ "$(1)" = t ] || [ "$(1)" = m ]; then \
		runtime/build_near_lib.sh $$(RUNTIME_WORK)/extracted/SourceFiles/lib/$$(1)/fp166s.lib \
			runtime/manifests/fp166l.modules $(1) \
			$$(RUNTIME_WORK)/extracted/SourceFiles/bin/a166.exe \
			$$(RUNTIME_WORK)/extracted/SourceFiles/etc/reg.def \
			$$(RUNTIME_WORK)/$$(1)-fp166$(1) $$$$@; \
	else \
		$$(PERL) runtime/build_lib.pl $$(RUNTIME_WORK)/extracted/SourceFiles/lib/$$(1)/fp166s.lib \
			runtime/manifests/fp166l.modules $(1) $$$$@; \
	fi

$$(RUNTIME_WORK)/rt166$(1)-startup/$$(1)/.stamp: runtime/build_rt166l_startup.sh $$(RUNTIME_EXTRACTED)
	@source=cstartx.asm; if [ "$$(1)" = ext2 ]; then source=cstartx2.asm; fi; \
	runtime/build_rt166l_startup.sh $(1) \
		$$(RUNTIME_WORK)/extracted/SourceFiles/bin/m166.exe \
		$$(RUNTIME_WORK)/extracted/SourceFiles/bin/a166.exe \
		$$(RUNTIME_WORK)/extracted/SourceFiles/lib/src/$$$$$$$$source \
		$$(RUNTIME_WORK)/extracted/SourceFiles/include \
		$$(RUNTIME_WORK)/rt166$(1)-startup/$$(1) \
		$$(RUNTIME_WORK)/rt166$(1)-startup/$$(1)/cstart.obj $$$$@

lib/$$(1)/rt166$(1).lib: runtime/build_lib.pl runtime/build_near_lib.sh runtime/append_lib.pl \
		runtime/manifests/rt166l.modules $$(RUNTIME_WORK)/rt166$(1)-startup/$$(1)/.stamp $$(RUNTIME_EXTRACTED) | lib/$$(1)
	@if [ "$(1)" = t ] || [ "$(1)" = m ]; then \
		runtime/build_near_lib.sh $$(RUNTIME_WORK)/extracted/SourceFiles/lib/$$(1)/rt166s.lib \
			runtime/manifests/rt166l.modules $(1) \
			$$(RUNTIME_WORK)/extracted/SourceFiles/bin/a166.exe \
			$$(RUNTIME_WORK)/extracted/SourceFiles/etc/reg.def \
			$$(RUNTIME_WORK)/$$(1)-rt166$(1)-imported \
			$$(RUNTIME_WORK)/$$(1)-rt166$(1)-imported.lib; \
	else \
		$$(PERL) runtime/build_lib.pl $$(RUNTIME_WORK)/extracted/SourceFiles/lib/$$(1)/rt166s.lib \
			runtime/manifests/rt166l.modules $(1) $$(RUNTIME_WORK)/$$(1)-rt166$(1)-imported.lib; \
	fi
	$$(PERL) runtime/append_lib.pl $$(RUNTIME_WORK)/$$(1)-rt166$(1)-imported.lib $$$$@ $(1) \
		$$(RUNTIME_WORK)/rt166$(1)-startup/$$(1)/cstart.obj
endef
$$(foreach variant,$$(RUNTIME_VARIANTS),$$(eval $$(call VARIANT_RULES_$(1),$$(variant))))
endef
$(foreach model,$(MEMORY_MODELS),$(eval $(call MODEL_RULES,$(model))))

$(OBJECT): src/fp_emitter.S | $(WORK)
	$(AS) --32 -o $@ $<

$(ELF): $(OBJECT) src/fp_emitter.ld
	$(LD) -m elf_i386 -T src/fp_emitter.ld -o $@ $(OBJECT)

$(PAYLOAD): $(ELF)
	$(OBJCOPY) -O binary -j .text $< $@

$(C166_BASE): $(C166_ORIGINAL) src/patches/c166_base.vkp src/apply_vkp.pl | $(WORK)
	$(PERL) src/apply_vkp.pl src/patches/c166_base.vkp $(C166_ORIGINAL) $@

$(C166_PATCHED): $(C166_BASE) $(PAYLOAD) src/patch_c166_fp.pl
	$(PERL) src/patch_c166_fp.pl $(C166_BASE) $@ $(PAYLOAD)

$(L166_PATCHED): $(L166_ORIGINAL) src/patches/l166_base.vkp src/apply_vkp.pl | $(WORK)
	$(PERL) src/apply_vkp.pl src/patches/l166_base.vkp $(L166_ORIGINAL) $@

$(XFW166_PATCHED): $(XFW166_ORIGINAL) src/patches/xfw166_base.vkp src/apply_vkp.pl | $(WORK)
	$(PERL) src/apply_vkp.pl src/patches/xfw166_base.vkp $(XFW166_ORIGINAL) $@

$(FINAL_C166_PATCH): $(C166_ORIGINAL) $(C166_PATCHED) src/make_vkp.pl | patches/final
	$(PERL) src/make_vkp.pl $(C166_ORIGINAL) $(C166_PATCHED) $@

$(FINAL_L166_PATCH): $(L166_ORIGINAL) $(L166_PATCHED) src/make_vkp.pl | patches/final
	$(PERL) src/make_vkp.pl $(L166_ORIGINAL) $(L166_PATCHED) $@

$(FINAL_XFW166_PATCH): $(XFW166_ORIGINAL) $(XFW166_PATCHED) src/make_vkp.pl | patches/final
	$(PERL) src/make_vkp.pl $(XFW166_ORIGINAL) $(XFW166_PATCHED) $@

$(FINAL_INSTALLER): $(ORIGINAL_INSTALLER) $(C166_PATCHED) $(L166_PATCHED) $(XFW166_PATCHED) \
		$(RUNTIME_LIBS) installer/build_sfx.sh installer/setup.cmd \
		installer/sfx-config.txt installer/vendor/7zSD.sfx | dist
	TASKING_C166_INSTALLER='$(abspath $(ORIGINAL_INSTALLER))' \
	C166_PATCHED='$(C166_PATCHED)' L166_PATCHED='$(L166_PATCHED)' \
	XFW166_PATCHED='$(XFW166_PATCHED)' \
	RUNTIME_LIB_ROOT='$(abspath lib)' \
	./installer/build_sfx.sh

verify: all
	$(PERL) src/apply_vkp.pl $(FINAL_C166_PATCH) $(C166_ORIGINAL) $(WORK)/c166-from-final-vkp.exe
	cmp $(C166_PATCHED) $(WORK)/c166-from-final-vkp.exe
	$(PERL) src/apply_vkp.pl $(FINAL_L166_PATCH) $(L166_ORIGINAL) $(WORK)/l166-from-final-vkp.exe
	cmp $(L166_PATCHED) $(WORK)/l166-from-final-vkp.exe
	$(PERL) src/apply_vkp.pl $(FINAL_XFW166_PATCH) $(XFW166_ORIGINAL) $(WORK)/xfw166-from-final-vkp.exe
	cmp $(XFW166_PATCHED) $(WORK)/xfw166-from-final-vkp.exe
	$(PERL) tests/check-archives.pl $(CURDIR)
	$(PERL) runtime/audit_libc.pl \
		$(RUNTIME_WORK)/extracted/SourceFiles/lib/ext/c166s.lib \
		runtime/manifests/c166l.modules \
		runtime/manifests/c166l-replaced.modules \
		runtime/manifests/c166l-pending.modules
	rm -rf -- $(WORK)/sfx-verify
	7z x -y $(FINAL_INSTALLER) -o$(WORK)/sfx-verify >/dev/null
	cmp installer/setup.cmd $(WORK)/sfx-verify/setup.cmd
	cmp $(ORIGINAL_INSTALLER) $(WORK)/sfx-verify/original-installer.exe
	cmp $(C166_PATCHED) $(WORK)/sfx-verify/payload/c166-patched.exe
	cmp $(L166_PATCHED) $(WORK)/sfx-verify/payload/l166-patched.exe
	cmp $(XFW166_PATCHED) $(WORK)/sfx-verify/payload/xfw166-patched.exe
	@for variant in $(RUNTIME_VARIANTS); do \
		for model in $(MEMORY_MODELS); do for family in c166 fp166 rt166; do \
			library=$$family$$model.lib; \
			cmp lib/$$variant/$$library \
				$(WORK)/sfx-verify/payload/lib/$$variant/$$library || exit 1; \
		done; done; \
	done

clean:
	rm -rf -- $(WORK)
	rm -rf -- $(RUNTIME_WORK)
	rm -rf -- lib
	rm -f -- $(FINAL_C166_PATCH) $(FINAL_L166_PATCH) $(FINAL_XFW166_PATCH) $(FINAL_INSTALLER)
