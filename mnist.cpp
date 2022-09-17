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
  if (torch::cuda::is_available())
  {
      std::cout << "CUDA available!  Training on GPU." << std::endl;
      device_type = torch::kCUDA;
  } else {
      std::cout << "Training on CPU." << std::endl;
      device_type = torch::kCPU;
  }
  torch::Device device(device_type);

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

    return 0;
}
