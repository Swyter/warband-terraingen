#include "TerrainGenerator.h"
#include "Perlin.h"
#include "Utils.h"

int g_possibleMoves[9][2] =
{
	{ 0, 1 },
	{ 1, 1 },
	{ 1, 0 },
	{ 1, -1 },
	{ 0, -1 },
	{ -1, -1 },
	{ -1, 0 },
	{ -1, 1 },
	{ 0, 0 },
};

int g_possibleMoveDistances[8] =
{
	{ 2 },
	{ 1 },
	{ 2 },
	{ 1 },
	{ 2 },
	{ 1 },
	{ 2 },
	{ 1 },
};

void TerrainVertex::initialize(const Vector4 &position)
{
	m_position = position;
	m_slope = 1.0f;
	m_terrainLayerFlags = 0;
	m_riverCurveNo = -1;
	m_riverDirection = Vector2(0.0f, 1.0f);

	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		m_layerIntensities[i] = 0.0f;
	}

	m_layerIntensities[tlt_rock] = 1.0f;
}

void TerrainFace::initialize()
{
	m_orientation = 0;
	m_layerNos[0] = 0;
	m_layerNos[1] = 0;
	m_normals[0] = Vector4(0.0f, 0.0f, 1.0f);
	m_normals[1] = Vector4(0.0f, 0.0f, 1.0f);

	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		m_layerIntensities[i] = 0;
	}
}

void TerrainLayer::setGroundSpec(int groundSpecNo)
{
	m_groundSpecNo = groundSpecNo;

	if (groundSpecNo >= 0)
		m_terrainType = -1;
}

TerrainGenerator::TerrainGenerator()
{
	m_detail = 2.0f;
	m_terrainBlockSize = 40;
	m_placeRiver = false;
	m_numRiverPasses = 2;
	initialize();
}

void TerrainGenerator::initialize()
{
	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		m_layers[i].m_groundSpecNo = ground_gray_stone;
		m_layers[i].m_terrainType = 0;
	}
}

std::string TerrainGenerator::getTerrainCode()
{
	char buf[51];

	sprintf_s(buf, "0x%.8x%.8x%.8x%.8x%.8x%.8x", m_keys[5], m_keys[4], m_keys[3], m_keys[2], m_keys[1], m_keys[0]);
	return buf;
}

void TerrainGenerator::setTerrainCode(const std::string &code)
{
	for (int i = 0; i < NUM_TERRAIN_KEYS; ++i)
	{
		m_keys[i] = 0;
	}

	int startPos = 0;
	int key = 0;

	if (code[0] == '0' && (code[1] == 'x' || code[1] == 'X'))
		startPos = 2;

	for (int i = code.length(); i >= startPos && key < NUM_TERRAIN_KEYS; i -= 8)
	{
		int start = Max(i - 8, startPos);
		std::string keyCode = code.substr(start, i - start);
		unsigned int keyValue = 0;

		for (size_t j = 0; j < keyCode.length(); ++j)
		{
			char c = keyCode[j];
			int val = 0;

			if (c - '0' <= 9)
				val = c - '0';
			else if (c - 'a' <= 5)
				val = c - 'W';
			else if (c - 'A' <= 5)
				val = c - '7';

			keyValue = keyValue * 16 + val;
		}

		m_keys[key++] = keyValue;
	}
}

void TerrainGenerator::setShadeOccluded(int value)
{
	m_keys[3] &= ~(0x1 << 30);
	m_keys[3] |= (Min(value, 1) << 30);
}

void TerrainGenerator::setDisableGrass(int value)
{
	m_keys[5] &= ~(0x3 << 2);
	m_keys[5] |= (Min(value, 3) << 2);
}

void TerrainGenerator::setPlaceRiver(int value)
{
	m_keys[3] &= ~(0x1 << 31);
	m_keys[3] |= (Min(value, 1) << 31);
}

void TerrainGenerator::setDeepWater(int value)
{
	m_keys[1] &= ~(0x1 << 31);
	m_keys[1] |= (Min(value, 1) << 31);
}

void TerrainGenerator::setSizeX(float value)
{
	m_keys[3] &= ~(0x3FF << 0);
	m_keys[3] |= (Min((int)value, 1023) << 0);
}

void TerrainGenerator::setSizeY(float value)
{
	m_keys[3] &= ~(0x3FF << 10);
	m_keys[3] |= (Min((int)value, 1023) << 10);
}

void TerrainGenerator::setHillHeight(float value)
{
	m_keys[4] &= ~(0x7F << 7);
	m_keys[4] |= (ClampExclusive(Round2(value), 0, 128) << 7);
}

void TerrainGenerator::setValley(float value)
{
	m_keys[4] &= ~(0x7F << 0);
	m_keys[4] |= (ClampExclusive(Round(value * 100.0f), 0, 100) << 0);
}

void TerrainGenerator::setRuggedness(int value)
{
	m_keys[4] &= ~(0x7F << 14);
	m_keys[4] |= (Min(value, 127) << 14);
}

