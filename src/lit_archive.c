#include "archive.h"
#include "lit/lit.h"

#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <dirent.h>

static int copy_data(LitVm* vm, struct archive *ar, struct archive *aw) {
	int r;
	const void *buff;
	size_t size;
	la_int64_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF) {
			return ARCHIVE_OK;
		}

		if (r < ARCHIVE_OK) {
			return r;
		}

		r = archive_write_data_block(aw, buff, size, offset);

		if (r < ARCHIVE_OK) {
			lit_runtime_error_exiting(vm, archive_error_string(aw));
			return r;
		}
	}
}

LIT_METHOD(archive_extract) {
	const char* name = LIT_CHECK_STRING(0);
	const char* output = LIT_GET_STRING(1, NULL);

	struct archive* a;
	struct archive* ext;
	struct archive_entry* entry;
	int flags;
	int r;

	flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;

	a = archive_read_new();

	archive_read_support_format_all(a);
	archive_read_support_compression_all(a);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);

	if (r = archive_read_open_filename(a, name, 10240)) {
		return FALSE_VALUE;
	}

	for (;;) {
		r = archive_read_next_header(a, &entry);

		if (r == ARCHIVE_EOF) {
			break;
		}

		if (r < ARCHIVE_OK) {
			lit_runtime_error_exiting(vm, archive_error_string(a));
		}

		if (r < ARCHIVE_WARN) {
			return FALSE_VALUE;
		}

		if (output != NULL) {
			archive_entry_set_pathname(entry, AS_CSTRING(lit_string_format(vm->state, "$/$", output, archive_entry_pathname(entry))));
		}

		r = archive_write_header(ext, entry);

		if (r < ARCHIVE_OK) {
			lit_runtime_error_exiting(vm, archive_error_string(ext));
		} else if (archive_entry_size(entry) > 0) {
			r = copy_data(vm, a, ext);

			if (r < ARCHIVE_OK) {
				lit_runtime_error_exiting(vm, archive_error_string(ext));
			}

			if (r < ARCHIVE_WARN) {
				return FALSE_VALUE;
			}
		}

		r = archive_write_finish_entry(ext);

		if (r < ARCHIVE_OK) {
			lit_runtime_error_exiting(vm, archive_error_string(ext));
		}

		if (r < ARCHIVE_WARN) {
			return FALSE_VALUE;
		}
	}

	archive_read_close(a);
	archive_read_free(a);
	archive_write_close(ext);
	archive_write_free(ext);

	return TRUE_VALUE;
}

void archive_file(struct archive* a, struct archive_entry* entry, const char* name) {
	struct stat st;
	char buff[8192];
	int len;
	int fd;

	stat(name, &st);
	entry = archive_entry_new();

	archive_entry_set_pathname(entry, name);
	archive_entry_set_size(entry, st.st_size);
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);
	archive_write_header(a, entry);

	fd = open(name, O_RDONLY);
	len = read(fd, buff, sizeof(buff));

	while (len > 0) {
		archive_write_data(a, buff, len);
		len = read(fd, buff, sizeof(buff));
	}

	close(fd);
	archive_entry_free(entry);
}

void archive_directory(LitVm* vm, struct archive* a, struct archive_entry* entry, const char* input, const char* directory) {
	DIR* dir;
	struct dirent* dirent;
	size_t len = strlen(directory);

	if (!(dir = opendir(directory))) {
		FILE* file;

		if (file = fopen(directory, "r")) {
			fclose(file);
			return archive_file(a, entry, directory);
		}

		lit_runtime_error_exiting(vm, "Path '%s' does not exist or is not a directory", directory);
	}

	while ((dirent = readdir(dir)) != NULL) {
		char *name = dirent->d_name;

		if (dirent->d_type == DT_DIR) {
			if (!strcmp(name, ".") || !strcmp(name, "..")) {
				continue;
			}

			char path[len + strlen(name) + 2];

			strcpy(path, directory);
			path[len] = '/';

			strcpy(path + len + 1, name);
			archive_directory(vm, a, entry, input, path);
		} else {
			archive_file(a, entry, AS_CSTRING(lit_string_format(vm->state, "$/$", directory, name)));
		}
	}

	closedir(dir);
}

LIT_METHOD(archive_create) {
	const char* input = LIT_CHECK_STRING(0);
	const char* output = LIT_CHECK_STRING(1);

	struct archive* a;
	struct archive_entry* entry;

	a = archive_write_new();

	archive_write_add_filter_gzip(a);
	archive_write_set_format_pax_restricted(a);
	archive_write_open_filename(a, output);

	archive_directory(vm, a, entry, input, input);

	archive_write_close(a);
	archive_write_free(a);

	return TRUE_VALUE;
}

void archive_open_library(LitState* state) {
	LIT_BEGIN_CLASS("Archive")
		LIT_BIND_STATIC_METHOD("extract", archive_extract)
		LIT_BIND_STATIC_METHOD("create", archive_create)
	LIT_END_CLASS()
}