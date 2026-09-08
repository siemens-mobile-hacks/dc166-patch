find_program(PERL_EXECUTABLE perl REQUIRED)

add_test(
  NAME linker.demo-limits
  COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/check-linker-demo-limits"
          "${DC166_RUNTIME_WORK}/extracted/SourceFiles/bin/a166.exe"
          "${CMAKE_BINARY_DIR}/work/l166-patched.exe"
          "${CMAKE_BINARY_DIR}/linker-tests"
)
set_tests_properties(linker.demo-limits PROPERTIES
  LABELS "linker;wine;demo-limits"
  RUN_SERIAL TRUE
  TIMEOUT 180
)

add_test(
  NAME runtime.archive-audit
  COMMAND "${PERL_EXECUTABLE}"
          "${CMAKE_CURRENT_SOURCE_DIR}/check-archives.pl"
          "${PROJECT_SOURCE_DIR}"
)
set_tests_properties(runtime.archive-audit PROPERTIES
  LABELS "runtime;static"
  TIMEOUT 15
)

add_test(
  NAME runtime.libc-parity-audit
  COMMAND "${PERL_EXECUTABLE}"
          "${PROJECT_SOURCE_DIR}/runtime/audit_libc.pl"
          "${DC166_RUNTIME_WORK}/extracted/SourceFiles/lib/ext/c166s.lib"
          "${PROJECT_SOURCE_DIR}/runtime/manifests/c166l.modules"
          "${PROJECT_SOURCE_DIR}/runtime/manifests/c166l-replaced.modules"
          "${PROJECT_SOURCE_DIR}/runtime/manifests/c166l-pending.modules"
)
set_tests_properties(runtime.libc-parity-audit PROPERTIES
  LABELS "runtime;static;libc;parity"
  TIMEOUT 15
)

foreach(variant IN ITEMS ext ext2)
  foreach(model IN ITEMS t m l h)
    foreach(family IN ITEMS c166 fp166 rt166)
      set(allowed_missing)
      if(model STREQUAL "t" AND family STREQUAL "c166")
        set(allowed_missing
            "${PROJECT_SOURCE_DIR}/runtime/manifests/c166t-missing-symbols.txt")
      endif()
      set(test_name "runtime.symbol-audit.${variant}.${family}${model}")
      add_test(
        NAME "${test_name}"
        COMMAND "${PERL_EXECUTABLE}"
                "${PROJECT_SOURCE_DIR}/runtime/audit_libc_symbols.pl"
                "${DC166_RUNTIME_WORK}/extracted/SourceFiles/lib/${variant}/${family}s.lib"
                "${PROJECT_SOURCE_DIR}/lib/${variant}/${family}${model}.lib"
                ${allowed_missing}
      )
      set_tests_properties("${test_name}" PROPERTIES
        LABELS "runtime;static;symbols;${variant};${model};${family}"
        TIMEOUT 15
      )
    endforeach()
  endforeach()
endforeach()

foreach(variant IN ITEMS ext ext2)
  set(test_name "runtime.symbol-audit.${variant}.fp166s")
  add_test(
    NAME "${test_name}"
    COMMAND "${PERL_EXECUTABLE}"
            "${PROJECT_SOURCE_DIR}/runtime/audit_libc_symbols.pl"
            "${DC166_RUNTIME_WORK}/extracted/SourceFiles/lib/${variant}/fp166s.lib"
            "${PROJECT_SOURCE_DIR}/lib/${variant}/fp166s.lib"
  )
  set_tests_properties("${test_name}" PROPERTIES
    LABELS "runtime;static;symbols;${variant};s;fp166"
    TIMEOUT 15
  )
endforeach()

function(add_runtime_iss_test variant model case_name)
  set(optimization "O0")
  set(test_suffix "")
  if(ARGC GREATER 3)
    set(optimization "${ARGV3}")
    set(test_suffix ".${optimization}")
  endif()
  set(test_name "runtime.iss.${variant}.${model}.${case_name}${test_suffix}")
  add_test(
    NAME "${test_name}"
    COMMAND "${CMAKE_COMMAND}" -E env
            "DC166_MEMORY_MODEL=${model}"
            "DC166_RUNTIME_VARIANT=${variant}"
            "DC166_RUNTIME_WORK=${DC166_RUNTIME_WORK}"
            "DC166_DEBUGGER=${DC166_RUNTIME_WORK}/extracted/SourceFiles/bin/xfw166-patched.exe"
            "DC166_DEBUG_INSTRUMENT_MODULE=disim166-patched"
            "DC166_COMPILER_OPTIMIZATION=${optimization}"
            "${PROJECT_SOURCE_DIR}/tools/run-library-test"
            "${CMAKE_CURRENT_SOURCE_DIR}/cases/${case_name}"
            "${case_name}-${model}-${variant}"
  )
  set_tests_properties("${test_name}" PROPERTIES
    LABELS "runtime;iss;${model};${variant};${case_name};${optimization}"
    RUN_SERIAL TRUE
    TIMEOUT 150
  )
