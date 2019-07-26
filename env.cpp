#include "env.h"
#include "global.h"

#include <iostream>
#include <vector>

namespace env
{
	struct pathtree
	{
		//btui8 nodez[WORLD_SIZE / 4][WORLD_SIZE]{ 0b00000000 };
		btui8 nodez[WORLD_SIZE][WORLD_SIZE]{ 0b00000000 };
	};

	pathtree trees[WORLD_SIZE][WORLD_SIZE]{}; // 1024 mb, 1gb

	size_t i = sizeof(trees);

	void set_node_dir(const btui8 srcx, const btui8 srcy, const btui8 dstx, const btui8 dsty, nbit::node_bit to)
	//void set_node_dir(int srcx, int srcy, int dstx, int dsty, nbit::node_bit to)
	{
		/*
		int xclust = floor(dstx / 4);
		int xlocal = dstx % 4;

		//int xclust = (dstx / 4) * 4; // round it to every 2nd number
		//int xlocal = dstx - xclust; // the leftovers
		int offset = xlocal * 2;

		btui8 bit = (to << (btui8)offset);
		nbit::node_bit bit2 = (nbit::node_bit)bit; // confirms that there is no problem with this function
		
		btui8 bitb = (nbit::CLEAR1 << (btui8)offset);
		nbit::node_bit bit2b = (nbit::node_bit)bit; // confirms that there is no problem with this function

		trees[srcx][srcy].nodez[xclust][dsty] &= bitb; // Clear bit

		trees[srcx][srcy].nodez[xclust][dsty] |= bit; // Set bit
		*/

		trees[srcx][srcy].nodez[dstx][dsty] = to; // Set bit
	}

	nbit::node_bit get_node_bit(btui8 srcx, btui8 srcy, btui8 dstx, btui8 dsty)
	{
		/*
		int xclust = floor(dstx / 4);
		int xlocal = dstx % 4;
		//int xclust = (dstx / 4) * 4; // round it to every 2nd number
		//int xlocal = dstx - xclust; // the leftovers
		int offset = xlocal * 2;

		//set_node_dir(srcx, srcy, dstx, dsty, nbit::E);

		//btui8 nodecluster = (nbit::node_bit)trees[srcx][srcy].nodez[xclust][dsty];
		btui8 nodecluster = (btui8)trees[srcx][srcy].nodez[xclust][dsty]; // wtf why 255

		btui8 bit1 = nodecluster & (btui8)nbit::MASK1;
		btui8 bit2 = nodecluster & (btui8)nbit::MASK2;
		btui8 bit3 = nodecluster & (btui8)nbit::MASK3;
		btui8 bit4 = nodecluster & (btui8)nbit::MASK4;

		nbit::node_bit bit1b = (nbit::node_bit)bit1;
		nbit::node_bit bit2b = (nbit::node_bit)(bit2 >> 2);
		nbit::node_bit bit3b = (nbit::node_bit)(bit3 >> 4);
		nbit::node_bit bit4b = (nbit::node_bit)(bit4 >> 6);

		// why are they all W?????

		switch (xlocal)
		{
		case 0:
			nodecluster &= (btui8)nbit::MASK1; break;
		case 1:
			nodecluster = nodecluster >> 2;
			nodecluster &= (btui8)nbit::MASK1; break;
		case 2:
			nodecluster = nodecluster >> 4;
			nodecluster &= (btui8)nbit::MASK1; break;
		case 3:
			nodecluster = nodecluster >> 6;
			nodecluster &= (btui8)nbit::MASK1; break;
		default:
			break;
		}

		nbit::node_bit bit = (nbit::node_bit)nodecluster;
		return (nbit::node_bit)nodecluster;

		//nbit::node_bit node = (nbit::node_bit)((btui8)nodecluster >> offset);
		//node = (nbit::node_bit)((btui8)node & nbit::MASK1);
		*/
		return (nbit::node_bit)trees[srcx][srcy].nodez[dstx][dsty];
	}

