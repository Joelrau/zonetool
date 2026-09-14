#include "stdafx.hpp"
#include "../Include.hpp"

#include "PackedImage.hpp"
#include "IwiImage.hpp"

#include <cstring>

namespace ZoneTool::IW5
{
	namespace IW7Converter
	{
		// Builds IW7's packed base-layer textures from IW5/IW3 sources.
		//
		// Every real IW7 material's base layer is semantic 14 (_cs) plus semantic 15 (_ng): of
		// 11,313 stock materials carrying textures, only 247 use semantic 5 or 8 without them and
		// all 247 are tools_* debug shaders. The separate colour/normal/specular arrangement IW5
		// uses only ever appears in IW7 as an *extra blend layer*.
		//
		// Channel roles and the hemi-octahedral normal encoding are documented, with the
		// measurements behind them, in docs/iw7-packed-textures.md. GameImageUtil's CoDNOGProcessor
		// and CoDFusedCSProcessor are the reference for reading these back.
		namespace
		{
			// ---- source decoding ---------------------------------------------------------------

			void bc1_decode_rgba(const std::uint8_t* block, std::uint8_t out[16][4])
			{
				const unsigned int c0 = block[0] | (block[1] << 8);
				const unsigned int c1 = block[2] | (block[3] << 8);

				auto expand = [](unsigned int c, float rgb[3])
				{
					rgb[0] = static_cast<float>(((c >> 11) & 31) * 255 / 31);
					rgb[1] = static_cast<float>(((c >> 5) & 63) * 255 / 63);
					rgb[2] = static_cast<float>((c & 31) * 255 / 31);
				};

				float p[4][4]{};
				expand(c0, p[0]);
				expand(c1, p[1]);
				p[0][3] = p[1][3] = 255.0f;

				if (c0 > c1)
				{
					for (int k = 0; k < 3; k++)
					{
						p[2][k] = (2.0f * p[0][k] + p[1][k]) / 3.0f;
						p[3][k] = (p[0][k] + 2.0f * p[1][k]) / 3.0f;
					}
					p[2][3] = p[3][3] = 255.0f;
				}
				else
				{
					for (int k = 0; k < 3; k++)
					{
						p[2][k] = (p[0][k] + p[1][k]) * 0.5f;
						p[3][k] = 0.0f;
					}
					p[2][3] = 255.0f;
					p[3][3] = 0.0f;   // the punch-through mode's transparent entry
				}

				const unsigned int bits = block[4] | (block[5] << 8) | (block[6] << 16) | (block[7] << 24);
				for (int t = 0; t < 16; t++)
				{
					const auto e = (bits >> (2 * t)) & 3;
					for (int k = 0; k < 4; k++)
					{
						out[t][k] = static_cast<std::uint8_t>(std::clamp(p[e][k], 0.0f, 255.0f));
					}
				}
			}

			void bc4_decode_block(const std::uint8_t* block, float out[16])
			{
				const float a0 = block[0];
				const float a1 = block[1];

				float pal[8];
				pal[0] = a0;
				pal[1] = a1;
				if (a0 > a1)
				{
					for (int k = 1; k <= 6; k++)
					{
						pal[1 + k] = ((7 - k) * a0 + k * a1) / 7.0f;
					}
				}
				else
				{
					for (int k = 1; k <= 4; k++)
					{
						pal[1 + k] = ((5 - k) * a0 + k * a1) / 5.0f;
					}
					pal[6] = 0.0f;
					pal[7] = 255.0f;
				}

				std::uint64_t bits = 0;
				for (int i = 0; i < 6; i++)
				{
					bits |= static_cast<std::uint64_t>(block[2 + i]) << (8 * i);
				}
				for (int t = 0; t < 16; t++)
				{
					out[t] = pal[(bits >> (3 * t)) & 7];
				}
			}

			void store_block(std::vector<std::uint8_t>& dst, unsigned int w, unsigned int h,
				unsigned int bx, unsigned int by, const std::uint8_t texel[16][4])
			{
				for (int t = 0; t < 16; t++)
				{
					const auto x = bx * 4 + (t % 4);
					const auto y = by * 4 + (t / 4);
					if (x >= w || y >= h)
					{
						continue;
					}
					std::memcpy(&dst[(static_cast<std::size_t>(y) * w + x) * 4], texel[t], 4);
				}
			}