void TerrainGenerator::setVegetation(float value)
{
	m_keys[4] &= ~(0x7F << 21);
	m_keys[4] |= (ClampExclusive(Round(value * 100.0f), 0, 100) << 21);
}

void TerrainGenerator::setRegionType(int value)
{
	m_keys[4] &= ~(0xF << 28);
	m_keys[4] |= (Min(value, 16) << 28);
}

void TerrainGenerator::setRegionDetail(int value)
{
	m_keys[5] &= ~(0x3 << 0);
	m_keys[5] |= (Min(value, 3) << 0);
}

void TerrainGenerator::setSeed(int index, int value)
{
	m_keys[index] &= ~0x7FFFFFFF;
	m_keys[index] |= Min(value, 0x7FFFFFFF);
}

int TerrainGenerator::getDisableGrass()
{
	return (m_keys[5] >> 2) & 0x3;
}

int TerrainGenerator::getPlaceRiver()
{
	return (m_keys[3] >> 31) & 0x1;
}

int TerrainGenerator::getDeepWater()
{
	return (m_keys[1] >> 31) & 0x1;
}

float TerrainGenerator::getSizeX()
{
	return (float)((m_keys[3] >> 0) & 0x3FF);
}

float TerrainGenerator::getSizeY()
{
	return (float)((m_keys[3] >> 10) & 0x3FF);
}

float TerrainGenerator::getHillHeight()
{
	return (float)((m_keys[4] >> 7) & 0x7F);
}

float TerrainGenerator::getValley()
{
	return (float)((m_keys[4] >> 0) & 0x7F) / 100.0f;
}

int TerrainGenerator::getRuggedness()
{
	return (m_keys[4] >> 14) & 0x7F;
}

float TerrainGenerator::getVegetation()
{
	return (float)((m_keys[4] >> 21) & 0x7F) / 100.0f;
}

int TerrainGenerator::getRegionType()
{
	return (m_keys[4] >> 28) & 0xF;
}

int TerrainGenerator::getRegionDetail()
{
	return (m_keys[5] >> 0) & 0x3;
}

int TerrainGenerator::getSeed(int index)
{
	return (m_keys[index] >> 0) & 0x7FFFFFFF;
}

void TerrainGenerator::generate()
{
	initialize();
	m_detail = getRegionDetail() + 2.0f;
	srand(getSeed(0));
	m_placeRiver = getPlaceRiver() != 0;
	m_size.x = getSizeX();
	m_size.y = getSizeY();
	m_numFaces[0] = ClampInclusive((int)(m_size.x / m_detail), MIN_NUM_TERRAIN_FACES_PER_AXIS, MAX_NUM_TERRAIN_FACES_PER_AXIS);
	m_numFaces[1] = ClampInclusive((int)(m_size.y / m_detail), MIN_NUM_TERRAIN_FACES_PER_AXIS, MAX_NUM_TERRAIN_FACES_PER_AXIS);
	m_size.x = m_numFaces[0] * m_detail;
	m_size.y = m_numFaces[1] * m_detail;
	generateLayers();

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			m_vertices[x][y].initialize(Vector4(x * m_detail, y * m_detail, 0.0f));
		}
	}

	for (int y = 0; y < m_numFaces[1]; ++y)
	{
		for (int x = 0; x < m_numFaces[0]; ++x)
		{
			m_faces[x][y].initialize();
		}
	}
	
	generateTerrain();
	srand(getSeed(1));
	generateRiver();
	smoothHeight();
	computeNormals();
	computeVertexLayerIntensities();
	selectFaceOrientation();
	selectFaceLayers();
	roughenRockVertices();
	srand(getSeed(2));
	computeNormals();
	computeFaceLayerIntensities();
}

void TerrainGenerator::generateLayers()
{
	for (int i = 0; i < NUM_TERRAIN_LAYERS; ++i)
	{
		m_layers[i].m_groundSpecNo = -1;
	}

	int random = rand() % 2;

	if (random == 1)
		m_layers[tlt_rock].setGroundSpec(ground_brown_stone);
	else if (random == 0)
		m_layers[tlt_rock].setGroundSpec(ground_gray_stone);

	rand();
	m_barrenness = 0.19f;
	
	int earthGroundSpec = ground_earth;
	int greenGroundSpec = ground_turf;
	int multitexGroundSpec = -1;

	switch (getRegionType())
	{
	case rt_plain:
		earthGroundSpec = ground_earth;
		greenGroundSpec = ground_turf;
		multitexGroundSpec = ground_turf;
		break;
	case rt_steppe:
		earthGroundSpec = ground_earth;
		greenGroundSpec = ground_steppe;
		multitexGroundSpec = ground_steppe;
		rand();
		break;
	case rt_snow:
	case rt_snow_forest:
		earthGroundSpec = ground_snow;
		greenGroundSpec = -1;
		multitexGroundSpec = -1;
		break;
	case rt_desert:
	case rt_desert_forest:
		earthGroundSpec = ground_desert;
		greenGroundSpec = -1;
		multitexGroundSpec = -1;
		m_barrenness = 0.26f;
		break;
	case rt_forest:
		earthGroundSpec = ground_forest;
		greenGroundSpec = ground_turf;
		multitexGroundSpec = ground_forest;
		break;
	case rt_steppe_forest:
		earthGroundSpec = ground_forest;
		greenGroundSpec = ground_steppe;
		multitexGroundSpec = -1;
		break;
	}

	m_layers[tlt_earth].setGroundSpec(earthGroundSpec);
	m_layers[tlt_green].setGroundSpec(greenGroundSpec);
	m_layers[tlt_riverbed].setGroundSpec(ground_pebbles);
}

