#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>

/*  Practice 8  */

#define NUMLINES 80000
#define CHAR_PER_LINE 14
#define ROOT 0

using namespace std;

struct Item {
    int idUser;
    int idMovie;
    double rating;
};

void printString(char* str, int count){
    for(int i = 0; i < count; i++){
        if(str[i] == '\n'){
            printf("\n");
        } else {
            printf("%c", str[i]);
        }
    }
}

std::string trim(const string& str){
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first){
        return str;
    }

    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> splitLines(char* str){
    std::vector<std::string> lines;
    std::stringstream ss(str);
    std::string to;

    if(str != NULL){
        while(std::getline(ss, to, '\n')){
            to = trim(to);
            lines.push_back(to);
        }
    }

    return lines;
}

std::vector<string> splitWhitespaces(const std::string &str){
    std::vector<string> tmp;
    string word = "";
    for(auto x: str){
        if (x == ' '){
            tmp.push_back(word);
            word = "";
        } else {
            word = word + x;
        }
    }
    tmp.push_back(word);

    return tmp;
}

std::vector<Item> getItems(const std::vector<std::string>& lines){
    std::vector<Item> items;
    for(int i = 0; i < lines.size(); i++){
        std::vector<string> tmp = splitWhitespaces(lines[i]);
        
        Item item;
        item.idUser = std::stoi(tmp[0]);
        item.idMovie = std::stoi(tmp[1]);
        item.rating = std::stoi(tmp[2]);

        items.push_back(item);
    }

    return items;
}

int main(int argc, char **argv){
    // MPI initialization
    MPI_Init(&argc, &argv);

    // Get cardinal of the set of processes
    int size = MPI::COMM_WORLD.Get_size();

    // Number of the active process
    int rank = MPI::COMM_WORLD.Get_rank();

    // Create file
    MPI_File file;
    MPI_File_open(MPI_COMM_WORLD, (char*) "./data.txt", MPI_MODE_RDONLY, MPI_INFO_NULL, &file);

    // Get file size
    MPI_Offset fileSize;
    MPI_File_get_size(file, &fileSize);

    // Read file
    int linesPerWorker = NUMLINES / size;
    int numBytes = linesPerWorker * CHAR_PER_LINE * sizeof(char); // Number of bytes handled by each process
    char * buffer = new char[numBytes];

    // Move pointer to data which is going to be read
    MPI_File_seek(file, rank * numBytes, MPI_SEEK_SET);
    // Read content of the file starting from pointer
    MPI_File_read(file, buffer, numBytes, MPI_CHAR, MPI_STATUS_IGNORE); 

    // Split lines that have been read by each process
    std::vector<std::string> lines = splitLines(buffer);
    printf("Num lines per worker: %d\n", static_cast<int>(lines.size()));

    // Split each line into: idUser, idMovie, rating and store that values in Item struct.
    std::vector<Item> items = getItems(lines);
    int itemsWorker = items.size();

    // Send all values to root process
    // Create struct type
    MPI_Datatype itemType;
    MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_DOUBLE};
    int longBlock[3] = {1, 1, 1};
    MPI_Aint offset[3];
    offset[0] = offsetof(Item, idUser);
    offset[1] = offsetof(Item, idMovie);
    offset[2] = offsetof(Item, rating);

    MPI_Type_create_struct(3, longBlock, offset, types, &itemType);
    MPI_Type_commit(&itemType);

    // Where to receive data in root process
    Item itemsRecv[NUMLINES];

    // Send all data to root process
    MPI_Gather(items.data(), itemsWorker, itemType, itemsRecv, itemsWorker, itemType, ROOT, MPI_COMM_WORLD);

    // printf("Process: %d User: %d Movie: %d Rating: %f\n", rank, itemsRecv[0].idUser, itemsRecv[0].idMovie, itemsRecv[0].rating);

    // Group data by movie in a matrix
    vector<vector<Item>> ratings(NUMLINES);
    for(Item &item: itemsRecv){
        ratings[item.idMovie].push_back
    }

    // Close file
    MPI_File_close(&file);

    // MPI finish
    MPI_Finalize();

    return 0;
}
