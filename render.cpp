#include "render.h"

#if DEF_SWR

#include "archive.hpp"

#include "glm\glm.hpp"
#include "glm/gtc\matrix_transform.hpp"
#include "glm/gtc\type_ptr.hpp"

#include "ltrrender\ltrrender.h"

#ifdef LR_MULTI_THREAD
#include <thread> 
#endif

#include "input.h"

graphics::Texture tx;

graphics::colour tx_new[(LR_RENDER_H * LR_RENDER_W) + 4];
// 64 bits extra attached to columns so that we can safely modify 64 bit chunks even on odd-sized screens
li16 tx_depth2[(LR_RENDER_W * LR_RENDER_H) + 4];

m::Vector3 viewP;
m::Quaternion viewR;

m::Vector3 ProjectNormal(m::Vector3 input, graphics::Matrix4x4* modelmat) {
	glm::vec4 glpos = glm::vec4(input.x, input.y, -input.z, 0.f);
	glm::vec4 pr = (*(glm::mat4*)modelmat * glpos);

	return m::Normalize(m::Vector3(pr.x, pr.y, pr.z));
}
glm::vec4 ProjectVert(m::Vector3 input, graphics::Matrix4x4* modelmat) {
	glm::vec4 glpos = glm::vec4(input.x, input.y, -input.z, 1.f);
	glm::mat4 matv = graphics::GetMatView();
	glm::mat4 matp = graphics::GetMatProj();
	glm::vec4 pr = matp * matv * (*(glm::mat4*)modelmat * glpos);

	pr.z = -pr.z; // Don't ask me
	pr.y = -pr.y; // Don't ask me

	pr.x /= pr.w;
	pr.y /= pr.w;
	pr.z /= pr.w;

	return pr;
}

#define NEAR_CLIPPING 0
#define CLIP_OFFSET 0.00f

#define TF_OFFSET_X LR_RENDER_W * 0.5
#define TF_OFFSET_Y LR_RENDER_H * 0.5

#define TF_SCALE_X LR_RENDER_W * 0.5
#define TF_SCALE_Y LR_RENDER_H * 0.5

// DEBUG
//#define TF_SCALE_X LR_RENDER_W * 0.125
//#define TF_SCALE_Y LR_RENDER_H * 0.125