void TerrainGenerator::generateTerrain()
{
	float sizeFactor = (m_size[1] + m_size[0]) * 0.5f * (Randf(1.0f, 1.5f));

	m_noiseFrequency = (m_size[1] + m_size[0]) * 0.2f * (Randf(1.0f, 1.5f));

	float valley = getValley();
	Vector4 positionRandomness;
	
	positionRandomness.x = Randf(10000.0f);
	positionRandomness.y = Randf(10000.0f);
	positionRandomness.z = Randf(10000.0f);

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			Vector4 noise = PerlinOctave(m_noiseFrequency, 0.6f, 1, m_vertices[x][y].m_position + positionRandomness);
			float slopeFactor = Clamp((noise.x + noise.y + noise.z) * valley * 9.0f, 0.0f, 1.0f);

			m_vertices[x][y].m_slope = (cos(slopeFactor * PI) + 1.0f) * 0.45f + 0.1f;
		}
	}

	float altitudeRandomness = Randf(7.0f, 10.0f);
	
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 13; ++j)
		{
			if (m_numFaces[1 - i] >= 0)
			{
				float altitudeFactor = sin((1.0f - j / 13.0f) * 3.2f) * altitudeRandomness;

				for (int k = 0; k <= m_numFaces[1 - i]; ++k)
				{
					int x1;
					int y1;
					int x2;
					int y2;

					if (i == 0)
					{
						x1 = j;
						x2 = m_numFaces[0] - j;
						y1 = k;
						y2 = k;
					}
					else
					{
						x1 = k;
						x2 = k;
						y1 = j;
						y2 = m_numFaces[1] - j;
					}

					TerrainVertex *vertex1 = &m_vertices[x1][y1];
					Vector4 noise1 = PerlinOctave(40.0f, 0.6f, 4, vertex1->m_position + positionRandomness);
					float altitude1 = (noise1.z + noise1.z + 0.4f) * altitudeFactor;

					if (altitude1 > vertex1->m_position.z)
						vertex1->m_position.z = altitude1;

					TerrainVertex *vertex2 = &m_vertices[x2][y2];
					Vector4 noise2 = PerlinOctave(40.0f, 0.6f, 4, vertex2->m_position + positionRandomness);
					float altitude2 = (noise2.z + noise2.z + 0.4f) * altitudeFactor;

					if (altitude2 > vertex2->m_position.z)
						vertex2->m_position.z = altitude2;
				}
			}
		}
	}

	float hillFactor = Max(sizeFactor / (pow(2.0f, (100 - ClampExclusive(getRuggedness(), 0, 100)) * 0.045f) * 4.0f), 2.0f);
	int numPasses = (int)(log(hillFactor) * LOG2_E);
	float heightFactor = 1.0f;
	
	for (int i = 0; i <= numPasses; ++i)
	{
		float densityFactor;

		if (i == numPasses)
			densityFactor = (hillFactor - BITSHIFT64(numPasses)) / BITSHIFT64(numPasses);
		else
			densityFactor = 1.0f;

		generateHills(sizeFactor / BITSHIFT64(i), densityFactor, heightFactor);
		heightFactor -= 0.05f;
	}

	float minHeight = 10000.0f;
	
	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			minHeight = Min(minHeight, m_vertices[x][y].m_position.z);
		}
	}

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			m_vertices[x][y].m_position.z -= minHeight;
		}
	}
}

void TerrainGenerator::generateHills(float widthFactor, float densityFactor, float heightFactor)
{
	int numHills = (int)(Round(m_size[0] * densityFactor * m_size[1] / (widthFactor * widthFactor * 0.25f)) * Randf(0.4f, 0.6f) * 30.0f);

	for (int i = 0; i < numHills; ++i)
	{
		float radius = (getRandom4X() * 0.5f + 0.05f) * widthFactor;
		Vector2 position;
		
		position.y = (m_size[1] + (widthFactor * 2)) * Randf() - widthFactor;
		position.x = (m_size[0] + (widthFactor * 2)) * Randf() - widthFactor;
		generateHill(position, radius, Randf(0.2f, 0.3f) * radius * heightFactor);
	}
}

