#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <sys/stat.h>

/*
  This code is derived from LZW@RosettaCode for UA CS435
*/

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

// Returns the contents of a file as a string
std::string blockIO(std::string fileName) {
    std::ifstream myfile2;
    myfile2.open (fileName.c_str(),  std::ios::binary);

    struct stat filestatus;
    stat(fileName.c_str(), &filestatus );
    long fsize = filestatus.st_size; //get the size of the file in bytes

    char c2[fsize];
    myfile2.read(c2, fsize);

    std::string s = "";
    long count = 0;
    while(count<fsize) {
        s+= c2[count];
        count++;
    }

    return s;
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

            std::ofstream out(file.substr(0, file.find(".lzw")) + "2");
            out << decompressed;
        } catch (std::exception e) {
            std::cout << e.what();
        }
    } else {
        std::cout << "Invalid second parameter. Must be either c(ompress) or d(ecompress).";
        return 1;
    }
}