			bool decode_mip(std::uint8_t format, const std::uint8_t* src, std::size_t src_size,
				unsigned int w, unsigned int h, std::vector<std::uint8_t>& out)
			{
				out.assign(static_cast<std::size_t>(w) * h * 4, 0);

				const auto bw = std::max(1u, (w + 3) / 4);
				const auto bh = std::max(1u, (h + 3) / 4);

				if (format == 11) // DXT1
				{
					if (src_size < static_cast<std::size_t>(bw) * bh * 8) return false;
					for (unsigned int by = 0; by < bh; by++)
						for (unsigned int bx = 0; bx < bw; bx++)
						{
							std::uint8_t texel[16][4];
							bc1_decode_rgba(src + (static_cast<std::size_t>(by) * bw + bx) * 8, texel);
							store_block(out, w, h, bx, by, texel);
						}
					return true;
				}

				if (format == 12 || format == 13) // DXT3 / DXT5
				{
					if (src_size < static_cast<std::size_t>(bw) * bh * 16) return false;
					for (unsigned int by = 0; by < bh; by++)
						for (unsigned int bx = 0; bx < bw; bx++)
						{
							const auto* blk = src + (static_cast<std::size_t>(by) * bw + bx) * 16;
							std::uint8_t texel[16][4];
							bc1_decode_rgba(blk + 8, texel);

							if (format == 13)
							{
								float a[16];
								bc4_decode_block(blk, a);
								for (int t = 0; t < 16; t++)
								{
									texel[t][3] = static_cast<std::uint8_t>(std::clamp(a[t], 0.0f, 255.0f));
								}
							}
							else // DXT3: 4 bits of explicit alpha per texel
							{
								for (int t = 0; t < 16; t++)
								{
									const auto nib = (blk[t / 2] >> ((t & 1) * 4)) & 0xF;
									texel[t][3] = static_cast<std::uint8_t>(nib * 17);
								}
							}
							store_block(out, w, h, bx, by, texel);
						}
					return true;
				}

				const auto pixels = static_cast<std::size_t>(w) * h;
				if (format == 1) // BITMAP_RGBA
				{
					if (src_size < pixels * 4) return false;
					std::memcpy(out.data(), src, pixels * 4);
					return true;
				}
				if (format == 2) // BITMAP_RGB
				{
					if (src_size < pixels * 3) return false;
					for (std::size_t p = 0; p < pixels; p++)
					{
						out[p * 4 + 0] = src[p * 3 + 0];
						out[p * 4 + 1] = src[p * 3 + 1];
						out[p * 4 + 2] = src[p * 3 + 2];
						out[p * 4 + 3] = 255;
					}
					return true;
				}
				if (format == 4 || format == 5) // LUMINANCE / ALPHA
				{
					if (src_size < pixels) return false;
					for (std::size_t p = 0; p < pixels; p++)
					{
						out[p * 4 + 0] = out[p * 4 + 1] = out[p * 4 + 2] = src[p];
						out[p * 4 + 3] = 255;
					}
					return true;
				}
				if (format == 200) // resident D3DFMT_A8R8G8B8, stored B G R A
				{
					if (src_size < pixels * 4) return false;
					for (std::size_t p = 0; p < pixels; p++)
					{
						out[p * 4 + 0] = src[p * 4 + 2];
						out[p * 4 + 1] = src[p * 4 + 1];
						out[p * 4 + 2] = src[p * 4 + 0];
						out[p * 4 + 3] = src[p * 4 + 3];
					}
					return true;
				}
				return false;
			}

			// ---- BC7 mode 6 encoder ------------------------------------------------------------
			//
			// One subset, RGBA, endpoints 7.7.7.7 plus a p-bit each, 4-bit indices. Validated
			// against a reference implementation on real assembled _ng data: mean error 0.28-1.02
			// out of 255 across three textures, which is inside the quantisation the DXT sources
			// already carry.

			const float kW4[16] = { 0, 4, 9, 13, 17, 21, 26, 30, 34, 38, 43, 47, 51, 55, 60, 64 };

			struct bits128
			{
				std::uint64_t lo = 0, hi = 0;

