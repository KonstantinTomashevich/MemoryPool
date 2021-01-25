#pragma once

#include <array>
#include <cstdint>

struct Component_32b
{
    uint64_t typeId_;
    uint64_t bulletsUntilReload_;
    uint64_t bulletsLeft_;
    uint64_t durability_;
};

static_assert (sizeof (Component_32b) == 32u);

struct Component_192b
{
    uint64_t playerId_;
    std::array <uint64_t, 16> playerAvatars_;
    std::array <uint32_t, 13> someOtherShitToMakeThisComponentHuge_;
};

static_assert (sizeof (Component_192b) == 192u);

struct Component_1032b
{
    std::array <uint64_t, 129> fictionData_;
};

static_assert (sizeof (Component_1032b) == 1032u);