void TerrainGenerator::generateHill(const Vector2 &position, float radius, float height)
{
	float hillHeight = getHillHeight();
	int min[2];
	int max[2];

	for (int i = 0; i < 2; ++i)
	{
		min[i] = Max(Round(ceil((position[i] - radius) / m_detail)), 0);
		max[i] = Min(Round(floor((position[i] + radius) / m_detail)), m_numFaces[i]);
	}

	for (int y = min[1]; y <= max[1]; ++y)
	{
		for (int x = min[0]; x <= max[0]; ++x)
		{
			TerrainVertex *vertex = &m_vertices[x][y];
			float centerDistance = (Vector2((float)x, (float)y) * m_detail - position).length() / radius;

			if (centerDistance < 1.0f)
				vertex->m_position.z += ((cos(centerDistance * PI) + 1.0f) * 0.5f * height * vertex->m_slope) * hillHeight * 0.01f;
		}
	}
}

void TerrainGenerator::generateRiver()
{
	bool riverPlaced = false;
	int start[2];
	int end[2];

	start[0] = 0;
	start[1] = 0;

	if (m_placeRiver)
	{
		for (int i = 0; i < 4 && !riverPlaced; ++i)
		{
			int tries = 20;
			int x;
			int y;
			float bestNoise = -10000.0f;

			do
			{
				if (Randf() >= 0.5f)
				{
					x = Rand(m_numFaces[0] - 3) + 3;

					y = rand() % 4 + 3;

					if (Randf() < 0.5f)
						y = m_numFaces[1] - y;
				}
				else
				{
					x = rand() % 4 + 3;
					
					if (Randf() < 0.5f)
						x = m_numFaces[0] - x;

					y = Rand(m_numFaces[1] - 3) + 3;
				}

				float noise = getVertexNoise(start[0], start[1]);

				if (noise >= bestNoise)
				{
					bestNoise = noise;
					start[0] = x;
					start[1] = y;
				}
			}
			while (--tries);

			bool endFound = false;

			tries = 50;

			while (tries && !endFound)
			{
				if (Randf() >= 0.5f)
				{
					end[1] = 0;
					
					if (Randf() < 0.5f)
						end[1] = m_numFaces[1] + 1;

					end[0] = Rand(m_numFaces[0] + 1);
				}
				else
				{
					end[0] = 0;
					
					if (Randf() < 0.5f)
						end[0] = m_numFaces[0] + 1;

					end[1] = Rand(m_numFaces[1] + 1);
				}
				
				int distX = end[0] - start[0];

				if (distX < 0)
					distX = -distX;

				int distY = end[1] - start[1];

				if (distY < 0)
					distY = -distY;

				if ((m_numFaces[0] + m_numFaces[1]) * 0.7f < (distX + distY))
					endFound = true;

				--tries;
			}

			if (endFound)
			{
				if (placeRiver(start, end, rlt_base, 1.5f, 0.9f, true) >= 3.5f)
					removeTerrainTypeFlags(BITSHIFT(rlt_base)|BITSHIFT(rlt_expanded));
				else
					riverPlaced = true;
			}
		}
	}

	m_numRiverPasses = rand() % 3;

	if (m_detail >= 2.1f && m_numRiverPasses > 1)
		m_numRiverPasses = 1;

	if (m_detail > 3.1f)
		m_numRiverPasses = 0;

	if (getDeepWater())
		m_numRiverPasses = 0;

	if (riverPlaced)
	{
		expandRiver(1, BITSHIFT(rlt_base), BITSHIFT(rlt_base)|BITSHIFT(rlt_expanded));
		
		for (int i = 0; i < m_numRiverPasses; ++i)
			expandRiver(2, BITSHIFT(rlt_base), BITSHIFT(rlt_base)|BITSHIFT(rlt_expanded));
		
		expandRiver(2, BITSHIFT(rlt_base), BITSHIFT(rlt_expanded));
		expandRiver(2, BITSHIFT(rlt_expanded), BITSHIFT(rlt_expanded));
		setTerrainTypeDepth(rlt_base, getDeepWater() ? -2.3f : -1.3f);
		smoothRiverHeight(rlt_base, 0.5f);
	}
}