				void put(int pos, int width, std::uint64_t value)
				{
					value &= (width == 64) ? ~0ull : ((1ull << width) - 1);
					if (pos < 64)
					{
						lo |= value << pos;
						if (pos + width > 64)
						{
							hi |= value >> (64 - pos);
						}
					}
					else
					{
						hi |= value << (pos - 64);
					}
				}
			};

			void quantise_endpoint(const float e[4], int p, int v[4], float q[4])
			{
				for (int c = 0; c < 4; c++)
				{
					v[c] = std::clamp(static_cast<int>(std::lround((e[c] - p) * 0.5f)), 0, 127);
					q[c] = static_cast<float>((v[c] << 1) | p);
				}
			}

			// Dominant axis by power iteration - enough for a 16-texel block and avoids pulling in
			// an eigensolver. Seeded from the bounding-box diagonal.
			void principal_axis(const float t[16][4], const float mean[4], float axis[4])
			{
				float lo[4], hi[4];
				for (int c = 0; c < 4; c++) { lo[c] = t[0][c]; hi[c] = t[0][c]; }
				for (int i = 1; i < 16; i++)
					for (int c = 0; c < 4; c++)
					{
						lo[c] = std::min(lo[c], t[i][c]);
						hi[c] = std::max(hi[c], t[i][c]);
					}
				for (int c = 0; c < 4; c++) { axis[c] = hi[c] - lo[c]; }

				for (int it = 0; it < 8; it++)
				{
					float next[4]{};
					for (int i = 0; i < 16; i++)
					{
						float d = 0.0f;
						for (int c = 0; c < 4; c++) { d += (t[i][c] - mean[c]) * axis[c]; }
						for (int c = 0; c < 4; c++) { next[c] += (t[i][c] - mean[c]) * d; }
					}
					float len = 0.0f;
					for (int c = 0; c < 4; c++) { len += next[c] * next[c]; }
					if (len <= 1e-9f)
					{
						return;
					}
					len = std::sqrt(len);
					for (int c = 0; c < 4; c++) { axis[c] = next[c] / len; }
				}
			}

