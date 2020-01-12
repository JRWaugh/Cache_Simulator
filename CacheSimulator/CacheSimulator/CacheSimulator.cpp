#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <Vector>
#include <string>
#include <optional>
#include "Cache.h"

enum class Choice { INIT = 1, READ, REPORT, EXIT };

int main()
{
    /* This program simulates a write-back/write-allocate cache.
     * The simulation assumes the bus between the cache and main memory is sufficiently wide to fill any block in one access.
     */
    Choice choice = Choice::INIT;
    unsigned int totalCycles = 0;
    std::string input, last_file;
    std::ifstream myfile;
    std::vector<std::shared_ptr<Cache>> caches;

    while (choice != Choice::EXIT) {
        while (std::cout <<
            "=== Cache Simulator ===\n1. Cache/memory initialisation\n2. Read trace file from disk\n3. Print report\n4. Exit program\nInput selection: " &&
            getline(std::cin, input) &&
            input.find_first_of("1234"))
            std::cout << "Invalid input." << std::endl;
        choice = static_cast<Choice>(input[0] - '0');
        std::cout << std::endl;

        switch (choice) {
        case Choice::INIT:
        {
            unsigned int memoryHitTime, cacheCount, blockSize, setSize, totalSize, policy, hitTime = 0;
            caches.clear();

            while (std::cout << "Enter memory access time: " && getline(std::cin, input) && input.find_first_of("0123456789"))
                std::cout << "Invalid input." << std::endl;
            memoryHitTime = std::stoi(input);

            while (std::cout << "Enter number of cache levels: " && getline(std::cin, input) && input.find_first_of("123456789"))
                std::cout << "Invalid input." << std::endl;
            cacheCount = std::stoi(input);

            for (unsigned int i = 0; i < cacheCount; ++i) {
                std::cout << "\nL" << i + 1 << " Cache Initialisation:" << std::endl;

                while (std::cout << "Enter block size in bytes as a power of 2: 2^" && getline(std::cin, input) && input.find_first_of("0123456789"))
                    std::cout << "Invalid input." << std::endl;
                blockSize = std::stoi(input);

                while (std::cout << "Enter set size in bytes as a power of 2: 2^" && getline(std::cin, input) && input.find_first_of("0123456789"))
                    std::cout << "Invalid input." << std::endl;
                setSize = std::stoi(input);

                while (std::cout << "Enter total cache size in bytes as a power of 2 (must be at least 2^" << blockSize + setSize << "): 2^" &&
                    getline(std::cin, input) &&
                    input.find_first_of("0123456789") ||
                    (totalSize = std::stoi(input)) < blockSize + setSize)
                    std::cout << "Invalid input." << std::endl;

                while (std::cout << "Select replacement policy (1. FIFO, 2. LRU, 3. Random): " && getline(std::cin, input) && input.find_first_of("123"))
                    std::cout << "Invalid input." << std::endl;
                policy = std::stoi(input);

                while (std::cout << "Enter hit time: " && getline(std::cin, input) && input.find_first_of("0123456789"))
                    std::cout << "Invalid input." << std::endl;
                hitTime = std::stoi(input);

                if (totalSize < 32) {
                    caches.emplace_back(new Cache(blockSize, setSize, totalSize, (Policy)policy, hitTime, memoryHitTime));
                    if (i > 0)
                        caches[i - 1]->setLowerMem(caches[i]);
                }
                else {
                    std::cout << "Invalid cache settings! Total size can not exceed 2^31 bytes." << std::endl;
                    --i;
                }


            }
            std::cout << std::endl;
        }
        break;

        case Choice::READ:
            if (caches.empty())
                std::cout << "Cannot read trace file until the cache has been initialised." << std::endl << std::endl;
            else {
                unsigned char instruction;
                unsigned int address, cycles;
                std::optional<unsigned int> result = 0;
                totalCycles = 0;

                std::cout << "Enter filename of trace file: ";
                getline(std::cin, input);
                myfile.open(input);

                if (myfile.is_open()) {
                    for (auto& n : caches) {
                        if (input != last_file) // New file requires invalidating all blocks
                            n->invalidateCache();
                        n->resetCacheStats();
                    }

                    while (myfile >> instruction >> std::hex >> address >> cycles && (result = caches[0]->accessAddress(address, instruction)))
                        totalCycles += result.value() + cycles;

                    if (result.has_value())
                        std::cout << "Simulation complete." << std::endl << std::endl;
                    else
                        std::cout << "Simulation terminated because an error occurred while reading from trace file." << std::endl << std::endl;
                    
                    myfile.close();
                    last_file = input;
                }
                else
                    std::cout << "Unable to open file." << std::endl << std::endl;
            }
            break;

        case Choice::REPORT:
            if (totalCycles == 0)
                std::cout << "Cannot print a report until a simulation has been run." << std::endl << std::endl;
            else {
                std::cout << "Report for each memory level:" << std::endl;
                for (unsigned int i = 0; i < caches.size(); ++i)
                    std::cout << "Cache level " << i + 1 << ":\n" << *caches[i] << std::endl << std::endl;
                std::cout << left << setw(20) << "Total cycles: " << right << setw(10) << totalCycles << std::endl << std::endl;
            }
            break;

        case Choice::EXIT:
            std::cout << "Closing program." << std::endl;
            break;

        default:
            std::cout << "Invalid input." << std::endl;
            break;
        }
    }
    return 0;
}