#pragma once

#include "stdafx.h"
#include "util.h"

std::vector<uint8_t> Decode(const std::vector<uint8_t>& data, const size_t outBufferSize = DEFAULT_BUFFER_SIZE) {
    BrotliDecoderState* state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
    if (state == nullptr) {
        return {};
    }

    std::vector<uint8_t> outputBuffer;
    BrotliDecoderResult result = BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT;

    size_t available_in = data.size();
    const uint8_t* next_in = data.data();

    uint8_t* output = new uint8_t[outBufferSize];

    size_t available_out = outBufferSize;
    uint8_t* next_out = output;

    size_t total_out = 0;

    do {
        result = BrotliDecoderDecompressStream(state, &available_in, &next_in,
            &available_out, &next_out, 0);

        if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
            size_t out_size = (size_t)(next_out - output);
            total_out += out_size;
            if (out_size == 0) break;

            outputBuffer.insert(outputBuffer.end(), output, output + out_size);

            available_out = outBufferSize;
            next_out = output;
        }
        else if (result == BROTLI_DECODER_RESULT_SUCCESS) {
            break;
        }
        else {
            return {};
        }

    } while (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT);

    if (output != nullptr) {
        delete[] output;
    }

    return outputBuffer;
}

std::vector<uint8_t> Encode(const std::vector<uint8_t>& data) {
    const size_t inputBufferSize = data.size();
    const uint8_t* inputBuffer = data.data();

    size_t outBufferSize = data.size();
    std::vector<uint8_t> outBuffer(outBufferSize);

    BrotliEncoderCompress(BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW,
        BrotliEncoderMode::BROTLI_MODE_TEXT, inputBufferSize,
        inputBuffer, &outBufferSize, outBuffer.data());

    outBuffer.resize(outBufferSize);
    return outBuffer;
}

bool ReadCache(const std::string& cache_path, std::vector<uint8_t>& out_cache)
{
    std::ifstream ifs(cache_path, std::ios::binary);

    if (!ifs.good()) return false;

    ifs.seekg(0, std::ios::end);
    std::streampos fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<uint8_t> encodedData(fileSize);
    ifs.read(reinterpret_cast<char*>(encodedData.data()), fileSize);
    ifs.close();

    out_cache = Decode(encodedData);
    encodedData.clear();
    
    return true;
}

bool WriteCache(const std::string& cache_path, std::vector<uint8_t>& in_cache)
{
    std::ofstream outfile(cache_path, std::ios::binary);
    if (!outfile.good()) return false;


    std::vector<uint8_t> encodedData = Encode(in_cache);
    outfile.write(reinterpret_cast<const char*>(encodedData.data()), encodedData.size());
    outfile.close();

    encodedData.clear();
    return true;
}
