import glob
import os
import re
import numpy as np

def parse_positions(filepath):
    """Extracts (x, y, z) coordinates from an output file."""
    positions = []
    with open(filepath, 'r') as f:
        reading = False
        for line in f:
            if "--- Final Particle Positions ---" in line:
                reading = True
                continue
            if reading and line.strip():
                parts = line.split()
                if len(parts) == 4:  # format: index x y z
                    positions.append([float(parts[1]), float(parts[2]), float(parts[3])])
    return np.array(positions)

def run_batch_verification(directory="."):
    # Find all parallel output files in directory
    # Adjust file extension or naming pattern if your files differ
    parallel_files = glob.glob(os.path.join(directory, "parallel_*.txt"))

    if not parallel_files:
        print("No parallel output files found matching pattern 'parallel_*.txt'!")
        return

    print(f"{'PARALLEL FILE':<35} | {'SERIAL MATCH':<30} | {'MAX ERROR':<12} | {'STATUS'}")
    print("-" * 95)

    passed_count = 0
    failed_count = 0
    missing_count = 0

    for p_file in sorted(parallel_files):
        filename = os.path.basename(p_file)

        # Extract particle size (N) from filename (e.g. 'parallel_ref_1000.txt' or 'parallel_omp_t8_N1000.txt')
        match = re.search(r'(\d+)', filename)
        if not match:
            continue

        n_val = match.group(1)

        # Match to corresponding serial file (e.g., 'serial_ref_1000.txt' or 'serial_1000.txt')
        # Tries common naming patterns automatically
        serial_candidates = [
            os.path.join(directory, f"serial_ref_{n_val}.txt"),
            os.path.join(directory, f"serial_{n_val}.txt"),
            os.path.join(directory, f"serial_N{n_val}.txt"),
        ]

        s_file = next((s for s in serial_candidates if os.path.exists(s)), None)

        if not s_file:
            print(f"{filename:<35} | {'No serial baseline found':<30} | {'N/A':<12} | SKIP")
            missing_count += 1
            continue

        # Parse coordinates and compare
        p_pos = parse_positions(p_file)
        s_pos = parse_positions(s_file)

        if p_pos.shape != s_pos.shape or p_pos.size == 0:
            print(f"{filename:<35} | {os.path.basename(s_file):<30} | {'Shape Error':<12} | FAIL")
            failed_count += 1
            continue

        max_diff = np.max(np.abs(p_pos - s_pos))

        # Absolute tolerance threshold (1e-5 handles floating-point precision in output)
        if max_diff < 1e-5:
            status = "PASSED"
            passed_count += 1
        else:
            status = "FAILED"
            failed_count += 1

        print(f"{filename:<35} | {os.path.basename(s_file):<30} | {max_diff:<12.2e} | {status}")

    print("-" * 95)
    print(f"Summary: {passed_count} Passed | {failed_count} Failed | {missing_count} Skipped")

if __name__ == "__main__":
    run_batch_verification(".")
