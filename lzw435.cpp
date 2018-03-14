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



int countBits(int num) {
    int bits = 0;

    while(num > 0) {
        bits++;
        num >>= 1;
    }

    return bits;
}

std::string int2BinaryString(int c, int cl) {
      std::string p = ""; //a binary code string with code length = cl
      int code = c;
      while (c>0) {
		   if (c%2==0)
            p="0"+p;
         else
            p="1"+p;
         c=c>>1;
      }
      int zeros = cl-p.size();
      if (zeros<0) {
         std::cout << "\nWarning: Overflow. code " << code <<" is too big to be coded by " << cl <<" bits!\n";
         p = p.substr(p.size()-cl);
      }
      else {
         for (int i=0; i<zeros; i++)  //pad 0s to left of the binary code if needed
            p = "0" + p;
      }
      return p;
}

int binaryString2Int(std::string p) {
   int code = 0;
   if (p.size()>0) {
      if (p.at(0)=='1')
         code = 1;
      p = p.substr(1);
      while (p.size()>0) {
         code = code << 1;
		   if (p.at(0)=='1')
            code++;
         p = p.substr(1);
      }
   }
   return code;
}

// Compress a string to a list of output symbols.
// The result will be written to the output iterator
// starting at "result"; the final iterator is returned.
//template <typename Iterator>
std::string compress(const std::string uncompressed) {
  // Build the dictionary.
    int dictSize = 256;
    std::map<std::string,int> dictionary;
    for (int i = 0; i < 256; i++)
    dictionary[std::string(1, i)] = i;

    std::string result = "";

    std::string w;
    for (std::string::const_iterator it = uncompressed.begin(); it != uncompressed.end(); ++it) {
        char c = *it;
        std::string wc = w + c;
        if (dictionary.count(wc))
            w = wc;
        else {
            result += int2BinaryString(dictionary[w], countBits(dictSize));
            // Add wc to the dictionary. Assuming the size < 2^16!!!
            if (dictSize < 65536)
                dictionary[wc] = dictSize++;
            w = std::string(1, c);
        }
    }

    // Output the code for w.
    if (!w.empty())
        result += int2BinaryString(dictionary[w], countBits(dictSize));
    return result;
}

