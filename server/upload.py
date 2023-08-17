import os
import sys
def main():
    path_info = os.environ.get('PATH_INFO', '')
    binary_data = sys.argv[1] if len(sys.argv) > 1 else ''
    binary_name = sys.argv[2] if len(sys.argv) > 2 else 'default.default'
    try:
        max_file_size = int(os.environ.get('MAX_FILE_SIZE', '0'))
        if len(binary_data) > max_file_size:
            sys.exit(1)
        with open(f"{binary_name}", "w") as f:
            f.write(f"{binary_data}")
        sys.exit(0)
    except Exception as e:
        sys.exit(1)
if __name__ == "__main__":
    main()