			void encode_bc7_block(const float t[16][4], std::uint8_t* out)
			{
				float mean[4]{};
				for (int i = 0; i < 16; i++)
					for (int c = 0; c < 4; c++) { mean[c] += t[i][c] / 16.0f; }

				float axis[4];
				principal_axis(t, mean, axis);

				float e0[4], e1[4];
				{
					float pmin = 1e30f, pmax = -1e30f;
					for (int i = 0; i < 16; i++)
					{
						float d = 0.0f;
						for (int c = 0; c < 4; c++) { d += (t[i][c] - mean[c]) * axis[c]; }
						pmin = std::min(pmin, d);
						pmax = std::max(pmax, d);
					}
					for (int c = 0; c < 4; c++)
					{
						e0[c] = std::clamp(mean[c] + pmin * axis[c], 0.0f, 255.0f);
						e1[c] = std::clamp(mean[c] + pmax * axis[c], 0.0f, 255.0f);
					}
				}

				int best_v0[4]{}, best_v1[4]{}, best_p0 = 0, best_p1 = 0, best_idx[16]{};
				float best_err = 1e30f;

				for (int pass = 0; pass < 3; pass++)
				{
					int pass_v0[4]{}, pass_v1[4]{}, pass_p0 = 0, pass_p1 = 0, pass_idx[16]{};
					float pass_err = 1e30f;

					for (int p0 = 0; p0 < 2; p0++)
						for (int p1 = 0; p1 < 2; p1++)
						{
							int v0[4], v1[4];
							float q0[4], q1[4];
							quantise_endpoint(e0, p0, v0, q0);
							quantise_endpoint(e1, p1, v1, q1);

							float pal[16][4];
							for (int k = 0; k < 16; k++)
								for (int c = 0; c < 4; c++)
								{
									pal[k][c] = std::floor((q0[c] * (64.0f - kW4[k]) + q1[c] * kW4[k] + 32.0f) / 64.0f);
								}

							// The palette is monotonic along the endpoint line, so project onto it
							// for a starting index and only check the two neighbours. Exhaustively
							// scanning all 16 entries gives the same answer and is 5x the work,
							// which matters in a debug build encoding a whole mip chain.
							float dir[4], dlen = 0.0f;
							for (int c = 0; c < 4; c++)
							{
								dir[c] = q1[c] - q0[c];
								dlen += dir[c] * dir[c];
							}
							const auto inv_dlen = dlen > 1e-6f ? 1.0f / dlen : 0.0f;

							float err = 0.0f;
							int idx[16];
							for (int i = 0; i < 16; i++)
							{
								float proj = 0.0f;
								for (int c = 0; c < 4; c++)
								{
									proj += (t[i][c] - q0[c]) * dir[c];
								}
								const auto guess = std::clamp(
									static_cast<int>(std::lround(proj * inv_dlen * 15.0f)), 0, 15);

								float bd = 1e30f;
								int bk = guess;
								for (int k = std::max(0, guess - 1); k <= std::min(15, guess + 1); k++)
								{
									float d = 0.0f;
									for (int c = 0; c < 4; c++)
									{
										const auto diff = t[i][c] - pal[k][c];
										d += diff * diff;
									}
									if (d < bd) { bd = d; bk = k; }
								}
								idx[i] = bk;
								err += bd;
							}

							if (err < pass_err)
							{
								pass_err = err;
								pass_p0 = p0; pass_p1 = p1;
								std::memcpy(pass_v0, v0, sizeof(v0));
								std::memcpy(pass_v1, v1, sizeof(v1));
								std::memcpy(pass_idx, idx, sizeof(idx));
							}
						}

					if (pass_err < best_err)
					{
						best_err = pass_err;
						best_p0 = pass_p0; best_p1 = pass_p1;
						std::memcpy(best_v0, pass_v0, sizeof(pass_v0));
						std::memcpy(best_v1, pass_v1, sizeof(pass_v1));
						std::memcpy(best_idx, pass_idx, sizeof(pass_idx));
					}

					// least-squares refit of the endpoints against the chosen indices
					float a = 0.0f, b = 0.0f, d = 0.0f, pv[4]{}, qv[4]{};
					for (int i = 0; i < 16; i++)
					{
						const auto w = kW4[pass_idx[i]] / 64.0f;
						a += (1.0f - w) * (1.0f - w);
						b += (1.0f - w) * w;
						d += w * w;
						for (int c = 0; c < 4; c++)
						{
							pv[c] += (1.0f - w) * t[i][c];
							qv[c] += w * t[i][c];
						}
					}
					const auto det = a * d - b * b;
					if (std::fabs(det) < 1e-6f)
					{
						break;
					}
					for (int c = 0; c < 4; c++)
					{
						e0[c] = std::clamp((d * pv[c] - b * qv[c]) / det, 0.0f, 255.0f);
						e1[c] = std::clamp((a * qv[c] - b * pv[c]) / det, 0.0f, 255.0f);
					}
				}

				// the anchor's high index bit is implicit, so index 0 must be <= 7
				if (best_idx[0] > 7)
				{
					for (int c = 0; c < 4; c++) { std::swap(best_v0[c], best_v1[c]); }
					std::swap(best_p0, best_p1);
					for (int i = 0; i < 16; i++) { best_idx[i] = 15 - best_idx[i]; }
				}

				bits128 bits;
				bits.put(6, 1, 1);                       // mode 6 marker
				int pos = 7;
				for (int c = 0; c < 4; c++)
				{
					bits.put(pos, 7, best_v0[c]); pos += 7;
					bits.put(pos, 7, best_v1[c]); pos += 7;
				}
				bits.put(63, 1, best_p0);
				bits.put(64, 1, best_p1);
				pos = 65;
				bits.put(pos, 3, best_idx[0]); pos += 3;
				for (int i = 1; i < 16; i++)
				{
					bits.put(pos, 4, best_idx[i]); pos += 4;
				}

				std::memcpy(out, &bits.lo, 8);
				std::memcpy(out + 8, &bits.hi, 8);
			}