endfunction()

# A case may provide host-<model>.c when one model needs different source.
set(runtime_all_model_cases
  fp-arithmetic
  fp-conversions
  fp-storage
  libc-allocation
  libc-casecmp
  libc-core
  libc-explicit-near-allocation
  libc-fss
  libc-lifecycle
  libc-locale-multibyte
  libc-printf
  libc-printf-streams
  libc-scanf
  libc-scanf-streams
  libc-scanf-strings
  libc-setjmp
  libc-stdio
  libc-stdio-streams
  libc-stdlib
  libc-time
  libc-wide
  libc-wide-scanf
  libc-wide-stdio
  libc-wide-streams
  rt-arithmetic
  rt-compound
  rt-copy
  runtime-startup
)

set(runtime_segmented_only_cases
  libc-allocation-pages
  libc-explicit-far-allocation
  libc-huge-allocation
  libc-huge-boundary
  libc-qualified
  rt-packed-huge
  rt-page-boundary
)

foreach(variant IN ITEMS ext ext2)
  foreach(model IN ITEMS tiny medium large huge)
    set(model_cases ${runtime_all_model_cases})
    if(NOT model STREQUAL "tiny")
      list(APPEND model_cases ${runtime_segmented_only_cases})
    endif()
    foreach(case_name IN LISTS model_cases)
      add_runtime_iss_test("${variant}" "${model}" "${case_name}")
    endforeach()

    add_runtime_iss_test("${variant}" "${model}" "runtime-link-all")
  endforeach()
endforeach()

# Exercise public double-return ABI paths at every optimization level.
# The return case is bidirectional; the conversion case uses separate C
# translation units so the ABI boundary cannot be optimized away.
foreach(variant IN ITEMS ext ext2)
  foreach(model IN ITEMS tiny medium large huge)
    foreach(optimization IN ITEMS O0 O1 O2 O3 Os Oz)
      add_runtime_iss_test("${variant}" "${model}" "abi-double-return"
                           "${optimization}")
      add_runtime_iss_test("${variant}" "${model}"
                           "abi-double-conversion-return" "${optimization}")
      add_runtime_iss_test("${variant}" "${model}"
                           "abi-packed-aggregate-arguments" "${optimization}")
      add_runtime_iss_test("${variant}" "${model}" "fp-signed-zero"
                           "${optimization}")
      add_runtime_iss_test("${variant}" "${model}" "fp-expression-chain"
                           "${optimization}")
    endforeach()
  endforeach()
endforeach()

foreach(optimization IN ITEMS O0 O1 O2 O3 Os Oz)
  add_runtime_iss_test("ext" "small" "abi-double-conversion-return"
                       "${optimization}")
  add_runtime_iss_test("ext" "small" "abi-packed-aggregate-arguments"
                       "${optimization}")
endforeach()

foreach(variant IN ITEMS ext ext2)
  foreach(optimization IN ITEMS O0 O1 O2 O3 Os Oz)
    add_runtime_iss_test("${variant}" "small" "fp-signed-zero"
                         "${optimization}")
  endforeach()
endforeach()

# Small keeps the original c166/rt libraries and uses the patched fp166s.
set(runtime_small_oracle_cases
  libc-casecmp
  libc-explicit-near-allocation
  libc-fss
  libc-lifecycle
  libc-locale-multibyte
  libc-printf
  libc-printf-streams
  libc-scanf
  libc-scanf-streams
  libc-scanf-strings
  libc-setjmp
  libc-stdio
  libc-stdio-streams
  libc-time
  libc-wide-scanf
  libc-wide-stdio
  libc-wide-streams
  runtime-startup
)

foreach(case_name IN LISTS runtime_small_oracle_cases)
  add_runtime_iss_test("ext" "small" "${case_name}")
endforeach()
