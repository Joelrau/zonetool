#include "stdafx.hpp"
#include "Material.hpp"

//#include "H1/Assets/Material.hpp"

namespace ZoneTool
{
	namespace H1
	{
		//r0 - replace
		//c0 - color map
		//n0 - normal map
		//s0 - specular map
		//p0 - parallax
		//a0 - Add
		//b0 - Blend
		//d0 - Detail
		//t0 - transparent ? (means that its replace + alpha test >= 128 meaning either full / no transparency per pixel)
		//q0 - (don't know the word, it's a detail map for normals)

		// _nfwpf = LIT_OPAQUE, opaque = not see through, blocks all light
		// _nfwpf_frt_aat = LIT_DECAL, decal = you cannot see past it but there is no background alpha
		// _nfwpf_frt_im_aat = same as _nfwpf_frt_aat, but for impact
		// _cltrans = LIT_TRANS, transparent = its see through
		// _lmpb_ causes speculars to be more intense than intended, would need to convert the specular image for those...
		// _ct_ = colorTint, you can use this whenever possible
		// ct0 = ^
		// sr0 = reflection
		// om0 = occlusion

		enum techset_map_index_e
		{
			opaque,
			decal,
			decal_impact,
			trans,
			count,
		};

		struct techset_map
		{
			std::string techset[techset_map_index_e::count];
		};

