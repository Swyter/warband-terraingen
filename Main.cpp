#include <Windows.h>
#include <Shlwapi.h>
#include <fstream>
#include "TerrainGenerator.h"

int main(int argc, char **argv)
{
	char path[MAX_PATH];

	GetModuleFileName(NULL, path, MAX_PATH);
	PathStripPath(path);

	if (argc != 3)
	{
		printf("usage: %s output_file hex_terrain_code\n", path);
		printf("example: %s terrain.txt 0x000000003a07b2320002589600003f3f000054c900007c97\n", path);
		system("pause");
		return EXIT_SUCCESS;
	}

	TerrainGenerator *terrainGen; // too big for stack

	try
	{
		terrainGen = new TerrainGenerator();
		terrainGen->setTerrainCode(argv[2]);
		terrainGen->generate();
	}
	catch (...) // just for SEH, needs /EHa
	{
		printf("exception while generating terrain. check your input mang :[\n");
		system("pause");
		return EXIT_SUCCESS;
	}
	
	std::ofstream outStream(argv[1], std::ios::trunc);

	if (!outStream.is_open())
	{
		printf("failed to create output file\n");
		system("pause");
		return EXIT_SUCCESS;
	}
	
	outStream << "Num vertices (x, y):" << std::endl;
	outStream << (terrainGen->m_numFaces[0] + 1) << " " << (terrainGen->m_numFaces[2] + 1) << std::endl;
	outStream << "Vertices (x, y, pos x, pos y, pos z):" << std::endl;
	
	for (int x = 0; x <= terrainGen->m_numFaces[0]; ++x)
	{
		for (int y = 0; y <= terrainGen->m_numFaces[1]; ++y)
		{
			TerrainVertex *vtx = &terrainGen->m_vertices[x][y];

			outStream << x << " " << y << " " << vtx->m_position.x << " " << vtx->m_position.y << " " << vtx->m_position.z << std::endl;
		}
	}

	outStream.close();
	printf("terrain data stored to %s\n", argv[1]);
	system("pause");
	return EXIT_SUCCESS;
}
