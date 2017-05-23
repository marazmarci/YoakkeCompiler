#pragma once

namespace yk {
	enum class verbosity_t {
		L0, L1, L2, L3
	};

	struct compiler_settings {
	public:
		static bool			ErrAlwaysTerminate;
		static bool			WarnIsErr;
		static verbosity_t	WarnVerbosity;
		static verbosity_t	ErrVerbosity;
	};

	bool		compiler_settings::ErrAlwaysTerminate	= true;
	bool		compiler_settings::WarnIsErr			= false;
	verbosity_t	compiler_settings::WarnVerbosity		= verbosity_t::L3;
	verbosity_t	compiler_settings::ErrVerbosity			= verbosity_t::L3;
}
