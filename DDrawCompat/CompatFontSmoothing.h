#pragma once

namespace CompatFontSmoothing
{
	struct SystemSettings
	{
		BOOL isEnabled;
		UINT type;
		UINT contrast;
		UINT orientation;

		bool operator==(const SystemSettings& rhs) const;
		bool operator!=(const SystemSettings& rhs) const;
	};

	extern SystemSettings g_origSystemSettings;

	SystemSettings getSystemSettings();
	void setSystemSettings(const SystemSettings& settings);
	void setSystemSettingsForced(const SystemSettings& settings);
}
