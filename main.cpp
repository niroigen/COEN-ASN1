#include <iostream>
#include <thread>
#include <fstream>
#include <string>

int FindPivot(int left, int right);
void FindDefective(int left, int right);
bool IsDefectiveSubarray(int start, int end);
int ReadFile(const std::string& file);
int numThreads = 0;
int * lightBulbs;

const std::string INPUT_FILE = "Input.txt";
const std::string NUMBER_OF_THREADS_USED = "The number of threads for this problem was : ";
const std::string UNABLE_TO_OPEN_FILE = "Unable to open file";
const std::string DEFECTIVE_LIGHT_BULB =  "Defective bulb at position ";
const std::string ENTER_RIGHT_AMOUNT_OF_LIGHT_BULBS = "Please enter right amount of light bulbs";

int main()
{
    int start = 0;
    int size = ReadFile(INPUT_FILE);

    auto startTime = std::chrono::high_resolution_clock::now();

    std::thread findDefectiveThread(FindDefective, start, size - 1);
    findDefectiveThread.join();

    auto finish = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = finish - startTime;

    std::cout << NUMBER_OF_THREADS_USED << numThreads << std::endl;
    std::cout << "Process has taken " << elapsed.count() << std::endl;

    return 0;
}

/*
 * Reading the file to extract the information
 * about the light bulbs, i.e. the number of files
 * and each light bulbs state (defective or working)
 */
int ReadFile(const std::string& file) {
    // this variable will be used to read every line of the file
    std::string line;

    // used to read the contents of the file
    std::ifstream inputFile(file);

    // used to store the size of the number of bulbs that are being used
    int size = 0;

    // The first line of the file will be used to store the number of bulbs there are
    bool isFirstLine = true;

    try
    {
        if (inputFile.is_open())
        {
            int line_num = 0;
            while (getline(inputFile, line))
            {
                if (isFirstLine)
                {
                    // If it is the first line then extract the number of light bulbs that will be used
                    size = std::stoi(line);

                    // Creating an array of the size provided in line 1 of the file
                    lightBulbs = new int[size];
                    isFirstLine = false;
                } else if (line_num >= size) {
                    std::cerr << "Expected " << size << " light bulbs, but received too many." << std::endl;
                    std::cerr << ENTER_RIGHT_AMOUNT_OF_LIGHT_BULBS << std::endl;
                    exit(0);
                } else {
                    // Storing the state of each of the light bulbs
                    int val =  std::stoi(line);

                    if (val != 0 && val != 1) {
                        throw std::invalid_argument("Bulb state can only be 0 or 1");
                    }

                    lightBulbs[line_num++] = std::stoi(line);
                }
            }

            if (line_num != size) {
                std::cerr << "Expected " << size << " light bulbs, but received only " << line_num << std::endl;
                std::cerr << ENTER_RIGHT_AMOUNT_OF_LIGHT_BULBS << std::endl;
                exit(0);
            }
        }

        // If the file is not readable then output a message
        else std::cout << UNABLE_TO_OPEN_FILE;
    }
    catch(const std::invalid_argument& ia)
    {
        std::cerr << "Invalid argument: " << ia.what() << std::endl;
        std::cerr << "File not in proper format" << std::endl;
        exit(0);
    }

    inputFile.close();

    return size;
}

/*
 * Using recursion to see whether a given array has a defective light bulb or not
 */
void FindDefective(int left, int right) {
    // Keeping track of the number of the thread
    numThreads++;

    // If the there is just one element in the subarray, then check whether it is defected o
    if (left == right) {
        if (lightBulbs[left] == 0) {
            std::cout << DEFECTIVE_LIGHT_BULB << left + 1 << std::endl;
            return;
        }
    }

    // If a subarray isn't defective then there is nothing to do
    if (!IsDefectiveSubarray(left, right)) return;

    // Getting the pivot point
    int pivot = FindPivot(left, right);

    // Starting a thread which will attempt to find a defective light bulb on the left
    // side of the light bulb array
    std::thread left_thread(FindDefective, left, pivot - 1);

    // Starting a thread which will attempt to find a defective light bulb on the right
    // side of the light bulb array
    std::thread right_thread(FindDefective, pivot, right);

    left_thread.join();
    right_thread.join();
}

/*
 * Will verify whether the array provided
 * contains at least one defective bulb
 */
bool IsDefectiveSubarray(int start, int end)
{
    for (int i = start; i <= end; i++)
        if (lightBulbs[i] == 0) return true;

    return false;
}

/*
 * Getting the pivot given a left and right value
 * where left is always less than right
 * The pivot will be the middle value in between left and right
 */
int FindPivot(int left, int right) {
    return ((right + 1) + left) / 2;
}