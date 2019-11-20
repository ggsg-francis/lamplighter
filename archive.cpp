#include "archive.hpp"
#include "serializer.h"
#include "memory.hpp"

// temp? for generating skin texture
#include "maths.hpp"
#include <time.h>

#ifndef DEF_ARCHIVER
namespace res
{
	graphics::Texture& GetT(btui32 index)
	{
		if (archive::assets[index].type == archive::types::ASSET_TEXTURE_FILE)
			return *(graphics::Texture*)archive::assets[index].asset;
		else return *(graphics::Texture*)archive::assets[DEFAULT_TEXTURE].asset;
	}

	graphics::Mesh& GetM(btui32 index)
	{
		if (archive::assets[index].type == archive::types::ASSET_MESH_FILE)
			return *(graphics::Mesh*)archive::assets[index].asset;
		else return *(graphics::Mesh*)archive::assets[DEFAULT_MESH].asset;
	}

	graphics::MeshBlend& GetMB(btui32 index)
	{
		if (archive::assets[index].type == archive::types::ASSET_MESHBLEND_FILE)
			return *(graphics::MeshBlend*)archive::assets[index].asset;
		else return *(graphics::MeshBlend*)archive::assets[DEFAULT_MESHBLEND].asset;
	}

	bool IsTexture(btui32 index)
	{
		return archive::assets[index].type == archive::types::ASSET_TEXTURE_FILE;
	}

	bool IsMesh(btui32 index)
	{
		return archive::assets[index].type == archive::types::ASSET_MESH_FILE;
	}

	bool IsMeshBlend(btui32 index)
	{
		return archive::assets[index].type == archive::types::ASSET_MESHBLEND_FILE;
	}

	graphics::ModifiableTexture skin_t[4];

	void Init()
	{
		serializer::load_archive("res/archive.UwUa");

		//for now, just load everything
		for (btui32 i = 0u; i < archive::assetCount; i++)
		{
			archive::LoadAsset(i);
		}

		srand(time(NULL)); //initialize the random seed

		graphics::colour col_base(0ui8, 0ui8, 0ui8, 255ui8);
		graphics::colour col_eyes(0ui8, 0ui8, 0ui8, 255ui8);
		graphics::colour col_nose(0ui8, 0ui8, 0ui8, 255ui8);

		graphics::ModifiableTexture skin_t_base;
		skin_t_base.LoadFile("res/Tchrskn1");

		const graphics::colour colEyes[]{
			graphics::colour(232ui8, 17ui8, 17ui8, 255ui8),
			graphics::colour(241ui8, 236ui8, 231ui8, 255ui8),
			graphics::colour(239ui8, 169ui8, 18ui8, 255ui8),
			graphics::colour(177ui8, 205ui8, 21ui8, 255ui8),
			graphics::colour(52ui8, 142ui8, 199ui8, 255ui8),
			graphics::colour(61ui8, 155ui8, 9ui8, 255ui8),
		};

		const graphics::colour colBase[]{
			graphics::colour(233ui8, 231ui8, 226ui8, 255ui8),
			graphics::colour(97ui8, 84ui8, 75ui8, 255ui8),
			graphics::colour(38ui8, 37ui8, 36ui8, 255ui8),
			graphics::colour(164ui8, 109ui8, 21ui8, 255ui8),
		};

		const graphics::colour colNose[]{
			graphics::colour(222ui8, 193ui8, 185ui8, 255ui8),
			graphics::colour(233ui8, 152ui8, 136ui8, 255ui8),
			graphics::colour(103ui8, 57ui8, 57ui8, 255ui8),
			graphics::colour(31ui8, 29ui8, 29ui8, 255ui8),
		};

		for (int i = 0; i < 4; i++)
		{
			col_base = colBase[rand() % 4];
			col_eyes = colEyes[rand() % 6];
			col_nose = colNose[rand() % 4];

			graphics::colour col_transparent = graphics::colour(0ui8, 0ui8, 0ui8, 0ui8);

			/*col_base.r += (btui8)m::Random(0.f, 16.f);
			col_base.g += (btui8)m::Random(0.f, 16.f);
			col_base.b += (btui8)m::Random(0.f, 16.f);*/

			#define SKIN_W 32u
			#define SKIN_H 32u

			btui32 roll_eyes = (btui32)(rand() % 3) * SKIN_W;

			skin_t[i].Init(SKIN_W, SKIN_H, col_base);
			for (int x = 0; x < SKIN_W; x++)
			{
				for (int y = 0; y < SKIN_W; y++)
				{
					// fade in eye colour
					skin_t[i].SetPixelChannelR(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).r, col_eyes.r, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).g / 256.f))));
					skin_t[i].SetPixelChannelG(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).g, col_eyes.g, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).g / 256.f))));
					skin_t[i].SetPixelChannelB(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).b, col_eyes.b, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).g / 256.f))));
					
					//skin_t[i].SetPixelChannelR(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).r, 0ui8, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).g / 256.f))));
					//skin_t[i].SetPixelChannelG(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).g, 0ui8, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).g / 256.f))));
					//skin_t[i].SetPixelChannelB(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).b, 0ui8, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).g / 256.f))));
					//skin_t[i].SetPixelChannelA(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).a, 0ui8, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).g / 256.f))));

					// fade in nose colour
					skin_t[i].SetPixelChannelR(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).r, col_nose.r, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).b / 256.f))));
					skin_t[i].SetPixelChannelG(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).g, col_nose.g, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).b / 256.f))));
					skin_t[i].SetPixelChannelB(x, y, (btui8)(m::Lerp((btf32)skin_t[i].GetPixel(x, y).b, col_nose.b, ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).b / 256.f))));

					// darken outline
					skin_t[i].SetPixelChannelR(x, y, (btui8)((btf32)skin_t[i].GetPixel(x, y).r * ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).r / 256.f)));
					skin_t[i].SetPixelChannelG(x, y, (btui8)((btf32)skin_t[i].GetPixel(x, y).g * ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).r / 256.f)));
					skin_t[i].SetPixelChannelB(x, y, (btui8)((btf32)skin_t[i].GetPixel(x, y).b * ((btf32)skin_t_base.GetPixel(x, y + roll_eyes).r / 256.f)));
				}
			}
			skin_t[i].ReBindGL(graphics::eLINEAR, graphics::eCLAMP);
		}
	}
	void End()
	{
		archive::ClearMemory();
	}
}
#endif

