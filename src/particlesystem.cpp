#include "particlesystem.hpp"

#include <algorithm>

void ParticleSystem::update(float dt) {
	for (auto& particle : m_particles) {
		particle.pos.x += particle.velocity.x * dt;
		particle.pos.y += particle.velocity.y * dt;

		particle.lifetime -= dt;
	}

	std::erase_if(m_particles, [](const Particle& p) {
		return p.lifetime <= 0.0f;
	});
}

void ParticleSystem::draw() const {
	for (const auto& particle : m_particles) {
		DrawCircleV(particle.pos, particle.size, particle.color);
	}
}

void ParticleSystem::emit(Vector2 pos, int count) {
	for (int i = 0; i < count; i++) {
		Particle p;
		p.pos = pos;

		p.velocity = {
			static_cast<float>(GetRandomValue(-100, 100)),
			static_cast<float>(GetRandomValue(-100, 100))
		};

		p.maxLifeTime = 0.5f;
		p.lifetime = p.maxLifeTime;

		p.size = 3.0f;
		p.color = ORANGE;
		m_particles.push_back(p);
	}
}