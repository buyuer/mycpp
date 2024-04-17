import mycpp;

#include <iostream>
#include <vector>

int main(int argc, char **argv)
{
    std::vector<int> nums{666, 3, 34, 2};

    mycpp::SelectionSort(nums.data(), nums.size(),
                         [](int a, int b) -> bool { return a > b; });

    for (auto &num : nums)
    {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    mycpp::ListLink<int> list;
}
