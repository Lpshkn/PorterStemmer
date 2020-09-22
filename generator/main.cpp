#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <vector>
#include <set>
#include <random>
#include "argparse.hpp"

// Function gets a list of words from the particular file
std::vector<std::wstring> getWordsFromDataset(const std::string& filename);
// Function makes an argument parser of the command line
ArgumentParser getArgumentParser(int argc, const char **argv);
// Function gets a list of parameters from the config file
std::vector<std::string> getConfigParameters(const std::string& filename);
// Function writes the output samples of the data
void writeOutputSamplesData(const std::vector<std::string>& parameters, const std::string& filename, const std::vector<std::wstring>& words);

int main(int argc, const char **argv) {
    auto parser = getArgumentParser(argc, argv);
    auto configFile = parser.retrieve<std::string>("config");
    auto outputFile = parser.retrieve<std::string>("output");
    auto dictionary = parser.retrieve<std::string>("dictionary");

    auto words = getWordsFromDataset(dictionary);
    auto parameters = getConfigParameters(configFile);

    writeOutputSamplesData(parameters, outputFile, words);

    return 0;
}

std::vector<std::wstring> getWordsFromDataset(const std::string& filename) {
    std::wifstream file(filename);
    std::vector<std::wstring> words;

    std::wstring line;
    if (file.is_open())
        while (std::getline(file, line))
            words.emplace_back(line);
    else {
        std::cerr << "Error: the name of the data file is incorrect!" << std::endl;
        exit(-1);
    }

    return words;
}

ArgumentParser getArgumentParser(int argc, const char **argv) {
    ArgumentParser parser;
    parser.addArgument("-c", "--config", 1, false);
    parser.addArgument("-o", "--output", 1, false);
    parser.addArgument("-d", "--dictionary", 1, false);
    parser.parse(argc, argv);

    return parser;
}

std::vector<std::string> getConfigParameters(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<std::string> parameters;

    std::string line;
    if (file.is_open())
        while (std::getline(file, line)) {
            parameters.emplace_back(line);
            if (std::regex_search(line, std::regex("[^0-9]+"))) {
                std::cerr << "Error: the config file contains incorrect data. Please rewrite the parameters as integer numbers." << std::endl;
                exit(-3);
            }
        }
    else {
        std::cerr << "Error: the name of a config file is incorrect!" << std::endl;
        exit(-2);
    }
    return parameters;
}

void writeOutputSamplesData(const std::vector<std::string>& parameters, const std::string& filename, const std::vector<std::wstring>& words) {
    std::string outputFilename;
    for (auto &parameter : parameters) {
        // Delete the extension of the file if it exists
        std::string processedFilename = std::regex_replace(filename, std::regex("\\.txt$"), "");
        outputFilename = processedFilename + parameter + ".txt";
        std::wofstream file(outputFilename);

        int count = 0;
        try {
            count = std::stoi(parameter);
        }
        catch (std::invalid_argument& e) {
            std::cerr << "Error: the config file contains incorrect data. Please rewrite the parameters as integer numbers." << std::endl;
            exit(-3);
        }

        // Get a vector of shuffled indexes
        std::vector<int> indexes(count);
        std::iota(indexes.begin(), indexes.end(), 0);
        std::random_shuffle(indexes.begin(), indexes.end());

        for (auto &index : indexes)
            file << words[index] << std::endl;

        file.close();
    }
}