float TerrainGenerator::placeRiver(int *start, int *end, int terrainType, float globalDirectionFactor, float globalHeightFactor, bool modifyVertices)
{
	float depth = 10000.0f;
	Vector2 direction((float)(end[0] - start[0]), (float)(end[1] - start[1]));
	int curX = start[0];
	int curY = start[1];
	int numCurves = 0;
	Vector2 possibleDirections[8];
	
	direction.normalize();

	for (int i = 0; i < 8; ++i)
	{
		possibleDirections[i] = Vector2((float)g_possibleMoves[i][0], (float)g_possibleMoves[i][1]);
		possibleDirections[i].normalize();
	}

	int terrainTypeFlags = BITSHIFT(rlt_base);
	TerrainVertex *startVertex = &m_vertices[start[0]][start[1]];
	TerrainVertex *curVertex = startVertex;

	startVertex->m_terrainLayerFlags |= terrainTypeFlags;

	if (modifyVertices)
		startVertex->m_riverDirection = direction;
	
	bool atEnd = false;
	float hillHeight = getHillHeight();
	float directionLikeliness[8];

	while (true)
	{
		Vector2 curDirection((float)(end[0] - curX), (float)(end[1] - curY));

		curDirection.normalize();

		for (int i = 0; i < 8; ++i)
		{
			directionLikeliness[i] = 0.0f;

			int nextX = curX + g_possibleMoves[i][0];
			int nextY = curY + g_possibleMoves[i][1];
			float placementFactor = 0.0f;
			int distanceFromBounds = Max(20 - nextX, 0) + Max(nextX + 20 - m_numFaces[0], 0) + Max(20 - nextY, 0) + Max(nextY + 20 - m_numFaces[1], 0);

			if (distanceFromBounds > 0)
			{
				if (nextX <= 20 && possibleDirections[i].x <= 0.0f ||
					nextY <= 20 && possibleDirections[i].y <= 0.0f ||
					nextX >= m_numFaces[0] - 20 && possibleDirections[i].x >= 0.0f ||
					nextY >= m_numFaces[1] - 20 && possibleDirections[i].y >= 0.0f)
				{
					placementFactor = Clamp((float)distanceFromBounds, 0.0f, 24.0f) * - 0.04f;
				}
			}

			if (nextX < 0 || nextX > m_numFaces[0] || nextY < 0 || nextY > m_numFaces[1])
			{
				directionLikeliness[i] = (possibleDirections[i].dot(direction) + 1.0f) / 2;
			}
			else
			{
				TerrainVertex *nextVertex = &m_vertices[nextX][nextY];

				if (!(nextVertex->m_terrainLayerFlags & terrainTypeFlags))
				{
					float heightFactor = (curVertex->m_position.z - nextVertex->m_position.z) * globalHeightFactor * 3.0f;

					if (heightFactor < 0.0f)
						heightFactor *= (0.25f - hillHeight * 0.0022f);

					directionLikeliness[i] = heightFactor + ((direction.dot(possibleDirections[i]) + 0.7f) * 0.5f * globalDirectionFactor) + ((curDirection.dot(possibleDirections[i]) + 0.7f) * 0.5f) * 0.65f + placementFactor;
				}
				else
				{
					if (nextVertex->m_riverCurveNo < numCurves - 30)
						atEnd = true;

					directionLikeliness[i] = -1.0f;
				}
			}
		}

		for (int i = 0; i < 8; ++i)
		{
			if (directionLikeliness[i] > 0.0f)
				directionLikeliness[i] = directionLikeliness[i] * directionLikeliness[i] * directionLikeliness[i];
		}

		float threshold = 0.0f;
		
		for (int i = 0; i < 8; ++i)
		{
			if (directionLikeliness[i] > 0.0f)
				threshold += directionLikeliness[i];
		}

		threshold *= Randf();

		float totalLikeliness = 0.0f;
		int selectedMove = -1;

		for (int i = 0; i < 8; ++i)
		{
			if (directionLikeliness[i] > 0.0f)
			{
				if (totalLikeliness <= threshold && directionLikeliness[i] + totalLikeliness > threshold)
					selectedMove = i;

				totalLikeliness += directionLikeliness[i];
			}
		}

		if (selectedMove != -1)
		{
			int newX = curX + g_possibleMoves[selectedMove][0];

			if (newX >= 3 && newX <= m_numFaces[0] - 3)
			{
				int newY = curY + g_possibleMoves[selectedMove][1];

				if (newY >= 3 && newY <= m_numFaces[1] - 3)
				{
					if (directionLikeliness[selectedMove] > 0.0f)
					{
						curX = newX;
						curY = newY;
						direction = direction * 0.9f + possibleDirections[selectedMove] * 0.1f;
						direction.normalize();
						numCurves++;
						curVertex = &m_vertices[curX][curY];
						curVertex->m_terrainLayerFlags |= terrainTypeFlags;
						curVertex->m_riverCurveNo = numCurves;

						if (modifyVertices)
						{
							curVertex->m_riverDirection = direction;
						}

						depth = Min(depth, curVertex->m_position.z);

						if (numCurves != 100000 && !atEnd)
							continue;
					}
				}
			}
		}

		break;
	}

	return depth;
}

