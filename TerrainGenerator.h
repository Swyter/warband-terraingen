#pragma once

#include <string>
#include <vector>
#include "Vector2.h"
#include "Vector4.h"

#define NUM_TERRAIN_KEYS 6
#define MIN_NUM_TERRAIN_FACES_PER_AXIS 40
#define MAX_NUM_TERRAIN_FACES_PER_AXIS 250
#define NUM_TERRAIN_LAYERS 15

extern int g_possibleMoves[9][2];
extern int g_possibleMoveDistances[8];

enum RiverLayerType
{
	rlt_base     = 2,
	rlt_expanded = 4,
};

enum TerrainLayerType
{
	tlt_rock     = 0,
	tlt_earth    = 1,
	tlt_green    = 2,
	tlt_riverbed = 3,
};

enum RegionType
{
	rt_ocean           = 0,
	rt_mountain        = 1,
	rt_steppe          = 2,
	rt_plain           = 3,
	rt_snow            = 4,
	rt_desert          = 5,
	rt_bridge          = 7,
	rt_river           = 8,
	rt_mountain_forest = 9,
	rt_steppe_forest   = 10,
	rt_forest          = 11,
	rt_snow_forest     = 12,
	rt_desert_forest   = 13,
	rt_shore           = 21,
	rt_foam            = 22,
	rt_waves           = 23,
};

enum GroundType
{
	ground_gray_stone  = 0,
	ground_brown_stone = 1,
	ground_turf        = 2,
	ground_steppe      = 3,
	ground_snow        = 4,
	ground_earth       = 5,
	ground_desert      = 6,
	ground_forest      = 7,
	ground_pebbles     = 8,
	ground_village     = 9,
	ground_path        = 10,
};

class TerrainVertex
{
public:
	Vector4 m_position;
	float m_slope;
	unsigned int m_terrainLayerFlags;
	Vector4 m_normal;
	Vector2 m_riverDirection;
	int m_riverCurveNo;
	float m_layerIntensities[NUM_TERRAIN_LAYERS];

public:
	void initialize(const Vector4 &position);
};

class TerrainFace
{
public:
	int m_orientation;
	int m_layerNos[2];
	Vector4 m_normals[2];
	float m_layerIntensities[NUM_TERRAIN_LAYERS];

public:
	void initialize();
};
	
class TerrainLayer
{
public:
	int m_groundSpecNo;
	int m_terrainType;

public:
	void setGroundSpec(int groundSpecNo);
};

class TerrainGenerator
{
public:
	unsigned int m_keys[6];
	TerrainLayer m_layers[NUM_TERRAIN_LAYERS];
	Vector2 m_size;
	int m_numFaces[2];
	int m_numRiverPasses;
	TerrainVertex m_vertices[MAX_NUM_TERRAIN_FACES_PER_AXIS + 1][MAX_NUM_TERRAIN_FACES_PER_AXIS + 1];
	TerrainFace m_faces[MAX_NUM_TERRAIN_FACES_PER_AXIS][MAX_NUM_TERRAIN_FACES_PER_AXIS];
	float m_detail;
	float m_barrenness;
	float m_noiseFrequency;
	Vector4 m_positionNoiseRandomness;
	int m_terrainBlockSize;
	bool m_placeRiver;
	
public:
	TerrainGenerator();
	void initialize();
	std::string getTerrainCode();
	void setTerrainCode(const std::string &code);
	void setShadeOccluded(int value);
	void setDisableGrass(int value);
	void setPlaceRiver(int value);
	void setDeepWater(int value);
	void setSizeX(float value);
	void setSizeY(float value);
	void setHillHeight(float value);
	void setValley(float value);
	void setRuggedness(int value);
	void setVegetation(float value);
	void setRegionType(int value);
	void setRegionDetail(int value);
	void setSeed(int index, int value);
	int getDisableGrass();
	int getPlaceRiver();
	int getDeepWater();
	float getSizeX();
	float getSizeY();
	float getHillHeight();
	float getValley();
	int getRuggedness();
	float getVegetation();
	int getRegionType();
	int getRegionDetail();
	int getSeed(int index);
	void generate();
	void generateLayers();
	void generateTerrain();
	void generateHills(float widthFactor, float densityFactor, float heightFactor);
	void generateHill(const Vector2 &position, float radius, float height);
	void generateRiver();
	float placeRiver(int *start, int *end, int terrainType, float globalDirectionFactor, float globalHeightFactor, bool modifyVertices);
	void expandRiver(int pass, unsigned int srcTerrainTypeFlags, unsigned int dstTerrainTypeFlags);
	void smoothRiverHeight(int terrainType, float smoothFactor);
	void smoothHeight();
	void computeNormals();
	void computeVertexLayerIntensities();
	void selectFaceOrientation();
	void selectFaceLayers();
	void roughenRockVertices();
	void computeFaceLayerIntensities();
	void getVerticesForFace(int x, int y, int triangleNo, int *vertex0, int *vertex1, int *vertex2);
	float getRandom4X();
	float getVertexNoise(int x, int y);
	void removeTerrainTypeFlags(unsigned int flags);
	void setTerrainTypeDepth(int type, float depth);
};
