#!/usr/bin/env python3
"""
Test array functionality and error cases for IFCC compiler
"""

import subprocess
import os
import sys

test_dir = "tests/cases"

# Tests that should compile and run successfully
valid_tests = [
    "59_array_simple_init",
    "60_array_element_access",
    "61_array_element_modification",
    "62_array_element_modification_complex",
    "63_array_dynamic_index",
    "64_array_dynamic_index_computed",
    "65_array_char",
    "66_array_double",
    "67_array_implicit_size",
    "68_array_single_element",
    "69_array_loop_sum",
    "70_array_loop_product",
    "71_array_last_element",
    "72_array_zero_index",
    "73_array_function_param",
    "74_array_function_param_access",
    "75_array_mixed_types",
    "76_array_nested_expr",
    "77_array_index_expr",
    "78_array_if_condition",
    "79_array_modify_in_loop",
    "80_array_accumulate",
    "81_array_function_modify",
    "82_array_complex_init",
    "83_array_zero_initialization",
    "84_array_negative_values",
    "85_array_bitwise_ops",
]

# Tests that should be REJECTED by the compiler
error_tests = [
    "86_error_missing_semicolon",
    "87_error_undeclared_variable",
    "88_error_redeclaration",
    "89_error_undefined_function",
    "90_error_missing_argument",
    "91_error_too_many_arguments",
    "92_error_invalid_expression",
    "93_error_missing_semicolon_multi",
    "94_error_missing_paren",
    "95_error_return_array",
    "96_error_missing_array_index",
    "97_error_array_partial_init",
    "98_error_zero_size_array",
    "99_error_empty_assignment",
    "100_error_while_missing_paren",
    "101_error_missing_type",
    "102_error_no_semicolon_before_return",
    "103_error_missing_return_type",
    "104_error_trailing_comma",
    "105_error_array_out_of_bounds",
    "106_error_decl_in_if",
    "107_error_missing_return_value",
    "108_error_non_constant_array_size",
    "109_error_index_non_array",
    "110_error_unclosed_paren",
    "111_error_array_literal_non_array",
    "112_error_missing_else_path",
    "113_error_missing_function_call_parens",
    "114_error_multiple_empty_statements",
    "115_error_pointer_type_mismatch",
    "116_error_reassign_array",
    "117_error_switch_statement",
    "118_error_do_while",
    "119_error_for_loop",
    "120_error_void_function_call",
    "121_error_uninitialized_pointer",
    "122_error_missing_operator",
    "123_error_param_redeclaration",
]

def test_compiler(test_name, should_succeed=True):
    """Test if a file compiles or fails as expected"""
    test_file = f"{test_dir}/{test_name}.c"
    
    if not os.path.exists(test_file):
        return None, f"File not found: {test_file}"
    
    try:
        result = subprocess.run(
            ["./build/ifcc", test_file],
            capture_output=True,
            text=True,
            timeout=5
        )
        
        # If compilation succeeded (exit code 0)
        if result.returncode == 0:
            if should_succeed:
                return True, "✓ Compiled successfully (as expected)"
            else:
                return False, "✗ Compiled but should have failed!"
        else:
            # Compilation failed
            if not should_succeed:
                return True, "✓ Rejected (as expected)"
            else:
                error_msg = result.stderr[:100] if result.stderr else result.stdout[:100]
                return False, f"✗ Compilation failed: {error_msg}"
                
    except subprocess.TimeoutExpired:
        return False, "✗ Timeout"
    except Exception as e:
        return False, f"✗ Error: {str(e)}"

def main():
    print("=" * 70)
    print("IFCC COMPREHENSIVE TEST SUITE - ARRAYS & ERROR CHECKING")
    print("=" * 70)
    
    # Check if compiler exists
    if not os.path.exists("./build/ifcc"):
        print("ERROR: Compiler not found. Run 'make' first.")
        sys.exit(1)
    
    print("\n[VALID TESTS] Tests that should compile and run successfully:\n")
    valid_passed = 0
    valid_failed = 0
    
    for test in sorted(valid_tests):
        success, msg = test_compiler(test, should_succeed=True)
        if success is None:
            print(f"  {test}: SKIP - {msg}")
        elif success:
            print(f"  {test}: {msg}")
            valid_passed += 1
        else:
            print(f"  {test}: {msg}")
            valid_failed += 1
    
    print(f"\nValid tests: {valid_passed} passed, {valid_failed} failed")
    
    print("\n" + "=" * 70)
    print("[ERROR TESTS] Tests that should be REJECTED by compiler:\n")
    error_passed = 0
    error_failed = 0
    
    for test in sorted(error_tests):
        success, msg = test_compiler(test, should_succeed=False)
        if success is None:
            print(f"  {test}: SKIP - {msg}")
        elif success:
            print(f"  {test}: {msg}")
            error_passed += 1
        else:
            print(f"  {test}: {msg}")
            error_failed += 1
    
    print(f"\nError tests: {error_passed} passed, {error_failed} failed")
    
    print("\n" + "=" * 70)
    print("SUMMARY")
    print("=" * 70)
    total_passed = valid_passed + error_passed
    total_failed = valid_failed + error_failed
    print(f"Total: {total_passed} passed, {total_failed} failed")
    print(f"Valid tests: {valid_passed}/{len(valid_tests)}")
    print(f"Error tests: {error_passed}/{len(error_tests)}")
    
    if total_failed == 0:
        print("\n✓ ALL TESTS PASSED!")
        return 0
    else:
        print(f"\n✗ {total_failed} test(s) failed")
        return 1

if __name__ == "__main__":
    sys.exit(main())