			// BC4: one 8-bit channel, 8 bytes a block - two endpoints then sixteen 3-bit
			// indices. Only the eight-value mode is emitted (red0 > red1), whose palette is the
			// two endpoints plus six evenly spaced interpolants; the six-value mode buys a hard
			// 0 and 255 at the cost of two interpolants, which is the wrong trade for an alpha
			// mask that is mostly flat runs. Endpoints are the block's own min and max, so a
			// block of one value is exact and a smooth block lands within half a step.
			void encode_bc4_block(const std::uint8_t v[16], std::uint8_t* out)
			{
				std::uint8_t lo = 255, hi = 0;
				for (int k = 0; k < 16; k++)
				{
					lo = std::min(lo, v[k]);
					hi = std::max(hi, v[k]);
				}

				out[0] = hi;
				out[1] = lo;

				// A flat block leaves red0 == red1, which selects the six-value mode - but there
				// index 0 still reads red0, so all-zero indices reproduce it exactly.
				std::uint64_t bits = 0;
				if (hi > lo)
				{
					float pal[8];
					pal[0] = static_cast<float>(hi);
					pal[1] = static_cast<float>(lo);
					for (int k = 1; k <= 6; k++)
					{
						pal[1 + k] = ((7 - k) * hi + k * lo) / 7.0f;
					}

					for (int t = 0; t < 16; t++)
					{
						int best = 0;
						auto best_err = std::fabs(pal[0] - v[t]);
						for (int i = 1; i < 8; i++)
						{
							const auto err = std::fabs(pal[i] - v[t]);
							if (err < best_err) { best_err = err; best = i; }
						}
						bits |= static_cast<std::uint64_t>(best) << (3 * t);
					}
				}

				for (int i = 0; i < 6; i++)
				{
					out[2 + i] = static_cast<std::uint8_t>((bits >> (8 * i)) & 0xFF);
				}
			}

			void encode_bc4_mip(const std::vector<std::uint8_t>& rgba, unsigned int w, unsigned int h,
				int channel, std::vector<std::uint8_t>& out)
			{
				const auto bw = std::max(1u, (w + 3) / 4);
				const auto bh = std::max(1u, (h + 3) / 4);
				out.assign(static_cast<std::size_t>(bw) * bh * 8, 0);

				for (unsigned int by = 0; by < bh; by++)
					for (unsigned int bx = 0; bx < bw; bx++)
					{
						std::uint8_t t[16];
						for (int k = 0; k < 16; k++)
						{
							const auto x = std::min(bx * 4 + (k % 4), w - 1);
							const auto y = std::min(by * 4 + (k / 4), h - 1);
							t[k] = rgba[(static_cast<std::size_t>(y) * w + x) * 4 + channel];
						}
						encode_bc4_block(t, &out[(static_cast<std::size_t>(by) * bw + bx) * 8]);
					}
			}

			void encode_bc7_mip(const std::vector<std::uint8_t>& rgba, unsigned int w, unsigned int h,
				std::vector<std::uint8_t>& out)
			{
				const auto bw = std::max(1u, (w + 3) / 4);
				const auto bh = std::max(1u, (h + 3) / 4);
				out.assign(static_cast<std::size_t>(bw) * bh * 16, 0);

				for (unsigned int by = 0; by < bh; by++)
					for (unsigned int bx = 0; bx < bw; bx++)
					{
						float t[16][4];
						for (int k = 0; k < 16; k++)
						{
							const auto x = std::min(bx * 4 + (k % 4), w - 1);
							const auto y = std::min(by * 4 + (k / 4), h - 1);
							const auto* p = &rgba[(static_cast<std::size_t>(y) * w + x) * 4];
							for (int c = 0; c < 4; c++) { t[k][c] = static_cast<float>(p[c]); }
						}
						encode_bc7_block(t, &out[(static_cast<std::size_t>(by) * bw + bx) * 16]);
					}
			}

			// ---- assembly ----------------------------------------------------------------------

			// Nearest-neighbour fetch, so a secondary map of a different size still lines up.
			const std::uint8_t* sample(const decoded_image& img, std::size_t level,
				unsigned int x, unsigned int y, unsigned int w, unsigned int h)
			{
				const auto lvl = std::min(level, img.mips.size() - 1);
				auto lw = static_cast<unsigned int>(img.width);
				auto lh = static_cast<unsigned int>(img.height);
				for (std::size_t i = 0; i < lvl; i++)
				{
					lw = std::max(1u, lw / 2);
					lh = std::max(1u, lh / 2);
				}
				const auto sx = std::min(lw - 1, w > 1 ? x * lw / w : 0u);
				const auto sy = std::min(lh - 1, h > 1 ? y * lh / h : 0u);
				return &img.mips[lvl][(static_cast<std::size_t>(sy) * lw + sx) * 4];
			}