void RenderDrawMesh(graphics::Mesh* mesh, graphics::Texture* txtr, graphics::Matrix4x4 matr)
{
	Vertex* vces = mesh->Vces();
	lui32* ices = mesh->Ices();
	lui32 icount = mesh->IcesSize();

	for (int i = 0; i < icount; i += 3u)
	{
		m::Quaternion r;

		// rotated normals
		m::Vector3 newnor1 = ProjectNormal(m::Vector3(vces[ices[i]].nor.x, vces[ices[i]].nor.y, vces[ices[i]].nor.z), &matr);
		m::Vector3 newnor2 = ProjectNormal(m::Vector3(vces[ices[i + 1]].nor.x, vces[ices[i + 1]].nor.y, vces[ices[i + 1]].nor.z), &matr);
		m::Vector3 newnor3 = ProjectNormal(m::Vector3(vces[ices[i + 2]].nor.x, vces[ices[i + 2]].nor.y, vces[ices[i + 2]].nor.z), &matr);

		// dots
		lf32 ndotl = m::Clamp(m::Dot(newnor1, m::Normalize(m::Vector3(0.5f, 1.f, 0.5f))), 0.f, 1.f);
		lf32 ndotl_b = m::Clamp(m::Dot(newnor2, m::Normalize(m::Vector3(0.5f, 1.f, 0.5f))), 0.f, 1.f);
		lf32 ndotl_c = m::Clamp(m::Dot(newnor3, m::Normalize(m::Vector3(0.5f, 1.f, 0.5f))), 0.f, 1.f);

		glm::vec4 pos_passtwo = glm::vec4();
		bool passtwo = false;

	redotest:

		// add camera rotation
		glm::vec4 newpos1 = ProjectVert(m::Vector3((vces[ices[i]].pos.x), (vces[ices[i]].pos.y), (vces[ices[i]].pos.z)), &matr);
		glm::vec4 newpos2 = ProjectVert(m::Vector3((vces[ices[i + 1]].pos.x), (vces[ices[i + 1]].pos.y), (vces[ices[i + 1]].pos.z)), &matr);
		glm::vec4 newpos3 = ProjectVert(m::Vector3((vces[ices[i + 2]].pos.x), (vces[ices[i + 2]].pos.y), (vces[ices[i + 2]].pos.z)), &matr);

		#if NEAR_CLIPPING

		// PERFORM NEAR PLANE CLIPPING
		int numclips = 0;
		bool clip_a = false;
		bool clip_b = false;
		bool clip_c = false;
		if (newpos1.z <= -1.f + CLIP_OFFSET) { ++numclips; clip_a = true; }
		if (newpos2.z <= -1.f + CLIP_OFFSET) { ++numclips; clip_b = true; }
		if (newpos3.z <= -1.f + CLIP_OFFSET) { ++numclips; clip_c = true; }
		if (numclips == 1) { // if one point is clipping, we have to make a quad
			//glm::vec4 renpos1_tri_a = glm::vec4();
			//glm::vec4 renpos2_tri_a = glm::vec4();
			//glm::vec4 renpos3_tri_a = glm::vec4();

			//glm::vec4 renpos1_tri_b = glm::vec4();
			//glm::vec4 renpos2_tri_b = glm::vec4();
			//glm::vec4 renpos3_tri_b = glm::vec4();

			if (passtwo) {
				// A is clipping
				if (clip_a) {
					lf32 lerpval = (newpos3.z + newpos1.z - CLIP_OFFSET) / fabsf(newpos3.z - newpos1.z);
					lerpval = 1.f - lerpval;
					newpos1.x = m::Lerp(newpos1.x, newpos3.x, lerpval);
					newpos1.y = m::Lerp(newpos1.y, newpos3.y, lerpval);
					newpos1.z = m::Lerp(newpos1.z, newpos3.z, lerpval);
					newpos1.w = m::Lerp(newpos1.w, newpos3.w, lerpval);

					newpos2 = pos_passtwo;
				}
				// B is clipping
				if (clip_b) {
					lf32 lerpval = (newpos3.z + newpos2.z - CLIP_OFFSET) / fabsf(newpos3.z - newpos2.z);
					lerpval = 1.f - lerpval;
					newpos2.x = m::Lerp(newpos2.x, newpos3.x, lerpval);
					newpos2.y = m::Lerp(newpos2.y, newpos3.y, lerpval);
					newpos2.z = m::Lerp(newpos2.z, newpos3.z, lerpval);
					newpos2.w = m::Lerp(newpos2.w, newpos3.w, lerpval);

					newpos1 = pos_passtwo;
				}
				// C is clipping
				if (clip_c) {
					lf32 lerpval = (newpos2.z + newpos3.z - CLIP_OFFSET) / fabsf(newpos2.z - newpos3.z);
					lerpval = 1.f - lerpval;
					newpos3.x = m::Lerp(newpos3.x, newpos2.x, lerpval);
					newpos3.y = m::Lerp(newpos3.y, newpos2.y, lerpval);
					newpos3.z = m::Lerp(newpos3.z, newpos2.z, lerpval);
					newpos3.w = m::Lerp(newpos3.w, newpos2.w, lerpval);

					newpos1 = pos_passtwo;
				}
			}
			else {
				// A is clipping
				if (clip_a) {
					lf32 lerpval = (newpos2.z + newpos1.z - CLIP_OFFSET) / fabsf(newpos2.z - newpos1.z);
					lerpval = 1.f - lerpval;
					newpos1.x = m::Lerp(newpos1.x, newpos2.x, lerpval);
					newpos1.y = m::Lerp(newpos1.y, newpos2.y, lerpval);
					newpos1.z = m::Lerp(newpos1.z, newpos2.z, lerpval);
					newpos1.w = m::Lerp(newpos1.w, newpos2.w, lerpval);
					pos_passtwo = newpos1;
				}
				// B is clipping
				if (clip_b) {
					lf32 lerpval = (newpos1.z + newpos2.z - CLIP_OFFSET) / fabsf(newpos1.z - newpos2.z);
					lerpval = 1.f - lerpval;
					newpos2.x = m::Lerp(newpos2.x, newpos1.x, lerpval);
					newpos2.y = m::Lerp(newpos2.y, newpos1.y, lerpval);
					newpos2.z = m::Lerp(newpos2.z, newpos1.z, lerpval);
					newpos2.w = m::Lerp(newpos2.w, newpos1.w, lerpval);
					pos_passtwo = newpos2;
				}
				// C is clipping
				if (clip_c) {
					lf32 lerpval = (newpos1.z + newpos3.z - CLIP_OFFSET) / fabsf(newpos1.z - newpos3.z);
					lerpval = 1.f - lerpval;
					newpos3.x = m::Lerp(newpos3.x, newpos1.x, lerpval);
					newpos3.y = m::Lerp(newpos3.y, newpos1.y, lerpval);
					newpos3.z = m::Lerp(newpos3.z, newpos1.z, lerpval);
					newpos3.w = m::Lerp(newpos3.w, newpos1.w, lerpval);
					pos_passtwo = newpos3;
				}
			}

			//*
			// Draw this triangle
			LRDrawTri(
				lr::LRVert((lr::LRVec3(newpos1.x, newpos1.y, newpos1.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) // scale to screen size
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), // offset to center
					lr::LRVec2(vces[ices[i]].uvc.x, vces[ices[i]].uvc.y),
					lr::LRVec3(ndotl * 2.f), newpos1.w),
				lr::LRVert((lr::LRVec3(newpos2.x, newpos2.y, newpos2.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(vces[ices[i + 1]].uvc.x, vces[ices[i + 1]].uvc.y),
					lr::LRVec3(ndotl_b * 2.f), newpos2.w),
				lr::LRVert((lr::LRVec3(newpos3.x, newpos3.y, newpos3.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(vces[ices[i + 2]].uvc.x, vces[ices[i + 2]].uvc.y),
					lr::LRVec3(ndotl_c * 2.f), newpos3.w),
				txtr->buffer, txtr->width, txtr->height);
			//*/

			if (!passtwo) {
				passtwo = true;
				goto redotest;
			}
		}
		if (numclips == 2) { // if two points are clipping, just make a smaller tri
			m::Vector2 uv1 = m::Vector2(vces[ices[i]].uvc.x, vces[ices[i]].uvc.y);
			m::Vector2 uv2 = m::Vector2(vces[ices[i + 1]].uvc.x, vces[ices[i + 1]].uvc.y);
			m::Vector2 uv3 = m::Vector2(vces[ices[i + 2]].uvc.x, vces[ices[i + 2]].uvc.y);

			// A is clipping
			if (clip_a && !clip_b) {
				lf32 lerpval = (newpos2.z + newpos1.z - CLIP_OFFSET) / fabsf(newpos2.z - newpos1.z);
				lerpval = 1.f - lerpval;
				newpos1.x = m::Lerp(newpos1.x, newpos2.x, lerpval);
				newpos1.y = m::Lerp(newpos1.y, newpos2.y, lerpval);
				newpos1.z = m::Lerp(newpos1.z, newpos2.z, lerpval);
				newpos1.w = m::Lerp(newpos1.w, newpos2.w, lerpval);
				uv1.x = m::Lerp(vces[ices[i]].uvc.x, vces[ices[i + 1]].uvc.x, lerpval);
				uv1.y = m::Lerp(vces[ices[i]].uvc.y, vces[ices[i + 1]].uvc.y, lerpval);
			}
			if (clip_a && !clip_c) {
				lf32 lerpval = (newpos3.z + newpos1.z - CLIP_OFFSET) / fabsf(newpos3.z - newpos1.z);
				lerpval = 1.f - lerpval;
				newpos1.x = m::Lerp(newpos1.x, newpos3.x, lerpval);
				newpos1.y = m::Lerp(newpos1.y, newpos3.y, lerpval);
				newpos1.z = m::Lerp(newpos1.z, newpos3.z, lerpval);
				newpos1.w = m::Lerp(newpos1.w, newpos3.w, lerpval);
				uv1.x = m::Lerp(vces[ices[i]].uvc.x, vces[ices[i + 2]].uvc.x, lerpval);
				uv1.y = m::Lerp(vces[ices[i]].uvc.y, vces[ices[i + 2]].uvc.y, lerpval);
			}
			// B is clipping
			if (clip_b && !clip_a) {
				lf32 lerpval = (newpos1.z + newpos2.z - CLIP_OFFSET) / fabsf(newpos1.z - newpos2.z);
				lerpval = 1.f - lerpval;
				newpos2.x = m::Lerp(newpos2.x, newpos1.x, lerpval);
				newpos2.y = m::Lerp(newpos2.y, newpos1.y, lerpval);
				newpos2.z = m::Lerp(newpos2.z, newpos1.z, lerpval);
				newpos2.w = m::Lerp(newpos2.w, newpos1.w, lerpval);
				uv2.x = m::Lerp(vces[ices[i + 1]].uvc.x, vces[ices[i]].uvc.x, lerpval);
				uv2.y = m::Lerp(vces[ices[i + 1]].uvc.y, vces[ices[i]].uvc.y, lerpval);
			}
			if (clip_b && !clip_c) {
				lf32 lerpval = (newpos3.z + newpos2.z - CLIP_OFFSET) / fabsf(newpos3.z - newpos2.z);
				lerpval = 1.f - lerpval;
				newpos2.x = m::Lerp(newpos2.x, newpos3.x, lerpval);
				newpos2.y = m::Lerp(newpos2.y, newpos3.y, lerpval);
				newpos2.z = m::Lerp(newpos2.z, newpos3.z, lerpval);
				newpos2.w = m::Lerp(newpos2.w, newpos3.w, lerpval);
				uv2.x = m::Lerp(vces[ices[i + 1]].uvc.x, vces[ices[i + 2]].uvc.x, lerpval);
				uv2.y = m::Lerp(vces[ices[i + 1]].uvc.y, vces[ices[i + 2]].uvc.y, lerpval);
			}
			// C is clipping
			if (clip_c && !clip_a) {
				lf32 lerpval = (newpos1.z + newpos3.z - CLIP_OFFSET) / fabsf(newpos1.z - newpos3.z);
				lerpval = 1.f - lerpval;
				newpos3.x = m::Lerp(newpos3.x, newpos1.x, lerpval);
				newpos3.y = m::Lerp(newpos3.y, newpos1.y, lerpval);
				newpos3.z = m::Lerp(newpos3.z, newpos1.z, lerpval);
				newpos3.w = m::Lerp(newpos3.w, newpos1.w, lerpval);
				uv3.x = m::Lerp(vces[ices[i + 2]].uvc.x, vces[ices[i]].uvc.x, lerpval);
				uv3.y = m::Lerp(vces[ices[i + 2]].uvc.y, vces[ices[i]].uvc.y, lerpval);
			}
			if (clip_c && !clip_b) {
				lf32 lerpval = (newpos2.z + newpos3.z - CLIP_OFFSET) / fabsf(newpos2.z - newpos3.z);
				lerpval = 1.f - lerpval;
				newpos3.x = m::Lerp(newpos3.x, newpos2.x, lerpval);
				newpos3.y = m::Lerp(newpos3.y, newpos2.y, lerpval);
				newpos3.z = m::Lerp(newpos3.z, newpos2.z, lerpval);
				newpos3.w = m::Lerp(newpos3.w, newpos2.w, lerpval);
				uv3.x = m::Lerp(vces[ices[i + 2]].uvc.x, vces[ices[i + 1]].uvc.x, lerpval);
				uv3.y = m::Lerp(vces[ices[i + 2]].uvc.y, vces[ices[i + 1]].uvc.y, lerpval);
			}

			// Draw this triangle
			LRDrawTri(
				lr::LRVert((lr::LRVec3(newpos1.x, newpos1.y, newpos1.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) // scale to screen size
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), // offset to center
					lr::LRVec2(uv1.x, uv1.y),
					lr::LRVec3(ndotl * 2.f), newpos1.w),
				lr::LRVert((lr::LRVec3(newpos2.x, newpos2.y, newpos2.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(uv2.x, uv2.y),
					lr::LRVec3(ndotl_b * 2.f), newpos2.w),
				lr::LRVert((lr::LRVec3(newpos3.x, newpos3.y, newpos3.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(uv3.x, uv3.y),
					lr::LRVec3(ndotl_c * 2.f), newpos3.w),
				txtr->buffer, txtr->width, txtr->height);
		}
		// seems like w > 0 means its in front of the clip plane
		// .. if that helps
		else { // will make this check 3 when clipping is done
			// Draw this triangle
			LRDrawTri(
				lr::LRVert((lr::LRVec3(newpos1.x, newpos1.y, newpos1.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) // scale to screen size
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), // offset to center
					lr::LRVec2(vces[ices[i]].uvc.x, vces[ices[i]].uvc.y),
					lr::LRVec3(ndotl * 2.f), newpos1.w),
				lr::LRVert((lr::LRVec3(newpos2.x, newpos2.y, newpos2.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(vces[ices[i + 1]].uvc.x, vces[ices[i + 1]].uvc.y),
					lr::LRVec3(ndotl_b * 2.f), newpos2.w),
				lr::LRVert((lr::LRVec3(newpos3.x, newpos3.y, newpos3.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(vces[ices[i + 2]].uvc.x, vces[ices[i + 2]].uvc.y),
					lr::LRVec3(ndotl_c * 2.f), newpos3.w),
				txtr->buffer, txtr->width, txtr->height);
		}

		#else

		lr::LRVec3 amb(0.25f, 0.27f, 0.38f);

		if (newpos1.w > 0.f && newpos2.w > 0.f && newpos3.w > 0.f) {
			// Draw this triangle
			LRDrawTri(
				lr::LRVert((lr::LRVec3(newpos1.x, newpos1.y, newpos1.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) // scale to screen size
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), // offset to center
					lr::LRVec2(vces[ices[i]].uvc.x, vces[ices[i]].uvc.y),
					lr::LRVec3(ndotl * 2.f) + amb, newpos1.w),
				lr::LRVert((lr::LRVec3(newpos2.x, newpos2.y, newpos2.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(vces[ices[i + 1]].uvc.x, vces[ices[i + 1]].uvc.y),
					lr::LRVec3(ndotl_b * 2.f) + amb, newpos2.w),
				lr::LRVert((lr::LRVec3(newpos3.x, newpos3.y, newpos3.z)
					* lr::LRVec3(TF_SCALE_X, TF_SCALE_Y, 1.f)) //
					+ lr::LRVec3(TF_OFFSET_X, TF_OFFSET_Y, 0.f), //
					lr::LRVec2(vces[ices[i + 2]].uvc.x, vces[ices[i + 2]].uvc.y),
					lr::LRVec3(ndotl_c * 2.f) + amb, newpos3.w),
				txtr->buffer2, txtr->width, txtr->height);
		}

		#endif
	}

}

void RenderInit()
{
	lr::LRSetBufferTemp(&tx_new, &tx_depth2);

	tx.InitGLTest();
	tx.width = LR_RENDER_W;
	tx.height = LR_RENDER_H;
	tx.buffer2 = (graphics::colour*)&tx_new;
	//tx.Init(CT_RENDER_W, CT_RENDER_H, graphics::colour(128, 128, 128, 255));
}

void RenderClear()
{
	lr::LRClear();
}

void RenderTexture(li32 x, li32 y, graphics::Texture* tx) {
	lr::LRDrawTxtr(x, y, tx->buffer2, tx->width, tx->height);
}
void RenderSprite(li32 x, li32 y, graphics::Texture* tx, lui32 spritesize, lui32 num) {
	int x2 = num % (tx->width / spritesize);
	int y2 = num / (tx->height / spritesize);
	lr::LRDrawTxtrSegment(x, y, tx->buffer2, tx->width, tx->height, spritesize, spritesize, x2 * spritesize, y2 * spritesize);
}

void RenderPrint()
{
	// do this if not using opengl
	//SDL_UpdateWindowSurface(sdl_window);

	// update opengl display
	tx.ReBindGL(graphics::TextureFilterMode::eLINEAR, graphics::TextureEdgeMode::eCLAMP);
	//tx.ReBindGL(graphics::TextureFilterMode::eNEAREST, graphics::TextureEdgeMode::eCLAMP);
	//graphics::DrawGUITexture((graphics::Texture*)&tx, 0, 0, LR_RENDER_W, LR_RENDER_H);
	graphics::DrawGUITexture((graphics::Texture*)&tx, 0, 0, LR_RENDER_W * 2, LR_RENDER_H * 2);
}

#endif