void TerrainGenerator::expandRiver(int pass, unsigned int srcTerrainTypeFlags, unsigned int dstTerrainTypeFlags)
{
	int modifiedVertices[MAX_NUM_TERRAIN_FACES_PER_AXIS + 1][MAX_NUM_TERRAIN_FACES_PER_AXIS + 1];
	int numDirections;
	int directions[8];

	if (pass == 1)
	{
		numDirections = 4;
		directions[0] = 2;
		directions[1] = 4;
		directions[2] = 5;
		directions[3] = 6;
	}
	else
	{
		numDirections = 8;
		directions[0] = 0;
		directions[1] = 1;
		directions[2] = 2;
		directions[3] = 3;
		directions[4] = 4;
		directions[5] = 5;
		directions[6] = 6;
		directions[7] = 7;
	}

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			modifiedVertices[x][y] = 0;
		}
	}

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			Vector2 avgRiverDirection = 0.0f;
			int numMatches = 0;
			TerrainVertex *vertex = &m_vertices[x][y];

			for (int i = 0; i < numDirections; ++i)
			{
				int possibleX = x + g_possibleMoves[directions[i]][0];
				int possibleY = y + g_possibleMoves[directions[i]][1];

				if (possibleX >= 0 && possibleX <= m_numFaces[0])
				{
					if (possibleY >= 0 && possibleY <= m_numFaces[1])
					{
						TerrainVertex *possibleVertex = &m_vertices[possibleX][possibleY];

						if (possibleVertex->m_terrainLayerFlags & srcTerrainTypeFlags)
						{
							numMatches++;
							avgRiverDirection += possibleVertex->m_riverDirection;
						}
					}
				}
			}

			if (numMatches)
			{
				modifiedVertices[x][y] = 1;
				vertex->m_riverDirection = avgRiverDirection / (float)numMatches;
				vertex->m_riverDirection.normalize();
			}
		}
	}

	if (dstTerrainTypeFlags)
	{
		for (int y = 0; y <= m_numFaces[1]; ++y)
		{
			for (int x = 0; x <= m_numFaces[0]; ++x)
			{
				if (modifiedVertices[x][y])
					m_vertices[x][y].m_terrainLayerFlags |= dstTerrainTypeFlags;
			}
		}
	}
}

void TerrainGenerator::smoothRiverHeight(int terrainType, float smoothFactor)
{
	unsigned int flag = BITSHIFT(terrainType);
	float height;

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			TerrainVertex *vertex = &m_vertices[x][y];

			if (!(vertex->m_terrainLayerFlags & terrainType))
			{
				int smoothType = 0;
				
				for (int i = 0; i < 8; ++i)
				{
					int newX = x + g_possibleMoves[i][0];
					int newY = y + g_possibleMoves[i][1];
					
					if (newX >= 0 && newX <= m_numFaces[0])
					{
						if (newY >= 0 && newY <= m_numFaces[1])
						{
							if (m_vertices[newX][newY].m_terrainLayerFlags & flag)
							{
								int newType = g_possibleMoveDistances[i];

								if (newType == 2 || (newType == 1 && smoothType == 0))
									smoothType = newType;

								height = m_vertices[newX][newY].m_position.z;
							}
						}
					}
				}

				if (smoothType)
				{
					float factor = smoothFactor;

					if (smoothType == 1)
						factor *= 0.7f;

					vertex->m_position.z += (height - vertex->m_position.z) * factor;
				}
			}
		}
	}
}

void TerrainGenerator::smoothHeight()
{
	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			TerrainVertex *vertex = &m_vertices[x][y];
			float heightSum = vertex->m_position.z;
			float smoothAmount = 1.0f;
			
			for (int i = 0; i < 8; ++i)
			{
				int newX = x + g_possibleMoves[i][0];
				int newY = y + g_possibleMoves[i][1];
					
				if (newX >= 0 && newX <= m_numFaces[0])
				{
					if (newY >= 0 && newY <= m_numFaces[1])
					{
						heightSum += m_vertices[newX][newY].m_position.z;
						smoothAmount += 1.0f;
					}
				}
			}

			float heightDiff = Clamp((heightSum / smoothAmount) - vertex->m_position.z, -10.0f, 2.0f);

			vertex->m_position.z += Randf(0.2f, 0.6f) * heightDiff;
		}
	}
}

void TerrainGenerator::computeNormals()
{
	for (int y = 0; y < m_numFaces[1]; ++y)
	{
		for (int x = 0; x < m_numFaces[0]; ++x)
		{
			TerrainFace *face = &m_faces[x][y];

			for (int i = 0; i < 2; ++i)
			{
				int vertex0[2];
				int vertex1[2];
				int vertex2[2];

				getVerticesForFace(x, y, i, vertex0, vertex1, vertex2);
				face->m_normals[i] = MakeNormalFromPlane(m_vertices[vertex0[0]][vertex0[1]].m_position, m_vertices[vertex1[0]][vertex1[1]].m_position, m_vertices[vertex2[0]][vertex2[1]].m_position);
			}
		}
	}

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			m_vertices[x][y].m_normal = 0.0f;
		}
	}

	for (int y = 0; y < m_numFaces[1]; ++y)
	{
		for (int x = 0; x < m_numFaces[0]; ++x)
		{
			TerrainFace *face = &m_faces[x][y];
			
			for (int i = 0; i < 2; ++i)
			{
				int vertex0[2];
				int vertex1[2];
				int vertex2[2];

				getVerticesForFace(x, y, i, vertex0, vertex1, vertex2);
				m_vertices[vertex0[0]][vertex0[1]].m_normal += face->m_normals[i];
				m_vertices[vertex1[0]][vertex1[1]].m_normal += face->m_normals[i];
				m_vertices[vertex2[0]][vertex2[1]].m_normal += face->m_normals[i];
			}
		}
	}

	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			m_vertices[x][y].m_normal.normalize();
		}
	}
}

