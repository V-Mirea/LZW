#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <sys/stat.h>
#include <cmath>

/*
  This code is derived from LZW@RosettaCode for UA CS435
*/
std::string compressInts(std::vector<int> code) {
    std::string compressed;
    int bits = 9; //Start with 9 bit numbers

    for (int i : code) {
        if (i >= pow(2, bits)) // If the number is too big, raise the bit amount by one to fit it
            bits++;


    }
}

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
template <typename Iterator>
Iterator compress(const std::string &uncompressed, Iterator result) {
  // Build the dictionary.
  int dictSize = 256;
  std::map<std::string,int> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[std::string(1, i)] = i;

  std::string w;
  for (std::string::const_iterator it = uncompressed.begin();
       it != uncompressed.end(); ++it) {
    char c = *it;
    std::string wc = w + c;
    if (dictionary.count(wc))
      w = wc;
    else {
      *result++ = dictionary[w];
      // Add wc to the dictionary. Assuming the size is 4096!!!
      if (dictionary.size()<4096)
         dictionary[wc] = dictSize++;
      w = std::string(1, c);
    }
  }

  // Output the code for w.
  if (!w.empty())
    *result++ = dictionary[w];
  return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
template <typename Iterator>
std::string decompress(Iterator begin, Iterator end) {
  // Build the dictionary.
  int dictSize = 256;
  std::map<int,std::string> dictionary;
  for (int i = 0; i < 256; i++)
    dictionary[i] = std::string(1, i);

  std::string w(1, *begin++);
  std::string result = w;
  //std::cout << result<<"???:::\n";
  std::string entry;
  for ( ; begin != end; begin++) {
    int k = *begin;
    if (dictionary.count(k))
      entry = dictionary[k];
    else if (k == dictSize)
      entry = w + w[0];
    else
      throw "Bad compressed k";

    result += entry;

    // Add w+entry[0] to the dictionary.
    if (dictionary.size()<4096)
      dictionary[dictSize++] = w + entry[0];

    w = entry;
  }
  return result;
}

// From BlockIO.cpp from project 1
std::string blockIO(std::string fname) {
   std::ifstream in (fname.c_str(), std::ios::binary);

   std::streampos begin = in.tellg(); // Set to the beginninggit rm -r --cached .
   in.seekg (0, std::ios::end); // Move to the end
   std::streampos end = in.tellg(); // Set to the end

   std::streampos size = end - begin; //size of the file in bytes

   in.seekg (0, std::ios::beg); // Move back to the beginning

   char* memblock = new char[size];
   in.read (memblock, size); //read the entire file
   memblock[size] = '\0'; //add a terminator

   in.close();

   //check what's in the block
   std::string str(memblock);

   return str;
}

int main(int argc, char* argv[]) {
    // Arguments: lzw435.exe c/d [filename]
    if (argc != 3) {
        std::cout << "Incorrect number of arguments.";
        return 1;
    }

    std::string file = argv[2];
    if (tolower(*argv[1]) == 'c') {
        try {
            std::vector<int> compressed;
            compress(blockIO(file), std::back_inserter(compressed));

            std::ofstream out(file + ".lzw");
            copy(compressed.begin(), compressed.end(), std::ostream_iterator<int>(out, ", ")); // Write the compressed string into the file
        } catch (std::exception e) {
            std::cout << e.what();
        }

    } else if (tolower(*argv[1]) == 'd') {
        try {
            std::ifstream in(file);
            std::vector<int> compressed;

            int i;

            while (in >> i) {
                compressed.push_back(i);

                if (in.peek() == ',')
                    in.ignore();
            }

            std::string decompressed = decompress(compressed.begin(), compressed.end());
            std::cout << decompressed << std::endl;

            std::ofstream out(file + '2', std::ios::binary);
            out.write(decompressed.c_str(), decompressed.size());
        } catch (std::exception e) {
            std::cout << e.what();
        }
    } else {
        std::cout << "Invalid second parameter. Must be either c(ompress) or d(ecompress).";
        return 1;
    }
}



