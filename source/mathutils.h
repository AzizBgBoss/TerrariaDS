// Math utilities for Perlin noise and other functions

static int clamp(int val, int min, int max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

static int min(int val, int min)
{
	if (val < min)
		return val;
	return min;
}

int rando(int min, int max) // Random function, returns a random integer between min and max (max included)
{
	return rand() % (max - min + 1) + min;
}

// Hash function
static inline uint32_t murmur_32_scramble(uint32_t k)
{
	k *= 0xcc9e2d51;
	k = (k << 15) | (k >> 17);
	k *= 0x1b873593;
	return k;
}
uint32_t murmur3_32(const uint8_t *key, size_t len, uint32_t seed)
{
	uint32_t h = seed;
	uint32_t k;
	/* Read in groups of 4. */
	for (size_t i = len >> 2; i; i--)
	{
		// Here is a source of differing results across endiannesses.
		// A swap here has no effects on hash properties though.
		memcpy(&k, key, sizeof(uint32_t));
		key += sizeof(uint32_t);
		h ^= murmur_32_scramble(k);
		h = (h << 13) | (h >> 19);
		h = h * 5 + 0xe6546b64;
	}
	/* Read the rest. */
	k = 0;
	for (size_t i = len & 3; i; i--)
	{
		k <<= 8;
		k |= key[i - 1];
	}
	// A swap is *not* necessary here because the preceding loop already
	// places the low bytes in the low places according to whatever endianness
	// we use. Swaps only apply when the memory is copied in a chunk.
	h ^= murmur_32_scramble(k);
	/* Finalize. */
	h ^= len;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
	return h;
}

// Hash function for Perlin noise
int hash(int x, int seed)
{
	// Use MurmurHash3 to generate a hash value based on the input x and seed
	uint32_t hashValue = murmur3_32((const uint8_t *)&x, sizeof(x), seed);
	// Return the hash value modulo 256 to fit in a byte
	return hashValue & 0x7fffffff; // Ensure it's positive
								   // Note: This will return a value in the range [0, 255] which is suitable for use in Perlin noise calculations.
}

// Linear interpolation
float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

// Fade curve (Perlin-style smoothing function)
float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

// Gradient function
float grad(int hash, float x)
{
	// Use last bit to determine gradient direction
	return (hash & 1) == 0 ? x : -x;
}

// 1D Perlin noise
float perlin1d(float x, int seed)
{
	int xi = (int)x;
	float xf = x - xi;

	int h0 = hash(xi, seed);
	int h1 = hash(xi + 1, seed);

	float g0 = grad(h0, xf);
	float g1 = grad(h1, xf - 1.0f);

	float u = fade(xf);

	return lerp(g0, g1, u); // Returns value in range ~[-1, 1]
}

float grad2D(int hash, float x, float y)
{
	int h = hash & 7; // 8 directions
	switch (h)
	{
	case 0:
		return x + y;
	case 1:
		return -x + y;
	case 2:
		return x - y;
	case 3:
		return -x - y;
	case 4:
		return x;
	case 5:
		return -x;
	case 6:
		return y;
	default:
		return -y;
	}
}

float perlin2d(float x, float y, int seed)
{
	int xi = (int)floorf(x);
	int yi = (int)floorf(y);
	float xf = x - xi;
	float yf = y - yi;

	int aa = hash(xi + hash(yi, seed), seed);
	int ab = hash(xi + hash(yi + 1, seed), seed);
	int ba = hash(xi + 1 + hash(yi, seed), seed);
	int bb = hash(xi + 1 + hash(yi + 1, seed), seed);

	float dotAA = grad2D(aa, xf, yf);
	float dotBA = grad2D(ba, xf - 1, yf);
	float dotAB = grad2D(ab, xf, yf - 1);
	float dotBB = grad2D(bb, xf - 1, yf - 1);

	float u = fade(xf);
	float v = fade(yf);

	float x1 = lerp(dotAA, dotBA, u);
	float x2 = lerp(dotAB, dotBB, u);

	return lerp(x1, x2, v); // value in ~[-1, 1]
}

float fractalPerlin1D(float x, int octaves, float persistence, float scale, int seed)
{
	float total = 0;
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin1d(x * frequency, seed) * amplitude;
		maxAmplitude += amplitude;

		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total / maxAmplitude; // normalize to -1..1
}

float fractalPerlin2D(float x, float y, int octaves, float persistence, float scale, int seed)
{
	float total = 0;
	float frequency = scale;
	float amplitude = 1;
	float maxAmplitude = 0;

	for (int i = 0; i < octaves; i++)
	{
		total += perlin2d(x * frequency, y * frequency, seed) * amplitude;
		maxAmplitude += amplitude;

		amplitude *= persistence;
		frequency *= 2.0f;
	}

	return total / maxAmplitude; // normalize to -1..1
}