		std::unordered_map<std::string, std::string> mapped_techsets =
		{
			//	IW5,									H1
			{"wc_l_sm_a0c0",							"wc_l_sm_a0c0_nfwpf_frt_aat"},
			{"wc_l_sm_a0c0_nocast",						"wc_l_sm_a0c0_nfwpf_frt_aat"},
			{"wc_l_sm_r0c0",							"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0_nocast",						"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0d0n0s0",						"wc_l_sm_r0c0d0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0d0n0s0p0",					"wc_l_sm_r0c0d0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0d0n0s0o0",					"wc_l_sm_lmpb_r0c0d0n0sd0om0_nfwpf"},
			{"wc_l_sm_r0c0n0",							"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0n0_nocast",					"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0n0s0",						"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0n0s0_nocast",					"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0n0s0p0",						"wc_l_sm_r0c0n0sd0p0_nfwpf"},
			{"wc_l_sm_r0c0n0s0p0_nocast",				"wc_l_sm_r0c0n0sd0p0_nfwpf"},
			{"wc_l_sm_r0c0n0s0o0",						"wc_l_sm_lmpb_r0c0n0sd0om0_nfwpf"},
			{"wc_l_sm_r0c0n0p0",						"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0q0n0",						"wc_l_sm_r0c0q0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0q0n0s0",						"wc_l_sm_r0c0q0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0q0n0s0p0",					"wc_l_sm_r0c0q0n0sd0p0_nfwpf"},
			{"wc_l_sm_r0c0q0n0s0o0p0",					"wc_l_sm_lmpb_r0c0q0n0sd0om0_nfwpf"},
			{"wc_l_sm_r0c0q0s0",						"wc_l_sm_r0c0q0n0sd0_nfwpf"},
			{"wc_l_sm_r0c0q0s0p0",						"wc_l_sm_r0c0q0n0sd0p0_nfwpf"},
			{"wc_l_sm_r0c0s0",							"wc_l_sm_r0c0sd0_nfwpf"},
			{"wc_l_sm_r0c0s0_nocast",					"wc_l_sm_r0c0sd0_nfwpf"},
			{"wc_l_sm_r0c0s0p0",						"wc_l_sm_r0c0sd0_nfwpf"},
			{"wc_l_sm_r0c0s0o0p0",						"wc_l_sm_lmpb_r0c0sd0om0_nfwpf"},
			{"wc_l_sm_r0c0p0",							"wc_l_sm_r0c0sd0_nfwpf"},
			{"wc_l_sm_r0c0p0_nocast",					"wc_l_sm_r0c0sd0_nfwpf"},
			{"wc_l_r0c0",								"wc_l_sm_r0c0_nfwpf"},
			{"wc_l_r0c0_nocast",						"wc_l_sm_r0c0_nfwpf_nocast"},
			{"wc_l_r0c0s0",								"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_r0c0n0_nocast",						"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_r0c0n0s0_nocast",					"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_r0c0n0s0p0_nocast",					"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_r0c0q0n0s0_nocast",					"wc_l_sm_r0c0q0n0sd0_nfwpf"},

			{"wc_l_sm_ua_r0c0n0s0p0",					"wc_l_sm_lmpb_ua_r0c0n0sd0_nfwpf_nocast"},
			{"wc_l_sm_ua_r0c0n0s0p0_nocast",			"wc_l_sm_lmpb_ua_r0c0n0sd0_nfwpf_nocast"},
			{"wc_l_sm_ua_r0c0nf0s0p0_nocast",			"wc_l_sm_lmpb_ua_r0c0n0sd0_nfwpf_nocast"},
			{"wc_l_sm_ua_r0c0q0n0s0p0",					"wc_l_sm_lmpb_ua_r0c0n0sd0_nfwpf_nocast"},
			{"wc_l_sm_ua_r0c0q0n0s0p0_nocast",			"wc_l_sm_lmpb_ua_r0c0n0sd0_nfwpf_nocast"},

			{"wc_l_ua_r0c0nf0s0p0_nocast",				"wc_l_sm_lmpb_ua_r0c0n0sd0_nfwpf_nocast"},

			{"wc_l_sm_du_dm_r0c0n0s0",					"wc_l_sm_r0c0n0sd0_nfwpf"},
			{"wc_l_sm_du_dm_r0c0n0s0p0",				"wc_l_sm_r0c0n0sd0_nfwpf"},

			//{"wc_ocean_flatn",						"wc_ocean_nolm_displace_depth_foam_gloss_ssr_detail_flatdn_cltrans_hdr"}, // ???
			//{"wc_ocean_foam_detail_flatn",			"wc_ocean_nolm_displace_depth_foam_gloss_ssr_detail_flatdn_cltrans_hdr"}, // ???
			//{"wc_ocean_sm_foam_detail_flatn",			"wc_ocean_nolm_displace_depth_foam_gloss_ssr_detail_flatdn_cltrans_hdr"}, // ???
			//{"wc_ocean_sm_displace_foam_detail_flatn","wc_ocean_nolm_displace_depth_foam_gloss_ssr_detail_flatdn_cltrans_hdr"}, // ???
			//{"wc_ocean_displace_foam_pfoam_detail_flatn","wc_ocean_nolm_displace_depth_foam_gloss_ssr_detail_flatdn_cltrans_hdr"}, // ???

			{"wc_unlit",								"wc_unlit_replace_lin_nfwpf"},
			{"wc_unlit_nofog",							"wc_unlit_blend_lin_ndw_cltrans"},
			{"wc_unlit_alphatest",						"wc_unlit_blend_lin_ndw_cltrans"},
			{"wc_unlit_add",							"wc_unlit_add_lin_ndw_cltrans"},
			{"wc_unlit_add_lin",						"wc_unlit_add_lin_ndw_cltrans"},
			{"wc_unlit_add_lin_nofog",					"wc_unlit_add_lin_ndw_cltrans"},
			{"wc_unlit_add_lin_ua",						"wc_unlit_add_lin_ndw_ua_nfwpf"},
			{"wc_unlit_blend",							"wc_unlit_blend_lin_ndw_cltrans"},
			{"wc_unlit_blend_lin",						"wc_unlit_blend_lin_ndw_cltrans"},
			{"wc_unlit_blend_lin_ua",					"wc_unlit_blend_lin_ndw_ua_cltrans"},
			{"wc_unlit_blend_lin_custom_objective",		"wc_unlit_blend_lin_ct_ndw_cltrans_objective"},
			{"wc_unlit_distfalloff",					"wc_unlit_distfalloff_replace_ndw_cltrans"},
			{"wc_unlit_distfalloff_replace",			"wc_unlit_distfalloff_replace_ndw_cltrans"},
			{"wc_unlit_falloff",						"wc_unlit_falloff_add_lin_ndw_cltrans"},
			{"wc_unlit_falloff_add",					"wc_unlit_falloff_add_lin_ndw_cltrans"},
			{"wc_unlit_falloff_add_lin",				"wc_unlit_falloff_add_lin_ndw_cltrans"},
			{"wc_unlit_falloff_add_lin_ua",				"wc_unlit_falloff_add_lin_ndw_ua_cltrans"},
			{"wc_unlit_falloff_blend_lin_ua",			"wc_unlit_falloff_add_lin_ndw_ua_cltrans"},
			{"wc_unlit_falloff_screen_lin",				"wc_unlit_falloff_screen_lin_ndw_cltrans"},
			{"wc_unlit_multiply",						"wc_unlit_multiply_lin_ndw_cltrans"},
			{"wc_unlit_multiply_lin",					"wc_unlit_multiply_lin_ndw_cltrans"},
			{"wc_unlit_multiply_lin_nofog",				"wc_unlit_multiply_lin_ndw_cltrans"},
			{"wc_unlit_replace",						"wc_unlit_atest_lin_nfwpf"},
			{"wc_unlit_replace_lin",					"wc_unlit_atest_lin_nfwpf"},
			{"wc_unlit_replace_lin_nocast",				"wc_unlit_atest_lin_nfwpf"},
			{"wc_unlit_replace_lin_ua",					"wc_unlit_blend_lin_ndw_ua_cltrans"},
			{"wc_unlit_screen_lin",						"wc_unlit_add_lin_ndw_cltrans"},
			{"wc_unlit_screen_lin_ua",					"wc_unlit_add_lin_ndw_ua_nfwpf"},
			{"wc_effect",								"effect_blend_ndw"}, // ??
			{"wc_effect_blend",							"effect_blend_ndw"}, // ??
			{"wc_effect_zfeather_blend",				"effect_zf_blend_ndw"}, // ??
			{"wc_ambient_t0c0",							"wc_unlit_atest_lin_nfwpf"}, // works
			{"wc_ambient_t0c0_nocast",					"wc_unlit_atest_lin_nfwpf"}, // works
			{"wc_sky",									"wc_sky_cso_nfwpf"},
			{"wc_sky_sg",								"wc_sky_cso_nfwpf"}, // codo
			{"wc_shadowcaster",							"wc_shadowcaster"},
			{"wc_water",								"2d"},
			{"wc_tools",								"wc_tools_r0c0ct0"},
			{"wc_tools_r0c0",							"wc_tools_r0c0ct0"},
			{"wc_distortion_scale",						"wc_distortion_scale_legacydst"},
			{"wc_distortion_scale_zfeather",			"wc_distortion_scale_legacydst"},
			{"wc_distortion_scale_ua",					"wc_distortion_scale_ua_legacydst_dat"},
			{"wc_distortion_scale_ua_zfeather",			"wc_distortion_scale_ua_legacydst_dat"},
			{"wc_default",								"wc_default"},

			{"mc_l_sm_a0c0",							"m_l_sm_a0c0_nfwpf_frt_aat"},
			{"mc_l_sm_a0c0_nocast",						"m_l_sm_a0c0_nfwpf_frt_aat"},
			{"mc_l_sm_r0c0",							"mc_l_sm_r0c0_nfwpf"},
			{"mc_l_sm_r0c0_fgcb",						"mc_l_sm_r0c0_nfwpf"}, // codo
			{"mc_l_sm_r0c0_nocast",						"mc_l_sm_r0c0_nfwpf"},
			{"mc_l_sm_r0c0d0",							"mc_l_sm_r0c0_nfwpf"},
			{"mc_l_sm_r0c0d0s0",						"mc_l_sm_r0c0sd0_nfwpf"},
			{"mc_l_sm_r0c0d0s0p0",						"m_l_sm_r0c0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0d0n0",						"mc_l_sm_r0c0n0_nfwpf"},
			{"mc_l_sm_r0c0d0n0s0",						"mc_l_sm_r0c0d0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0d0n0s0p0",					"mc_l_sm_r0c0d0n0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0d0n0s0o0",					"mc_l_sm_lmpb_r0c0d0n0sd0om0_nfwpf"},
			{"mc_l_sm_r0c0s0",							"mc_l_sm_r0c0sd0_nfwpf"},
			{"mc_l_sm_r0c0s0_nocast",					"mc_l_sm_r0c0sd0_nfwpf"},
			{"mc_l_sm_r0c0s0p0",						"mc_l_sm_r0c0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0s0p0_nocast",					"mc_l_sm_r0c0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0n0",							"mc_l_sm_r0c0n0_nfwpf"},
			{"mc_l_sm_r0c0n0_nocast",					"mc_l_sm_r0c0n0_nfwpf"},
			{"mc_l_sm_r0c0n0p0",						"mc_l_sm_r0c0n0_nfwpf"},
			{"mc_l_sm_r0c0n0s0",						"mc_l_sm_r0c0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0n0s0_fgcb",					"mc_l_sm_r0c0n0sd0_nfwpf"}, // codo
			{"mc_l_sm_r0c0n0s0_em",						"mc_l_sm_r0c0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0n0s0_nocast",					"mc_l_sm_r0c0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0n0s0o0",						"mc_l_sm_lmpb_r0c0n0sd0om0_nfwpf"},
			{"mc_l_sm_r0c0n0s0o0p0",					"mc_l_sm_lmpb_r0c0n0sd0om0_nfwpf"},
			{"mc_l_sm_r0c0n0s0p0",						"mc_l_sm_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0n0s0p0_nocast",				"m_l_sm_r0c0n0sd0p0_nfwpf_nocast"},
			{"mc_l_sm_r0c0p0",							"mc_l_sm_r0c0p0_nfwpf"},
			{"mc_l_sm_r0c0p0_nocast",					"mc_l_sm_r0c0p0_nfwpf"},
			{"mc_l_sm_r0c0q0s0",						"mc_l_sm_r0c0sd0_nfwpf"},
			{"mc_l_sm_r0c0q0s0p0",						"mc_l_sm_r0c0sd0p0_nfwpf"},
			{"mc_l_sm_r0c0q0n0",						"mc_l_sm_r0c0n0_nfwpf"},
			{"mc_l_sm_r0c0q0n0s0",						"m_l_sm_r0c0q0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0q0n0s0p0",					"m_l_sm_r0c0q0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0q0n0s0o0",					"mc_l_sm_lmpb_r0c0q0n0sd0om0_nfwpf"},
			{"mc_l_sm_r0c0q0n0s0o0p0",					"mc_l_sm_lmpb_r0c0q0n0sd0om0_nfwpf"},

			{"mc_l_sm_r0c0s0_custom_grenade",			"mc_l_sm_r0c0n0sd0_nfwpf"},
			{"mc_l_sm_r0c0n0s0_custom_objective",		"m_l_sm_r0c0n0sd0_nfwpf_objective"},

			{"mc_l_sm_nofog_r0c0n0s0",					"mc_l_sm_r0c0n0sd0_nfwpf"},

			{"mc_l_sm_ua_r0c0",							"m_l_sm_ua_r0c0sd0p0_nfwpf"},
			{"mc_l_sm_ua_r0c0s0p0",						"m_l_sm_ua_r0c0sd0p0_nfwpf"},
			{"mc_l_sm_ua_r0c0s0p0_nocast",				"m_l_sm_ua_r0c0sd0p0_nfwpf"},
			{"mc_l_sm_ua_r0c0n0s0p0",					"m_l_sm_ua_r0c0sd0p0_nfwpf"},

			{"mc_l_ua_r0c0_nocast",						"m_l_sm_ua_r0c0sd0p0_nfwpf"},

			{"mc_l_sm_scroll_ua_r0c0s0p0",				"m_l_sm_ua_r0c0sd0p0_nfwpf"},

			{"mc_l_sm_nofog_ua_b0c0n0s0p0_nocast",		"mc_l_sm_ndw_ua_b0c0_cltrans"},

			{"mc_l_ua_b0c0n0s0p0_nocast",				"mc_l_sm_ndw_ua_b0c0_cltrans"},

			{"mc_l_sm_scroll_ua_b0c0s0p0",				"mc_l_sm_ndw_ua_b0c0_cltrans"},

			{"mc_l_sm_du_dm_r0c0n0s0",					"m_l_sm_du_dm_r0c0n0sd0_nfwpf"},
			{"mc_l_sm_du_dm_r0c0n0s0_nocast",			"m_l_sm_du_dm_r0c0n0sd0_nfwpf"},
			{"mc_l_sm_du_dm_r0c0n0s0p0",				"m_l_sm_du_dm_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_du_dm_r0c0q0n0s0",				"m_l_sm_du_dm_r0c0q0n0sd0_nfwpf"},
			{"mc_l_sm_du_dm_r0c0q0n0s0p0",				"m_l_sm_du_dm_r0c0q0n0sd0_nfwpf"},
			{"mc_l_sm_du_dm_r0c0d0n0s0p0",				"m_l_sm_du_dm_r0c0n0sd0p0_nfwpf"},

			{"mc_l_sm_du_dm_b0c0n0s0",					"mc_l_sm_ndw_b0c0n0sd0_cltrans"},

			{"mc_l_sm_du_dm_t0c0q0n0s0p0",				"mc_l_sm_t0c0n0sd0p0_nfwpf"},

			{"mc_l_flag_t0c0",							"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_flag_t0c0p0",						"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_flag_t0c0_nocast",					"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_flag_t0c0n0s0",						"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0",						"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0_nocast",				"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0s0",						"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0p0",						"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0s0p0",					"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0n0",						"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0n0s0",					"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0n0s0p0",					"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},
			{"mc_l_sm_flag_t0c0n0s0p0_nocast",			"mc_l_sm_flag_fuv_t0c0n0sd0_nfwpf"},

			{"mc_l_r0c0",								"m_l_r0c0_nfwpf_nocast"},
			{"mc_l_r0c0_nocast",						"m_l_r0c0_nfwpf_nocast"},
			{"mc_l_r0c0n0",								"m_l_r0c0n0sd0_nfwpf_nocast"},
			{"mc_l_r0c0n0_nocast",						"m_l_r0c0n0sd0_nfwpf_nocast"},
			{"mc_l_r0c0n0s0",							"m_l_r0c0n0sd0_nfwpf_nocast"},
			{"mc_l_r0c0n0s0_nocast",					"m_l_r0c0n0sd0_nfwpf_nocast"},
			{"mc_l_r0c0n0s0p0_nocast",					"m_l_r0c0n0sd0_nfwpf_nocast"},
			{"mc_l_r0c0s0",								"m_l_r0c0sd0_nfwpf"},
			{"mc_l_r0c0s0_nocast",						"m_l_r0c0sd0_nfwpf_nocast"},
			{"mc_l_r0c0s0p0_nocast",					"m_l_r0c0n0sd0_nfwpf_nocast"},
			{"mc_l_r0c0d0n0_nocast",					"m_l_r0c0n0sd0_nfwpf_nocast"},
			{"mc_l_r0c0q0n0s0p0_nocast",				"m_l_r0c0n0sd0_nfwpf_nocast"},

			{"mc_l_t0c0q0n0s0o0",						"m_l_sm_lmpb_t0c0q0n0sd0om0_nfwpf"},
			{"mc_l_t0c0n0s0o0",							"m_l_sm_lmpb_t0c0n0sd0om0_nfwpf"},

			{"mc_l_sm_heat_r0c0",						"m_l_sm_heat_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_heat_r0c0p0",						"m_l_sm_heat_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_heat_r0c0s0",						"m_l_sm_heat_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_heat_r0c0s0p0",					"m_l_sm_heat_r0c0sd0p0_cltrans"},
			{"mc_l_sm_heat_r0c0n0s0",					"m_l_sm_heat_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_heat_r0c0n0s0p0",					"m_l_sm_heat_r0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_heat_b0c0",						"m_l_sm_heat_ndw_b0c0_cltrans"},
			{"mc_l_sm_heat_b0c0s0",						"m_l_sm_heat_ndw_b0c0sd0_cltrans"},
			{"mc_l_sm_heat_b0c0s0p0",					"m_l_sm_heat_ndw_b0c0sd0p0_cltrans"},
			{"mc_l_sm_heat_b0c0n0s0",					"m_l_sm_heat_ndw_b0c0n0sd0p0_cltrans"},
			{"mc_l_sm_heat_b0c0n0s0p0",					"m_l_sm_heat_ndw_b0c0n0sd0p0_cltrans"},
			{"mc_l_sm_heat_t0c0s0_nocast",				"m_l_sm_heat_t0c0sd0_nfwpf_nocast"},
			{"mc_l_sm_heat_t0c0s0p0",					"m_l_sm_heat_t0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_heat_t0c0n0s0p0",					"m_l_sm_heat_t0c0n0sd0p0_nfwpf"},
			{"mc_l_sm_heat_ua_b0c0n0s0",				"m_l_sm_heat_ndw_ua_b0c0n0sd0_cltrans"},

			{"mc_l_heat_r0c0n0s0_nocast",				"m_l_sm_heat_r0c0n0sd0p0_nfwpf"},
			{"mc_l_heat_b0c0s0_nocast",					"m_l_sm_heat_ndw_b0c0sd0_cltrans"},

			{"mc_l_sm_skin_r0c0s0",						"mc_l_sm_r0c0_nfwpf"},
			{"mc_l_sm_skin_r0c0n0s0",					"mc_l_sm_r0c0n0sd0_nfwpf"},

			{"mc_l_du_dm_r0c0n0s0_nocast",				"m_l_sm_du_dm_r0c0n0sd0_nfwpf"},

			{"mc_l_sm_scroll_t0c0n0s0",					"mc_l_sm_scroll_t0c0n0sd0_nfwpf"},

			{"mc_unlit",								"mc_unlit_replace_lin_nfwpf"},
			{"mc_unlit_nofog",							"mc_unlit_blend_lin_ndw_nfwpf"},
			{"mc_unlit_alphatest",						"mc_unlit_blend_lin_ndw_nfwpf"},
			{"mc_unlit_add",							"mc_unlit_add_lin_ndw_cltrans"},
			{"mc_unlit_add_lin",						"mc_unlit_add_lin_ndw_cltrans"},
			{"mc_unlit_add_lin_ua",						"m_unlit_add_lin_ndw_ua_nfwpf"},
			{"mc_unlit_add_lin_nofog_ua",				"m_unlit_add_lin_ndw_ua_nfwpf"},
			{"mc_unlit_blend_lin",						"m_unlit_blend_lin_ndw_cltrans"},
			{"mc_unlit_blend_lin_ua",					"m_unlit_blend_lin_ndw_cltrans"},
			{"mc_unlit_blend_lin_nofog_ua",				"m_unlit_blend_lin_ndw_ua_cltrans"},
			{"mc_unlit_blend_lin_eyeoffset_custom_objective", "m_unlit_blend_lin_ndw_cltrans"},
			{"mc_unlit_blend_ua",						"m_unlit_blend_lin_ndw_ua_cltrans"},
			{"mc_unlit_blend_nofog_ua",					"m_unlit_blend_lin_ndw_ua_cltrans"},
			{"mc_unlit_multiply",						"mc_unlit_multiply_lin_ndw_nfwpf"},
			{"mc_unlit_multiply_lin",					"mc_unlit_multiply_lin_ndw_nfwpf"},
			{"mc_unlit_falloff_add",					"mc_unlit_blend_lin_ndw_nfwpf"},
			{"mc_unlit_falloff_screen_lin",				"mc_unlit_blend_lin_ndw_nfwpf"},
			{"mc_unlit_screen_lin",						"m_unlit_screen_lin_ndw_cltrans"},
			{"mc_unlit_screen_lin_ua",					"mc_unlit_screen_lin_ndw_ua_cltrans"},
			{"mc_unlit_replace",						"mc_unlit_replace_lin_nfwpf"},
			{"mc_unlit_replace_lin",					"mc_unlit_replace_lin_nfwpf"},
			{"mc_unlit_replace_lin_ua",					"m_unlit_replace_lin_ua_nfwpf"},
			{"mc_unlit_replace_lin_nofog",				"mc_unlit_replace_lin_nfwpf"},
			{"mc_unlit_replace_lin_nocast",				"mc_unlit_replace_lin_nfwpf_nocast"},
			{"mc_ambient_r0c0",							"mc_l_sm_r0c0_nfwpf"}, // don't use ambient
			{"mc_ambient_t0c0",							"mc_l_sm_t0c0_nfwpf"}, // don't use ambient
			{"mc_ambient_t0c0_nocast",					"mc_l_sm_t0c0_nfwpf_nocast"}, // don't use ambient
			{"mc_shadowcaster",							"mc_shadowcaster_atest"},
			{"mc_shadowcaster_atest",					"mc_shadowcaster_atest"},
			{"mc_reflexsight",							"mc_reflexsight"},

			{"mc_effect",								"m_effect_blend_ndw"},
			{"mc_effect_nofog",							"m_effect_blend_nofog_ndw"},
			{"mc_effect_replace",						"m_effect_replace_ndw"},
			{"mc_effect_replace_nofog",					"m_effect_replace_nofog_ndw"},
			{"mc_effect_replace_lin",					"m_effect_replace_lin"},
			{"mc_effect_add_nofog",						"effect_add_nofog_ndw"}, // me_effect_add_nofog_ndw
			{"mc_effect_add_eyeoffset",					"effect_add_eo"},
			{"mc_effect_blend",							"m_effect_blend_ndw"},
			{"mc_effect_blend_nofog",					"m_effect_blend_nofog_ndw"},
			{"mc_effect_falloff_add_nofog",				"effect_falloff_add_nofog_ndw"},
			{"mc_effect_falloff_add_lin_nofog",			"effect_falloff_add_nofog_ndw"},
			{"mc_effect_zfeather_falloff_add_nofog",	"mc_effect_zf_add_nofog_ndw"},
			{"mc_effect_zfeather_falloff_add_nofog_eyeoffset", "mc_effect_zf_falloff_add_nofog_eo_ndw"},
			{"mc_effect_zfeather_falloff_add_lin_nofog", "mc_effect_zf_add_nofog_ndw"},
			{"mc_effect_zfeather_falloff_add_lin_nofog_eyeoffset", "mc_effect_zf_falloff_add_nofog_eo_ndw"},
			{"mc_effect_zfeather_falloff_screen_nofog_eyeoffset", "mc_effect_zf_screen_nofog_spot_sm_ndw"},

			{"mc_threed_cinematic",						"m_threed_cinematic"},

			//{"mc_water",								"2d" },
			{"mc_tools",								"m_tools_r0c0ct0_nocast"},
			{"mc_tools_r0c0",							"m_tools_r0c0ct0_nocast"},
			{"mc_tools_r0c0_nocast",					"m_tools_r0c0ct0_nocast"},

			{"mc_objective",							"mc_unlit_add_lin_ct_ndw_cltrans_objective2"},
			{"mc_unlit_add_lin_custom_objective2",		"mc_unlit_add_lin_ct_ndw_cltrans_objective2"},

			{"l_sm_r0c0",								"2d"}, // not needed (iw3)
			{"ambient_r0c0",							"2d" }, // not needed (iw3)

			{"2d",										"2d"},

			{"distortion_scale",						"distortion_scale_legacydst_dat"},
			{"distortion_scale_zfeather",				"distortion_scale_legacydst_zf_dat"},

			{"effect",									"effect_add_ndw"},
			{"effect_screen",							"effect_add_ndw"},
			{"effect_screen_nofog",						"effect_add_ndw"},
			{"effect_nofog",							"effect_blend_nofog_ndw"},
			{"effect_add",								"effect_add_ndw"},
			{"effect_add_eyeoffset",					"effect_add_eo"},
			{"effect_add_nofog",						"effect_add_nofog_ndw"},
			{"effect_add_nofog_eyeoffset",				"effect_add_nofog_eo_ndw"},
			{"effect_blend",							"effect_blend_ndw"},
			{"effect_blend_nofog",						"effect_blend_nofog_ndw"},
			{"effect_replace_lin",						"effect_blend_ndw"},
			{"effect_falloff",							"effect_falloff_blend_ndw"},
			{"effect_falloff_add",						"effect_falloff_add_ndw"},
			{"effect_falloff_add_nofog",				"effect_falloff_add_nofog_ndw"},
			{"effect_falloff_blend",					"effect_falloff_blend_ndw"},
			{"effect_falloff_blend_nofog",				"effect_falloff_blend_ndw"},
			{"effect_falloff_screen_nofog",				"effect_add_nofog_ndw"}, // lets hope this works
			{"effect_zfeather",							"effect_zf_blend_ndw"},
			{"effect_zfeather_nofog",					"effect_zf_blend_nofog_ndw" },
			{"effect_zfeather_add",						"effect_zf_add_ndw"},
			{"effect_zfeather_add_nofog",				"effect_zf_add_nofog_ndw"},
			{"effect_zfeather_add_nofog_eyeoffset",		"effect_zf_add_nofog_eo_ndw"},
			{"effect_zfeather_add_eyeoffset",			"effect_zf_add_eo_ndw"},
			{"effect_zfeather_add_lin_nofog",			"effect_zf_add_nofog_ndw"},
			{"effect_zfeather_blend",					"effect_zf_blend_ndw"},
			{"effect_zfeather_blend_nofog",				"effect_zf_blend_nofog_ndw"},
			{"effect_zfeather_blend_nofog_eyeoffset",	"effect_zf_blend_eo_ndw"},
			{"effect_zfeather_blend_eyeoffset",			"effect_zf_blend_eo_ndw"},
			{"effect_zfeather_blend_spot_sm",			"effect_zf_blend_ndw"},
			{"effect_zfeather_falloff",					"effect_zf_falloff_blend_ndw"},
			{"effect_zfeather_falloff_nofog",			"effect_zf_falloff_blend_nofog_ndw"},
			{"effect_zfeather_falloff_add",				"effect_zf_falloff_add_ndw"},
			{"effect_zfeather_falloff_add_nofog",		"effect_zf_falloff_add_nofog_ndw" },
			{"effect_zfeather_falloff_add_eyeoffset",	"effect_zf_falloff_add_eo_ndw"},
			{"effect_zfeather_falloff_add_nofog_eyeoffset", "effect_zf_falloff_add_nofog_eo_ndw"},
			{"effect_zfeather_falloff_add_lin_nofog_eyeoffset", "effect_zf_falloff_add_nofog_eo_ndw"},
			{"effect_zfeather_falloff_blend",			"effect_zf_falloff_blend_ndw"},
			{"effect_zfeather_falloff_blend_nofog",			"effect_zf_falloff_blend_nofog_ndw"},
			{"effect_zfeather_falloff_screen",			"effect_zf_falloff_screen_nofog_ndw"},
			{"effect_zfeather_falloff_screen_nofog_eyeoffset", "effect_zf_falloff_add_nofog_eo_ndw"},
			{"effect_zfeather_falloff_screen_nofog",	"effect_zf_falloff_screen_nofog_ndw"},
			{"effect_zfeather_outdoor",					"effect_zf_outdoor_blend_ndw" },
			{"effect_zfeather_outdoor_blend",			"effect_zf_outdoor_blend_ndw"},
			{"effect_zfeather_outdoor_blend_nofog",		"effect_zf_outdoor_blend_nofog_ndw" },
			{"effect_zfeather_screen",					"effect_zf_screen_ndw"},
			{"effect_zfeather_screen_nofog",			"effect_zf_screen_ndw"},
			{"effect_zfeather_screen_nofog_spot_sm",	"effect_zf_screen_ndw"},
			{"effect_zfeather_screen_eyeoffset",		"effect_zf_screen_ndw"},

			{"particle_cloud",							"particle_cloud_replace_ga"},
			{"particle_cloud_add",						"particle_cloud_add_ga"},
			{"particle_cloud_outdoor",					"particle_cloud_outdoor_replace_ga"},
			{"particle_cloud_outdoor_add",				"particle_cloud_outdoor_add_ga"},
			{"particle_cloud_sparkf_add",				"particle_cloud_sparkf_add_ga"},
			{"particle_cloud_spark",					"particle_cloud_spark_add_ga"},
			{"particle_cloud_sparkf",					"particle_cloud_sparkf_add_ga"},
			{"particle_cloud_spark_add",				"particle_cloud_spark_add_ga"},
			{"particle_cloud_spark_outdoor",			"particle_cloud_spark_add_ga"},
			{"particle_cloud_spark_outdoor_add",		"particle_cloud_spark_add_ga"},

			{"grain_overlay",							"grain_overlay"},
			{"grain_overlay_uvscale",					"grain_overlay_uvscale"},

			{"splatter_alt",							"splatter_alt"},

			{"post_light",								"post_light"},
			{"laser",									"laser"},

			{"unlit",									"unlit_blend_lin_ndw_cltrans"},
			{"unlit_add_lin",							"unlit_add_lin_ndw_cltrans"},
			{"unlit_blend_lin",							"unlit_blend_lin_ndw_cltrans"},

			{"tools",									"tools_r0c0ct0"},
			{"tools_b0c0",								"tools_b0c0ct0"},
		};

		// wc

		techset_map wc_l_sm_b0c0 = { "wc_l_sm_ndw_b0c0_nfwpf_nocast", "wc_l_sm_ndw_b0c0_nfwpf_frt_aat", "wc_l_sm_ndw_b0c0_nfwpf_frt_im_aat", "wc_l_sm_ndw_b0c0_cltrans" };
		techset_map wc_l_sm_b0c0s0 = { "", "wc_l_sm_ndw_b0c0sd0_nfwpf_frt_aat", "wc_l_sm_ndw_b0c0sd0_nfwpf_frt_im_aat", "wc_l_sm_ndw_b0c0sd0_cltrans" };
		techset_map wc_l_sm_b0c0n0s0 = { "wc_l_sm_ndw_b0c0n0sd0_nfwpf", "wc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_aat", "wc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "wc_l_sm_ndw_b0c0n0sd0_cltrans" };
		techset_map wc_l_sm_b0c0n0s0o0 = { "wc_l_sm_lmpb_ndw_b0c0n0sd0om0_nfwpf", "wc_l_sm_lmpb_ndw_b0c0n0sd0om0_nfwpf_frt_aat", "", "wc_l_sm_lmpb_ndw_b0c0n0sd0om0_cltrans" };

		techset_map wc_l_sm_ua_b0c0 = { "", "", "", "wc_l_sm_ndw_ua_b0c0_cltrans_nocast" };
		techset_map wc_l_sm_ua_b0c0n0s0p0 = { "", "", "", "wc_l_sm_ndw_ua_b0c0n0sd0p0_cltrans_nocast_frt_aat" };

		techset_map wc_l_sm_t0c0 = { "wc_l_sm_t0c0_nfwpf", "wc_l_sm_t0c0_nfwpf_frt_aat", "", "" };
		techset_map wc_l_sm_t0c0s0 = { "wc_l_sm_t0c0sd0_nfwpf", "wc_l_sm_t0c0sd0_nfwpf_frt_aat", "", "" };
		techset_map wc_l_sm_t0c0n0s0 = { "wc_l_sm_t0c0n0sd0_nfwpf", "wc_l_sm_t0c0n0sd0_nfwpf_frt_aat", "", "" };

		// mc

		techset_map mc_l_sm_b0c0 = { "", "mc_l_sm_ndw_b0c0_nfwpf_frt_im_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0_cltrans" };
		techset_map mc_l_sm_b0c0d0p0 = { "", "", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "mc_l_sm_ndw_b0c0d0p0_cltrans" };
		techset_map mc_l_sm_b0c0d0n0s0p0 = { "", "mc_l_sm_ndw_b0c0n0sd0p0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "mc_l_sm_ndw_b0c0n0sd0p0_cltrans" };
		techset_map mc_l_sm_b0c0s0 = { "", "m_l_sm_ndw_b0c0sd0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0sd0_cltrans" };
		techset_map mc_l_sm_b0c0s0_nocast = { "", "m_l_sm_ndw_b0c0sd0_nfwpf_nocast_frt_aat", "mc_l_sm_ndw_b0c0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0sd0_cltrans" };
		techset_map mc_l_sm_b0c0s0p0 = { "", "m_l_sm_ndw_b0c0sd0p0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0sd0p0_cltrans" };
		techset_map mc_l_sm_b0c0n0 = { "", "m_l_sm_ndw_b0c0n0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0n0_cltrans" };
		techset_map mc_l_sm_b0c0n0s0 = { "mc_l_sm_ndw_b0c0n0sd0_nfwpf", "m_l_sm_ndw_b0c0n0sd0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "mc_l_sm_ndw_b0c0n0sd0_cltrans" };
		techset_map mc_l_sm_b0c0n0s0p0 = { "m_l_sm_ndw_b0c0n0sd0p0_nfwpf", "mc_l_sm_ndw_b0c0n0sd0p0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0n0sd0p0_cltrans" };
		techset_map mc_l_sm_b0c0q0n0s0 = { "", "mc_l_sm_ndw_b0c0q0n0sd0_nfwpf_frt_aat", "", "m_l_sm_lmpb_ndw_b0c0q0n0sd0_cltrans" };

		techset_map mc_l_sm_ua_b0c0 = { "mc_l_sm_ndw_ua_b0c0_cltrans", "", "", "" };

		techset_map m_l_sm_b0c0s0 = { "", "m_l_sm_ndw_b0c0sd0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0sd0_cltrans" };
		techset_map m_l_sm_b0c0n0s0 = { "m_l_sm_ndw_b0c0n0sd0_nfwpf", "m_l_sm_ndw_b0c0n0sd0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0n0sd0_cltrans" };
		techset_map m_l_sm_b0c0n0s0p0 = { "m_l_sm_ndw_b0c0n0sd0p0_nfwpf", "m_l_sm_ndw_b0c0n0sd0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0n0sd0p0_cltrans" };
		techset_map m_l_sm_b0c0q0n0s0 = { "m_l_sm_lmpb_ndw_b0c0q0n0sd0_nfwpf", "m_l_sm_lmpb_ndw_b0c0q0n0sd0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_lmpb_ndw_b0c0q0n0sd0_cltrans" };
		techset_map m_l_sm_b0c0q0n0s0p0 = { "m_l_sm_lmpb_ndw_b0c0q0n0sd0_nfwpf", "m_l_sm_lmpb_ndw_b0c0q0n0sd0_nfwpf_frt_aat", "mc_l_sm_ndw_b0c0n0sd0_nfwpf_frt_im_aat", "m_l_sm_ndw_b0c0q0n0sd0p0_cltrans" };

		techset_map mc_l_sm_t0c0 = { "mc_l_sm_t0c0_nfwpf", "mc_l_sm_t0c0n0sd0_nfwpf_frt_aat", "", "" };
		techset_map mc_l_sm_t0c0_nocast = { "mc_l_sm_t0c0_nfwpf_nocast", "mc_l_sm_t0c0n0sd0_nfwpf_frt_aat", "", "" };
		techset_map mc_l_sm_t0c0s0 = { "mc_l_sm_t0c0sd0_nfwpf", "mc_l_sm_t0c0n0sd0_nfwpf_frt_aat", "", "" };
		techset_map mc_l_sm_t0c0s0p0 = { "mc_l_sm_t0c0sd0p0_nfwpf", "", "", "" };
		techset_map mc_l_sm_t0c0n0 = { "m_l_sm_t0c0n0_nfwpf", "m_l_sm_t0c0n0_nfwpf_frt_aat", "", "m_l_sm_t0c0n0_cltrans" };
		techset_map mc_l_sm_t0c0n0_nocast = { "m_l_sm_t0c0n0_nfwpf_nocast", "m_l_sm_t0c0n0_nfwpf_frt_aat", "", "m_l_sm_t0c0n0_cltrans" };
		techset_map mc_l_sm_t0c0n0s0 = { "m_l_sm_t0c0n0sd0_nfwpf", "m_l_sm_t0c0n0sd0_nfwpf_frt_aat", "", "" };
		techset_map mc_l_sm_t0c0n0s0_nocast = { "m_l_sm_t0c0n0sd0_nfwpf_nocast", "m_l_sm_t0c0n0sd0_nfwpf_frt_aat", "", "" };
		techset_map mc_l_sm_t0c0n0s0p0 = { "mc_l_sm_t0c0n0sd0p0_nfwpf", "", "", "" };
		techset_map mc_l_sm_t0c0q0n0s0 = { "m_l_sm_t0c0q0n0sd0_nfwpf", "", "", "" };
		techset_map mc_l_sm_t0c0d0n0s0 = { "m_l_sm_lmpb_t0c0d0n0sd0_nfwpf", "", "", "" };

		std::unordered_map<std::string, techset_map> mapped_techsets_ =
		{
			// wc
			// b0

			{"wc_l_sm_b0c0",							wc_l_sm_b0c0},
			{"wc_l_sm_b0c0_nocast",						wc_l_sm_b0c0},
			{"wc_l_sm_b0c0d0n0",						wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0d0n0s0",						wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0d0n0s0o0",					wc_l_sm_b0c0n0s0o0},
			{"wc_l_sm_b0c0n0",							wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0n0s0",						wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0n0s0_nocast",					wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0n0s0p0",						wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0n0s0p0_nocast",				wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0n0s0o0",						wc_l_sm_b0c0n0s0o0},
			{"wc_l_sm_b0c0n0p0",						wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0p0",							wc_l_sm_b0c0},
			{"wc_l_sm_b0c0p0_nocast",					wc_l_sm_b0c0},
			{"wc_l_sm_b0c0s0",							wc_l_sm_b0c0s0},
			{"wc_l_sm_b0c0s0_nocast",					wc_l_sm_b0c0s0},
			{"wc_l_sm_b0c0s0p0",						wc_l_sm_b0c0s0},
			{"wc_l_sm_b0c0q0n0",						wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0q0n0s0",						wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0q0n0s0_nocast",				wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0q0n0s0p0",					wc_l_sm_b0c0n0s0},

			{"wc_l_sm_ua_b0c0s0p0_nocast",				wc_l_sm_ua_b0c0n0s0p0},
			{"wc_l_sm_ua_b0c0n0s0",						{"wc_l_sm_lmpb_ndw_ua_b0c0n0sd0sr0_nfwpf_nocast_frt_aat", "wc_l_sm_lmpb_ndw_ua_b0c0n0sd0sr0_nfwpf_nocast_frt_aat", "wc_l_sm_lmpb_ndw_ua_b0c0n0sd0sr0_nfwpf_nocast_frt_aat", "wc_l_sm_lmpb_ndw_ua_b0c0n0sd0_cltrans_nocast"}},
			{"wc_l_sm_ua_b0c0n0s0p0",					wc_l_sm_ua_b0c0n0s0p0},
			{"wc_l_sm_ua_b0c0n0s0p0_nocast",			wc_l_sm_ua_b0c0n0s0p0},
			{"wc_l_sm_ua_b0c0nf0s0p0_nocast",			wc_l_sm_ua_b0c0n0s0p0},
			{"wc_l_sm_ua_b0c0q0n0s0",					wc_l_sm_ua_b0c0n0s0p0},
			{"wc_l_sm_ua_b0c0q0n0s0p0_nocast",			wc_l_sm_ua_b0c0n0s0p0},

			{"wc_l_sm_nofog_ua_b0c0n0s0p0_nocast",			wc_l_sm_ua_b0c0n0s0p0},

			{"wc_l_sm_b0c0n0s0_custom_growing_ice_cracks", wc_l_sm_b0c0n0s0},
			{"wc_l_sm_b0c0n0s0p0_custom_blood_pool",	wc_l_sm_b0c0n0s0},

			{"wc_l_b0c0",								wc_l_sm_b0c0},
			{"wc_l_b0c0_nocast",						wc_l_sm_b0c0},
			{"wc_l_b0c0p0_nocast",						wc_l_sm_b0c0},
			{"wc_l_b0c0s0_nocast",						wc_l_sm_b0c0s0},
			{"wc_l_b0c0n0s0p0",							wc_l_sm_b0c0n0s0},
			{"wc_l_b0c0n0s0p0_nocast",					wc_l_sm_b0c0n0s0},

			{"wc_l_ua_b0c0_nocast",						wc_l_sm_ua_b0c0},
			{"wc_l_ua_b0c0nf0s0p0_nocast",				wc_l_sm_ua_b0c0n0s0p0},
			{"wc_l_ua_b0c0n0s0p0_nocast",				wc_l_sm_ua_b0c0n0s0p0},

			// t0

			{"wc_l_sm_t0c0",							wc_l_sm_t0c0},
			{"wc_l_sm_t0c0_nocast",						wc_l_sm_t0c0},
			{"wc_l_sm_t0c0n0",							wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0n0s0",						wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0n0s0_nocast",					wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0n0s0p0",						wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0n0s0p0_nocast",				wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0n0p0",						wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0q0n0p0",						wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0q0n0s0",						wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0q0n0s0p0",					wc_l_sm_t0c0n0s0},
			{"wc_l_sm_t0c0s0",							wc_l_sm_t0c0s0},
			{"wc_l_sm_t0c0s0_nocast",					wc_l_sm_t0c0s0},
			{"wc_l_sm_t0c0s0p0",						wc_l_sm_t0c0s0},
			{"wc_l_sm_t0c0p0",							wc_l_sm_t0c0},
			{"wc_l_sm_t0c0d0n0s0p0",					wc_l_sm_t0c0n0s0},

			{"wc_l_t0c0",								wc_l_sm_t0c0},
			{"wc_l_t0c0_nocast",						wc_l_sm_t0c0},
			{"wc_l_t0c0n0s0_nocast",					wc_l_sm_t0c0n0s0},

			{"wc_l_sm_ua_t0c0s0_nocast",				wc_l_sm_t0c0s0},

			// mc

			// b0
			{"mc_l_sm_b0c0",							mc_l_sm_b0c0},
			{"mc_l_sm_b0c0_nocast",						mc_l_sm_b0c0},
			{"mc_l_sm_b0c0d0p0",						mc_l_sm_b0c0d0p0},
			{"mc_l_sm_b0c0d0n0s0p0",					mc_l_sm_b0c0d0n0s0p0},
			{"mc_l_sm_b0c0s0",							mc_l_sm_b0c0s0},
			{"mc_l_sm_b0c0s0_nocast",					mc_l_sm_b0c0s0_nocast},
			{"mc_l_sm_b0c0s0p0",						mc_l_sm_b0c0s0p0},
			{"mc_l_sm_b0c0n0",							mc_l_sm_b0c0n0},
			{"mc_l_sm_b0c0n0p0",						mc_l_sm_b0c0n0},
			{"mc_l_sm_b0c0n0s0",						mc_l_sm_b0c0n0s0},
			{"mc_l_sm_b0c0n0s0_nocast",					mc_l_sm_b0c0n0s0},
			{"mc_l_sm_b0c0n0s0p0",						mc_l_sm_b0c0n0s0p0},
			{"mc_l_sm_b0c0n0s0p0_nocast",				mc_l_sm_b0c0n0s0p0},
			{"mc_l_sm_b0c0p0",							mc_l_sm_b0c0},
			{"mc_l_sm_b0c0p0_fgca",						mc_l_sm_b0c0}, // codo
			{"mc_l_sm_b0c0q0s0",						mc_l_sm_b0c0q0n0s0},
			{"mc_l_sm_b0c0q0n0s0",						mc_l_sm_b0c0q0n0s0},
			{"mc_l_sm_b0c0q0n0s0p0",					mc_l_sm_b0c0q0n0s0},

			{"mc_l_sm_nofog_b0c0n0s0",					mc_l_sm_b0c0n0s0},

			{"mc_l_sm_b0c0n0s0_custom_growing_ice_cracks", mc_l_sm_b0c0n0s0},
			{"mc_l_sm_b0c0n0s0p0_custom_blood_pool",	mc_l_sm_b0c0n0s0p0},

			{"m_l_sm_b0c0s0",							m_l_sm_b0c0s0},
			{"m_l_sm_b0c0n0s0",							m_l_sm_b0c0n0s0},
			{"m_l_sm_b0c0n0s0p0",						m_l_sm_b0c0n0s0p0},
			{"m_l_sm_b0c0s0p0",							mc_l_sm_b0c0s0p0},
			{"m_l_sm_b0c0q0n0s0",						m_l_sm_b0c0q0n0s0},
			{"m_l_sm_b0c0q0n0s0p0",						m_l_sm_b0c0q0n0s0p0},

			{"m_l_b0c0s0_nocast",						m_l_sm_b0c0s0},

			{"mc_l_b0c0_nocast",						mc_l_sm_b0c0},
			{"mc_l_b0c0s0",								mc_l_sm_b0c0s0},
			{"mc_l_b0c0s0_nocast",						mc_l_sm_b0c0s0_nocast},
			{"mc_l_b0c0s0p0",							mc_l_sm_b0c0s0p0},
			{"mc_l_b0c0n0s0_nocast",					mc_l_sm_b0c0n0s0},

			// t0
			{"mc_l_sm_t0c0",							mc_l_sm_t0c0},
			{"mc_l_sm_t0c0_nocast",						mc_l_sm_t0c0_nocast},
			{"mc_l_sm_t0c0s0",							mc_l_sm_t0c0s0},
			{"mc_l_sm_t0c0s0p0",						mc_l_sm_t0c0s0p0},
			{"mc_l_sm_t0c0n0",							mc_l_sm_t0c0n0},
			{"mc_l_sm_t0c0n0_nocast",					mc_l_sm_t0c0n0_nocast},
			{"mc_l_sm_t0c0n0s0",						mc_l_sm_t0c0n0s0},
			{"mc_l_sm_t0c0n0s0_nocast",					mc_l_sm_t0c0n0s0_nocast},
			{"mc_l_sm_t0c0n0s0p0",						mc_l_sm_t0c0n0s0p0},
			{"mc_l_sm_t0c0n0s0p0_nocast",				mc_l_sm_t0c0n0s0p0},
			{"mc_l_sm_t0c0p0",							mc_l_sm_t0c0},
			{"mc_l_sm_t0c0q0n0s0",						mc_l_sm_t0c0q0n0s0},
			{"mc_l_sm_t0c0q0n0s0p0",					mc_l_sm_t0c0q0n0s0},
			{"mc_l_sm_t0c0d0n0s0p0",					mc_l_sm_t0c0d0n0s0},

			{"mc_l_sm_ua_t0c0",							mc_l_sm_ua_b0c0}, // !
			{"mc_l_sm_ua_t0c0n0s0p0",					mc_l_sm_ua_b0c0}, // !

			{"mc_l_t0c0",								mc_l_sm_t0c0},
			{"mc_l_t0c0_nocast",						mc_l_sm_t0c0_nocast},
			{"mc_l_t0c0o0",								mc_l_sm_t0c0},
			{"mc_l_t0c0n0",								mc_l_sm_t0c0n0},
			{"mc_l_t0c0n0s0",							mc_l_sm_t0c0n0s0},
			{"mc_l_t0c0n0s0_nocast",					mc_l_sm_t0c0n0s0_nocast},
			{"mc_l_t0c0n0s0p0",							mc_l_sm_t0c0n0s0p0},
			{"mc_l_t0c0n0s0p0_nocast",					mc_l_sm_t0c0n0s0p0},
			{"mc_l_t0c0s0p0",							mc_l_sm_t0c0s0p0},
			{"mc_l_t0c0q0n0s0",							mc_l_sm_t0c0q0n0s0},
		};

		namespace
		{
			std::string get_h1_techset_by_sortkey(std::string name, std::string matname, bool* result, const std::uint8_t h1_sortkey)
			{
				int index = 0;
				if (h1_sortkey >= 1 && h1_sortkey <= 2) // opaque start, opaque end
				{
					index = 0;
				}
				else if (h1_sortkey >= 7 && h1_sortkey <= 17) // decal start, decal end
				{
					if (h1_sortkey == 14) // impact decal
					{
						index = 2;
					}
					else
					{
						index = 1;
					}
				}
				else if (h1_sortkey >= 18 && h1_sortkey <= 37) // blend start, blend end
				{
					index = 3;
				}

				if (mapped_techsets_.contains(name))
				{
					auto& t = mapped_techsets_[name];

					const auto get_best = [&]()
					{
						std::string t_str = t.techset[index];

						// If the initial index holds an empty string, check the other (1 if index is 2, 2 if index is 1)
						if (t_str.empty())
						{
							if (index == 1)
							{
								t_str = t.techset[2];
							}
							else if (index == 2)
							{
								t_str = t.techset[1];
							}
						}

						// If still empty, perform forward and backward search
						if (t_str.empty())
						{
							// First pass: search forward from the current index
							for (int current_index = index; current_index < techset_map_index_e::count; ++current_index)
							{
								if (!t.techset[current_index].empty())
								{
									t_str = t.techset[current_index];
									return t_str; // Return as soon as a valid string is found
								}
							}

							// Second pass: search backward from the current index
							for (int current_index = index - 1; current_index >= 0; --current_index)
							{
								if (!t.techset[current_index].empty())
								{
									t_str = t.techset[current_index];
									return t_str; // Return as soon as a valid string is found
								}
							}
						}

						return t_str; // Return the found string or empty if none is found
					};

					std::string t_str = get_best();
					if (!t_str.empty())
					{
						*result = true;
						return t_str;
					}
					else
					{
						ZONETOOL_FATAL("[get_h1_techset_by_sortkey]: Empty string: %s, %s, %d", matname.data(), name.data(), h1_sortkey);
					}
				}
				
				*result = false;
				return "2d";
			}

			std::string get_h1_techset(std::string name, std::string matname, bool* result, const std::uint8_t h1_sortkey = 0)
			{
				if (h1_sortkey)
				{
					std::string h1_techset = get_h1_techset_by_sortkey(name, matname, result, h1_sortkey);
					if (*result)
					{
						return h1_techset;
					}
				}

				if (mapped_techsets.find(name) == mapped_techsets.end())
				{
					ZONETOOL_ERROR("Could not find mapped H1 techset for IW5 techset \"%s\" (material: %s)", name.data(), matname.data());
					*result = false;
					return "2d";
				}

				*result = true;
				return mapped_techsets[name];
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_sortkeys =
			{
				{0, 1},		// Opaque ambient
				{1, 2},		// Opaque
				{2, 3},		// Sky
				{3, 4},		// Skybox sun/moon
				{4, 5},		// Clouds  NOT SURE
				{5, 6},		// Horizon NOT SURE
				{6, 7},		// Decal bottom 1
				{7, 8},		// Decal bottom 2
				{8, 9},		// Decal bottom 3
				{9, 10},	// Decal static
				{10, 11},	// Decal mid 1
				{11, 12},	// Decal mid 2
				{12, 13},	// Decal mid 3
				{13, 14},	// Weapon Impact
				{14, 15},	// Decal top 1
				{15, 16},	// Decal top 2
				{16, 17},	// Decal top 3
				{17, 19},	// ?
				{18, 20},	// ?
				{19, 21},	// ?
				{20, 22},	// ?
				//{21, },	// ?
				//{22, },	// ?
				//{23, },	// ?
				{24, 26},	// Window inside
				{25, 27},	// Window outside
				{26, 28},	// Cloud 1
				{27, 29},	// Cloud 2
				{28, 30},	// Cloud 3
				{29, 31},	// Blend 1
				{30, 32},	// Blend 2
				{31, 33},	// Blend 3
				//{32, },	// ?
				{33, 36},	// Gun blend | in h1 35 = gun emissive, 36 = gun blend, 37 = gun depth hack
				{34, 38},	// Shadow Caster
				//{35, 35},	// ?
				//{36, },	// ?
				//{37, },	// ?
				//{38, },	// ?
				//{39, },	// ?
				//{40, },	// ?
				//{41, },	// ?
				//{42, },	// ?
				{43, 48},	// Distortion
				{44, 49},	// Distortion heavy
				{45, 50},	// ?
				{46, 51},	// ?
				{47, 52},	// Effect 1
				{48, 53},	// Effect 2
				{49, 54},	// Effect 3
				{50, 55},	// ?
				{51, 56},	// ?
				//{52, 57},	// ?
				{53, 36},	// Viewmodel Effect
				{54, 60},	// 2D
			};

			std::unordered_map<std::string, std::uint8_t> mapped_sortkeys_by_techset =
			{
				{"2d", 60},
				{"mc_shadowcaster_atest", 38},
				{"wc_shadowcaster", 38},
			};

			bool is_blend_sortkey(std::uint8_t h1_sortkey)
			{
				return (h1_sortkey >= 18 && h1_sortkey <= 36);
			}

			bool is_decal_sortkey(std::uint8_t h1_sortkey)
			{
				return (h1_sortkey >= 7 && h1_sortkey <= 17);
			}

			bool is_impact_decal_sortkey(std::uint8_t h1_sortkey)
			{
				return (h1_sortkey == 14);
			}

			bool is_opaque_sortkey(std::uint8_t h1_sortkey)
			{
				return (h1_sortkey >= 1 && h1_sortkey <= 2);
			}

			std::uint8_t fixup_h1_sortkey(std::uint8_t h1_sortkey, std::string h1_techset)
			{
				if (!is_opaque_sortkey(h1_sortkey) && !is_decal_sortkey(h1_sortkey) && !is_blend_sortkey(h1_sortkey))
				{
					return h1_sortkey;
				}

				if (h1_techset.contains("_cltrans") && !is_blend_sortkey(h1_sortkey))
				{
					return 31; // default blend sortkey
				}
				if ((h1_techset.contains("_nfwpf_frt_aat") || h1_techset.contains("_nfwpf_frt_im_aat")) && !is_decal_sortkey(h1_sortkey))
				{
					if (h1_techset.contains("_nfwpf_frt_im_aat"))
					{
						return 14; // impact decal sortkey
					}
					return 12; // default decal sortkey
				}
				if (h1_techset.contains("_nfwpf") && !is_opaque_sortkey(h1_sortkey))
				{
					return 2; // default opaque sortkey
				}

				if (h1_techset.contains("_effect") && h1_sortkey == 36)
				{
					return 59; // effect sortkey
				}

				return h1_sortkey;
			}

			std::uint8_t get_h1_sortkey(std::uint8_t sortkey, std::string matname, std::string h1_techset = "")
			{
				if (mapped_sortkeys_by_techset.find(h1_techset) != mapped_sortkeys_by_techset.end())
				{
					return fixup_h1_sortkey(mapped_sortkeys_by_techset[h1_techset], h1_techset);
				}

				if (mapped_sortkeys.contains(sortkey))
				{
					return fixup_h1_sortkey(mapped_sortkeys[sortkey], h1_techset);
				}

				ZONETOOL_ERROR("Could not find mapped H1 sortkey for sortkey: %d (material: %s)", sortkey, matname.data());

				return sortkey;
			}

			std::unordered_map<std::uint8_t, std::uint8_t> mapped_camera_regions =
			{
				{IW5::CAMERA_REGION_LIT_OPAQUE, H1::CAMERA_REGION_LIT_OPAQUE},
				{IW5::CAMERA_REGION_LIT_TRANS, H1::CAMERA_REGION_LIT_TRANS},
				{IW5::CAMERA_REGION_EMISSIVE, H1::CAMERA_REGION_EMISSIVE},
				{IW5::CAMERA_REGION_DEPTH_HACK, H1::CAMERA_REGION_DEPTH_HACK},
				{IW5::CAMERA_REGION_NONE, H1::CAMERA_REGION_NONE},
			};

			std::unordered_map<std::string, std::uint8_t> mapped_camera_regions_by_techset =
			{
				{"2d", H1::CAMERA_REGION_NONE},
				{"mc_shadowcaster_atest", H1::CAMERA_REGION_NONE},
				{"wc_shadowcaster", H1::CAMERA_REGION_NONE},
			};

			std::uint8_t get_h1_camera_region(std::uint8_t camera_region, std::string matname, std::string h1_techset = "")
			{
				if (h1_techset.contains("_cltrans"))
				{
					return H1::CAMERA_REGION_LIT_TRANS;
				}

				if (h1_techset.contains("_nfwpf_frt_aat") || h1_techset.contains("_nfwpf_frt_im_aat"))
				{
					return H1::CAMERA_REGION_LIT_DECAL;
				}

				if (h1_techset.contains("_nfwpf"))
				{
					return H1::CAMERA_REGION_LIT_OPAQUE;
				}

				if (mapped_camera_regions_by_techset.find(h1_techset) != mapped_camera_regions_by_techset.end())
				{
					return mapped_camera_regions_by_techset[h1_techset];
				}

				if (mapped_camera_regions.contains(camera_region))
				{
					return mapped_camera_regions[camera_region];
				}

				ZONETOOL_ERROR("Could not find mapped H1 camera region for camera region: %d (material: %s)", camera_region, matname.data());

				return camera_region;
			}

			std::uint8_t get_material_type_from_name(const std::string& name)
			{
				auto prefix_idx = name.find("/");
				if (prefix_idx != std::string::npos && prefix_idx)
				{
					std::string prefix = std::string(name.begin(), name.begin() + prefix_idx);
					if (prefix == "m")
					{
						return H1::MTL_TYPE_MODEL;
					}
					else if (prefix == "me")
					{
						return H1::MTL_TYPE_MODEL_EFFECT;
					}
					else if (prefix == "mc")
					{
						return H1::MTL_TYPE_MODEL_VERTCOL;
					}
					else if (prefix == "w")
					{
						return H1::MTL_TYPE_WORLD;
					}
					else if (prefix == "wc")
					{
						return H1::MTL_TYPE_WORLD_VERTCOL;
					}
					else if (prefix != "gfx")
					{
						ZONETOOL_WARNING("Unknown material type for prefix \"%s\" (material: %s)", prefix.data(), name.data());
					}
				}
				return H1::MTL_TYPE_DEFAULT;
			}
		}
	}

	namespace IW5::H1Dumper
	{
		std::string clean_name(const std::string& name)
		{
			auto new_name = name;

			for (auto i = 0u; i < name.size(); i++)
			{
				switch (new_name[i])
				{
				case '*':
					new_name[i] = '_';
					break;
				}
			}

			return new_name;
		}

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(asset->entry);

#define MATERIAL_DUMP(entry) \
	matdata[#entry] = asset->entry;

#define MATERIAL_DUMP_INFO(entry) \
	matdata[#entry] = asset->info.entry;

		void dump(Material* asset)
		{
			if (asset)
			{
				auto c_name = clean_name(asset->name);

				const auto path = "materials\\"s + c_name + ".json"s;
				auto file = zonetool::filesystem::file(path);

				ordered_json matdata;

				MATERIAL_DUMP_STRING(name);

				std::string techset;
				std::string h1_techset;
				if (asset->techniqueSet)
				{
					techset = asset->techniqueSet->name;

					if (techset.ends_with(".3x")) // iw3xport
					{
						auto idx = techset.find(".3x");
						techset.erase(idx, 3);
					}

					if (techset.ends_with(".5x")) // iw5xport
					{
						auto idx = techset.find(".5x");
						techset.erase(idx, 3);
					}

					if (techset.starts_with("iw3/")) // iw3
					{
						techset.erase(0, 4);
					}

					if (techset.ends_with("_sat")) // iw5
					{
						auto idx = techset.find("_sat");
						techset.erase(idx, 4);
					}

					if (techset.ends_with("_z")) // codo
					{
						auto idx = techset.find("_z");
						techset.erase(idx, 2);
					}

					bool result = false;
					h1_techset = H1::get_h1_techset(techset, asset->name, &result, H1::get_h1_sortkey(asset->info.sortKey, asset->name));
					//if (!result)
					//{
					//	ZONETOOL_ERROR("Not dumping material \"%s\"", asset->name);
					//	return;
					//}
					matdata["techniqueSet->name"] = h1_techset;
					matdata["techniqueSet->og_name"] = techset;
				}

				matdata["gameFlags"] = asset->info.gameFlags; // convert
				matdata["sortKey"] = H1::get_h1_sortkey(asset->info.sortKey, asset->name, h1_techset);
				matdata["renderFlags"] = 0; // idk

				matdata["textureAtlasRowCount"] = asset->info.textureAtlasRowCount;
				matdata["textureAtlasColumnCount"] = asset->info.textureAtlasColumnCount;
				matdata["textureAtlasFrameBlend"] = 0;
				matdata["textureAtlasAsArray"] = 0;

				matdata["surfaceTypeBits"] = asset->info.surfaceTypeBits; // convert
				// hashIndex;

				matdata["stateFlags"] = asset->stateFlags; // same
				if (zonetool::dumping_source == dump_source::iw4 || zonetool::dumping_source == dump_source::iw5)
				{
					if ((asset->stateFlags & 0x2) != 0) // stateflag 0x2 is different for iw4 and iw5. it's not cull front
					{
						// fix stateflags, we could check if the material has front cull from loadbits but thats too much work, lets just use "none" if front is used...
						auto stateflags = asset->stateFlags & ~0x2;
						matdata["stateFlags"] = stateflags;
					}
				}
				
				matdata["cameraRegion"] = H1::get_h1_camera_region(asset->cameraRegion, asset->name, h1_techset);
				matdata["materialType"] = H1::get_material_type_from_name(asset->name);
				matdata["assetFlags"] = H1::MTL_ASSETFLAG_NONE;

				// fix gun sortkey
				if (matdata["sortKey"].get<int>() == 36)
				{
					if (matdata["cameraRegion"].get<int>() == H1::CAMERA_REGION_DEPTH_HACK)
						matdata["sortKey"] = 37;
					else if (matdata["cameraRegion"].get<int>() == H1::CAMERA_REGION_EMISSIVE)
						matdata["sortKey"] = 35;
				}

				ordered_json constant_table = json::array();
				for (int i = 0; i < asset->constantCount && techset != "2d"; i++)
				{
					ordered_json table;
					std::string constant_name = asset->constantTable[i].name;
					const auto constant_hash = asset->constantTable[i].nameHash;

					if (constant_name.size() > 12)
					{
						constant_name.resize(12);
					}

					if (h1_techset.find("_ct_") == std::string::npos || h1_techset.find("ct0") == std::string::npos)
					{
						if (constant_hash == 3054254906 &&
							asset->constantTable[i].literal[0] == 1.0f &&
							asset->constantTable[i].literal[1] == 1.0f &&
							asset->constantTable[i].literal[2] == 1.0f &&
							asset->constantTable[i].literal[3] == 1.0f)
						{
							continue;
						}
					}

					if (constant_hash == 332331467 || // dustParms
						constant_hash == 2222493873 || // dustTint
						constant_hash == 2276781490) // dustEyeParms
					{
						if (h1_techset.find("_du_dm_") == std::string::npos)
						{
							continue;
						}
					}

					if (constant_hash == 148072969) // detailScale
					{
						// Find positions of "d0" and "q0"
						const auto s1 = h1_techset.find("d0");
						const auto s1_1 = h1_techset.find("q0");

						// Ensure "d0" is not part of "sd0"
						bool isD0Valid = (s1 != std::string::npos) && (s1 == 0 || h1_techset[s1 - 1] != 's');

						// If either "d0" or "q0" is missing, continue
						if (!(isD0Valid || s1_1 != std::string::npos))
						{
							continue;
						}
					}

					table["name"] = constant_name.data();
					table["nameHash"] = asset->constantTable[i].nameHash;

					nlohmann::json literal_entry;

					literal_entry[0] = asset->constantTable[i].literal[0];
					literal_entry[1] = asset->constantTable[i].literal[1];
					literal_entry[2] = asset->constantTable[i].literal[2];
					literal_entry[3] = asset->constantTable[i].literal[3];

					if (constant_hash == 1033475292) // envMapParms
					{
						if (h1_techset.find("_lmpb_") != std::string::npos)
						{
							literal_entry[1] = asset->constantTable[i].literal[1] * 0.1f;
						}
					}

					table["literal"] = literal_entry;

					constant_table[constant_table.size()] = table;
				}

#define CONSTANT_TABLE_ADD_IF_NOT_FOUND(CONST_NAME, CONST_HASH, LITERAL_1, LITERAL_2, LITERAL_3, LITERAL_4) \
				{ \
					bool has_const = false; \
					std::size_t insert_position = constant_table.size(); \
					for (std::size_t i = 0; i < constant_table.size(); i++) \
					{ \
						if (constant_table[i]["nameHash"].get<std::size_t>() == CONST_HASH) \
						{ \
							has_const = true; \
							break; \
						} \
						if (constant_table[i]["nameHash"].get<std::size_t>() > CONST_HASH) \
						{ \
							insert_position = i; \
							break; \
						} \
					} \
					if (!has_const) \
					{ \
						ordered_json table; \
						table["name"] = CONST_NAME; \
						table["nameHash"] = CONST_HASH; \
						nlohmann::json literal_entry; \
						literal_entry[0] = LITERAL_1; \
						literal_entry[1] = LITERAL_2; \
						literal_entry[2] = LITERAL_3; \
						literal_entry[3] = LITERAL_4; \
						table["literal"] = literal_entry; \
						constant_table.insert(constant_table.begin() + insert_position, table); \
					} \
				}

				if (h1_techset.find("sd0") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("envMapParms", 1033475292, 0.0f, 0.0f, 0.0f, 0.0f);
				}

				if (h1_techset.find("_flag_") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("flagParms", 2292903761, 1.0f, 0.0f, 0.0f, 0.0f);
				}

				if (h1_techset.find("_cso_") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("hdrColorParm", 3379990750, 1.0f, 0.0f, 0.0f, 0.0f);
				}

				if (h1_techset.find("_zf_") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("featherParms", 1300144692, 0.025f, 40.0f, 0.0f, 0.0f);
				}

				if (h1_techset.find("shadowcaster") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("uvScale", 3809220315, 1.0f, 1.0f, 0.0f, 0.0f);
				}

				if (h1_techset.find("_ct_") != std::string::npos || h1_techset.find("ct0") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("colorTint", 3054254906, 1.0f, 1.0f, 1.0f, 1.0f);
				}

				if (h1_techset.find("_objective2") != std::string::npos)
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("colorObjHdrP", 3345677433, 1.0f, 0.0f, 0.0f, 0.0f);
				}

				if (h1_techset == "mc_reflexsight" || h1_techset == "m_reflexsight")
				{
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("detailScale", 148072969, 40.0f, 40.0f, 0.0f, 0.0f);
					CONSTANT_TABLE_ADD_IF_NOT_FOUND("sceneLumParm", 3719717047, 0.0f, 0.0f, 0.0f, 0.0f);
				}

				matdata["constantTable"] = constant_table;

				ordered_json material_images = json::array();
				for (auto i = 0; i < asset->textureCount; i++)
				{
					ordered_json image;
					if (asset->textureTable[i].semantic == 11)
					{
						auto* water = reinterpret_cast<water_t*>(asset->textureTable[i].u.image);
						if (water->image && water->image->name)
						{
							image["image"] = water->image->name;
						}
						else
						{
							image["image"] = "";
						}
						//image["waterinfo"] = nullptr;
					}
					else
					{
						if (asset->textureTable[i].u.image && asset->textureTable[i].u.image->name)
						{
							image["image"] = asset->textureTable[i].u.image->name;
						}
						else
						{
							ZONETOOL_WARNING("material %s has an empty image, assigning default...", asset->name);
							image["image"] = "default";
						}
					}

					image["semantic"] = asset->textureTable[i].semantic == 11 ? 2 : asset->textureTable[i].semantic; // convert? ( should be the same )
					image["samplerState"] = asset->textureTable[i].samplerState == 11 ? 19 : asset->textureTable[i].samplerState; // convert? ( should be fine )
					image["lastCharacter"] = asset->textureTable[i].nameEnd;
					image["firstCharacter"] = asset->textureTable[i].nameStart;
					image["typeHash"] = asset->textureTable[i].nameHash;

					// add image data to material
					material_images.push_back(image);
				}

#define IMAGE_ADD_IF_NOT_FOUND(IMAGE, SEMANTIC, SAMPLER_STATE, LAST_CHARACTER, FIRST_CHARACTER, HASH) \
				bool has_image = false; \
				std::size_t insert_position = material_images.size(); \
				for (std::size_t i = 0; i < material_images.size(); i++) \
				{ \
					if (material_images[i]["typeHash"].get<std::size_t>() == HASH) \
					{ \
						has_image = true; \
						break; \
					} \
					if (material_images[i]["typeHash"].get<std::size_t>() > HASH) \
					{ \
						insert_position = i; \
						break; \
					} \
				} \
				if (!has_image) \
				{ \
					ordered_json image; \
					image["image"] = IMAGE; \
					image["semantic"] = SEMANTIC; \
					image["samplerState"] = SAMPLER_STATE; \
					image["lastCharacter"] = LAST_CHARACTER; \
					image["firstCharacter"] = FIRST_CHARACTER; \
					image["typeHash"] = HASH; \
					material_images.insert(material_images.begin() + insert_position, image); \
				}

				if (h1_techset.find("n0") != std::string::npos)
				{
					IMAGE_ADD_IF_NOT_FOUND("$identitynormalmap", 5, 1, 112, 110, 1507003663);
				}

				if (h1_techset.find("sd0") != std::string::npos)
				{
					IMAGE_ADD_IF_NOT_FOUND("$identitynormalmap", 8, 19, 112, 115, 887934131); // ~$white-r32g32b32l
				}

				if (h1_techset.find("om0") != std::string::npos)
				{
					IMAGE_ADD_IF_NOT_FOUND("~$white-l", 16, 19, 112, 111, 1610742065); // ~$white-l02
				}

				if (h1_techset.find("_lmpb_") != std::string::npos || h1_techset.find("_flag_") != std::string::npos)
				{
					IMAGE_ADD_IF_NOT_FOUND("~envbrdflut_ggx_16-rg", 15, 226, 116, 101, 3447584578);
				}

				matdata["textureTable"] = material_images;

				auto str = matdata.dump(4, ' ', true, nlohmann::detail::error_handler_t::replace);

				matdata.clear();

				file.open("wb");
				file.write(str);
				file.close();
			}
		}
	}
}