// Decompress a list of output ks to a string.
// "begin" and "end" must form a valid range of ints
//template <typename Iterator>
std::string decompress(const std::string &compressed) {
  // Build the dictionary.
    int dictSize = 256;
    std::map<int,std::string> dictionary;
    for (int i = 0; i < 256; i++)
        dictionary[i] = std::string(1, i);

    int index = 0; // Will track our position in the compressed string
    int bits = 9;
    std::string entry;
    int currentNum;

    std::string w(1, binaryString2Int(compressed.substr(index, bits)));
    std::string result = w;
    index += bits;

    while (index < compressed.size()) {
    //for ( ; begin != end; begin++) {

        currentNum = binaryString2Int(compressed.substr(index, bits));

        if (dictionary.count(currentNum))
            entry = dictionary[currentNum];
        else if (currentNum == dictSize)
            entry = w + w[0];
        else
            throw "Bad compressed k";

        result += entry;

        // Add w+entry[0] to the dictionary.
        if (dictSize < 65536) {
            dictionary[dictSize++] = w + entry[0];
            bits = countBits(dictSize);
        }

        w = entry;
        index += bits;

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
   for (int i = 0; i < size; i++) {
    in.read (memblock + i, 1); //read the file byte by byte
   }
   memblock[size] = '\0'; //add a terminator

   in.close();

   //check what's in the block
   std::string str(memblock);

   return str;
}



/*
void binaryIODemo(std::vector<int> compressed) {
   int c = 69;
   int bits = 9;
   std::string p = int2BinaryString(c, bits);
   std::cout << "c=" << c <<" : binary string="<<p<<"; back to code=" << binaryString2Int(p)<<"\n";

   std::string bcode= "";
   for (std::vector<int>::iterator it = compressed.begin() ; it != compressed.end(); ++it) {
      if (*it<256)
         bits = 8;
      else
         bits = 9;

      bits = 12;
      p = int2BinaryString(*it, bits);
      std::cout << "c=" << *it <<" : binary string="<<p<<"; back to code=" << binaryString2Int(p)<<"\n";
      bcode+=p;
   }

   //writing to file
   std::cout << "string 2 save : "<<bcode << "\n";
   std::string fileName = "example435.lzw";
   std::ofstream myfile;
   myfile.open(fileName.c_str(),  std::ios::binary);

   std::string zeros = "00000000";
   if (bcode.size()%8!=0) //make sure the length of the binary string is a multiple of 8
      bcode += zeros.substr(0, 8-bcode.size()%8);

   int b;
   for (int i = 0; i < bcode.size(); i+=8) {
      b = 1;
      for (int j = 0; j < 8; j++) {
         b = b<<1;
         if (bcode.at(i+j) == '1')
           b+=1;
      }
      char c = (char) (b & 255); //save the string byte by byte
      myfile.write(&c, 1);
   }
   myfile.close();

   //reading from a file
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
      unsigned char uc =  (unsigned char) c2[count];
      std::string p = ""; //a binary string
      for (int j=0; j<8 && uc>0; j++) {
		   if (uc%2==0)
            p="0"+p;
         else
            p="1"+p;
         uc=uc>>1;
      }
      p = zeros.substr(0, 8-p.size()) + p; //pad 0s to left if needed
      s+= p;
      count++;
   }
   myfile2.close();
   std::cout << " saved string : "<<s << "\n";
}
*/

int main(int argc, char* argv[]) {
    // Arguments: lzw435.exe c/d [filename]
    if (argc != 3) {
        std::cout << "Incorrect number of arguments.";
        return 1;
    }

    std::string file = argv[2];
    if (tolower(*argv[1]) == 'c') {
        try {
            std::string bcode = compress(blockIO(file));

            /*
            int bits = 9;
            std::string bcode= "";
            std::string p;
            for (std::vector<int>::iterator it = compressed.begin() ; it != compressed.end(); ++it) {
                if (*it >= pow(2, bits))
                    bits++;
                //bits = 12;

                p = int2BinaryString(*it, bits);
                bcode+=p;
            }
            */

            //std::cout << bcode;
            std::string zeros = "00000000";
            if (bcode.size()%8!=0) //make sure the length of the binary string is a multiple of 8
                bcode += zeros.substr(0, 8-bcode.size()%8);

            //std::cout << bcode;
            std::ofstream myfile;
            myfile.open((file + ".lzw").c_str(),  std::ios::binary);

            //save the string byte by byte
            int b;
            for (int i = 0; i < bcode.size(); i+=8) {
                b = 1;
                for (int j = 0; j < 8; j++) {
                    b = b<<1;
                    if (bcode.at(i+j) == '1')
                    b+=1;
                }
                char c = (char) (b & 255);
                myfile.write(&c, 1);
            }
            myfile.close();

            //binaryIODemo(compressed);

            //std::ofstream out(file + ".lzw");
            //copy(compressed.begin(), compressed.end(), std::ostream_iterator<int>(out, ", ")); // Write the compressed string into the file
        } catch (std::exception e) {
            std::cout << e.what();
        }

    } else if (tolower(*argv[1]) == 'd') {
        try {
            /*
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
            */

            std::ifstream myfile2;
            myfile2.open (file.c_str(),  std::ios::binary);

            struct stat filestatus;
            stat(file.c_str(), &filestatus );
            long fsize = filestatus.st_size; //get the size of the file in bytes

            char c2[fsize];
            myfile2.read(c2, fsize);

            std::string zeros = "00000000";
            std::string code = "";
            long count = 0;
            while(count<fsize) {
                unsigned char uc =  (unsigned char) c2[count];
                std::string p = ""; //a binary string
                for (int j=0; j<8 && uc>0; j++) {
                    if (uc%2==0)
                        p="0"+p;
                    else
                        p="1"+p;
                    uc=uc>>1;
                }
                p = zeros.substr(0, 8-p.size()) + p; //pad 0s to left if needed
                code += p;
                count++;
            }

            myfile2.close();

            std::string decompressed = decompress(code);
            //std::cout << decompressed;

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



