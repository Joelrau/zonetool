#include "stdafx.hpp"

#include "GfxWorld.hpp"
#include "Converter/H1/Assets/GfxWorld.hpp"
#include "H1/Assets/GfxWorld.hpp"

//#define GENERATE_SUN_FILE

namespace ZoneTool::IW5::H1Dumper
{
#ifdef GENERATE_SUN_FILE
	static inline float dot_to_angle_deg(float d)
	{
		if (d > 1.0f) d = 1.0f;
		if (d < -1.0f) d = -1.0f;
		const float pi = 3.14159265358979323846f;
		return std::acosf(d) * 180.0f / pi;
	}

	static inline std::string format_float(float v, int decimals)
	{
		char buf[64];

		if (decimals == 0)
		{
			// %g prints integers as "20", not "20."
			std::snprintf(buf, sizeof(buf), "%g", v);
			return std::string(buf);
		}

		std::snprintf(buf, sizeof(buf), "%.*f", decimals, v);

		std::string s(buf);
		auto pos = s.find_last_not_of('0');
		if (pos == std::string::npos)
			return "0";

		s.erase(pos + 1);

		if (!s.empty() && s.back() == '.')
			s.pop_back();

		if (s == "-0")
			return "0";

		return s;
	}

	static inline void vector_to_angles(const float in[3], float out[3])
	{
		const float pi = 3.14159265358979323846f;
		float yaw = std::atan2f(in[1], in[0]) * 180.0f / pi;
		float forward = std::sqrtf(in[0] * in[0] + in[1] * in[1]);
		float pitch = std::atan2f(-in[2], forward) * 180.0f / pi;
		out[0] = yaw;
		out[1] = pitch;
		out[2] = 0.0f;
	}

	std::string make_sun_file(const sunflare_t& sun)
	{
		std::ostringstream out;

		const char* sprite_name = (sun.spriteMaterial && sun.spriteMaterial->name) ? sun.spriteMaterial->name : "";
		const char* flare_name = (sun.flareMaterial && sun.flareMaterial->name) ? sun.flareMaterial->name : "";

		float flare_min_angle = std::round(dot_to_angle_deg(sun.flareMinDot));
		float flare_max_angle = std::round(dot_to_angle_deg(sun.flareMaxDot));

		float blind_min_angle = std::round(dot_to_angle_deg(sun.blindMinDot));
		float blind_max_angle = std::round(dot_to_angle_deg(sun.blindMaxDot));

		float glare_min_angle = std::round(dot_to_angle_deg(sun.glareMinDot));
		float glare_max_angle = std::round(dot_to_angle_deg(sun.glareMaxDot));

		float flare_fadein = sun.flareFadeInTime / 1000.0f;
		float flare_fadeout = sun.flareFadeOutTime / 1000.0f;
		float blind_fadein = sun.blindFadeInTime / 1000.0f;
		float blind_fadeout = sun.blindFadeOutTime / 1000.0f;
		float glare_fadein = sun.glareFadeInTime / 1000.0f;
		float glare_fadeout = sun.glareFadeOutTime / 1000.0f;

		float angles[3]{};
		vector_to_angles(sun.sunFxPosition, angles);

		out << "r_sunsprite_shader \"" << sprite_name << "\"\n";
		out << "r_sunsprite_size \"" << format_float(sun.spriteSize, 4) << "\"\n";
		out << "r_sunflare_shader \"" << flare_name << "\"\n";
		out << "r_sunflare_min_size \"" << format_float(sun.flareMinSize * 2.0f, 4) << "\"\n";
		out << "r_sunflare_min_angle \"" << format_float(flare_min_angle, 0) << "\"\n";
		out << "r_sunflare_max_size \"" << format_float(sun.flareMaxSize * 2.0f, 4) << "\"\n";
		out << "r_sunflare_max_angle \"" << format_float(flare_max_angle, 0) << "\"\n";
		out << "r_sunflare_max_alpha \"" << format_float(sun.flareMaxAlpha, 4) << "\"\n";
		out << "r_sunflare_fadein \"" << format_float(flare_fadein, 3) << "\"\n";
		out << "r_sunflare_fadeout \"" << format_float(flare_fadeout, 3) << "\"\n";
		out << "r_sunblind_min_angle \"" << format_float(blind_min_angle, 0) << "\"\n";
		out << "r_sunblind_max_angle \"" << format_float(blind_max_angle, 0) << "\"\n";
		out << "r_sunblind_max_darken \"" << format_float(sun.blindMaxDarken, 5) << "\"\n";
		out << "r_sunblind_fadein \"" << format_float(blind_fadein, 3) << "\"\n";
		out << "r_sunblind_fadeout \"" << format_float(blind_fadeout, 3) << "\"\n";
		out << "r_sunglare_min_angle \"" << format_float(glare_min_angle, 0) << "\"\n";
		out << "r_sunglare_max_angle \"" << format_float(glare_max_angle, 0) << "\"\n";
		out << "r_sunglare_max_lighten \"" << format_float(sun.glareMaxLighten, 4) << "\"\n";
		out << "r_sunglare_fadein \"" << format_float(glare_fadein, 3) << "\"\n";
		out << "r_sunglare_fadeout \"" << format_float(glare_fadeout, 3) << "\"\n";

		out << "r_sun_fx_position \""
			<< format_float(angles[0], 0) << ' '
			<< format_float(angles[1], 0) << ' '
			<< format_float(angles[2], 0) << "\"\n";

		return out.str();
	}

	void dump_sun_file(const sunflare_t& sun, const std::string& output_path)
	{
		std::string sun_file_content = make_sun_file(sun);

		auto file = filesystem::file(output_path);
		file.open("wb");
		file.write(sun_file_content.data(), sun_file_content.size());
		file.close();
	}
#endif

	void dump(GfxWorld* asset)
	{
		// generate h1 gfxworld
		allocator allocator;
		auto* h1_asset = H1Converter::convert(asset, allocator);

		// dump h1 gfxworld
		H1::IGfxWorld::dump(h1_asset);

#ifdef GENERATE_SUN_FILE
		dump_sun_file(asset->sun, va("sun/%s.sun", asset->baseName));
#endif
	}
}