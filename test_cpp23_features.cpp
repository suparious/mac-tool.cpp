// C++23 Feature Test
// Compile with: g++ -std=c++23 test_cpp23_features.cpp -o test_features.exe

#include <iostream>
#include <version>
#include <string>

int main() {
    std::cout << "C++ Standard Version: " << __cplusplus << std::endl;
    std::cout << "Compiler: " << 
#ifdef __GNUC__
        "GCC " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__
#elif defined(_MSC_VER)
        "MSVC " << _MSC_VER
#elif defined(__clang__)
        "Clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__
#else
        "Unknown"
#endif
    << std::endl << std::endl;

    std::cout << "C++23 Feature Support:" << std::endl;
    std::cout << "----------------------" << std::endl;

    // Test for various C++23 features
#ifdef __cpp_lib_print
    std::cout << "✓ std::print/println (P2093R14): SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::print/println: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_expected
    std::cout << "✓ std::expected (P0323R12): SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::expected: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_explicit_this_parameter
    std::cout << "✓ Deducing this (P0847R7): SUPPORTED" << std::endl;
#else
    std::cout << "✗ Deducing this: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_string_contains
    std::cout << "✓ std::string::contains (P1679R3): SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::string::contains: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_ranges_zip
    std::cout << "✓ std::ranges::zip (P2321R2): SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::ranges::zip: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_format
    std::cout << "✓ std::format: SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::format: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_jthread
    std::cout << "✓ std::jthread: SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::jthread: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_span
    std::cout << "✓ std::span: SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::span: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_concepts
    std::cout << "✓ Concepts: SUPPORTED" << std::endl;
#else
    std::cout << "✗ Concepts: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_chrono
    std::cout << "✓ Chrono improvements: SUPPORTED" << std::endl;
#else
    std::cout << "✗ Chrono improvements: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_designated_initializers
    std::cout << "✓ Designated initializers: SUPPORTED" << std::endl;
#else
    std::cout << "✗ Designated initializers: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_optional
    std::cout << "✓ std::optional: SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::optional: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_filesystem
    std::cout << "✓ std::filesystem: SUPPORTED" << std::endl;
#else
    std::cout << "✗ std::filesystem: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_constexpr_dynamic_alloc
    std::cout << "✓ constexpr dynamic allocation: SUPPORTED" << std::endl;
#else
    std::cout << "✗ constexpr dynamic allocation: NOT SUPPORTED" << std::endl;
#endif

#ifdef __cpp_lib_ranges_contains
    std::cout << "✓ ranges::contains: SUPPORTED" << std::endl;
#else
    std::cout << "✗ ranges::contains: NOT SUPPORTED" << std::endl;
#endif

    std::cout << std::endl;
    std::cout << "Note: Full C++23 support requires:" << std::endl;
    std::cout << "  - GCC 13+ or Clang 16+ or MSVC 2022 17.5+" << std::endl;
    std::cout << "  - Some features may still be experimental" << std::endl;
    
    return 0;
}
