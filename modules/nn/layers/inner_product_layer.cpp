#include "inner_product_layer.h"
#include <glog/logging.h>
#include "nn/layer_factory.h"
#include "math/math_op.h"

namespace alchemy {

template<typename T>
void InnerProductLayer<T>::setup(const vector<Tensor<T> *> &input,
                                 const vector<Tensor<T> *> &output)
{
    LOG(INFO) << "Setting up " << this->param_.name();
    LOG(INFO) << "input  #0: "  << input[0]->shape();

    auto output_size = static_cast<int>(ip_param_.output_size());
    auto input_size = input[0]->count(1, 4);

    output[0]->reshape({ input[0]->shape(0), 1, output_size, 1 });
    LOG(INFO) << "output #0: "  << output[0]->shape();

    /// N x C x R x C
    M_ = input[0]->num();
    N_ = output_size;
    K_ = input_size;

    if(this->learnable_params_.empty()) {
        biasmer_.reshape({ input[0]->shape(0) });
        weights_ = LayerFactory<T>::GetSharedParam(this->param_.name(), 0);
        biases_ = LayerFactory<T>::GetSharedParam(this->param_.name(), 1);

        weights_->reshape({ output_size, input_size });
        biases_->reshape({ output_size });

        biases_.reset(new Tensor<T>({ output_size }));

        this->learnable_params_.resize(2);
        this->learnable_params_[0] = std::make_tuple(weights_, ip_param_.wlr(), ip_param_.weight_decay()/input[0]->shape(0));
        this->learnable_params_[1] = std::make_tuple(biases_, ip_param_.blr(), 0.0);

        vector_set(input[0]->shape(0), (T)1.0, biasmer_.cpu_data());

        Filler<T>::fill(*weights_, ip_param_.weight_filler());
        Filler<T>::fill(*biases_, ip_param_.bias_filler());
    }
}

template<typename T>
void InnerProductLayer<T>::ForwardCPU(const vector<Tensor<T>*>& input,
                                      const vector<Tensor<T>*>& output)
{
    auto input_data = input[0]->cpu_data();
    auto weight = weights_->cpu_data();
    auto output_data = output[0]->cpu_data();

    // w * x
    matrix_mul(CblasNoTrans, CblasTrans,
               M_, N_, K_,
               (T)1, input_data, weight,
               (T)0, output_data);

    // output_data += bias
    matrix_mul(CblasNoTrans, CblasNoTrans,
               M_, N_, 1,
               (T)1, biasmer_.cpu_data(), biases_->cpu_data(),
               (T)1, output_data);
}

template<typename T>
void InnerProductLayer<T>::BackwardCPU(const vector<Tensor<T>*>& input,
                                       const vector<Tensor<T>*>& output)
{
    // 向前传递误差
    matrix_mul(CblasNoTrans, CblasNoTrans,
               M_, K_, N_,
               (T)1, output[0]->cpu_diff(), weights_->cpu_data(),
               (T)0, input[0]->cpu_diff());


    // 计算参数的更新值
    // weights
    matrix_mul(CblasTrans, CblasNoTrans,
               N_, K_, M_,
               (T)1, output[0]->cpu_diff(), input[0]->cpu_data(),
               (T)0, weights_->cpu_diff());

    // biases
    matvec_mul(CblasTrans, M_, N_,
               (T)1, output[0]->cpu_diff(), biasmer_.cpu_data(),
               (T)0, biases_->cpu_diff());
}

template class InnerProductLayer<float>;
template class InnerProductLayer<double>;
}