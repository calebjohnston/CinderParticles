#include "cinder/Rand.h"

#include "ParticleSystem.h"
#include "Emitter.h"

Emitter::Emitter(const unsigned int rate, const ci::Vec2f& pos, const ci::Vec2f& dir)
 : mEmitterRate(rate), mPosition(pos), mDirection(dir)
{}

Emitter::~Emitter()
{}