	node_coord get_node_from(btui8 srcx, btui8 srcy, btui8 dstx, btui8 dsty)
	{
		/*
		node_coord nc;

		int xclust = floor(dstx / 4);
		int xlocal = dstx % 4;
		//int xclust = (dstx / 4) * 4; // round it to every 2nd number
		//int xlocal = dstx - xclust; // the leftovers
		if (xlocal > 1)
			int i = 1;
		int offset = xlocal * 2;

		btui8 nodecluster = trees[srcx][srcy].nodez[xclust][dsty];

		//nbit::node_bit node = (nbit::node_bit)(nodecluster & nbit::MASK);
		//nbit::node_bit node = (nbit::node_bit)((btui8)node >> offset);
		nbit::node_bit node = (nbit::node_bit)((btui8)nodecluster >> offset);
		node = (nbit::node_bit)((btui8)node & nbit::MASK1);

		switch (node) // we'll do this with function ptrs later
		{
		case nbit::N: nc.x = dstx; nc.y = ++dsty; break;
		case nbit::S: nc.x = dstx; nc.y = --dsty; break;
		case nbit::E: nc.x = ++dstx; nc.y = dsty; break;
		case nbit::W: nc.x = --dstx; nc.y = dsty; break;
		default:
			break;
		}
		return nc;
		*/

		node_coord nc;

		nbit::node_bit node = (nbit::node_bit)trees[srcx][srcy].nodez[dstx][dsty];

		switch (node) // we'll do this with function ptrs later
		{
		case nbit::N: nc.x = dstx; nc.y = dsty + 1; break;
		case nbit::S: nc.x = dstx; nc.y = dsty - 1; break;
		case nbit::E: nc.x = dstx + 1; nc.y = dsty; break;
		case nbit::W: nc.x = dstx - 1; nc.y = dsty; break;
		default:
			break;
		}
		return nc;
	}

