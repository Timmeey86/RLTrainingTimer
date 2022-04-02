#pragma once

// https://discord.com/channels/862068148328857700/862081441080410143/959773281290948618
namespace ImGui
{
    struct Disable
    {
        explicit Disable(const bool should_disable_, float alpha = 0.5f);

        Disable(const Disable& other) = delete;
        Disable(Disable&& other) noexcept = delete;
        Disable& operator=(const Disable& other) = delete;
        Disable& operator=(Disable&& other) noexcept = delete;
        ~Disable();


        bool should_disable;
    };
}