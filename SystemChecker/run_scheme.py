import subprocess
import sys
import os



# --- Configuration ---
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
TEMPLATE_FILE = os.path.join(SCRIPT_DIR, "checkSystem.hpp")
OUTPUT_EXE = "scheme_test.exe"
RESULT_FILE = "result.txt"
GPP = "clang++"  # Ensure g++ is available in PATH


def parse_float_list(s):
    """Parses string like '1,2,3' -> [1.0, 2.0, 3.0]"""
    try:
        return [float(x.strip()) for x in s.split(',')]
    except ValueError:
        raise ValueError(f"Invalid number format: {s}")


def validate_and_run_scheme(user_code, a_str, h_str, init_str):
    print("[INFO] Reading template...")
    with open(TEMPLATE_FILE, 'r', encoding='utf-8') as f:
        template = f.read()

    if "<<<< USER_INITIAL_VALUES >>>>" not in template or "<<<< USER_SCHEME >>>>" not in template:
        print("[ERROR] Template markers not found: <<<< USER_INITIAL_VALUES >>>> or <<<< USER_SCHEME >>>>")
        return False

    try:
        a_values = parse_float_list(a_str)
        x_values = parse_float_list(init_str)
        h_value = float(h_str.strip())
    except ValueError as e:
        print(f"[ERROR] Invalid number format: {e}")
        return False

    if len(x_values) < 1:
        print("[ERROR] At least one initial value X[0] is required")
        return False
    if len(a_values) < 1:
        print("[ERROR] At least one parameter a[0] is required")
        return False


    x_list = ", ".join(f"{x:.15g}" for x in x_values)
    a_list = ", ".join(f"{a:.15g}" for a in a_values)

    new_init = f"""    std::vector<double> X = {{{x_list}}};
    std::vector<double> a = {{{a_list}}};
    double h = {h_value:.15g};"""

    # Split template into lines
    lines = template.splitlines()

    # Replace markers line by line
    new_lines = []
    for line in lines:
        if line.strip() == "// <<<< USER_INITIAL_VALUES >>>>":
            new_lines.append(new_init)
        elif line.strip() == "// <<<< USER_SCHEME >>>>":
            new_lines.append(user_code.strip())
        else:
            new_lines.append(line)

    # Rebuild modified code
    modified_code = "\n".join(new_lines)

    # Save temporary file for compilation
    temp_cpp = "temp_generated.cpp"
    with open(temp_cpp, 'w', encoding='utf-8') as f:
        f.write(modified_code)

    print("[INFO] Compiling...")
    try:
        result = subprocess.run(
            [GPP, "-o", OUTPUT_EXE, temp_cpp, "-lm"],
            capture_output=True,
            text=True,
            timeout=10,
            encoding='utf-8',
            errors='replace'
        )
    except FileNotFoundError:
        print("[ERROR] g++ not found. Please install MinGW-w64 and add it to PATH.")
        return False
    except UnicodeDecodeError as e:
        print(f"[ERROR] Failed to decode g++ output: {e}")
        return False

    if result.returncode != 0:
        print("[ERROR] Compilation failed:")
        print(result.stderr)
        return False

    print("[INFO] Running executable...")

    # Rebuild code to insert output after system() call
    final_lines = modified_code.splitlines()
    output_inserted = False
    for i, line in enumerate(final_lines):
        if "system(X, a, h);" in line:
            final_lines.insert(i + 1, "    std::cout << X[0] << \"\\n\" << X[1] << \"\\n\";")
            output_inserted = True
            break

    if not output_inserted:
        print("[WARNING] Could not find system() call — unable to insert output")
        return False

    modified_code = "\n".join(final_lines)

    # Save updated code
    with open(temp_cpp, 'w', encoding='utf-8') as f:
        f.write(modified_code)

    # Run program and capture stdout
    try:
        run_result = subprocess.run(
            [OUTPUT_EXE],
            capture_output=True,
            text=True,
            timeout=5,
            encoding='utf-8',
            errors='replace'
        )
    except Exception as e:
        print(f"[ERROR] Failed to run executable: {e}")
        return False

    print("[INFO] Program output:")
    print("[INFO] Program output:")
    for line in run_result.stdout.splitlines():
        try:
            print(line)
        except UnicodeEncodeError:
            print(line.encode('ascii', 'replace').decode('ascii'))

    if run_result.stderr:
        print("[WARNING] stderr output:")
        print(run_result.stderr)

    # Validate result: parse X[0], X[1] from stdout
    output_lines = run_result.stdout.strip().splitlines()
    if len(output_lines) < 2:
        print("[ERROR] Program did not output two values: X[0] and X[1]!")
        return False

    return True


if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage: python run_scheme.py <scheme_file.txt> \"a0,a1\" \"h\" \"x0,y0\"")
        print("Example:")
        print("  python run_scheme.py test.txt \"0.5,0.2\" \"0.001\" \"0.1,0.1\"")
        sys.exit(1)

    scheme_file = sys.argv[1]
    a_str = sys.argv[2]      # a[0], a[1]
    h_str = sys.argv[3]      # integration step h
    init_str = sys.argv[4]   # X[0], X[1]

    # Read scheme from file
    try:
        with open(scheme_file, 'r', encoding='utf-8') as f:
            user_scheme = f.read()
    except FileNotFoundError:
        print(f"[ERROR] Scheme file not found: {scheme_file}")
        sys.exit(1)

    success = validate_and_run_scheme(user_scheme, a_str, h_str, init_str)
    sys.exit(0 if success else 1)