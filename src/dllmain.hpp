#pragma once
#include <Windows.h>
#include <amethyst/runtime/AmethystContext.hpp>
#include <minecraft/src/common/world/item/BlockItem.hpp>
#include "turtle/TurtleBlock.hpp"

#define ModFunction extern "C" __declspec(dllexport)

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    return TRUE;
}

void RegisterItems(ItemRegistry* registry);
void RegisterBlocks(BlockDefinitionGroup* blockDef);