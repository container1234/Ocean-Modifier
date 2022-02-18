#include <tesla.hpp>
#ifndef OCEAN_MODIFIER_INCLUDE_RANDOM_H_
#define OCEAN_MODIFIER_INCLUDE_RANDOM_H_
namespace sead
{
    namespace random
    {
        class Random
        {
        public:
            Random(u32 seed)
            {
                this->seed1 = 0x6C078965 * (seed ^ (seed >> 30)) + 1;
                this->seed2 = 0x6C078965 * (seed1 ^ (seed1 >> 30)) + 2;
                this->seed3 = 0x6C078965 * (seed2 ^ (seed2 >> 30)) + 3;
                this->seed4 = 0x6C078965 * (seed3 ^ (seed3 >> 30)) + 4;
            }

            u32 getU32()
            {
                u32 n = seed1 ^ (seed1 << 11);
                seed1 = seed2;
                seed2 = seed3;
                seed3 = seed4;
                seed4 = n ^ (n >> 8) ^ seed4 ^ (seed4 >> 19);
                return seed4;
            }

        private:
            u32 seed1;
            u32 seed2;
            u32 seed3;
            u32 seed4;
        };
    }
}
#endif // OCEAN_MODIFIER_INCLUDE_RANDOM_H_
