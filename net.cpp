#include "net.h"
#include <iostream>

Net::Net()
      // Conv2d model(Conv2dOptions(3, 2, 3).stride(1).bias(false));
      // ConvOptions(int_64_t in_channels, int64_t out_channels, kernel_size);
      :  conv1(torch::nn::Conv2dOptions(1, 6, /*kernel_size=*/5).padding(2)),
         conv2(torch::nn::Conv2dOptions(6, 16, /*kernel_size=*/5)),
         fc1(400,120),
         fc2(120,84),
         fc3(84,10)
{
    register_module("conv1", conv1);
    register_module("conv2", conv2);
    register_module("fc1", fc1);
    register_module("fc2", fc2);
    register_module("fc3", fc3);

}

torch::Tensor Net::forward(torch::Tensor x)
{
    x = torch::sigmoid(conv1->forward(x));
    x = torch::avg_pool2d(x, 2);
    x = torch::sigmoid(conv2->forward(x));
    x = torch::avg_pool2d(x, 2);
    x = x.view({-1, 400});
    x = torch::sigmoid(fc1->forward(x));
    x = torch::sigmoid(fc2->forward(x));
    x = torch::sigmoid(fc3->forward(x));
    x = torch::log_softmax(x, 1);
    // std::cout << x << std::endl;
    return x;
}
