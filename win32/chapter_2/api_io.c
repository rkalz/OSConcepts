#include <Windows.h>

// Rofael Aleezada, May 14, 2019
// Operating Systems Concepts 9th Edition (South Asia), Problem 2.15
// Prompting and reading files names to move a file using only the Windows API

int write_message_to_handle(HANDLE hnd, char* message) {
	DWORD chars_to_write = lstrlenA(message);
	DWORD chars_written = 0;
	if (!WriteFile(hnd, message, chars_to_write, &chars_written, NULL) || chars_written != chars_to_write) {
		CloseHandle(hnd);
		return 0;
	}

	return 1;
}

int print_to_std_out(const char* message) {
	// Get handle to stdout
	HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (std_out == NULL || std_out == INVALID_HANDLE_VALUE) return 0;

	// Write our message to stdout, ensuring it was written correctly
	// A stands for ASCII (W for wchar_t)
	return write_message_to_handle(std_out, message);
}

int read_from_handle(HANDLE hnd, char* buffer, int buffer_size) {
	ZeroMemory(buffer, buffer_size);
	DWORD bytes_read = 0;
	if (!ReadFile(hnd, buffer, buffer_size, &bytes_read, NULL)) {
		CloseHandle(hnd);
		return 0;
	}

	// closing stdin/out will close it permanently
	return bytes_read;
}

// Read input from stdin
int read_from_std_in(char* buffer, int buffer_size) {
	HANDLE std_in = GetStdHandle(STD_INPUT_HANDLE);
	if (std_in == NULL || std_in == INVALID_HANDLE_VALUE) return ERROR_INVALID_HANDLE;

	// ReadFile blocks until a carriage return is read
	int bytes_read = read_from_handle(std_in, buffer, buffer_size);
	if (!bytes_read) return 0;

	// Remove CRLF
	buffer[bytes_read - 1] = 0;
	buffer[bytes_read - 2] = 0;
	return bytes_read;
}

// Open file handle
HANDLE open_existing_file_handle(char* filename) {
	return CreateFile(filename, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

HANDLE open_file_for_write(char* filename) {
	HANDLE hnd = CreateFile(filename, GENERIC_WRITE, NULL, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hnd == INVALID_HANDLE_VALUE && GetLastError() == ERROR_FILE_EXISTS) {
		print_to_std_out("File already exists! Will be overwritten.\n");
		hnd = CreateFile(filename, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	return hnd;
}

// TODO: Get working with WinMain, perhaps? (GetStdHandle returns 0)
int main() {
	// Prompt for source file
	if (!print_to_std_out("Enter the path to the source file:\n")) return ERROR_FUNCTION_FAILED;
	char input_file_name_buffer[256];
	if (!read_from_std_in(input_file_name_buffer, 256)) return ERROR_FUNCTION_FAILED;

	// Prompt for destination file
	if (!print_to_std_out("Enter the path to the destination file:\n")) return ERROR_FUNCTION_FAILED;
	char output_file_name_buffer[256];
	if (!read_from_std_in(output_file_name_buffer, 256)) return ERROR_FUNCTION_FAILED;

	// Get handle to source file, read contents, and close handle;
	HANDLE input_file_handle = open_existing_file_handle(input_file_name_buffer);
	if (input_file_handle == INVALID_HANDLE_VALUE) {
		print_to_std_out("Unable to open the file! Exiting!\n");
		return ERROR_FUNCTION_FAILED;
	}
	char input_file_content[256];
	if (!read_from_handle(input_file_handle, input_file_content, 256)) {
		print_to_std_out("Unable to read input file contents! Exiting!\n");
		return ERROR_FUNCTION_FAILED;
	}
	CloseHandle(input_file_handle);

	// Get handle to destination file, write contents, and close handle;
	HANDLE output_file_handle = open_file_for_write(output_file_name_buffer);
	if (output_file_handle == INVALID_HANDLE_VALUE) {
		print_to_std_out("Unable to create file! Exiting!\n");
		return ERROR_FUNCTION_FAILED;
	}
	if (!write_message_to_handle(output_file_handle, input_file_content)) {
		print_to_std_out("Failed to write to output file! Exiting!\n");
		return ERROR_FUNCTION_FAILED;
	}
	CloseHandle(output_file_handle);
		
	print_to_std_out("File copy was successful! Press enter to exit!\n");
	while (GetKeyState(VK_RETURN) & ~(1 << 15)) {}
	return ERROR_SUCCESS;
}