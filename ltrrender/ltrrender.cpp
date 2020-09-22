#include "ltrrender.h"

namespace lr {

	// Clear colour definition is off unless necessary, because it's slow
	// and we should be drawing completely over the last frame anyway
	#if LR_FILL_MODE == LR_FILL_MODE_COVERAGE || LR_FILL_MODE == LR_FILL_MODE_WIREFRAME
	#define LR_CLEAR_COLOUR
	#define LR_CLEAR_BITS \
	((lui64)0b1111111100000000000000000000000011111111000000000000000000000000)
	//        <A     ><B     ><G     ><R     ><A     ><B     ><G     ><R     >
	#elif LR_FORCE_CLEAR == 1
	#define LR_CLEAR_COLOUR
	#define LR_CLEAR_BITS \
	((lui64)0b1111111110000000100000001000000011111111100000001000000010000000)
	//        <A     ><B     ><G     ><R     ><A     ><B     ><G     ><R     >
	#endif

	void* screenBufferColour_temp;
	void* screenBufferDepth_temp;

	//void* texture00;
	//lui32 texture00_w;
	//lui32 texture00_h;

	LRVec2 operator-(const LRVec2& a, const LRVec2& b) { return LRVec2(a.x - b.x, a.y - b.y); }
	LRVec3 operator*(const LRVec3& a, const LRVec3& b) { return LRVec3(a.x * b.x, a.y * b.y, a.z * b.z); }
	LRVec3 operator*(const lf32& f, const LRVec3& v) { return LRVec3(v.x * f, v.y * f, v.z * f); }
	LRVec3 operator+(const LRVec3& a, const LRVec3& b) { return LRVec3(a.x + b.x, a.y + b.y, a.z + b.z); }

	void LRClear() {
		// in my dreams
		//SDL_FillRect(sdl_surface, 0, r + (g << 8) + (b << 16));
		//-------------------------------- CLEAR TEXTURE
		#ifdef LR_CLEAR_COLOUR
		// Clear colour 64 bits at once (that's two colours in one) (white)
		for (lui32 i = 0; i < (LR_RENDER_W * LR_RENDER_H) / 2; ++i)
			*(lui64*)(((char*)screenBufferColour_temp) + (i * 8))
			= LR_CLEAR_BITS;
		#endif
		// Clear depth 64 bits at once
		for (lui32 i = 0; i < (LR_RENDER_W * LR_RENDER_H) / 4; ++i)
			(((li64*)screenBufferDepth_temp)[i])
			= ((li64)0b1000000000000000100000000000000010000000000000001000000000000000);
	}

	const li16 ditherMatPSX[16] {
		-4, 0, -3, 1,
		2, -2, 3, -1,
		-3, 1, -4, 0,
		3, -1, 2, -2
	};

	lf32 Lerp(lf32 a, lf32 b, lf32 t)
	{
		return (1 - t) * a + t * b;
	}

	// TODO: add 'add I8' function?
	__forceinline lui8 ClampI8(li16 i) {
		if (i < 0) return 0u;
		else if (i > 255) return 255u;
		else return (lui8)i;
	}

	__forceinline void DrawPixel(lui32 x, lui32 y, li32 r, li32 g, li32 b, li32 a) {
		//clamp
		if (r > 255) r = 255;
		if (g > 255) g = 255;
		if (b > 255) b = 255;
		if (a > 255) a = 255;
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;
		if (a < 0) a = 0;

		#if LR_DITHER
		// Dither dat shit
		lui32 dx = x % 4;
		lui32 dy = (y % 4) * 4;
		r = ClampI8((li16)r + ditherMatPSX[(dx + dy)] * 2);
		g = ClampI8((li16)g + ditherMatPSX[(dx + dy)] * 2);
		b = ClampI8((li16)b + ditherMatPSX[(dx + dy)] * 2);
		#endif
		#if LR_POSTERIZE
		// Posterize
		r = ClampI8(((li16)r / 16) * 16);
		g = ClampI8(((li16)g / 16) * 16);
		b = ClampI8(((li16)b / 16) * 16);
		#endif
		// Apply colour
		*(lui32*)(((char*)screenBufferColour_temp) + (x * 4 + (y * 4 * LR_RENDER_W)))
			= (lui32)r + ((lui32)g << 8) + ((lui32)b << 16) + ((lui32)a << 24);
	}
	__forceinline void DrawPixelAdd(lui32 x, lui32 y, lui8 r, lui8 g, lui8 b, lui8 a) {
		LRCol* col = (LRCol*)(((char*)screenBufferColour_temp) + (x * 4 + (y * 4 * LR_RENDER_W)));
		col->r += r;
		col->g += g;
		col->b += b;
		col->a = a;
	}