			IW7::GfxImage* finish(const char* name, std::uint8_t semantic, unsigned short width,
				unsigned short height, const std::vector<std::vector<std::uint8_t>>& mips,
				allocator& mem, DXGI_FORMAT format = DXGI_FORMAT_BC7_UNORM)
			{
				std::size_t total = 0;
				for (const auto& m : mips) { total += m.size(); }

				auto* pixels = mem.allocate<std::uint8_t>(static_cast<unsigned int>(total));
				std::size_t off = 0;
				for (const auto& m : mips)
				{
					std::memcpy(pixels + off, m.data(), m.size());
					off += m.size();
				}

				const auto image = mem.allocate<IW7::GfxImage>();
				image->name = mem.duplicate_string(name);
				image->imageFormat = format;
				image->flags = 0;
				image->mapType = IW7::MAPTYPE_2D;
				image->semantic = static_cast<IW7::TextureSemantic>(semantic);
				image->category = IW7::IMG_CATEGORY_LOAD_FROM_FILE;
				image->picmip.platform[0] = 0;
				image->picmip.platform[1] = 2;
				image->dataLen1 = static_cast<unsigned int>(total);
				image->dataLen2 = image->dataLen1;
				image->width = width;
				image->height = height;
				image->depth = 1;
				image->numElements = 1;
				image->levelCount = static_cast<unsigned char>(mips.size());
				image->streamed = false;
				image->pixelData = pixels;
				return image;
			}
		}

		bool decode_source(GfxImage* asset, decoded_image& out)
		{
			source_image src{};
			if (!load_source(asset, src))
			{
				return false;
			}

			out.width = src.width;
			out.height = src.height;
			out.mips.clear();

			auto w = static_cast<unsigned int>(src.width);
			auto h = static_cast<unsigned int>(src.height);
			for (const auto& level : src.levels)   // largest first
			{
				std::vector<std::uint8_t> rgba;
				if (!decode_mip(src.format, level.first, level.second, w, h, rgba))
				{
					return !out.mips.empty();
				}
				out.mips.push_back(std::move(rgba));
				w = std::max(1u, w / 2);
				h = std::max(1u, h / 2);
			}
			return !out.mips.empty();
		}

		IW7::GfxImage* build_packed_cs(const char* name, const decoded_image& colour,
			const decoded_image* spec, allocator& mem)
		{
			// IW5 has no metalness channel, so everything converts as a dielectric: RGB is the
			// albedo unchanged and alpha sits below GameImageUtil's insulatorSpecRange of 0.1, which
			// makes the shader read a flat ~0.04 reflectance. 10/255 is what the one resident stock
			// _cs carries. The IW5 specular *colour* is dropped here - only its gloss survives, in
			// the paired _ng - because inventing metalness from a bright specular would be a guess.
			constexpr std::uint8_t dielectric_reflectance = 10;

			std::vector<std::vector<std::uint8_t>> out;
			auto w = static_cast<unsigned int>(colour.width);
			auto h = static_cast<unsigned int>(colour.height);

			for (std::size_t level = 0; level < colour.mips.size(); level++)
			{
				std::vector<std::uint8_t> rgba(static_cast<std::size_t>(w) * h * 4);
				for (unsigned int y = 0; y < h; y++)
					for (unsigned int x = 0; x < w; x++)
					{
						const auto* c = &colour.mips[level][(static_cast<std::size_t>(y) * w + x) * 4];
						auto* d = &rgba[(static_cast<std::size_t>(y) * w + x) * 4];
						d[0] = c[0];
						d[1] = c[1];
						d[2] = c[2];
						d[3] = dielectric_reflectance;
					}

				std::vector<std::uint8_t> encoded;
				encode_bc7_mip(rgba, w, h, encoded);
				out.push_back(std::move(encoded));

				w = std::max(1u, w / 2);
				h = std::max(1u, h / 2);
			}

			(void)spec;
			return finish(name, IW7::TextureSemantic::TS_COLOR_SPECULAR_MAP, colour.width, colour.height,
				out, mem);
		}

