// dllmain.cpp : Defines the entry point for the DLL application.
#pragma once
#include "pch.h"
#include "aisearch.h"

#define EXPORT __declspec(dllexport)

extern "C"
{
    EXPORT int* create_puzzle(int size)
    {
        int* return_puzzle = new int[size * size];
        std::vector<std::vector<int>> random_puzzle = aisearch::CreatePuzzle(size);
        std::cout << "1\n";
        int i = 0;
        for (auto const& row : random_puzzle) {
            for (auto const& val : row) {
                return_puzzle[i] = (int)val;
                i++;
            }
        }
        std::cout << "2\n";
        return return_puzzle;
    }

    EXPORT int release_memory(int* pArray)
    {
        delete[] pArray;
        return 0;
    }

    EXPORT int test()
    {
        std::cout << "hi" << std::endl;
        return 666;
    }
}


