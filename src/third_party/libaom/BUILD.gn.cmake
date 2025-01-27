# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

import("//build/buildflag_header.gni")
import("//build/config/arm.gni")
import("//build/config/android/config.gni")
import("//build/config/sanitizers/sanitizers.gni")
import("//third_party/libaom/options.gni")
import("//third_party/libaom/libaom_srcs.gni")
import("//third_party/yasm/yasm_assemble.gni")

buildflag_header("av1_buildflags") {
  header = "av1_buildflags.h"

  flags = [ "ENABLE_AV1_DECODER=$enable_av1_decoder" ]
}

if (enable_av1_decoder) {
  # Sets the architecture name for building libaom.
  if (current_cpu == "x86") {
    cpu_arch_full = "ia32"
  } else if (current_cpu == "x64") {
    if (is_msan) {
      # TODO(johannkoenig): Check if MSAN builds pass. libaom is favoring
      # intrinsics over assembly.
      cpu_arch_full = "generic"
    } else {
      cpu_arch_full = "x64"
    }
  } else if (current_cpu == "arm") {
    if (arm_use_neon) {
      cpu_arch_full = "arm-neon"
    } else if (is_android) {
      cpu_arch_full = "arm-neon-cpu-detect"
    } else {
      cpu_arch_full = "arm"
    }
  } else {
    cpu_arch_full = current_cpu
  }

  if (is_nacl) {
    platform_include_dir = "source/config/linux/generic"
  } else {
    if (is_ios && current_cpu == "arm") {
      os_category = current_os
    } else if (is_posix || is_fuchsia) {
      # Should cover linux, fuchsia, mac, and the ios simulator.
      os_category = "linux"
    } else {  # This should only match windows.
      os_category = current_os
    }
    platform_include_dir = "source/config/$os_category/$cpu_arch_full"
  }

  libaom_include_dirs = [
    "source/config",
    platform_include_dir,
    "source/libaom",
  ]

  config("libaom_config") {
    include_dirs = libaom_include_dirs

    # gn orders flags on a target before flags from configs. The default config
    # adds -Wall, and these flags have to be after -Wall -- so they need to come
    # from a config and can't be on the target directly.
    if (is_clang) {
      cflags = [
        # TODO(johannkoenig): these may no longer apply.
        # libvpx heavily relies on implicit enum casting.
        "-Wno-conversion",

        # libvpx does `if ((a == b))` in some places.
        "-Wno-parentheses-equality",

        # libvpx has many static functions in header, which trigger this warning.
        "-Wno-unused-function",
      ]

      # TODO(johannkoenig): these may no longer apply.
      # Fixes a mac / ios simulator link error for vpx_scaled_2d:
      # Undefined symbols for architecture x86_64:
      # "_vpx_scaled_2d", referenced from:
      # _vp9_scale_and_extend_frame_c in libvpx.a(vp9_frame_scale.o)
      # (maybe you meant: _vpx_scaled_2d_ssse3)
      # ld: symbol(s) not found for architecture x86_64
      if (is_mac || (is_ios && (current_cpu == "x86" || current_cpu == "x64"))) {
        cflags += [ "-fno-common" ]
      }
    } else if (!is_win) {
      cflags = [
        "-Wno-unused-function",
        "-Wno-sign-compare",
      ]
    }
  }

  # This config is applied to targets that depend on libaom.
  config("libaom_external_config") {
    include_dirs = [ "source/libaom" ]
  }

  if (current_cpu == "x86" || (current_cpu == "x64" && !is_msan)) {
    yasm_assemble("libaom_yasm") {
      sources = aom_dsp_common_asm_sse2
      sources += aom_dsp_common_asm_ssse3
      sources += aom_ports_asm_x86
      if (current_cpu == "x64") {
      }

      defines = [ "CHROMIUM" ]
      if (is_android) {
        # TODO(johannkoenig): this was for vp8 assembly. May no longer apply.
        # On Android, define __ANDROID__ to use alternative standard library
        # functions.
        defines += [ "__ANDROID__" ]
      }
      include_dirs = libaom_include_dirs
    }
  }

  if (current_cpu == "x86" || (current_cpu == "x64" && !is_msan)) {
    # The following targets are deliberately source_set rather than
    # static_library. The :libaom target exposes these intrinsic implementations
    # via global function pointer symbols, which hides the object dependency at
    # link time. On Mac, this results in undefined references to the intrinsic
    # symbols.

    source_set("libaom_intrinsics_sse2") {
      configs -= [ "//build/config/compiler:chromium_code" ]
      configs += [ "//build/config/compiler:no_chromium_code" ]
      configs += [ ":libaom_config" ]
      if (!is_win || is_clang) {
        cflags = [ "-msse2" ]
      }
      sources = aom_av1_common_intrin_sse2
      sources += aom_dsp_common_intrin_sse2
    }

    source_set("libaom_intrinsics_ssse3") {
      configs -= [ "//build/config/compiler:chromium_code" ]
      configs += [ "//build/config/compiler:no_chromium_code" ]
      configs += [ ":libaom_config" ]
      if (!is_win || is_clang) {
        cflags = [ "-mssse3" ]
      }
      sources = aom_av1_common_intrin_ssse3
      sources += aom_dsp_common_intrin_ssse3
    }

    source_set("libaom_intrinsics_sse4_1") {
      configs -= [ "//build/config/compiler:chromium_code" ]
      configs += [ "//build/config/compiler:no_chromium_code" ]
      configs += [ ":libaom_config" ]
      if (!is_win || is_clang) {
        cflags = [ "-msse4.1" ]
      }
      sources = aom_av1_common_intrin_sse4_1
      sources += aom_dsp_common_intrin_sse4_1
    }

    source_set("libaom_intrinsics_sse4_2") {
      configs -= [ "//build/config/compiler:chromium_code" ]
      configs += [ "//build/config/compiler:no_chromium_code" ]
      configs += [ ":libaom_config" ]
      if (!is_win || is_clang) {
        cflags = [ "-msse4.2" ]
      }
    }

    source_set("libaom_intrinsics_avx") {
      configs -= [ "//build/config/compiler:chromium_code" ]
      configs += [ "//build/config/compiler:no_chromium_code" ]
      configs += [ ":libaom_config" ]
      if (is_win) {
        cflags = [ "/arch:AVX" ]
      } else {
        cflags = [ "-mavx" ]
      }
    }

    source_set("libaom_intrinsics_avx2") {
      configs -= [ "//build/config/compiler:chromium_code" ]
      configs += [ "//build/config/compiler:no_chromium_code" ]
      configs += [ ":libaom_config" ]
      if (is_win) {
        cflags = [ "/arch:AVX2" ]
      } else {
        cflags = [ "-mavx2" ]
      }
      sources = aom_av1_common_intrin_avx2
      sources += aom_dsp_common_intrin_avx2
    }
  }

  if (cpu_arch_full == "arm-neon-cpu-detect") {
    static_library("libaom_intrinsics_neon") {
      configs -= [ "//build/config/compiler:compiler_arm_fpu" ]
      configs += [ ":libaom_config" ]
      cflags = [ "-mfpu=neon" ]
      sources = libaom_srcs_arm_neon_cpu_detect_neon
    }
  }

  if (current_cpu == "arm") {
    if (cpu_arch_full == "arm-neon") {
      arm_assembly_sources = libaom_srcs_arm_neon_assembly
    } else if (cpu_arch_full == "arm-neon-cpu-detect") {
      arm_assembly_sources = libaom_srcs_arm_neon_cpu_detect_assembly
    } else {
      arm_assembly_sources = libaom_srcs_arm_assembly
    }
  }

  # Converts ARM assembly files to GAS style.
  if (current_cpu == "arm" && arm_assembly_sources != []) {
    action_foreach("convert_arm_assembly") {
      script = "//third_party/libaom/run_perl.py"
      sources = arm_assembly_sources
      gen_file =
          get_label_info("//third_party/libaom/source/libaom", "root_gen_dir") +
          "/{{source_root_relative_dir}}/{{source_file_part}}.S"
      outputs = [
        gen_file,
      ]
      if (is_ios) {
        ads2gas_script =
            "//third_party/libaom/source/libaom/build/make/ads2gas_apple.pl"
      } else {
        ads2gas_script =
            "//third_party/libaom/source/libaom/build/make/ads2gas.pl"
      }
      args = [
        "-s",
        rebase_path(ads2gas_script, root_build_dir),
        "-i",
        "{{source}}",
        "-o",
        rebase_path(gen_file),
      ]
    }

    static_library("libaom_assembly_arm") {
      sources = get_target_outputs(":convert_arm_assembly")
      configs -= [ "//build/config/compiler:compiler_arm_fpu" ]
      configs += [ ":libaom_config" ]
      if (cpu_arch_full == "arm-neon" ||
          cpu_arch_full == "arm-neon-cpu-detect") {
        asmflags = [ "-mfpu=neon" ]

        # allow asm files to include generated sources which match the source
        # tree layout, e.g., aom_dsp/arm/...
        include_dirs = [ get_label_info("//third_party/libaom/source/libaom",
                                        "target_gen_dir") ]
      }
      deps = [
        ":convert_arm_assembly",
      ]
    }
  }

  static_library("libaom") {
    if (!is_debug && is_win) {
      configs -= [ "//build/config/compiler:default_optimization" ]
      configs += [ "//build/config/compiler:optimize_max" ]
    }

    if (is_nacl) {
      sources = libaom_srcs_generic
    } else if (current_cpu == "x86") {
      sources = libaom_srcs_x86
    } else if (current_cpu == "x64") {
      if (is_msan) {
        sources = libaom_srcs_generic
      } else {
        sources = aom_av1_common_sources
        sources += aom_av1_decoder_sources
        sources += aom_dsp_common_sources
        sources += aom_dsp_decoder_sources
        sources += aom_mem_sources
        sources += aom_rtcd_sources
        sources += aom_scale_sources
        sources += aom_sources
        sources += aom_util_sources
      }
    } else if (current_cpu == "mipsel" || current_cpu == "mips64el") {
      sources = libaom_srcs_mips
    } else if (current_cpu == "arm") {
      if (arm_use_neon) {
        sources = libaom_srcs_arm_neon
      } else if (is_android) {
        sources = libaom_srcs_arm_neon_cpu_detect
      } else {
        sources = libaom_srcs_arm
      }
    } else if (current_cpu == "arm64") {
      sources = libaom_srcs_arm64
    }

    configs -= [ "//build/config/compiler:chromium_code" ]
    configs += [ "//build/config/compiler:no_chromium_code" ]
    configs += [ ":libaom_config" ]
    deps = []
    if (current_cpu == "x86" || (current_cpu == "x64" && !is_msan)) {
      deps += [
        ":libaom_intrinsics_avx",
        ":libaom_intrinsics_avx2",
        ":libaom_intrinsics_sse2",
        ":libaom_intrinsics_sse4_1",
        ":libaom_intrinsics_sse4_2",
        ":libaom_intrinsics_ssse3",
        ":libaom_yasm",
      ]
    }
    if (cpu_arch_full == "arm-neon-cpu-detect") {
      deps += [ ":libaom_intrinsics_neon" ]
    }
    if (is_android) {
      deps += [ "//third_party/android_tools:cpu_features" ]
    }
    if (current_cpu == "arm" && arm_assembly_sources != []) {
      deps += [ ":libaom_assembly_arm" ]
    }

    public_configs = [ ":libaom_external_config" ]
  }
}
