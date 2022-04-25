#include "lit_archive.h"

int main(int argc, const char** argv) {
	LitState* state = lit_new_state();

	archive_open_library(state);
	lit_interpret_file(state, "test.lit");
	lit_free_state(state);
}