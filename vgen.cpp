#include <fstream>
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <array>
#include <mutex>
//#include <future>
#include <ctime>
#include <random>

/**
 * @brief Function that returns a vector of a selected size filled with numbers from array
 * 
 * @tparam T Datatype to be used for array and returned vector
 * @param numers Takes a reference to an array with which numbers are to be used in the returned vector
 * @param len Sets the ammount of members the returned std::vector should have
 * 
 * @return std::vector<T>
 */
template<typename T, std::size_t N>
std::vector<T> RandomFillVector(std::array<T, N>& numbers,size_t len){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator(seed);
    std::uniform_int_distribution<int> randRange(0, N);

    std::vector<T> temp;
    temp.reserve(len);
    for (size_t i = 0; i < len; i++)
    {
        temp.push_back(numbers[randRange(generator)]);
    }
    temp.shrink_to_fit();
    return temp;
}
/**
 * @brief Assistant function for multithreaded version of RandomFillVector, it handles the multithreaded generation of the vector by 
 * handling locking resources and assigning what chunks will certain threads generate
 * 
 * @param numers Takes a reference to an array with which numbers are to be used in the returned vector
 * @param len The length of the desired vector 
 * @param dest Reference vector where the final data is going to be stored
 * @param mutex Mutex reference needed so all the cores know if they can acces destination vector
 */
template<typename T, std::size_t N>
void FRTAssist(std::array<T, N>& numbers,size_t len, std::vector<T>& dest, std::mutex& mutex){
    std::vector<T> temp = RandomFillVector(numbers,len);
    {
        std::lock_guard<std::mutex> lock(mutex);
        for (size_t i = 0; i < temp.size(); i++)
        {
            dest.push_back(temp[i]);
        }
    }
}
/**
 * @brief Function that returns a vector of a selected size filled with numbers from array with the use of multithreading
 * 
 * @tparam T Datatype to be used for array and returned vector
 * @param numers Takes a reference to an array with which numbers are to be used in the returned vector
 * @param len Sets the ammount of members the returned std::vector should have
 * @param tCount Ammount of threads to run the function on
 * @return std::vector<T> 
 */
template<typename T, std::size_t N>
std::vector<T> RandomFillVectorThreaded(std::array<T, N>& numbers,size_t len, int tCount){
    if (tCount > len) tCount = len;
    int clen = len/tCount;

    std::vector<T> temp;
    temp.reserve(len);
    
    std::vector<std::thread> threads;
    std::mutex mutex;
    if (tCount > 0)
    {
        for (size_t i = 0; i < tCount - 1; i++)
        {
            threads.emplace_back(std::thread(FRTAssist<T, N>, std::ref(numbers), clen, std::ref(temp), std::ref(mutex)));
            len = len - clen;
        }
        threads.emplace_back(std::thread(FRTAssist<T, N>, std::ref(numbers), len, std::ref(temp), std::ref(mutex)));
    }
    for (size_t i = 0; i < tCount; i++)
    {
        threads[i].join();
    }
    temp.shrink_to_fit();
    return temp;
}
/**
 * @brief Prints the vector to cout styled as "{n,n,n}" where n are numbers
 * 
 * @param targetVector What is supposed to be printed
 */
template<typename T>
void CoutVectorSet(std::vector<T>& targetVector){
    for (size_t i = 0; i < targetVector.size(); i++)
    {
        if (i == 1)
        {
            std::cout << "{" << targetVector[i] << ",";
        }
        else if(i == targetVector.size() - 1){
            std::cout << targetVector[i] << "}";
        }
        else{
            std::cout << targetVector[i] << ",";
        }
    }
    printf("\n");
}


int main(int argc, char** argv ){
    
    std::array<int, 9> arr = {1,76282,3,5,6,7,8,9,0};
    //std::vector<long> test = RandomFillVector<long>(arr,__INT_MAX__);
    //std::vector<long> test = RandomFillVectorThreaded<long>(arr,4294967295,16);
    std::vector<int> test = RandomFillVectorThreaded<int>(arr,1000000,2);
    CoutVectorSet(test);
    //std::cout << test.size() << "\n";
    
    return 0;
}  