namespace archive
{
	//items
	item* items[ITEMS_COUNT]{ nullptr };
	types::ItemType item_types[ITEMS_COUNT];
	btID item_index = 0;

	// NEW STUFF !!!!!!!!!!!!!!!!!!!!!

	archive_asset assets[FN_COUNT];
	btui32 assetCount;


	#ifndef DEF_ARCHIVER

	void LoadAsset(btui32 i)
	{
		if (!assets[i].loaded)
		{
			switch (assets[i].type)
			{
			case types::ASSET_TEXTURE_FILE:
				assets[i].asset = new graphics::Texture;
				((graphics::Texture*)assets[i].asset)->LoadFile(assets[i].filename);
				assets[i].loaded = true; 
				return;
			case types::ASSET_MESH_FILE:
				assets[i].asset = new graphics::Mesh;
				((graphics::Mesh*)assets[i].asset)->LoadFile(assets[i].filename);
				assets[i].loaded = true;
				return;
			case types::ASSET_MESHBLEND_FILE:
				assets[i].asset = new graphics::MeshBlend;
				((graphics::MeshBlend*)assets[i].asset)->LoadFile(assets[i].filename);
				assets[i].loaded = true;
				return;
			};
		}
	}

	void UnloadAsset(btui32 i)
	{
		// Old (seems to leak)
		if (assets[i].asset != nullptr && assets[i].loaded)
			delete assets[i].asset;
	};

	#endif // DEF_ARCHIVER

	void ClearMemory()
	{
		for (int i = 0; i < archive::item_index; i++)
		{
			delete(archive::items[i]);
		}
		#ifndef DEF_ARCHIVER
		for (int i = 0; i < FN_COUNT; i++)
		{
			UnloadAsset(i);
		}
		#endif // DEF_ARCHIVER
	}
}