	// Read texture based on UV coordinates
	__forceinline LRCol ReadTexel(lf32 u, lf32 v, void* txtr, lui32 tw, lui32 th) {
		#if LR_BILINEAR
		// get fractions
		lf32 modu = fmodf(u * (lf32)tw, 1.f);
		lf32 modv = fmodf(v * (lf32)th, 1.f);
		// Get texture coords
		lui32 iu1 = (lui32)floorf(u * (lf32)tw);
		lui32 iv1 = (lui32)floorf(v * (lf32)th);
		// Repeat texture
		lui32 iu2 = (iu1 + 1) % tw;
		lui32 iv2 = (iv1 + 1) % th;
		iu1 = iu1 % tw;
		iv1 = iv1 % th;
		// Read the texel colour
		#define LERPR  Lerp(((LRCol*)txtr)[iu1 * tw + iv1].r, ((LRCol*)txtr)[iu2 * tw + iv1].r, modu)
		#define LERPG  Lerp(((LRCol*)txtr)[iu1 * tw + iv1].g, ((LRCol*)txtr)[iu2 * tw + iv1].g, modu)
		#define LERPB  Lerp(((LRCol*)txtr)[iu1 * tw + iv1].b, ((LRCol*)txtr)[iu2 * tw + iv1].b, modu)
		#define LERPA  Lerp(((LRCol*)txtr)[iu1 * tw + iv1].a, ((LRCol*)txtr)[iu2 * tw + iv1].a, modu)
		#define LERPR2 Lerp(((LRCol*)txtr)[iu1 * tw + iv2].r, ((LRCol*)txtr)[iu2 * tw + iv2].r, modu)
		#define LERPG2 Lerp(((LRCol*)txtr)[iu1 * tw + iv2].g, ((LRCol*)txtr)[iu2 * tw + iv2].g, modu)
		#define LERPB2 Lerp(((LRCol*)txtr)[iu1 * tw + iv2].b, ((LRCol*)txtr)[iu2 * tw + iv2].b, modu)
		#define LERPA2 Lerp(((LRCol*)txtr)[iu1 * tw + iv2].a, ((LRCol*)txtr)[iu2 * tw + iv2].a, modu)
		LRCol colret;
		colret.r = (lui8)Lerp(LERPR, LERPR2, modv);
		colret.g = (lui8)Lerp(LERPG, LERPG2, modv);
		colret.b = (lui8)Lerp(LERPB, LERPB2, modv);
		colret.a = (lui8)Lerp(LERPA, LERPA2, modv);
		return colret;
		#else
		// Get texture coords
		lui32 iu = (lui32)floorf(u * (lf32)tw);
		lui32 iv = (lui32)floorf(v * (lf32)th);
		// Repeat texture
		iu = iu % tw;
		iv = iv % th;
		// Read the texel colour
		return ((LRCol*)txtr)[iu * tw + iv];
		#endif
	}
	//LRCol* TxPixel(void* buffer, li32 x, li32 y, lui32 width) {
	//	return &((LRCol*)buffer)[x * width + y];
	//}
	void LRDrawTxtr(li32 X, li32 Y, void* tx, lui32 width, lui32 height) {
		// If fully on screen
		if (X > 0 && Y > 0 && LR_RENDER_W - (X + width) > 0 && LR_RENDER_H - (Y + height) > 0) {
			for (lui32 y = 0; y < height; ++y) {
				for (lui32 x = 0; x < width; ++x) {
					//LRCol* c = TxPixel(tx, y, x, width);
					LRCol* c = &((LRCol*)tx)[y * width + x];
					if (c->a > 128) // Alpha check
						DrawPixel(x + X, y + Y, c->r, c->g, c->b, c->a);
				}
			}
		}
	}

