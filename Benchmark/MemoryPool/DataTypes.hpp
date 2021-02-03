#pragma once

#include <array>
#include <cstdint>

struct Component32b
{
    uint64_t typeId_ = 0u;
    uint64_t bulletsUntilReload_ = 0u;
    uint64_t bulletsLeft_ = 0u;
    uint64_t durability_ = 0u;
};

static_assert (sizeof (Component32b) == 32u);

struct Component192b
{
    uint64_t playerId_ = 0u;
    std::array <uint64_t, 16> playerAvatars_ {0u};
    std::array <uint32_t, 13> someOtherShitToMakeThisComponentHuge_ {0u};
};

static_assert (sizeof (Component192b) == 192u);

struct Component1032b
{
    std::array <uint64_t, 129> fictionData_ {0u};
};

static_assert (sizeof (Component1032b) == 1032u);

struct TrivialComponent32b
{
    uint64_t typeId_;
    uint64_t bulletsUntilReload_;
    uint64_t bulletsLeft_;
    uint64_t durability_;
};

static_assert (sizeof (TrivialComponent32b) == 32u);
static_assert (std::is_trivial_v <TrivialComponent32b>);

struct TrivialComponent192b
{
    uint64_t playerId_;
    std::array <uint64_t, 16> playerAvatars_;
    std::array <uint32_t, 13> someOtherShitToMakeThisComponentHuge_;
};

static_assert (sizeof (TrivialComponent192b) == 192u);
static_assert (std::is_trivial_v <TrivialComponent192b>);

struct TrivialComponent1032b
{
    std::array <uint64_t, 129> fictionData_;
};

static_assert (sizeof (TrivialComponent1032b) == 1032u);
static_assert (std::is_trivial_v <TrivialComponent1032b>);