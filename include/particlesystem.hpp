#pragma once
#include <raylib.h>

#include <vector>

struct Particle {
	Vector2 pos;
	Vector2 velocity;
	float lifetime;
	float maxLifeTime;
	float size;
	Color color;
};

class ParticleSystem
{
public:
	void update(float dt);
	void draw() const;

	void emit(Vector2 pos, int count);

private:
	std::vector<Particle> m_particles;
};