	__forceinline void DrawTriPixel(lui32 x, lui32 y,
		const LRVert& point_a, const LRVert& point_b, const LRVert& point_c,
		void* txtr, lui32 tw, lui32 th);
	__forceinline void DrawLineBhPx(lui32 x, lui32 y, lui8 r, lui8 g, lui8 b)
	{
		if (x < LR_RENDER_W && y < LR_RENDER_H)
			DrawPixel(x, y, r, g, b, 255);
	}
	void LRDrawLineBh(int x1, int y1, int x2, int y2, lui8 r, lui8 g, lui8 b,
		const LRVert& point_a, const LRVert& point_b, const LRVert& point_c,
		void* txtr, lui32 tw, lui32 th)
	{
		int x, y, diff_x, diff_y, diff_abs_x, diff_abs_y, difference, end;
		// Calculate the difference between each point
		diff_x = x2 - x1;
		diff_y = y2 - y1;
		// Get absolute differences
		diff_abs_x = abs(diff_x);
		diff_abs_y = abs(diff_y);
		// If X difference is longer
		if (diff_abs_y <= diff_abs_x) {
			//
			difference = 2 * diff_abs_y - diff_abs_x;
			// If the line travels positively, start at the beginning and go to the end
			if (diff_x >= 0) {
				x = x1; y = y1; end = x2;
			}
			// If the line travels negatively, take the reverse order
			else {
				x = x2; y = y2; end = x1;
			}
			// Check start point
			DrawTriPixel(x, y, point_a, point_b, point_c, txtr, tw, th);
			// Loop until we reach the end (loop along the X axis)
			for (int i = 0; x < end; i++) {
				x = x + 1;
				if (difference < 0)
					difference = difference + 2 * diff_abs_y;
				else {
					if ((diff_x < 0 && diff_y < 0) || (diff_x > 0 && diff_y > 0))
						y = y + 1;
					else
						y = y - 1;
					difference = difference + 2 * (diff_abs_y - diff_abs_x);
				}
				DrawTriPixel(x, y, point_a, point_b, point_c, txtr, tw, th);
			}
		}
		// If Y difference is longer
		else {
			//
			difference = 2 * diff_abs_x - diff_abs_y;
			if (diff_y >= 0) {
				x = x1; y = y1; end = y2;
			}
			else {
				x = x2; y = y2; end = y1;
			}
			// Check start point
			DrawTriPixel(x, y, point_a, point_b, point_c, txtr, tw, th);
			// Loop until we reach the end (loop along the Y axis)
			for (int i = 0; y < end; i++) {
				y = y + 1;
				if (difference <= 0)
					difference = difference + 2 * diff_abs_x;
				else {
					if ((diff_x < 0 && diff_y < 0) || (diff_x > 0 && diff_y > 0))
						x = x + 1;
					else
						x = x - 1;
					difference = difference + 2 * (diff_abs_x - diff_abs_y);
				}
				DrawTriPixel(x, y, point_a, point_b, point_c, txtr, tw, th);
			}
		}
	}

	#define DEF_CCW

