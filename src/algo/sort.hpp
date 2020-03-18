#pragma once

#include <cstddef>


/*
 * 通用排序算法：
 * 1、冒泡排序
 * 2、选择排序
 * 3、插入排序
 * 5、归并排序
 * 6、快速排序
 * */

namespace my {

    //冒泡排序
    template<typename Type, typename Compare>
    void BubbleSort(Type *src, size_t length, Compare compare) noexcept {
        for (size_t i = 1; i < length; i++) {
            bool flag = true;
            for (size_t j = 0; j < length - i; j++) {
                if (compare(src[j], src[j + 1])) {
                    Type temp = src[j];
                    src[j] = src[j + 1];
                    src[j + 1] = temp;
                    flag = false;
                }
            }
            if (flag)
                break;
        }
    }

    //选择排序
    template<typename Type, typename Compare>
    void SelectionSort(Type *src, size_t length, Compare compare) noexcept {
        for (size_t i = 0; i < length - 1; i++) {
            size_t target = i;
            for (size_t j = i + 1; j < length; j++) {
                if (compare(src[target], src[j])) {
                    target = j;
                }
            }
            if (target != i) {
                Type temp = src[target];
                src[target] = src[i];
                src[i] = temp;
            }
        }
    }

    //插入排序
    template<typename Type, typename Compare>
    void InsertSort(Type *src, size_t length, Compare compare) noexcept {
        for (size_t i = 1; i < length; i++) {
            Type min = src[i];
            size_t j = i;
            for (; j > 0 && compare(src[j - 1], min);) {
                src[j] = src[j - 1];
                --j;
            }
            if (i != j) {
                src[j] = min;
            }
        }
    }

    //希尔排序
    template<typename Type, typename Compare>
    void ShellSort(Type *src, size_t length, Compare compare) noexcept {
        size_t gap = 1;
        while (gap < length) {
            gap = gap * 3 + 1;
        }
        while (gap > 0) {
            for (size_t i = gap; i < length; i++) {
                Type temp = src[i];
                size_t j = i - gap;
                while (j >= 0 && src[j] > temp) {
                    src[j + gap] = src[j];
                    j -= gap;
                }
                src[j + gap] = temp;
            }
            gap /= 3;
        }
    }

    //希尔排序实现2
    template<typename Type, typename Compare>
    void ShellSort2(Type *src, size_t length, Compare compare) noexcept {
        size_t gap = length / 2;
        while (gap >= 1) {
            for (size_t i = 0; i < gap; i++) {
                for (size_t j = i; j < length - gap; j += gap) {
                    Type min = src[j + gap];
                    size_t k = j;
                    for (; k >= i && compare(src[k], min); k -= gap) {
                        src[k + gap] = src[k];
                    }
                    src[k] = min;
                }
            }
            gap /= 2;
        }
    }

    //归并排序（循环实现）
    template<typename Type, typename Compare>
    void MergeSort(Type *src, size_t length, Compare compare) noexcept {
        //合并函数，将有序数组src1和src2合并到长度为（src1_len + src2_len2）result数组中
        static auto merge = [&compare](Type *src1, size_t src1_len, Type *src2, size_t src2_len,
                                       Type *result) noexcept -> void {
            size_t i = 0, i1 = 0, i2 = 0;
            for (; i1 < src1_len && i2 < src2_len;) {
                if (compare(src1[i1], src2[i2])) {
                    result[i++] = src1[i1++];
                } else {
                    result[i++] = src2[i2++];
                }
            }
            if (i1 >= src1_len) {
                for (; i2 < src2_len;) {
                    result[i++] = src2[i2++];
                }
            }
            if (i2 >= src2_len) {
                for (; i1 < src1_len;) {
                    result[i++] = src1[i1++];
                }
            }
        };

        //缓冲数组
        Type *buffer1 = src;
        Type *buffer2 = new Type[length];

        //层次循环
        for (size_t mid = 1; mid < length; mid *= 2) {
            size_t remainder = length % (mid * 2);
            size_t lim = remainder > mid ? length : length - remainder;
            //每层循环
            for (size_t i = 0; i < lim; i += mid * 2) {
                merge(buffer1 + i,
                      mid,
                      buffer1 + i + mid,
                        //末尾的组可能个数不够中值
                      (lim - i) >= (2 * mid) ? mid : (lim - i - mid),
                      buffer2 + i);
            }
            for (size_t i = lim; i < length; i++) {
                buffer2[i] = buffer1[i];
            }
            //buffer1和buffer2依次作为merge的结果数组
            Type *temp = buffer1;
            buffer1 = buffer2;
            buffer2 = temp;
        }
        //判断最后一次最为结果数组是否为src，如果不是将另一个buffer拷贝到src
        if (buffer2 == src) {
            for (size_t i = 0; i < length; i++) {
                src[i] = buffer1[i];
            }
        }
    }