	void FloodFill(btui8 srcx, btui8 srcy)
	{
		const int frontier_size = 32;

		//node_coord frontier2[frontier_size];
		bool visited[WORLD_SIZE][WORLD_SIZE]{ false };
		visited[srcx][srcy] = true;
		std::vector<node_coord> frontier;
		frontier.push_back(node_coord(srcx, srcy)); // add the first node right off the bat
		//frontier2[0] = node_coord(srcx, srcy);
		node_coord current(srcx, srcy);
		//iterate through every element of the frontier
		for (int i = 0; i < frontier.size(); i++)
		//int frontier_index = 1;
		//for (int i = 1; i < frontier_size; i++)
		{
			current = frontier[i];
			//current.x = frontier2[i - 1].x;
			//current.y = frontier2[i - 1].y;
			std::cout << "ANALYZING FRONTIER " << i << " | SRC: " << (int)srcx << "," << (int)srcy << " COORD: " << (int)current.x << "," << (int)current.y << std::endl;

			// for each neighbor (optimize later etc)
			node_coord nei;

			if ((bool)((current.x + current.y) % 2)) // is this an odd number
			{
				if (current.y < WORLD_SIZE - 1) // if we are in bounds
				{
					nei.x = current.x; nei.y = current.y + 1; // N
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this node to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir(srcx, srcy, nei.x, nei.y, nbit::S); // Set the directional value for this node
					}
				}
				if (current.y > 0) // if we are in bounds
				{
					nei.x = current.x; nei.y = current.y - 1; // S
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this node to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir(srcx, srcy, nei.x, nei.y, nbit::N); // Set the directional value for this node
					}
				}
				if (current.x > 0) // if we are in bounds
				{
					nei.x = current.x - 1; nei.y = current.y; // W
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this node to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir(srcx, srcy, nei.x, nei.y, nbit::E); // Set the directional value for this node
					}
				}
				if (current.x < WORLD_SIZE - 1) // if we are in bounds
				{
					nei.x = current.x + 1; nei.y = current.y; // E
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this node to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir((int)srcx, (int)srcy, (int)nei.x, (int)nei.y, nbit::W); // srsly what the fuck
					}
				}

			}
			else // Clockwise
			{
				if (current.x < WORLD_SIZE - 1) // if we are in bounds
				{
					nei.x = current.x + 1; nei.y = current.y; // E
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this node to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir((int)srcx, (int)srcy, (int)nei.x, (int)nei.y, nbit::W); // srsly what the fuck
					}
				}
				if (current.x > 0) // if we are in bounds
				{
					nei.x = current.x - 1; nei.y = current.y; // W
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this node to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir(srcx, srcy, nei.x, nei.y, nbit::E); // Set the directional value for this node
					}
				}
				if (current.y > 0) // if we are in bounds
				{
					nei.x = current.x; nei.y = current.y - 1; // S
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this node to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir(srcx, srcy, nei.x, nei.y, nbit::N); // Set the directional value for this node
					}
				}
				if (current.y < WORLD_SIZE - 1) // if we are in bounds
				{
					nei.x = current.x; nei.y = current.y + 1; // N
					if (!visited[nei.x][nei.y] && !Get(nei.x, nei.y, eflag::eIMPASSABLE)) // if this node has not already been analyzed
					{
						visited[nei.x][nei.y] = true; // set this neighbor to already having been analyzed
						frontier.push_back(node_coord(nei.x, nei.y)); // Add node to frontier
						set_node_dir(srcx, srcy, nei.x, nei.y, nbit::S); // Set the directional value for this node
					}
				}
			}
		}
	}

	void GeneratePaths()
	{
		//generate tile flags from binary info
		for (btui8 x = 0; x < WORLD_SIZE; x++)
		{
			for (btui8 y = 0; y < WORLD_SIZE; y++)
			{
				if (!Get(x, y, eflag::eIMPASSABLE))
					FloodFill(x, y);
			}
		}
	}

	//get these tf out of here
	node_v001 nodes[WORLD_SIZE][WORLD_SIZE];
	path_node pnodes[WORLD_SIZE][WORLD_SIZE];

	bool Get(uint x, uint y, eflag::flag bit)
	{
		return mem::bvget((uint32_t)nodes[x][y].flags, (uint32_t)bit);
	}
	void Set(uint x, uint y, eflag::flag bit)
	{
		mem::bvset((uint32_t&)nodes[x][y].flags, (uint32_t)bit);
	}
	void UnSet(uint x, uint y, eflag::flag bit)
	{
		mem::bvunset((uint32_t&)nodes[x][y].flags, (uint32_t)bit);
	}
	
	void Tick()
	{
		//
	}

	void SaveBin()
	{
		std::cout << "Saving [world.ltrwld]" << std::endl;
		FILE *out = fopen("save/world.ltrwld", "wb");
		if (out != NULL)
		{
			fseek(out, 0, SEEK_SET); // Seek the beginning of the file
			for (int x = 0; x < WORLD_SIZE; x++)
				const size_t wrote = fwrite(&nodes[x][0], sizeof(node_v001), WORLD_SIZE, out);
			fwrite(trees, sizeof(trees), 1, out); // Read paths
			fclose(out);
		}
	}
	void LoadBin()
	{
		std::cout << "Loading [world.ltrwld]" << std::endl;
		FILE *in = fopen("save/world.ltrwld", "rb");
		if (in != NULL)
		{
			fseek(in, 0, SEEK_SET); // Seek the beginning of the file
			for (int x = 0; x < WORLD_SIZE; x++)
				const size_t read = fread(&nodes[x][0], sizeof(node_v001), WORLD_SIZE, in);
			fread(trees, sizeof(trees), 1, in); // Read paths
			fclose(in);
		}

		//temp
		//set_node_dir(8, 8, 5, 5, nbit::N);
		//set_node_dir(8, 8, 4, 5, nbit::E);
		//node_coord nc = get_node_from(8, 8, 5, 5);


	}

	void GeneratePhysicsSurfaces()
	{
		//generate tile flags from binary info
		//probably temporary, we'll see.
		for (int x = 1; x < WORLD_SIZE - 1; x++)
		{
			for (int y = 1; y < WORLD_SIZE - 1; y++)
			{
				env::UnSet(x, y, eflag::eSurfN);
				env::UnSet(x, y, eflag::eSurfS);
				env::UnSet(x, y, eflag::eSurfE);
				env::UnSet(x, y, eflag::eSurfW);
				env::UnSet(x, y, eflag::eCorOutNE);
				env::UnSet(x, y, eflag::eCorOutNW);
				env::UnSet(x, y, eflag::eCorOutSE);
				env::UnSet(x, y, eflag::eCorOutSW);

				//don't bother doing anything if this tile is not solid
				if (!env::Get(x, y, eflag::eIMPASSABLE))
				//if (!ltrmem::bitget(env::nodes[x][y].flags, EF_IMPASSABLE))
				{
					//edges
					//n
					if (env::Get(x, y + 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfN);
						//if (mem::bitget32_auto(env::nodes[x][y + 1].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_N);
					//s
					if (env::Get(x, y - 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfS);
						//if (mem::bitget32_auto(env::nodes[x][y - 1].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_S);
					//e
					if (env::Get(x + 1, y, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfE);
						//if (mem::bitget32_auto(env::nodes[x + 1][y].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_E);
					//w
					if (env::Get(x - 1, y, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eSurfW);
						//if (mem::bitget32_auto(env::nodes[x - 1][y].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOCKED_W);
					//corners
					//ne
					if (env::Get(x + 1, y + 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutNE);
						//if (mem::bitget32_auto(env::nodes[x + 1][y + 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x + 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y + 1].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_NE);
					//nw
					if (env::Get(x - 1, y + 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutNW);
						//if (mem::bitget32_auto(env::nodes[x - 1][y + 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x - 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y + 1].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_NW);
					//se
					if (env::Get(x + 1, y - 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutSE);
						//if (mem::bitget32_auto(env::nodes[x + 1][y - 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x + 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y - 1].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_SE);
					//sw
					if (env::Get(x - 1, y - 1, eflag::eIMPASSABLE))
						env::Set(x, y, eflag::eCorOutSW);
						//if (mem::bitget32_auto(env::nodes[x - 1][y - 1].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x - 1][y].flags, envflag::EF_IMPASSABLE) && !mem::bitget32_auto(env::nodes[x][y - 1].flags, envflag::EF_IMPASSABLE))
						//mem::bitset32_auto(env::nodes[x][y].flags, envflag::EF_BLOKCOR_SW);
				}
			}
		}
	}

	void GenerateModelTypes()
	{
		//generate tile flags from binary info
		//probably temporary, we'll see.
		for (int x = 1; x < WORLD_SIZE - 1; x++)
		{
			for (int y = 1; y < WORLD_SIZE - 1; y++)
			{
				//only draw models if its on an impassable tile (for now)
				if (env::Get(x, y, eflag::eIMPASSABLE))
				{
					bool face_n = false, face_s = false, face_e = false, face_w = false;

					//wall facing up
					if (!env::Get(x, y + 1, eflag::eIMPASSABLE))
					{
						face_n = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_N;
					}
					//wall facing down
					if (!env::Get(x, y - 1, eflag::eIMPASSABLE))
					{
						face_s = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_S;
					}
					//wall facing right
					if (!env::Get(x + 1, y, eflag::eIMPASSABLE))
					{
						face_e = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_E;
					}
					//wall facing left
					if (!env::Get(x - 1, y, eflag::eIMPASSABLE))
					{
						face_w = true;
						//env::nodes[x][y].model_type = EMT_WALL_STR_W;
					}

					if (face_n)
						env::nodes[x][y].model_type = EMT_WALL_STR_N;
					else if (face_s)
						env::nodes[x][y].model_type = EMT_WALL_STR_S;
					else if (face_e)
						env::nodes[x][y].model_type = EMT_WALL_STR_E;
					else if (face_w)
						env::nodes[x][y].model_type = EMT_WALL_STR_W;

					//ne
					if (face_n && face_e)
						env::nodes[x][y].model_type = EMT_WALL_COR_OUT_NE;
					//nw
					else if (face_n && face_w)
						env::nodes[x][y].model_type = EMT_WALL_COR_OUT_NW;
					//se
					else if (face_s && face_e)
						env::nodes[x][y].model_type = EMT_WALL_COR_OUT_SE;
					//sw
					else if (face_s && face_w)
						env::nodes[x][y].model_type = EMT_WALL_COR_OUT_SW;
				}
			}
		}
	}
}

namespace ltr
{
	//layers contain cells which contain tiles

	//obviously doesnt belong in this file but who cares right now
	//eventuall encapsulate in class
	unsigned int tilesTemplate[8][8]
	{
		{	1,	1,	1,	1,	1,	1,	1,	1, },
		{	1,	0,	0,	0,	1,	0,	0,	1, },
		{	1,	1,	0,	0,	1,	0,	0,	1, },
		{	1,	0,	0,	0,	0,	0,	0,	1, }, //top side here for some reason
		{	1,	1,	0,	0,	0,	0,	0,	0, },
		{	1,	1,	0,	0,	1,	0,	0,	1, },
		{	1,	1,	0,	0,	0,	0,	0,	1, },
		{	1,	1,	1,	1,	1,	1,	1,	1, },
	};
}