	__forceinline lf32 Dot(const LRVec2& a, const LRVec2& b) {
		return a.x * b.x + a.y * b.y;
	}
	// Compute barycentric coordinates, do I know how this works? no
	__forceinline void TriBary(LRVec2 p, LRVec2 a, LRVec2 b, LRVec2 c, lf32 &u, lf32 &v, lf32 &w)
	{
		LRVec2 v0 = b - a, v1 = c - a, v2 = p - a;
		lf32 d00 = Dot(v0, v0);
		lf32 d01 = Dot(v0, v1);
		lf32 d11 = Dot(v1, v1);
		lf32 d20 = Dot(v2, v0);
		lf32 d21 = Dot(v2, v1);
		lf32 denom = d00 * d11 - d01 * d01;
		v = (d11 * d20 - d01 * d21) / denom;
		w = (d00 * d21 - d01 * d20) / denom;
		u = 1.0f - v - w;
	}
	__forceinline void DrawTriPixel(lui32 x, lui32 y,
		const LRVert& point_a, const LRVert& point_b, const LRVert& point_c,
		void* txtr, lui32 tw, lui32 th)
	{
		if (x < LR_RENDER_W && y < LR_RENDER_H) {
			#if LR_FILL_MODE == LR_FILL_MODE_COVERAGE
			DrawPixelAdd(x, y, 32, 16, 8, 255);
			#else
			lf32 baryU, baryV, baryW;
			TriBary(LRVec2((lf32)x, (lf32)y),
				LRVec2((lf32)point_a.pos_x, (lf32)point_a.pos_y),
				LRVec2((lf32)point_b.pos_x, (lf32)point_b.pos_y),
				LRVec2((lf32)point_c.pos_x, (lf32)point_c.pos_y),
				baryU, baryV, baryW);
			
			#if LR_PERSP_CORRECT_UV
			// TODO: calculate the division values per-vertex to save time
			lf32 u_w = (baryU * (point_a.uv.x / point_a.pos_w) + baryV * (point_b.uv.x / point_b.pos_w) + baryW * (point_c.uv.x / point_c.pos_w));
			lf32 v_w = (baryU * (point_a.uv.y / point_a.pos_w) + baryV * (point_b.uv.y / point_b.pos_w) + baryW * (point_c.uv.y / point_c.pos_w));
			lf32 one_w = (baryU * (1.f / point_a.pos_w) + baryV * (1.f / point_b.pos_w) + baryW * (1.f / point_c.pos_w));
			#endif

			lf32 depth = (baryU * point_a.pos_z + baryV * point_b.pos_z + baryW * point_c.pos_z) * 32767.f;
			if (depth > 32767.f || depth < -32768.f) return; // Don't draw if it's outside the clipping range
			// Depth test
			#define TXDEPTH *(li16*)(((char*)screenBufferDepth_temp) + (x * 2 + (y * 2 * LR_RENDER_W)))
			if (TXDEPTH < (li16)depth) {
				// debug: if invalid uvs, draw cyan
				#if LR_DEBUG_HELP
				if (uvx > tw || uvy > th) {
					DrawPixel(x, y, 0u, 255u, 255u, 255u);
				}
				else
					#endif
				{
					// Get texture colour from uv coords
					//LRCol* c = TxPixel(txtr, uvy, uvx, tw);
					#if LR_PERSP_CORRECT_UV
					lf32 uvx = u_w / one_w;
					lf32 uvy = v_w / one_w;
					#else
					lf32 uvx = ((baryU * point_a.uv.x + baryV * point_b.uv.x + baryW * point_c.uv.x));
					lf32 uvy = ((baryU * point_a.uv.y + baryV * point_b.uv.y + baryW * point_c.uv.y));
					#endif
					LRCol c = ReadTexel(uvy, uvx, txtr, tw, th);
					// Get vertex colour
					LRVec3 colInterp = baryU * point_a.col + baryV * point_b.col + baryW * point_c.col;
					// Set the pixel
					// Draw bary
					//DrawPixel(x, y, u * 255, v * 255, w * 255, 255);
					// Draw texture
					//DrawPixel(x, y, c.r, c.g, c.b, 255);
					// Draw shaded texture
					DrawPixel(x, y,
						(li32)((lf32)c.r * (colInterp.x)),
						(li32)((lf32)c.g * (colInterp.y)),
						(li32)((lf32)c.b * (colInterp.z)), 255);
					// Write depth
					TXDEPTH = (li16)depth;
				}
				#if LR_DEBUG_HELP
				if (u > 1.f || v > 1.f || w > 1.f || u < 0.f || v < 0.f || w < 0.f) {
					DrawPixel(x, y, 255u, 0u, 255u, 255u);
					//TXDEPTH = (li16)(0b0111111111111111);
					return;
				}
				#endif
			}
			#undef TXDEPTH
			#endif // Draw Coverage
		}
	}
	struct TriLine
	{
		#if LR_MODE == LR_MODE_FLO_LERP
		lf32 x1, y1, x2, y2;
		lf32 deltax, deltay;
		#else
		li32 x1, y1, x2, y2;
		li32 deltax, deltay;
		#endif
		#if LR_MODE == LR_MODE_FLO_LERP
		lf32 slope;
		#endif
	};
	__forceinline void InitializeTriLine(TriLine* tl, LRVert* point_a, LRVert* point_b)
	{
		tl->x1 = point_a->pos_x;
		tl->y1 = point_a->pos_y;
		tl->x2 = point_b->pos_x;
		tl->y2 = point_b->pos_y;
		tl->deltax = tl->x2 - tl->x1;
		tl->deltay = tl->y2 - tl->y1;
		#if LR_MODE == LR_MODE_FLO_LERP
		if (tl->deltax != 0)
			tl->slope = (lf32)tl->deltay / (lf32)tl->deltax;
		else tl->slope = 0.f;
		#endif
	}
	#if LR_MODE == LR_MODE_INT_BHM
	// old version
	//*
	__forceinline void TriBHTick(TriLine& line, li32& error, li32& y_int)
	{
		if (line.deltay > 0) {
			error = error - line.deltay;
			while (error < 0) {
				y_int += 1;
				error += line.deltax;
			}
		}
		else {
			error = error - line.deltay;
			while (error > 0) {
				y_int -= 1;
				error -= line.deltax;
			}
		}
	}//*/
	// experimental version
	/*
	__forceinline void TriBHTick(TriLine& line, li32& error, li32& y_int)
	{
		if (line.deltay > 0) {
			if (abs(line.deltax) > abs(line.deltay)) {
				if (error >= 0) {
					y_int += 1;
					error += 2 * line.deltay - 2 * line.deltax;
				}
				else {
					error += 2 * line.deltay;
				}
			}
			else {
				error = error - line.deltay;
				while (error < 0) {
					y_int += 1;
					error += line.deltax;
				}
			}
		}
		else {
			if (abs(line.deltax) > abs(line.deltay)) {
				if (error >= 0) {
					y_int -= 1;
					error -= 2 * line.deltay + 2 * line.deltax;
				}
				else {
					error -= 2 * line.deltay;
				}
			}
			else {
				error = error - line.deltay;
				while (error > 0) {
					y_int -= 1;
					error -= line.deltax;
				}
			}
		}
	}*/
	#endif
	__forceinline li32 ClampScreenX(li32 in) {
		if (in >= LR_RENDER_W) in = LR_RENDER_W - 1;
		else if (in < 0) in = 0;
		return in;
	}
	__forceinline lf32 ClampScreenY(lf32 in) {
		if (in >= (lf32)LR_RENDER_H) in = (lf32)LR_RENDER_H - 1.f;
		else if (in < 0.f) in = 0.f;
		return in;
	}
	void LRDrawTri(LRVert point_a, LRVert point_b, LRVert point_c, void* txtr, li32 tw, li32 th)
	{
		#ifndef DEF_CCW
		// Check if it's clockwise
		if ((point_b.pos_y - point_a.pos_y) * (point_c.pos_x - point_b.pos_x)
			- (point_c.pos_y - point_b.pos_y) * (point_b.pos_x - point_a.pos_x) < 0.f)
			#else
		// Or counter-clockwise (temp?)
		if ((point_b.pos_y - point_a.pos_y) * (point_c.pos_x - point_b.pos_x)
			- (point_c.pos_y - point_b.pos_y) * (point_b.pos_x - point_a.pos_x) > 0.f)
			#endif
		{
			// Crossover pointer
			LRVert* point_temp;
			// Pointers for shuffling points into order
			LRVert* point_min = &point_a;
			LRVert* point_med = &point_b;
			LRVert* point_max = &point_c;

			// Sort the triangles by their X
			if (point_min->pos_x > point_med->pos_x) { point_temp = point_min; point_min = point_med; point_med = point_temp; }
			if (point_min->pos_x > point_max->pos_x) { point_temp = point_min; point_min = point_max; point_max = point_temp; }
			if (point_med->pos_x > point_max->pos_x) { point_temp = point_med; point_med = point_max; point_max = point_temp; }

			// Create line structures
			TriLine line_a, line_b, line_c;
			InitializeTriLine(&line_a, point_min, point_med);
			InitializeTriLine(&line_b, point_min, point_max);
			InitializeTriLine(&line_c, point_med, point_max);

			#if LR_FILL_MODE == LR_FILL_MODE_WIREFRAME
			// wireframe mode
			LRDrawLineBh(line_a.x1, line_a.y1, line_a.x2, line_a.y2, 255, 0, 0, point_a, point_b, point_c, txtr, tw, th);
			LRDrawLineBh(line_b.x1, line_b.y1, line_b.x2, line_b.y2, 0, 255, 0, point_a, point_b, point_c, txtr, tw, th);
			LRDrawLineBh(line_c.x1, line_c.y1, line_c.x2, line_c.y2, 0, 0, 255, point_a, point_b, point_c, txtr, tw, th);
			#else
			#if LR_MODE == LR_MODE_FLO_LERP
			// WIP triangle fill
			if (line_a.slope != 0.f || line_b.slope != 0.f || line_c.slope != 0.f) {
				li32 x_start = ClampScreenX((li32)roundf(line_a.x1));
				li32 x_end = ClampScreenX((li32)roundf(line_a.x2)); 
				li32 x = x_start;
				// Left of horizontal division
				while (x <= x_end) {
					lf32 lerpval_a = (line_a.x2 - (lf32)x) / (line_a.x2 - line_a.x1);
					lf32 lerpval_b = (line_b.x2 - (lf32)x) / (line_b.x2 - line_b.x1);
					if (lerpval_b >= 0.f && lerpval_b <= 1.f && lerpval_a >= 0.f && lerpval_a <= 1.f) { // temporary measure
						lf32 ya2 = ClampScreenY(Lerp(line_a.y2, line_a.y1, lerpval_a));
						lf32 yb2 = ClampScreenY(Lerp(line_b.y2, line_b.y1, lerpval_b));
						if (ya2 <= yb2)
							for (li32 i = (li32)ceilf(ya2); i <= (li32)floorf(yb2); ++i)
								DrawTriPixel(x, i, point_a, point_b, point_c, txtr, tw, th);
						else
							for (li32 i = (li32)floorf(ya2); i >= (li32)ceilf(yb2); --i)
								DrawTriPixel(x, i, point_a, point_b, point_c, txtr, tw, th);
					}
					++x;
				}
				x_start = ClampScreenX((li32)roundf(line_c.x1));
				x_end = ClampScreenX((li32)roundf(line_c.x2));
				x = x_start;
				// Right of horizontal division
				while (x <= x_end) {
					lf32 lerpval_b = (line_b.x2 - (lf32)x) / (line_b.x2 - line_b.x1);
					lf32 lerpval_c = (line_c.x2 - (lf32)x) / (line_c.x2 - line_c.x1);
					if (lerpval_b >= 0.f && lerpval_b <= 1.f && lerpval_c >= 0.f && lerpval_c <= 1.f) { // temporary measure
						lf32 yb2 = ClampScreenY(Lerp(line_b.y2, line_b.y1, lerpval_b)); 
						lf32 yc2 = ClampScreenY(Lerp(line_c.y2, line_c.y1, lerpval_c));
						if (yc2 <= yb2)
							for (li32 i = (li32)ceilf(yc2); i <= (li32)floorf(yb2); ++i)
								DrawTriPixel(x, i, point_a, point_b, point_c, txtr, tw, th);
						else
							for (li32 i = (li32)floorf(yc2); i >= (li32)ceilf(yb2); --i)
								DrawTriPixel(x, i, point_a, point_b, point_c, txtr, tw, th);
					}
					++x;
				}
			}
			#elif LR_MODE == LR_MODE_INT_BHM
			// wip bresenham fill
			if (line_a.deltax != 0 || line_b.deltax != 0 || line_c.deltax != 0) {
				// Initialize values for line drawing
				li32 error_a = 0;
				li32 error_b = 0;
				li32 ya = line_a.y1;
				li32 yb = line_b.y1;
				li32 x = line_a.x1;
				// Left of horizontal division (Division at the end of line A)
				while (x < line_a.x2) {
					if (ya <= yb)
						for (li32 i = ya; i < yb; ++i)
							DrawTriPixel(x, i, *point_min, *point_med, *point_max, txtr, tw, th);
					else
						for (li32 i = yb; i < ya; ++i)
							DrawTriPixel(x, i, *point_min, *point_med, *point_max, txtr, tw, th);

					TriBHTick(line_a, error_a, ya);
					TriBHTick(line_b, error_b, yb);

					++x;
				}
				// Set A values to start of line C
				error_a = 0;
				ya = line_c.y1;
				// Right of horizontal division
				//*
				while (x < line_c.x2) {
					if (ya <= yb)
						for (li32 i = ya; i < yb; ++i)
							DrawTriPixel(x, i, *point_min, *point_med, *point_max, txtr, tw, th);
					else
						for (li32 i = yb; i < ya; ++i)
							DrawTriPixel(x, i, *point_min, *point_med, *point_max, txtr, tw, th);

					TriBHTick(line_c, error_a, ya);
					TriBHTick(line_b, error_b, yb);

					++x;
				}//*/
			}
			#endif
			#endif
		}
	}
	
	void LRSetBufferTemp(void * colour, void * depth)
	{
		screenBufferColour_temp = colour;
		screenBufferDepth_temp = depth;	
	}
	//void LRSetTxtr(void * txtr, lui32 tx_w, lui32 tx_h)
	//{
	//	texture00 = txtr;
	//	texture00_w = tx_w;
	//	texture00_h = tx_h;
	//}
}