void TerrainGenerator::computeVertexLayerIntensities()
{
	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			TerrainVertex *vertex = &m_vertices[x][y];

			if ((m_layers[tlt_earth].m_terrainType == -1 || vertex->m_terrainLayerFlags & BITSHIFT(m_layers[tlt_earth].m_terrainType)) && vertex->m_position.z <= 10000.0f)
				vertex->m_layerIntensities[tlt_earth] = 1.0f - Clamp(((1.0f - vertex->m_normal.z) - m_barrenness) * 12.0f, 0.0f, 1.0f);

			if (m_layers[tlt_green].m_groundSpecNo >= 0)
			{
				Vector4 noise = PerlinOctave(15.0f, 0.6f, 3, Vector4((float)x, (float)y, 0.0f));
				float intensity = Clamp((noise.x + noise.y + noise.z) * 4.5f + 0.7f, 0.0f, 1.0f);

				vertex->m_layerIntensities[tlt_green] = vertex->m_layerIntensities[tlt_earth] * intensity;

				if (intensity > 0.99f)
					vertex->m_layerIntensities[tlt_earth] = 0.0f;
			}

			if (m_placeRiver && vertex->m_position.z < 0.0f)
				vertex->m_layerIntensities[tlt_riverbed] = 1.0f;
		}
	}
}

void TerrainGenerator::selectFaceOrientation()
{
	int possibleDirections[] = { 8, 2, 1, 0 };
	int layerNos[4];

	for (int y = 0; y < m_numFaces[1]; ++y)
	{
		for (int x = 0; x < m_numFaces[0]; ++x)
		{
			TerrainFace *face = &m_faces[x][y];

			for (int i = 0; i < 4; ++i)
			{
				int newX = x + g_possibleMoves[possibleDirections[i]][0];
				int newY = y + g_possibleMoves[possibleDirections[i]][1];

				if (newX >= 0 && newX <= m_numFaces[0] && newY >= 0 && newY <= m_numFaces[1])
				{
					TerrainVertex *newVertex = &m_vertices[newX][newY];

					layerNos[i] = 0;

					for (int j = 1; j < NUM_TERRAIN_LAYERS; ++j)
					{
						if (newVertex->m_layerIntensities[j] > 0.4f)
							layerNos[i] = j;
					}
				}
				else
				{
					layerNos[i] = -1;
				}
			}

			int numInverts = -1;

			if (layerNos[1] != layerNos[0] && layerNos[3] != layerNos[0])
				numInverts = 0;

			if (layerNos[2] != layerNos[1] && layerNos[0] != layerNos[1])
				numInverts = 1;

			if (layerNos[3] != layerNos[2] && layerNos[1] != layerNos[2])
				numInverts = 2;

			if (layerNos[0] == layerNos[3] || layerNos[2] == layerNos[3])
			{
				if (numInverts == -1)
				{
					Vector4 upRight = m_vertices[x + 1][y + 1].m_position - m_vertices[x][y].m_position;
					Vector4 right = m_vertices[x + 1][y].m_position - m_vertices[x][y].m_position;
					Vector4 up = m_vertices[x][y + 1].m_position - m_vertices[x][y].m_position;

					if (upRight.dot(right.cross(up)) > 0.0f)
						face->m_orientation = 0;
					else
						face->m_orientation = 1;
				}
				else if (numInverts == 0 || numInverts == 2)
				{
					face->m_orientation = 0;
				}
				else
				{
					face->m_orientation = 1;
				}
			}
			else
			{
				face->m_orientation = 1;
			}
		}
	}
}

void TerrainGenerator::selectFaceLayers()
{
	int possibleDirections[] = { 8, 2, 1, 0 };
	int layerNos[4];

	for (int y = 0; y < m_numFaces[1]; ++y)
	{
		for (int x = 0; x < m_numFaces[0]; ++x)
		{
			TerrainFace *face = &m_faces[x][y];

			for (int i = 0; i < 4; ++i)
			{
				int newX = x + g_possibleMoves[possibleDirections[i]][0];
				int newY = y + g_possibleMoves[possibleDirections[i]][1];

				if (newX >= 0 && newX <= m_numFaces[0] && newY >= 0 && newY <= m_numFaces[1])
				{
					TerrainVertex *newVertex = &m_vertices[newX][newY];

					layerNos[i] = 0;

					for (int j = 1; j < NUM_TERRAIN_LAYERS; ++j)
					{
						if (newVertex->m_layerIntensities[j] > 0.4f)
							layerNos[i] = j;
					}
				}
				else
				{
					layerNos[i] = -1;
				}
			}

			if (face->m_orientation)
			{
				if (layerNos[0] > 0)
					face->m_layerNos[1] = layerNos[0];
				else if (layerNos[2] > 0)
					face->m_layerNos[1] = layerNos[2];
				else
					face->m_layerNos[1] = layerNos[3];

				if (layerNos[0] > 0)
					face->m_layerNos[0] = layerNos[0];
				else if (layerNos[1] > 0)
					face->m_layerNos[0] = layerNos[1];
				else
					face->m_layerNos[0] = layerNos[2];
			}
			else
			{
				if (layerNos[1] > 0)
					face->m_layerNos[1] = layerNos[1];
				else if (layerNos[2] > 0)
					face->m_layerNos[1] = layerNos[2];
				else
					face->m_layerNos[1] = layerNos[3];
				
				if (layerNos[1] > 0)
					face->m_layerNos[0] = layerNos[1];
				else if (layerNos[3] > 0)
					face->m_layerNos[0] = layerNos[3];
				else
					face->m_layerNos[0] = layerNos[0];
			}
		}
	}
}