		IW7::GfxImage* build_packed_a(const char* name, const decoded_image& colour, allocator& mem)
		{
			// Semantic 16 on a pa0 technique: the opacity the packed pair cannot carry, because
			// _packed_cs spends its alpha on reflectance. One channel, so BC4 rather than BC7 -
			// which is also what stock ships (imageFormat 80 on all 2360 stock _packed_a).
			//
			// The source is the colour map's own alpha, which is where IW3 and IW5 keep opacity
			// for a blend material and the cutout mask for an alpha-tested one.
			std::vector<std::vector<std::uint8_t>> out;
			auto w = static_cast<unsigned int>(colour.width);
			auto h = static_cast<unsigned int>(colour.height);

			for (std::size_t level = 0; level < colour.mips.size(); level++)
			{
				std::vector<std::uint8_t> encoded;
				encode_bc4_mip(colour.mips[level], w, h, 3, encoded);
				out.push_back(std::move(encoded));

				w = std::max(1u, w / 2);
				h = std::max(1u, h / 2);
			}

			return finish(name, IW7::TextureSemantic::TS_ALPHA_REVEAL_THICKNESS_MAP, colour.width,
				colour.height, out, mem, DXGI_FORMAT_BC4_UNORM);
		}

		IW7::GfxImage* build_packed_ng(const char* name, const decoded_image& normal,
			const decoded_image* spec, allocator& mem)
		{
			// Gloss for materials whose specular carried none. IW3's combined
			// "~<spec>-rgb&<cos>-l-11" images do carry a real one; a plain DXT1 specular does not,
			// and neither does a material with no specular at all.
			constexpr std::uint8_t fallback_gloss = 64;

			// No IW5 source has baked occlusion. 255 is what a stock _ng carries (measured 254.6
			// +/- 0.5); only the _nog variant has a real AO channel.
			constexpr std::uint8_t no_occlusion = 255;

			std::vector<std::vector<std::uint8_t>> out;
			auto w = static_cast<unsigned int>(normal.width);
			auto h = static_cast<unsigned int>(normal.height);

			for (std::size_t level = 0; level < normal.mips.size(); level++)
			{
				std::vector<std::uint8_t> rgba(static_cast<std::size_t>(w) * h * 4);
				for (unsigned int y = 0; y < h; y++)
					for (unsigned int x = 0; x < w; x++)
					{
						const auto* n = &normal.mips[level][(static_cast<std::size_t>(y) * w + x) * 4];

						// IW5/IW3 normal: X in alpha, Y in the greyscale colour block, Z implied
						const auto nx = n[3] / 127.5f - 1.0f;
						const auto ny = n[1] / 127.5f - 1.0f;
						const auto nz2 = 1.0f - nx * nx - ny * ny;
						const auto nz = nz2 > 0.0f ? std::sqrt(nz2) : 0.0f;

						// hemi-octahedron: project onto the octahedron, then rotate 45 degrees so
						// the diamond |x|+|y| <= 1 fills the whole unit square
						const auto len = std::fabs(nx) + std::fabs(ny) + nz;
						const auto inv = len > 1e-6f ? 1.0f / len : 0.0f;
						const auto px = nx * inv;
						const auto py = ny * inv;

						auto encode = [](float v)
						{
							return static_cast<std::uint8_t>(
								std::clamp(std::lround((v * 0.5f + 0.5f) * 255.0f), 0L, 255L));
						};

						std::uint8_t gloss = fallback_gloss;
						if (spec)
						{
							gloss = sample(*spec, level, x, y, w, h)[3];
						}

						auto* d = &rgba[(static_cast<std::size_t>(y) * w + x) * 4];
						d[0] = gloss;
						d[1] = encode(px + py);   // normal X
						d[2] = no_occlusion;
						d[3] = encode(px - py);   // normal Y
					}

				std::vector<std::uint8_t> encoded;
				encode_bc7_mip(rgba, w, h, encoded);
				out.push_back(std::move(encoded));

				w = std::max(1u, w / 2);
				h = std::max(1u, h / 2);
			}

			return finish(name, IW7::TextureSemantic::TS_NORMAL_OCCLUSSION_GLOSS_MAP, normal.width,
				normal.height, out, mem);
		}
	}
}
