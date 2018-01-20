#ifndef ALCHEMY_CORE_TENSOR_H
#define ALCHEMY_CORE_TENSOR_H

#include "core/common.h"
#include "util/memory.h"

namespace alchemy {

template <typename T>
class Tensor {
public:
    Tensor() = default;
    explicit Tensor(const vector<int>& shape);
    Tensor(const Tensor&) = default;
    Tensor&operator=(const Tensor&) = default;
    ~Tensor() = default;

    explicit operator bool() { return count_ == 0; }

    void reshape(const vector<int>& shape);

    inline int num() const { return shape_[0]; }
    inline int count(int start, int end) const {
        assert((unsigned)start < (unsigned)shape_.size());
        assert((unsigned)end <= (unsigned)shape_.size());
        assert((unsigned)start < (unsigned)end);

        int result = 1;
        for(auto i = start; i < end; ++i) {
            result *= shape_[i];
        }
        return result;
    }
    inline bool empty() const { return count_ == 0; }
    inline int count() const { return count_; }
    inline vector<int> shape() const { return shape_; }
    inline int shape(int axis) const { assert((unsigned)axis < this->shape_.size());  return shape_[axis]; }

//    inline T * data() const { return Global::mode() == Global::CPU ? reinterpret_cast<T *>(data_->cpu_data()) : reinterpret_cast<T *>(data_->gpu_data()); }
//    inline T * diff() const { return Global::mode() == Global::CPU ? reinterpret_cast<T *>(diff_->cpu_data()) : reinterpret_cast<T *>(diff_->gpu_data()); }

    inline T * cpu_data() const { return reinterpret_cast<T *>(data_->cpu_data()); }
    inline T * gpu_data() const { return reinterpret_cast<T *>(data_->gpu_data()); }
    inline T * cpu_diff() const { return reinterpret_cast<T *>(diff_->cpu_data()); }
    inline T * gpu_diff() const { return reinterpret_cast<T *>(diff_->gpu_data()); }

//    inline T& diff_at(int i0, int i1, int i2, int i3);
//    inline const T& diff_at(int i0, int i1, int i2, int i3) const;
//
//    inline T& data_at(int i0, int i1, int i2, int i3);
//    inline const T& data_at(int i0, int i1, int i2, int i3) const;
private:
    shared_ptr<Memory> data_;
    shared_ptr<Memory> diff_;
    vector<int> shape_;

    int count_ = 0;
};

template<typename T>
Tensor<T>::Tensor(const vector<int> &shape)
{
    reshape(shape);
}

template<typename T>
void Tensor<T>::reshape(const vector<int> &shape)
{
    shape_ = shape;
    count_ = 1;
    for(const auto& i: shape) {
        count_ *= i;
    }
    // 分配内存
    data_.reset(new Memory(count_ * sizeof(T)));
    diff_.reset(new Memory(count_ * sizeof(T)));
}
//
//template<typename T>
//const T& Tensor<T>::data_at(int i0, int i1, int i2, int i3) const
//{
//    assert(shape_.size() == 4);
//    assert((unsigned)i0 < (unsigned)shape_[0]);
//    assert((unsigned)i1 < (unsigned)shape_[1]);
//    assert((unsigned)i2 < (unsigned)shape_[2]);
//    assert((unsigned)i3 < (unsigned)shape_[3]);
//
//    auto data_ptr = data();
//    auto c_1_3 = count(1, 4);
//    auto c_2_3 = count(2, 4);
//    auto c_3 = shape_[3];
//
//    return data_ptr[i0 * c_1_3 + i1 * c_2_3 + i2 *c_3 + i3];
//}
//
//template<typename T>
//T& Tensor<T>::data_at(int i0, int i1, int i2, int i3)
//{
//    assert(shape_.size() == 4);
//    assert((unsigned)i0 < (unsigned)shape_[0]);
//    assert((unsigned)i1 < (unsigned)shape_[1]);
//    assert((unsigned)i2 < (unsigned)shape_[2]);
//    assert((unsigned)i3 < (unsigned)shape_[3]);
//
//    auto data_ptr = data();
//    auto c_1_3 = count(1, 4);
//    auto c_2_3 = count(2, 4);
//    auto c_3 = shape_[3];
//
//    return data_ptr[i0 * c_1_3 + i1 * c_2_3 + i2 *c_3 + i3];
//}
//
//template<typename T>
//T &Tensor<T>::diff_at(int i0, int i1, int i2, int i3)
//{
//    assert(shape_.size() == 4);
//    assert((unsigned)i0 < (unsigned)shape_[0]);
//    assert((unsigned)i1 < (unsigned)shape_[1]);
//    assert((unsigned)i2 < (unsigned)shape_[2]);
//    assert((unsigned)i3 < (unsigned)shape_[3]);
//
//    auto diff_ptr = diff();
//    auto c_1_3 = count(1, 4);
//    auto c_2_3 = count(2, 4);
//    auto c_3 = shape_[3];
//
//    return diff_ptr[i0 * c_1_3 + i1 * c_2_3 + i2 *c_3 + i3];
//}
//
//template<typename T>
//const T &Tensor<T>::diff_at(int i0, int i1, int i2, int i3) const
//{
//    assert(shape_.size() == 4);
//    assert((unsigned)i0 < (unsigned)shape_[0]);
//    assert((unsigned)i1 < (unsigned)shape_[1]);
//    assert((unsigned)i2 < (unsigned)shape_[2]);
//    assert((unsigned)i3 < (unsigned)shape_[3]);
//
//    auto diff_ptr = diff();
//    auto c_1_3 = count(1, 4);
//    auto c_2_3 = count(2, 4);
//    auto c_3 = shape_[3];
//
//    return diff_ptr[i0 * c_1_3 + i1 * c_2_3 + i2 *c_3 + i3];
//}

}
#endif //! ALCHEMY_CORE_TENSOR_H