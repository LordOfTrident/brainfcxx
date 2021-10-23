#include <app.hh>

int main(const int argc, const char* argv[]) {
	BF::App app(
		argc,
		argv,
		BF::Interpreter::CellCountDefault,
		BF::Interpreter::CellSizeUi8
	);

	return app.GetExitcode();
};
