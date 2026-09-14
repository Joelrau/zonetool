#pragma once

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		IW7::GfxWorld* convert(GfxWorld* asset, allocator& allocator);

		// IW5 reserves reflection probe 0 as the "invalid" sentinel: a flat red cubemap the
		// compiler points unresolved surfaces at (measured 255,58,58 solid on every face of
		// mp_test_h1's *reflection_probe0, against real captures in 1..6). IW7 has no such slot -
		// index 0 there is an ordinary probe, and every static model in every stock map sampled
		// it - so the sentinel is dropped and the rest shift down one.
		//
		// The converter and the dumper both have to skip the same probes, or the array image and
		// the probe metadata that indexes it disagree. This is that shared rule.
		unsigned int first_reflection_probe(unsigned int reflection_probe_count);
	}
}