void TerrainGenerator::roughenRockVertices()
{
	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			TerrainVertex *vertex = &m_vertices[x][y];

			if (vertex->m_layerIntensities[tlt_green] + vertex->m_layerIntensities[tlt_earth] < 0.5f)
			{
				float increase = Randf(-0.5f, 0.5f) * (1.0f - vertex->m_layerIntensities[tlt_earth]) * m_detail * 0.7f;

				vertex->m_position.z += increase;
			}
		}
	}
}

void TerrainGenerator::computeFaceLayerIntensities()
{
	int possibleDirections[] = { 8, 2, 1, 0 };
	int layerNos[4];

	for (int y = 0; y < m_numFaces[1]; ++y)
	{
		for (int x = 0; x < m_numFaces[0]; ++x)
		{
			TerrainFace *face = &m_faces[x][y];
			int numGreenLayers = 0;
			float greenLayerIntensity = 0.0f;

			for (int i = 0; i < 4; ++i)
			{
				int newX = x + g_possibleMoves[possibleDirections[i]][0];
				int newY = y + g_possibleMoves[possibleDirections[i]][1];
				TerrainVertex *newVertex = &m_vertices[newX][newY];

				if (newX >= 0 && newX <= m_numFaces[0] && newY >= 0 && newY <= m_numFaces[1])
				{
					layerNos[i] = 0;

					for (int j = 1; j < NUM_TERRAIN_LAYERS; ++j)
					{
						if (newVertex->m_layerIntensities[j] > 0.4f)
							layerNos[i] = j;
					}
				}
				else
				{
					layerNos[i] = -1;
				}

				if (layerNos[i] == tlt_green)
					numGreenLayers++;

				greenLayerIntensity += newVertex->m_layerIntensities[tlt_green];
				
				float intensity = 1.0f;
				
				for (int j = NUM_TERRAIN_LAYERS - 1; j >= 0; --j)
				{
					float val = newVertex->m_layerIntensities[j] * intensity;

					face->m_layerIntensities[j] += val * 0.25f;
					intensity -= val;
				}
			}
		}
	}
}

void TerrainGenerator::getVerticesForFace(int x, int y, int triangleNo, int *vertex0, int *vertex1, int *vertex2)
{
	TerrainFace *face = &m_faces[x][y];

	if (face->m_orientation)
	{
		vertex0[0] = x;
		vertex0[1] = y;

		if (triangleNo == 0)
		{
			vertex1[0] = x + 1;
			vertex1[1] = y;
			vertex2[0] = x + 1;
			vertex2[1] = y + 1;
		}
		else
		{
			vertex1[0] = x + 1;
			vertex1[1] = y + 1;
			vertex2[0] = x;
			vertex2[1] = y + 1;
		}
	}
	else
	{
		vertex0[0] = x + 1;
		vertex0[1] = y;

		if (triangleNo == 0)
		{
			vertex1[0] = x;
			vertex1[1] = y + 1;
			vertex2[0] = x;
			vertex2[1] = y;
		}
		else
		{
			vertex1[0] = x + 1;
			vertex1[1] = y + 1;
			vertex2[0] = x;
			vertex2[1] = y + 1;
		}
	}
}

float TerrainGenerator::getRandom4X()
{
	return (Randf() + Randf() + Randf() + Randf()) / 4;
}

float TerrainGenerator::getVertexNoise(int x, int y)
{
	Vector4 noise = PerlinOctave(m_noiseFrequency, 0.5f, 1, m_positionNoiseRandomness + m_vertices[x][y].m_position);

	return noise.x + noise.y + noise.z;
}

void TerrainGenerator::removeTerrainTypeFlags(unsigned int flags)
{
	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			m_vertices[x][y].m_terrainLayerFlags &= ~flags;
		}
	}
}

void TerrainGenerator::setTerrainTypeDepth(int type, float depth)
{
	for (int y = 0; y <= m_numFaces[1]; ++y)
	{
		for (int x = 0; x <= m_numFaces[0]; ++x)
		{
			if (m_vertices[x][y].m_terrainLayerFlags & BITSHIFT(type))
				m_vertices[x][y].m_position.z = depth;
		}
	}
}
