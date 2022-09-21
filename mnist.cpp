#include <opencv2/opencv.hpp>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "net.h"
#include "dataloaders.tpp"

// Where to find dataset
const char* dataRoot = "./data";

// Batch size training
const int64_t trainBatchSize = 128;

// Batch size testing
const int64_t testBatchSize = 1000;

// num epochs
const int64_t numEpochs = 10;

int main()
{
  // figure out device and name it
  torch::DeviceType device_type;
  /*
  if (torch::cuda::is_available())
  {
      std::cout << "CUDA available!  Training on GPU." << std::endl;
      device_type = torch::kCUDA;
      device("cuda:0");
  } else {
      std::cout << "Training on CPU." << std::endl;
      device_type = torch::kCPU;
      device(device_type);
  }
  */
  // torch::Device device(device_type);
  torch::Device device(torch::kCPU);

  Net model;
  model.to(device);

  // Training data
  auto train_dataset = torch::data::datasets::MNIST(dataRoot)
    .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
    .map(torch::data::transforms::Stack<>());

  const size_t train_dataset_size = train_dataset.size().value();

  auto train_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(std::move(train_dataset), trainBatchSize);

  // Test data
  auto test_dataset = torch::data::datasets::MNIST(
      dataRoot, torch::data::datasets::MNIST::Mode::kTest)
    .map(torch::data::transforms::Normalize<>(0.1307, 0.3081))
    .map(torch::data::transforms::Stack<>());

  const size_t test_dataset_size = test_dataset.size().value();

  auto test_loader = torch::data::make_data_loader(std::move(test_dataset), testBatchSize);

  torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.5));

  for (size_t epoch = 1; epoch <= numEpochs; ++epoch)
  {
      train(epoch, model, device, *train_loader, optimizer, train_dataset_size);
      test(model, device, *test_loader, test_dataset_size);
  }


  // while true
  while (true)
  {
    // ask for an image
    std::cout << "To test an image, enter the location of an image or press 'q' to quit \n" << std::endl;
    std::string user_input;
    std::cin >> user_input;

    // or press q to leave
    if (user_input == "q")
    {
        return 0;
    }
    
    // read image with opencv
    cv::Mat image = cv::imread(user_input);

    // cv::Mat to torch::Tensor
    torch::Tensor tensor_image = torch::from_blob(
        image.data,
        {static_cast<int64_t>(image.rows), // height
        static_cast<int64_t>(image.cols)}, // width
        torch::TensorOptions()
          .dtype(at::kChar)
          .device(device)).to(torch::kFloat);

    // normalize
    for (int i = 0; i < 28; i++)
    {
        for (int j = 0; j < 28; j++)
        {
            tensor_image[i][j] = (3.5 * (tensor_image[i][j]/255)) - .5;
        }
    }
    std::cout << "tensor_image" << tensor_image << std::endl;

    model.eval();

    auto tensor_squeeze = tensor_image.unsqueeze(0).unsqueeze(0);
    auto output = model.forward(tensor_squeeze);

    // argmax
    auto pred = output.argmax(1);

    // cout
    std::cout << "Predicted numeral is: " << pred << "\n" << std::endl;
  }

    return 0;
}