    //归并排序（递归实现）
    template<typename Type, typename Compare>
    void MergeSort2(Type *src, size_t length, Compare compare) noexcept {
        //合并函数，将有序数组src1和src2合并到长度为（src1_len + src2_len2）result数组中
        static auto merge = [&compare](Type *src1, size_t src1_len, Type *src2, size_t src2_len,
                                       Type *result) noexcept -> void {
            size_t i = 0, i1 = 0, i2 = 0;
            for (; i1 < src1_len && i2 < src2_len;) {
                if (compare(src1[i1], src2[i2])) {
                    result[i++] = src1[i1++];
                } else {
                    result[i++] = src2[i2++];
                }
            }
            if (i1 >= src1_len) {
                for (; i2 < src2_len;) {
                    result[i++] = src2[i2++];
                }
            }
            if (i2 >= src2_len) {
                for (; i1 < src1_len;) {
                    result[i++] = src1[i1++];
                }
            }
        };

        size_t mid = length / 2;
        //递归条件
        if (mid == 0) {
            return;
        } else {
            Type *result = new Type[length];
            MergeSort(src, mid, compare);
            MergeSort(src + mid, length - mid, compare);
            merge(src, mid, src + mid, length - mid, result);
            for (size_t i = 0; i < length; i++) {
                src[i] = result[i];
            }
            delete[]result;
        }
    }

    //快速排序(递归实现）
    template<typename Type, typename Compare>
    void QuickSort(Type *src, size_t length, Compare compare) noexcept {
        //分区函数，把nums数组的start到end索引内的元素按nums[start]为基准分区
        static auto partition = [&compare](Type *nums, size_t start, size_t end) noexcept -> size_t {
            //设定基准
            size_t pivot = start;
            size_t index = pivot + 1;
            for (size_t i = index; i < end; i++) {
                if (compare(nums[i], nums[pivot])) {
                    Type temp = nums[index];
                    nums[index] = nums[i];
                    nums[i] = temp;
                    ++index;
                }
            }
            size_t temp = nums[pivot];
            nums[pivot] = nums[index - 1];
            nums[index - 1] = temp;
            return index - 1;
        };
        //可以递归的函数
        static void
        (*quick)(Type *, size_t, size_t) = [](Type *nums, size_t start, size_t end) noexcept -> void {
            //递归的条件
            if (end > start) {
                size_t partitionIndex = partition(nums, start, end);
                quick(nums, start, partitionIndex);
                quick(nums, partitionIndex + 1, end);
            }
        };
        //调用递归函数
        quick(src, 0, length);
    }

    //快速排序(循环实现）待实现
    template<typename Type, typename Compare>
    void QuickSort2(Type *src, size_t length, Compare compare) noexcept {
        //分区函数，把nums数组的start到end索引内的元素按nums[start]为基准分区
        static auto partition = [&compare](Type *nums, size_t start, size_t end) noexcept -> size_t {
            //设定基准
            size_t pivot = start;
            size_t index = pivot + 1;
            for (size_t i = index; i < end; i++) {
                if (compare(nums[i], nums[pivot])) {
                    Type temp = nums[index];
                    nums[index] = nums[i];
                    nums[i] = temp;
                    ++index;
                }
            }
            size_t temp = nums[pivot];
            nums[pivot] = nums[index - 1];
            nums[index - 1] = temp;
            return index - 1;
        };

        size_t partitionIndex = partition(src, 0, length);

        for (; partition;) {

        }
    }

}