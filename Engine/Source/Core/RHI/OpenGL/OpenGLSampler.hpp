#include "Sampler.hpp"

namespace RightEngine
{
    class OpenGLSampler : public Sampler
    {
    public:
        OpenGLSampler(const SamplerDescriptor& desc);
        virtual ~OpenGLSampler();

        virtual void Bind(uint32_t slot = 0) const;

    private:
        uint32_t id;

    private:
        void Init();
    };
}
