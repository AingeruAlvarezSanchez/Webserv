import os
import sys
import base64
def main():
    path_info = os.environ.get('PATH_INFO', '')
    encoded_binary_data = sys.argv[1] if len(sys.argv) > 1 else ''
    binary_name = sys.argv[2] if len(sys.argv) > 2 else 'default.default'
    try:
        max_file_size = int(os.environ.get('MAX_FILE_SIZE', '0'))
        if len(encoded_binary_data) > max_file_size:
            sys.exit(1)
        binary_data = base64.b64decode(encoded_binary_data)
        file_path = os.path.join(path_info, binary_name)
        with open(file_path, "wb") as f:
            f.write(binary_data)
        sys.exit(0)
    except Exception as e:
        print(e)
        sys.exit(1)
if __name__ == "__main__":
    main()