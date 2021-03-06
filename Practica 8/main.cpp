#include <iostream>
#include <mpi.h>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>

/*  Practice 8  */

#define NUMLINES 80000
#define CHAR_PER_LINE 14
#define ROOT 0

using namespace std;

struct Item {
    int idUser;
    int idMovie;
    double rating;

    static bool compare(const Item &a, const Item &b){
        return a.idMovie < b.idMovie;
    };

    static bool compareByRating(const Item &a, const Item &b){
        return a.rating > b.rating;
    };
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

void printArray(int* arr, int count){
    for(int i = 0; i < count; i+=2){
        printf("%d %d,", arr[i], arr[i+1]);
    }

    printf("\n");
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

bool sortbysec(const pair<int, int> &a, const pair<int, int> &b){
    return a.second > b.second;
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

    // If num lines is not divisible by workers, abort
    if (NUMLINES % size != 0){
        printf("Number of workers have to divide %d. Execution finalizes.\n", NUMLINES);
        MPI_Finalize();
        return 0;
    }

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
    //printf("Num lines per worker: %d\n", static_cast<int>(lines.size()));

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

    if (rank == ROOT){
        // Group data by movie in a matrix
        vector<vector<Item>> ratings(NUMLINES);
        map<int, int> indices; // Index of the movie in itemsRecv array. This will ease the distribution of the data in the processes.
        int previousId = -1;
        int count = 0;

        std::sort(itemsRecv, itemsRecv + NUMLINES, Item::compare); // Even though processes send data in an ordered way, movies id are not naturally sorted.

        for(Item &item: itemsRecv){
            ratings[item.idMovie].push_back(item);
            if (item.idMovie != previousId){
                previousId = item.idMovie;
                indices.insert(pair<int,int>(item.idMovie, count)); // Map with idMovie, index in initial array
            }
            count ++;
        }

        vector<std::pair<int, int>> idRatings;
        for(int i = 0; i < ratings.size(); i++){
            if(ratings[i].size() >= 20){
                idRatings.push_back(std::pair<int, int>(i, ratings[i].size()));
            }
        }
        //printf("Number of movies: %d\n", idRatings.size());

        // Sort movies by number of ratings done
        sort(idRatings.begin(), idRatings.end(), sortbysec);

        // Distribute data to each process
        vector<vector<pair<int, int>>> movies_workers(size); // id, numMovies which are going to be calculated by each process
        int worker = 0;
        for(int i = 0; i < idRatings.size(); i++){
            movies_workers[worker + 1].push_back(pair<int, int>(/*index*/indices[idRatings[i].first], /*lines to read*/idRatings[i].second));
            worker = (worker + 1) % (size-1);
        }

        // Send data to every process
        for(int i = 1; i < movies_workers.size(); i++){
            //printf("worker %d num_movies %d\n", i, movies_workers[i].size());

            int countToTransfer = movies_workers[i].size() * 2; // Including both elements in pairs
            int * transferData = new int[countToTransfer];

            for (int j = 0; j < movies_workers[i].size(); j++){
                transferData[j * 2] = movies_workers[i][j].first;
                transferData[j * 2 + 1] = movies_workers[i][j].second;
            }

            // Send to node the number of movies that have to look for
            MPI_Send(&countToTransfer, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Ssend to wait for the receipment
            MPI_Send(transferData, countToTransfer, MPI_INT, i, 0, MPI_COMM_WORLD);

            // Free memory
            delete [] transferData;
        }
    }

    // Broadcast initial matrix to all processes
    MPI_Bcast(itemsRecv, NUMLINES, itemType, ROOT, MPI_COMM_WORLD);

    if (rank != ROOT){
        // This number will be sended by ROOT PROC
        int moviesToCompute = 0;

        // Receive number of movies to look for (indices) and evaluations of each (amount of lines that must be read again)
        MPI_Recv(&moviesToCompute, 1, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("Rank %d received %d\n", rank, moviesToCompute);

        // recvData contains index, numRatings for each movie. 
        int * recvData = new int[moviesToCompute];

        MPI_Recv(recvData, moviesToCompute, MPI_INT, ROOT, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // The real number of movies to compute is moviesToCompute/2 because both index in itemsRecv and number of movies are included.
        int numberOfMovies = moviesToCompute/2;

        // Movies to be sended to ROOT after being calculated
        Item * calculated = new Item[numberOfMovies];

        // Calculate mean score for each received movie
        for(int i = 0; i < numberOfMovies; i++){
            int index = recvData[i * 2];
            int numberOfRatings = recvData[i * 2 + 1];
            int movieId = itemsRecv[index].idMovie;
            float summa = 0;
            for (int j = 0; j < numberOfRatings; j++){
                summa += itemsRecv[index + j].rating;
            }

            // Calculate mean for this movie
            summa /= numberOfRatings;
            
            // Store that movie
            calculated[i].idMovie = movieId;
            calculated[i].rating = summa;
            calculated[i].idUser = numberOfRatings;
        }

        // Send movies to root proc (only ten first)
        std::sort(calculated, calculated + numberOfMovies, Item::compareByRating);
        MPI_Send(calculated, 10, itemType, ROOT, 0, MPI_COMM_WORLD);

        // Free memory
        delete [] calculated;
        delete [] recvData;
    }

    if (rank == ROOT){
        // Receive 10 movies from each process
        int totalReceivedMovies = 10 * (size - 1);
        Item * calculatedMovies = new Item[totalReceivedMovies]; // 10 movies from each worker except root process.
        for(int i = 1; i < size; i++){
            MPI_Recv(calculatedMovies + (i-1) * 10, 10, itemType, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Sort received movies and select ten first
        std::sort(calculatedMovies, calculatedMovies + totalReceivedMovies, Item::compareByRating);

        // Show ten first movies sorted descending by avg rating
        for(int i = 0; i < 10; i++){
            printf("Puesto %d: Película %d, valorada %d veces. Valoración media: %f\n", i + 1, calculatedMovies[i].idMovie, calculatedMovies[i].idUser, calculatedMovies[i].rating);
        }

        // Free memory 
        delete [] calculatedMovies;
    }

    // Free memory
    delete [] buffer;

    // Close file
    MPI_File_close(&file);

    // MPI finish
    MPI_Finalize();

    return 0;
}
